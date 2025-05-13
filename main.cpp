#include "cmdline.h"
#include "utils.h"
#include "subnx.h"
#include <ctime>
#include <iomanip>
#include <fstream>
#include <iostream>

void log(const std::string& msg) {
    std::time_t time = std::time(nullptr);
    std::tm* localtime = std::localtime(&time);
    std::cout << std::put_time(localtime, "[%H:%M:%S]") << " " << msg << std::endl;
}

int main(int argc, char** argv) {
    std::time_t start = std::time(nullptr);
    cmdline::parser parser;
    parser.add<std::string>("id", 'i', "taxon ID (e.g., 5455 for Colletotrichum)", true);
    parser.add<std::string>("taxdmp-dir", 't', "decompressed taxdmp.zip directory", true);
    parser.add<std::string>("accession2taxid-file", 'a', "decompressed accession2taxid file", true);
    parser.add<std::string>("nx-file", 'n', "decompressed nt/nr sequences file", true);
    parser.add("full-lineage", 'f', "output full lineage instead of principal ranks only");
    parser.add<std::string>("output-seqs-file", 's', "output sequences file, omit if not provide", false);
    parser.add<std::string>("output-taxa-file", 'T', "output taxa file", true);
    parser.parse_check(argc, argv);

    const std::string id = parser.get<std::string>("id");
    const std::string taxdmp_dir = parser.get<std::string>("taxdmp-dir");
    const std::string accession2taxid_file = parser.get<std::string>("accession2taxid-file");
    const std::string nx_file = parser.get<std::string>("nx-file");
    const bool full_lineage = parser.exist("full-lineage");
    const std::string output_seqs_file = parser.get<std::string>("output-seqs-file");
    const std::string output_taxa_file = parser.get<std::string>("output-taxa-file");

    // check if all files or directories exist
    if (! os::path::exists(taxdmp_dir)) {
        std::cerr << taxdmp_dir << ": No such file or directory" << std::endl;
        return 1;
    }
    if (! os::path::exists(nx_file)) {
        std::cerr << nx_file << ": No such file or directory" << std::endl;
        return 1;
    }
    if (! os::path::exists(accession2taxid_file)) {
        std::cerr << accession2taxid_file << ": No such file or directory" << std::endl;
        return 1;
    }

    // check if the taxdmp directory is valid
    std::string names_file = os::path::join({taxdmp_dir, "names.dmp"});
    if (! os::path::exists(names_file)) {
        std::cerr << names_file << ": No such file or directory" << std::endl;
        return 1;
    }
    std::string nodes_file = os::path::join({taxdmp_dir, "nodes.dmp"});
    if (! os::path::exists(nodes_file)) {
        std::cerr << nodes_file << ": No such file or directory" << std::endl;
        return 1;
    }

    std::unordered_map<std::string, std::string> names;  // taxid to name
    std::unordered_map<std::string, Node*> nodes;  // taxid to node
    Node* node = nullptr;  // the node
    std::vector<const Node*> descendants;  // node descendants
    std::unordered_set<std::string> taxids;  // node descendant taxids
    std::unordered_map<std::string, std::string> accession2taxid;  // accession to taxid
    std::unordered_set<std::string> accessions;  // accessions
    std::vector<Index> indexes;  // indexes

    try {
        // parse nodes
        NameIO::parse(names_file, names);
        if (names.empty()) {
            std::cerr << names_file << ": Invalid names file" << std::endl;
            return 1;
        }
        NodeIO::parse(nodes_file, names, nodes);
        if (nodes.empty()) {
            std::cerr << nodes_file << ": Invalid nodes file" << std::endl;
            return 1;
        }
        log("Loaded " + std::to_string(nodes.size()) + " nodes from " + taxdmp_dir);

        // get node
        if (nodes.find(id) == nodes.end()) {
            std::cerr << id << ": Taxon ID not found" << std::endl;
            return 1;
        }
        node = nodes.at(id);

        // get descendant nodes
        node->expand(descendants);
        for (const Node* descendant : descendants) {
            taxids.insert(descendant->id);
        }
        log("Traced " + std::to_string(descendants.size()) + " descendant nodes for node " + id);

        // parse accessions
        Accession2TaxIdIO::parse(accession2taxid_file, taxids, accession2taxid);
        for (const auto& pair : accession2taxid) {
            accessions.insert(pair.first);
        }
        log("Found " + std::to_string(accession2taxid.size()) + " related accessions in " + accession2taxid_file);

        // index sequences file if index doesn't exist
        const std::string index_file = nx_file + ".fai";
        if (! os::path::exists(index_file)) {
            log("Indexing, estimated time required: 1 hour (required only for the first run)");
            IndexIO::create(nx_file, index_file);
        }

        // parse indexes
        IndexIO::parse(index_file, accessions, indexes);
        log("Found " + std::to_string(indexes.size()) + "" + " accessions existing in " + nx_file);

        // write results
        ResultIO::write_taxa(indexes, accession2taxid, nodes, full_lineage, output_taxa_file);
        log("Taxonomic information has been written to " + output_taxa_file);
        if (! output_seqs_file.empty()) {
            ResultIO::write_seqs(nx_file, indexes, output_seqs_file);
            log("Sequences have been written to " + output_seqs_file);
        }

        // destroy all nodes
        NodeIO::destroy(nodes);
        std::time_t end = std::time(nullptr);
        log("Finished. Total time elapsed: " + std::to_string(end - start) + "s");
    } catch (const std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        return 1;
    }
    return 0;
}
