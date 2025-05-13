#include "subnx.h"
#include "utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

template <typename T>
void assert_equal(const T& actual, const T& expected) {
    if (actual != expected) {
        ostringstream oss;
        oss << "Assertion failed: " << actual << " != " << expected;
        throw runtime_error(oss.str());
    }
}

template<typename T>
void assert_ptr_equal(const T* actual, const T* expected) {
    if (actual != expected) {
        ostringstream oss;
        oss << "Assertion failed: " << actual << " != " << expected;
        throw runtime_error(oss.str());
    }
}

template <typename T>
void assert_nullptr(const T* actual) {
    if (actual != nullptr) {
        ostringstream oss;
        oss << "Assertion failed: " << actual << " is not nullptr";
        throw runtime_error(oss.str());
    }
}

template <typename T>
void assert_not_nullptr(const T* actual) {
    if (actual == nullptr) {
        ostringstream oss;
        oss << "Assertion failed: is nullptr";
        throw runtime_error(oss.str());
    }
}

void assert_true(bool actual) {
    if (not actual) {
        throw runtime_error("not true");
    }
}

void assert_false(bool actual) {
    if (actual) {
        throw runtime_error("not false");
    }
}

class TestNameIO {
public:
    void test1() {
        cout << "Test NameIO::parse(const string&, unordered_map<string, string>&)" << endl;
        unordered_map<string, string> names;
        NameIO::parse("test-data/taxdmp/names.dmp", names);
        assert_equal(names.size(), size_t(18));
        assert_equal(names.at("1"), string("root"));
        assert_equal(names.at("147538"), string("Pezizomycotina"));
        assert_equal(names.at("1028384"), string("Glomerellales"));
    }
    void test2() {
        cout << "Test NameIO::parse(const string&, unordered_map<string, string>&) (empty)" << endl;
        unordered_map<string, string> names;
        NameIO::parse("test-data/taxdmp/nodes.dmp", names);
        assert_equal(names.size(), size_t(0));
    }
    void test() {
        cout << "Test NameIO" << endl;
        test1();
        test2();
    }
};

class TestNode {
public:
    void test_construct_method1() {
        cout << "Test Node::Node(string, string)" << endl;
        Node node("1", "root");
        assert_equal(node.id, string("1"));
        assert_equal(node.name, string("root"));
        assert_equal(node.rank, string(""));
        assert_nullptr(node.parent);
    }
    void test_construct_method2() {
        cout << "Test Node::Node(string, string, Node*, string)" << endl;
        Node parent("1", "root", nullptr, "no rank");
        Node node("131567", "cellular organisms", &parent, "cellular root");
        parent.append(&node);

        // parent
        assert_equal(parent.id, string("1"));
        assert_equal(parent.name, string("root"));
        assert_nullptr(parent.parent);
        assert_equal(parent.rank, string("no rank"));
        assert_equal(parent.children[0], &node);

        // node
        assert_equal(node.id, string("131567"));
        assert_equal(node.name, string("cellular organisms"));
        assert_equal(node.parent, &parent);
        assert_equal(node.rank, string("cellular root"));
    }
    void test_is_root() {
        cout << "Test Node::is_root()" << endl;
        Node parent("1", "root", nullptr, "no rank");
        Node node("131567", "cellular organisms", &parent, "cellular root");
        parent.append(&node);
        assert_equal(parent.is_root(), true);
        assert_equal(node.is_root(), false);
    }
    void test_append() {
        cout << "Test Node::append(Node*)" << endl;
        Node parent("1", "root", nullptr, "no rank");
        Node node("131567", "cellular organisms", &parent, "cellular root");
        assert_equal(parent.children.size(), size_t(0));
        parent.append(&node);
        assert_equal(parent.children.size(), size_t(1));
        parent.append(&node);
        assert_equal(parent.children.size(), size_t(2));
    }
    void test_trace() {
        cout << "Test Node::trace(vector<const Node*>)" << endl;
        Node t1("1", "root", nullptr, "no rank");
        Node t131567("131567", "cellular organisms", &t1, "cellular root");
        Node t2759("2759", "Eukaryota", &t131567, "domain");
        Node t33154("33154", "Opisthokonta", &t2759, "clade");
        Node t4751("4751", "Fungi", &t33154, "kingdom");
        Node t451864("451864", "Dikarya", &t4751, "subkingdom");
        Node t4890("4890", "Ascomycota", &t451864, "phylum");
        Node t716545("716545", "saccharomyceta", &t4890, "clade");
        Node t147538("147538", "Pezizomycotina", &t716545, "subphylum");
        Node t716546("716546", "leotiomyceta", &t147538, "clade");
        Node t715989("715989", "sordariomyceta", &t716546, "clade");
        Node t147550("147550", "Sordariomycetes", &t715989, "class");
        Node t222543("222543", "Hypocreomycetidae", &t147550, "subclass");
        Node t1028384("1028384", "Glomerellales", &t222543, "order");
        Node t681950("681950", "Glomerellaceae", &t1028384, "family");
        Node t5455("5455", "Colletotrichum", &t681950, "genus");
        Node t5462("5462", "Colletotrichum lagenaria", &t5455, "species");
        Node t27358("27358", "Colletotrichum coccodes", &t5455, "species");
        vector<const Node*> ancestors;
        t5462.trace(ancestors);
        assert_ptr_equal(ancestors[0], &t5462);
        assert_ptr_equal(ancestors[1], &t5455);
        assert_ptr_equal(ancestors[2], &t681950);
        assert_ptr_equal(ancestors[3], &t1028384);
        assert_ptr_equal(ancestors[4], &t222543);
        assert_ptr_equal(ancestors[5], &t147550);
        assert_ptr_equal(ancestors[6], &t715989);
        assert_ptr_equal(ancestors[7], &t716546);
        assert_ptr_equal(ancestors[8], &t147538);
        assert_ptr_equal(ancestors[9], &t716545);
        assert_ptr_equal(ancestors[10], &t4890);
        assert_ptr_equal(ancestors[11], &t451864);
        assert_ptr_equal(ancestors[12], &t4751);
        assert_ptr_equal(ancestors[13], &t33154);
        assert_ptr_equal(ancestors[14], &t2759);
        assert_ptr_equal(ancestors[15], &t131567);
        assert_ptr_equal(ancestors[16], &t1);
    }
    void test_expand() {
        cout << "Test Node::trace(vector<const Node*>)" << endl;
        Node t1("1", "root", nullptr, "no rank");
        Node t131567("131567", "cellular organisms", &t1, "cellular root");
        Node t2759("2759", "Eukaryota", &t131567, "domain");
        Node t33154("33154", "Opisthokonta", &t2759, "clade");
        Node t4751("4751", "Fungi", &t33154, "kingdom");
        Node t451864("451864", "Dikarya", &t4751, "subkingdom");
        Node t4890("4890", "Ascomycota", &t451864, "phylum");
        Node t716545("716545", "saccharomyceta", &t4890, "clade");
        Node t147538("147538", "Pezizomycotina", &t716545, "subphylum");
        Node t716546("716546", "leotiomyceta", &t147538, "clade");
        Node t715989("715989", "sordariomyceta", &t716546, "clade");
        Node t147550("147550", "Sordariomycetes", &t715989, "class");
        Node t222543("222543", "Hypocreomycetidae", &t147550, "subclass");
        Node t1028384("1028384", "Glomerellales", &t222543, "order");
        Node t681950("681950", "Glomerellaceae", &t1028384, "family");
        Node t5455("5455", "Colletotrichum", &t681950, "genus");
        Node t5462("5462", "Colletotrichum lagenaria", &t5455, "species");
        Node t27358("27358", "Colletotrichum coccodes", &t5455, "species");
        t1.append(&t131567);
        t131567.append(&t2759);
        t2759.append(&t33154);
        t33154.append(&t4751);
        t4751.append(&t451864);
        t451864.append(&t4890);
        t4890.append(&t716545);
        t716545.append(&t147538);
        t147538.append(&t716546);
        t716546.append(&t715989);
        t715989.append(&t147550);
        t147550.append(&t222543);
        t222543.append(&t1028384);
        t1028384.append(&t681950);
        t681950.append(&t5455);
        t5455.append(&t5462);
        t5455.append(&t27358);
        vector<const Node*> descendants;
        t1.expand(descendants);
        assert_ptr_equal(descendants[0], &t1);
        assert_ptr_equal(descendants[1], &t131567);
        assert_ptr_equal(descendants[2], &t2759);
        assert_ptr_equal(descendants[3], &t33154);
        assert_ptr_equal(descendants[4], &t4751);
        assert_ptr_equal(descendants[5], &t451864);
        assert_ptr_equal(descendants[6], &t4890);
        assert_ptr_equal(descendants[7], &t716545);
        assert_ptr_equal(descendants[8], &t147538);
        assert_ptr_equal(descendants[9], &t716546);
        assert_ptr_equal(descendants[10], &t715989);
        assert_ptr_equal(descendants[11], &t147550);
        assert_ptr_equal(descendants[12], &t222543);
        assert_ptr_equal(descendants[13], &t1028384);
        assert_ptr_equal(descendants[14], &t681950);
        assert_ptr_equal(descendants[15], &t5455);
        assert_ptr_equal(descendants[16], &t5462);
        assert_ptr_equal(descendants[17], &t27358);
    }
    void test_is_principal() {
        cout << "Test Node::is_principal()" << endl;
        Node t1("1", "root", nullptr, "no rank");
        assert_false(t1.is_principal());
        Node t131567("131567", "cellular organisms", &t1, "cellular root");
        assert_false(t131567.is_principal());
        Node t2759("2759", "Eukaryota", &t131567, "domain");
        assert_false(t2759.is_principal());
        Node t33154("33154", "Opisthokonta", &t2759, "clade");
        assert_false(t33154.is_principal());
        Node t4751("4751", "Fungi", &t33154, "kingdom");
        assert_true(t4751.is_principal());
        Node t451864("451864", "Dikarya", &t4751, "subkingdom");
        assert_false(t451864.is_principal());
        Node t4890("4890", "Ascomycota", &t451864, "phylum");
        assert_true(t4890.is_principal());
        Node t716545("716545", "saccharomyceta", &t4890, "clade");
        assert_false(t716545.is_principal());
        Node t147538("147538", "Pezizomycotina", &t716545, "subphylum");
        assert_false(t147538.is_principal());
        Node t716546("716546", "leotiomyceta", &t147538, "clade");
        assert_false(t716546.is_principal());
        Node t715989("715989", "sordariomyceta", &t716546, "clade");
        assert_false(t715989.is_principal());
        Node t147550("147550", "Sordariomycetes", &t715989, "class");
        assert_true(t147550.is_principal());
        Node t222543("222543", "Hypocreomycetidae", &t147550, "subclass");
        assert_false(t222543.is_principal());
        Node t1028384("1028384", "Glomerellales", &t222543, "order");
        assert_true(t1028384.is_principal());
        Node t681950("681950", "Glomerellaceae", &t1028384, "family");
        assert_true(t681950.is_principal());
        Node t5455("5455", "Colletotrichum", &t681950, "genus");
        assert_true(t5455.is_principal());
        Node t5462("5462", "Colletotrichum lagenaria", &t5455, "species");
        assert_true(t5462.is_principal());
        Node t27358("27358", "Colletotrichum coccodes", &t5455, "species");
        assert_true(t27358.is_principal());
    }
    void test_str() {
        cout << "Test Node::str()" << endl;
        Node t1("1", "root", nullptr, "no rank");
        assert_equal(t1.str(), string("no_rank__root"));
        assert_equal(t1.str(true), string("no_rank__root"));
        assert_equal(t1.str(false), string("no_rank__root"));
        Node t5462("5462", "Colletotrichum lagenaria", nullptr, "species");
        assert_equal(t5462.str(), string("s__Colletotrichum_lagenaria"));
        assert_equal(t5462.str(true), string("s__Colletotrichum_lagenaria"));
        assert_equal(t5462.str(false), string("species__Colletotrichum_lagenaria"));
    }
    void test_lineage() {
        cout << "Test Node::lineage()" << endl;
        Node t1("1", "root", nullptr, "no rank");
        Node t131567("131567", "cellular organisms", &t1, "cellular root");
        Node t2759("2759", "Eukaryota", &t131567, "domain");
        Node t33154("33154", "Opisthokonta", &t2759, "clade");
        Node t4751("4751", "Fungi", &t33154, "kingdom");
        Node t451864("451864", "Dikarya", &t4751, "subkingdom");
        Node t4890("4890", "Ascomycota", &t451864, "phylum");
        Node t716545("716545", "saccharomyceta", &t4890, "clade");
        Node t147538("147538", "Pezizomycotina", &t716545, "subphylum");
        Node t716546("716546", "leotiomyceta", &t147538, "clade");
        Node t715989("715989", "sordariomyceta", &t716546, "clade");
        Node t147550("147550", "Sordariomycetes", &t715989, "class");
        Node t222543("222543", "Hypocreomycetidae", &t147550, "subclass");
        Node t1028384("1028384", "Glomerellales", &t222543, "order");
        Node t681950("681950", "Glomerellaceae", &t1028384, "family");
        Node t5455("5455", "Colletotrichum", &t681950, "genus");
        Node t5462("5462", "Colletotrichum lagenaria", &t5455, "species");
        Node t27358("27358", "Colletotrichum coccodes", &t5455, "species");
        string lineage = "k__Fungi; p__Ascomycota; c__Sordariomycetes; o__Glomerellales; f__Glomerellaceae; g__Colletotrichum; s__Colletotrichum_lagenaria";
        string full_lineage = "no_rank__root; cellular_root__cellular_organisms; domain__Eukaryota; clade__Opisthokonta; kingdom__Fungi; subkingdom__Dikarya; phylum__Ascomycota; clade__saccharomyceta; subphylum__Pezizomycotina; clade__leotiomyceta; clade__sordariomyceta; class__Sordariomycetes; subclass__Hypocreomycetidae; order__Glomerellales; family__Glomerellaceae; genus__Colletotrichum; species__Colletotrichum_lagenaria";
        assert_equal(t5462.lineage(), lineage);
        assert_equal(t5462.lineage(true), lineage);
        assert_equal(t5462.lineage(false), full_lineage);
    }
    void test() {
        cout << "Test Node" << endl;
        test_construct_method1();
        test_construct_method2();
        test_append();
        test_trace();
        test_expand();
        test_str();
        test_lineage();
    }
};

class TestNodeIO {
public:
    void test_parse1() {
        cout << "Test NodeIO::parse(const string&, const unordered_map<string, string>&, unordered_map<string, Node*>&)" << endl;
        unordered_map<string, string> names;
        NameIO::parse("test-data/taxdmp/names.dmp", names);

        unordered_map<string, Node*> nodes;
        NodeIO::parse("test-data/taxdmp/nodes.dmp", names, nodes);

        assert_equal(nodes.size(), size_t(18));
        Node* t1 = nodes.at("1");

        // 131567
        Node* t131567 = nodes.at("131567");
        assert_ptr_equal(t131567->parent, t1);
        assert_equal(t1->children.size(), size_t(1));
        assert_ptr_equal(t1->children[0], t131567);

        // 2759
        Node* t2759 = nodes.at("2759");
        assert_ptr_equal(t2759->parent, t131567);
        assert_equal(t131567->children.size(), size_t(1));
        assert_ptr_equal(t131567->children[0], t2759);

        // 33154
        Node* t33154 = nodes.at("33154");
        assert_ptr_equal(t33154->parent, t2759);
        assert_equal(t2759->children.size(), size_t(1));
        assert_ptr_equal(t2759->children[0], t33154);

        // 4751
        Node* t4751 = nodes.at("4751");
        assert_ptr_equal(t4751->parent, t33154);
        assert_equal(t33154->children.size(), size_t(1));
        assert_ptr_equal(t33154->children[0], t4751);

        // 451864
        Node* t451864 = nodes.at("451864");
        assert_ptr_equal(t451864->parent, t4751);
        assert_equal(t4751->children.size(), size_t(1));
        assert_ptr_equal(t4751->children[0], t451864);

        // 4890
        Node* t4890 = nodes.at("4890");
        assert_ptr_equal(t4890->parent, t451864);
        assert_equal(t451864->children.size(), size_t(1));
        assert_ptr_equal(t451864->children[0], t4890);

        // 716545
        Node* t716545 = nodes.at("716545");
        assert_ptr_equal(t716545->parent, t4890);
        assert_equal(t4890->children.size(), size_t(1));
        assert_ptr_equal(t4890->children[0], t716545);

        // 147538
        Node* t147538 = nodes.at("147538");
        assert_ptr_equal(t147538->parent, t716545);
        assert_equal(t716545->children.size(), size_t(1));
        assert_ptr_equal(t716545->children[0], t147538);

        // 716546
        Node* t716546 = nodes.at("716546");
        assert_ptr_equal(t716546->parent, t147538);
        assert_equal(t147538->children.size(), size_t(1));
        assert_ptr_equal(t147538->children[0], t716546);

        // 715989
        Node* t715989 = nodes.at("715989");
        assert_ptr_equal(t715989->parent, t716546);
        assert_equal(t716546->children.size(), size_t(1));
        assert_ptr_equal(t716546->children[0], t715989);

        // 147550
        Node* t147550 = nodes.at("147550");
        assert_ptr_equal(t147550->parent, t715989);
        assert_equal(t715989->children.size(), size_t(1));
        assert_ptr_equal(t715989->children[0], t147550);

        // t222543
        Node* t222543 = nodes.at("222543");
        assert_ptr_equal(t222543->parent, t147550);
        assert_equal(t147550->children.size(), size_t(1));
        assert_ptr_equal(t147550->children[0], t222543);

        // 1028384
        Node* t1028384 = nodes.at("1028384");
        assert_ptr_equal(t1028384->parent, t222543);
        assert_equal(t222543->children.size(), size_t(1));
        assert_ptr_equal(t222543->children[0], t1028384);

        // 681950
        Node* t681950 = nodes.at("681950");
        assert_ptr_equal(t681950->parent, t1028384);
        assert_equal(t1028384->children.size(), size_t(1));
        assert_ptr_equal(t1028384->children[0], t681950);

        // 5455
        Node* t5455 = nodes.at("5455");
        assert_ptr_equal(t5455->parent, t681950);
        assert_equal(t681950->children.size(), size_t(1));
        assert_ptr_equal(t681950->children[0], t5455);

        // 5462
        Node* t5462 = nodes.at("5462");
        assert_ptr_equal(t5462->parent, t5455);
        assert_equal(t5455->children.size(), size_t(2));
        assert_ptr_equal(t5455->children[0], t5462);

        // 27358
        Node* t27358 = nodes.at("27358");
        assert_ptr_equal(t27358->parent, t5455);
        assert_ptr_equal(t5455->children[1], t27358);
    }
    void test_parse2() {
        cout << "Test NodeIO::parse(const string&, const unordered_map<string, string>&, unordered_map<string, Node*>&) (empty)" << endl;
        unordered_map<string, string> names;
        NameIO::parse("test-data/taxdmp/names.dmp", names);

        unordered_map<string, Node*> nodes;
        NodeIO::parse("test-data/taxdmp/names.dmp", names, nodes);

        assert_equal(nodes.size(), size_t(0));
    }
    void test_destroy() {
        cout << "Test NodeIO::destroy()" << endl;
        unordered_map<string, string> names;
        NameIO::parse("test-data/taxdmp/names.dmp", names);

        unordered_map<string, Node*> nodes;
        NodeIO::parse("test-data/taxdmp/nodes.dmp", names, nodes);

        assert_not_nullptr(nodes.at("1"));
        NodeIO::destroy(nodes);
        assert_nullptr(nodes.at("1"));
    }
    void test() {
        cout << "Test NodeIO" << endl;
        test_parse1();
        test_parse2();
        test_destroy();
    }
};

class TestAccession2TaxIdIO {
public:
    void test() {
        cout << "Test Accession2TaxIdIO" << endl;
        unordered_map<string, string> names;
        NameIO::parse("test-data/taxdmp/names.dmp", names);
        unordered_map<string, Node*> nodes;
        NodeIO::parse("test-data/taxdmp/nodes.dmp", names, nodes);
        Node* node = nodes.at("5455");
        vector<const Node*> descendants;
        node->expand(descendants);
        unordered_set<string> taxids;
        for (const Node* descendant : descendants) {
            taxids.insert(descendant->id);
        }
        unordered_map<string, string> accession2taxid;
        Accession2TaxIdIO::parse("test-data/nucl_gb.accession2taxid", taxids, accession2taxid);
        assert_equal(accession2taxid.size(), size_t(2));

        assert_true(accession2taxid.find("HG799543") != accession2taxid.end());
        assert_equal(accession2taxid["HG799543"], string("27358"));

        assert_true(accession2taxid.find("ON631770") != accession2taxid.end());
        assert_equal(accession2taxid["ON631770"], string("5462"));

        assert_false(accession2taxid.find("X17276") != accession2taxid.end());
    }
};

class TestIndexIO {
public:
    void compare_index(const Index& index1, const Index& index2) {
        assert_equal(index1.accession, index2.accession);
        assert_equal(index1.accession_version, index2.accession_version);
        assert_equal(index1.pos, index2.pos);
        assert_equal(index1.length, index2.length);
    }
    void test_create() {
        cout << "Test IndexIO::create(const string&, const string&)" << endl;
        IndexIO::create("test-data/nt", "test-data/nt.fai");
        vector<Index> actual_indexes;
        unordered_set<string> accessions = {"X17276", "HG799543", "ON631770"};
        IndexIO::parse("test-data/nt.fai", accessions, actual_indexes);
        vector<Index> expected_indexes = {
            Index("X17276", "X17276.1", 0, 601),
            Index("HG799543", "HG799543.1", 601, 396),
            Index("ON631770", "ON631770.1", 997, 797)
        };
        compare_index(actual_indexes[0], expected_indexes[0]);
        compare_index(actual_indexes[1], expected_indexes[1]);
        compare_index(actual_indexes[2], expected_indexes[2]);
    }
    void test_parse() {
        cout << "Test IndexIO::parse(const string&, const unordered_set<string>&, vector<Index>&)" << endl;
        vector<Index> actual_indexes;
        unordered_set<string> accessions = {"HG799543", "ON631770"};
        IndexIO::parse("test-data/nt.fai", accessions, actual_indexes);
        vector<Index> expected_indexes = {
            Index("HG799543", "HG799543.1", 601, 396),
            Index("ON631770", "ON631770.1", 997, 797)
        };
        assert_equal(actual_indexes.size(), size_t(2));
        compare_index(actual_indexes[0], expected_indexes[0]);
        compare_index(actual_indexes[1], expected_indexes[1]);
    }
    void test() {
        cout << "Test IndexIO" << endl;
        test_create();
        test_parse();
    }
};

int main() {
    try {
        TestNameIO test_name_io{};
        test_name_io.test();
        TestNode test_node{};
        test_node.test();
        TestNodeIO test_node_io{};
        test_node_io.test();
        TestAccession2TaxIdIO test_accession2taxid{};
        test_accession2taxid.test();
        TestIndexIO test_index_io{};
        test_index_io.test();
    } catch (const exception& exc) {
        cerr << exc.what() << endl;
    }
    return 0;
}