#ifndef SUBNX_H
#define SUBNX_H
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

static const std::unordered_set<std::string> PRINCIPALS = {
    "kingdom", "phylum", "class", "order", "family", "genus", "species"
};
static const std::string RANK_DELIMITER = "__";

class NameIO {
public:
    static void parse(const std::string& file, std::unordered_map<std::string, std::string>& names);
private:
    static bool is_scientific_name(const std::string& name);
};

class Node {
public:
    Node();
    Node(std::string id, std::string name);
    Node(std::string id, std::string name, Node* parent, std::string rank);

    bool is_root() const;
    void append(Node* child);
    void trace(std::vector<const Node*>& ancestors) const;
    void expand(std::vector<const Node*>& descendants) const;
    bool is_principal() const;
    std::string str(bool abbr=true) const;
    std::string lineage(bool principal=true) const;

public:
    std::string id;
    std::string name;
    Node* parent;
    std::string rank;
    std::vector<Node*> children;
};

class NodeIO {
public:
    static void parse(
        const std::string& file,
        const std::unordered_map<std::string, std::string>& names,
        std::unordered_map<std::string, Node*>& nodes
    );
    static void destroy(std::unordered_map<std::string, Node*>& nodes);
};

class Accession2TaxIdIO {
public:
    static void parse(
        const std::string& file,
        const std::unordered_set<std::string>& taxids,
        std::unordered_map<std::string, std::string>& accession2taxid
    );
};

class Index {
public:
    Index();
    Index(std::string accession, std::string accession_version, std::size_t pos, std::size_t length);
public:
    std::string accession;
    std::string accession_version;
    std::size_t pos;
    std::size_t length;
};

class IndexIO {
public:
    static void create(const std::string& infile, const std::string& outfile);
    static void parse(
        const std::string& file,
        const std::unordered_set<std::string>& accessions,
        std::vector<Index>& indexes
    );
};

class ResultIO {
public:
    static void write_taxa(
        const std::vector<Index>& indexes,
        const std::unordered_map<std::string, std::string>& accession2taxid,
        const std::unordered_map<std::string, Node*>& nodes,
        bool full_lineage,
        const std::string& outfile
    );
    static void write_seqs(
        const std::string& infile,
        const std::vector<Index>& indexes,
        const std::string& outfile
    );
};

#endif
