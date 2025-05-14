#include "utils.h"
#include "subnx.h"
#include <fstream>

void NameIO::parse(const std::string& file, std::unordered_map<std::string, std::string>& names) {
    std::ifstream in(file);
    if (! in) {
        throw std::runtime_error(file + ": Failed to open file");
    }
    std::string line;
    std::vector<std::string> row;
    while (std::getline(in, line)) {
        str::split(line, "\t|\t", row);
        if ((row.size() == 4) && is_scientific_name(row[3])) {
            names.emplace(row[0], row[1]);
        }
        row.clear();
    }
    in.close();
}

bool NameIO::is_scientific_name(const std::string& name) {
    return name.rfind("scientific name", 0) == 0;
}

Node::Node() : parent(nullptr) {}

Node::Node(std::string id, std::string name)
    : id(std::move(id))
    , name(std::move(name))
    , parent(nullptr) {}

Node::Node(std::string id, std::string name, Node* parent, std::string rank)
    : id(std::move(id))
    , name(std::move(name))
    , parent(parent)
    , rank(std::move(rank)) {
}

bool Node::is_root() const {
    return parent == nullptr;
}

void Node::append(Node* child) {
    children.push_back(child);
}

void Node::trace(std::vector<const Node*>& ancestors) const {
    ancestors.push_back(this);
    if (is_root()) return;
    parent->trace(ancestors);
}

void Node::expand(std::vector<const Node*>& descendants) const {
    descendants.push_back(this);
    if (children.empty()) return;
    for (const Node* child : children) {
        child->expand(descendants);
    }
}

bool Node::is_principal() const {
    return PRINCIPALS.find(rank) != PRINCIPALS.end();
}

std::string Node::str(bool abbr) const {
    std::string str;
    if (abbr && is_principal()) {
        str = rank[0] + RANK_DELIMITER + name;
    } else {
        str = rank + RANK_DELIMITER + name;
    }
    str::replace(str, " ", "_");
    return str;
}

std::string Node::lineage(bool principal) const {
    std::vector<const Node*> ancestors;
    trace(ancestors);
    std::string lineage;
    for (const Node* node : ancestors) {
        if (principal && (! node->is_principal())) {
            continue;
        }
        
        if (lineage.empty()) {
            lineage = node->str(principal);
        } else {
            lineage = node->str(principal) + "; " + lineage;
        }
    }
    return lineage;
}

void NodeIO::parse(
    const std::string& file,
    const std::unordered_map<std::string, std::string>& names,
    std::unordered_map<std::string, Node*>& nodes
) {
    std::ifstream in(file);
    if (! in) {
        throw std::runtime_error(file + ": Failed to open file");
    }
    std::string line;
    std::vector<std::string> row;
    while (std::getline(in, line)) {
        str::split(line, "\t|\t", row);
        if (row.size() != 13) {  // omit malformed line
            row.clear();
            continue;
        }

        // get or create parent node
        Node* parent = nullptr;
        if (row[0] != "1") {  // nullptr if is root
            if (nodes.find(row[1]) != nodes.end()) {  // get if already exists
                parent = nodes.at(row[1]);
            } else {  // create if not exist
                parent = new Node(row[1], names.at(row[1]));
                nodes.emplace(row[1], parent);
            }
        }

        // create or update node
        Node* node = nullptr;
        if (nodes.find(row[0]) != nodes.end()) {  // update if already exists
            node = nodes.at(row[0]);
            node->parent = parent;
            node->rank = row[2];
        } else {  // create if not exist
            node = new Node(row[0], names.at(row[0]), parent, row[2]);
            nodes.emplace(row[0], node);
        }

        // append node to parent's children
        if (parent != nullptr) {
            parent->append(node);
        }
        row.clear();
    }
}

void NodeIO::destroy(std::unordered_map<std::string, Node*>& nodes) {
    for (auto it=nodes.begin(); it!=nodes.end(); ++it) {
        delete it->second;
        it->second = nullptr;
    }
}

void Accession2TaxIdIO::parse(
    const std::string& file,
    const std::unordered_set<std::string>& taxids,
    std::unordered_map<std::string, std::string>& accession2taxid
) {
    std::ifstream in(file);
    if (! in) {
        throw std::runtime_error(file + ": Failed to open file");
    }

    // omit header
    std::string line;
    if (! std::getline(in, line)) {
        return;
    }

    std::vector<std::string> row;
    while (std::getline(in, line)) {
        str::split(line, '\t', row);
        if ((row.size() == 4) && (taxids.find(row[2]) != taxids.end())) {
            accession2taxid.emplace(row[0], row[2]);
        }
        row.clear();
    }
    in.close();
}

Index::Index() : pos(0) , length(0) {
}

Index::Index(std::string accession, std::string accession_version, std::size_t pos, std::size_t length)
    : accession(std::move(accession))
    , accession_version(std::move(accession_version))
    , pos(pos)
    , length(length) {
}

void IndexIO::create(const std::string& infile, const std::string& outfile) {
    std::ifstream in(infile);
    std::ofstream out(outfile);
    if (! in) {
        throw std::runtime_error(infile + ": Failed to open file");
    }
    if (! out) {
        throw std::runtime_error(outfile + ": Failed to open file");
    }

    std::streampos begin = 0;
    std::string line;
    std::getline(in, line);
    if ((line.length() < 3) || line[0] != '>') {
        throw std::runtime_error(infile + ": Invalid fasta file");
    }

    std::size_t dot_pos = line.find('.');
    std::string accession = line.substr(1, dot_pos-1);
    std::size_t space_pos = line.find(' ', dot_pos+1);
    std::string accession_version = line.substr(1, space_pos-1);
    std::streampos end = in.tellg();
    while (std::getline(in, line)) {
        if (line[0] == '>') {
            out << accession << '\t' << accession_version << '\t' << begin << '\t' << (end - begin) << '\n';
            dot_pos = line.find('.');
            accession = line.substr(1, dot_pos-1);
            space_pos = line.find(' ', dot_pos+1);
            accession_version = line.substr(1, space_pos-1);
            begin = end;
        }
        end = in.tellg();
    }
    out << accession << '\t' << accession_version << '\t' << begin << '\t' << (end - begin) << '\n';
    in.close();
    out.close();
}

void IndexIO::parse(
    const std::string& file,
    const std::unordered_set<std::string>& accessions,
    std::vector<Index>& indexes
) {
    std::ifstream in(file);
    if (! in) {
        throw std::runtime_error(file + ": Failed to open file");
    }
    std::string line;
    std::vector<std::string> row;
    while (std::getline(in, line)) {
        str::split(line, '\t', row);
        if (accessions.find(row[0]) != accessions.end()) {
            indexes.emplace_back(row[0], row[1], std::stoull(row[2]), std::stoull(row[3]));
        }
        row.clear();
    }
    in.close();
}

void ResultIO::write_taxa(
    const std::vector<Index>& indexes,
    const std::unordered_map<std::string, std::string>& accession2taxid,
    const std::unordered_map<std::string, Node*>& nodes,
    bool full_lineage,
    const std::string& outfile
) {
    std::ofstream out(outfile);
    if (! out) {
        throw std::runtime_error(outfile + ": Failed to open file");
    }
    std::string taxid;
    Node* node = nullptr;
    bool principal = ! full_lineage;
    for (const Index& index : indexes) {
        taxid = accession2taxid.at(index.accession);
        node = nodes.at(taxid);
        out << index.accession_version << '\t' << node->lineage(principal) << '\n';
    }
    out.close();
}

void ResultIO::write_seqs(
    const std::string& infile,
    const std::vector<Index>& indexes,
    const std::string& outfile
) {
    std::ifstream in(infile);
    std::ofstream out(outfile);
    if (! in) {
        throw std::runtime_error(infile + ": Failed to open file");
    }
    if (! out) {
        throw std::runtime_error(outfile + ": Failed to open file");
    }
    for (const Index& index : indexes) {
        std::vector<char> buffer(index.length);
        in.seekg(index.pos, std::ios::beg);
        in.read(buffer.data(), index.length);
        out.write(buffer.data(), index.length);
    }
}
