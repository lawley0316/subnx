# subnx: one-click extraction of nt/nr sub-databases

![subnx.gif](subnx.gif)

## Goal

A few days ago, a client approached me wanting to create a sequence database for the Colletotrichum genus. After some research, I found that there aren't many good tools available for this purpose. While [taxonkit](https://github.com/shenwei356/taxonkit) can accomplish this, it is a general-purpose tool with many features, making it overly cumbersome for the specific task of constructing nt/nr sub-databases.

Taking the construction of a Colletotrichum nt sub-database as an example, our goal is to obtain sequences for all taxa under the Colletotrichum genus. Specifically, we aim to produce the following two files:

(1) Sequences File

```
>Z18982.1 C.capsici encoding 28S ribosomal RNA
GCATGCCTGTTCGAGCGTCATTTCAACCCTCAAGCTCTGCTTGGTGTTGGGGCTCTACGGTTGACGTAGGCCCTTAAAGG
TAGTGGCGGACCCTCTCGGAGCCTCCTTTGCGTAGTAACATTTCGTCTCGCATTGGGATTCGGAGGGACTCTAGCCGTAA
AACCCCCAATTTTACTAAGGTTGACCTCGGATCAGGTAGGAATACCCGCTGAACTTAAGCATATCAATAAGCGGAGGAAA
AGAAACCAACAGGGATTGCCCCAGTAACGGCGAGTGAAGCGGCAACAGCTCAAATTTGAAATCTGGNCCCTAGGCCCGAG
TTGTAATTTGCAGAGGATGCTTTTGGCGCGGTGCCTTCCGAGTTCCCTGGAACGGGACGCCATAGAGGGTGAGAGCCCCG
TACGGTTGGACACCAAGTCTTTGTAAAGCTCCTTCGACGAGTCGAGTAGTTTGGGAATGCTGCTCAAAATGGGAGGTATA
TTTCTTCTAAAGCTAAATATTGGCCAGAGACCGATAGCGCACAAGTAGAGTGATCGAAAGATGAAAAGCACTTTGAAAAG
AGGGTTAAACAGCACGTGAAATTGTTAAAAGGGAAGCGCTTGTGACCAGACTTGCGTCCGGTGAATCACCCAGCTCTCGC
GGCTGGGGCATTTTGCCGGCTCAGGCCAGCATCAGCTTGCCGTCGGGGACAAAAGCTTCGGGAACGTAGCTCTCTTCGGG
GAGTGTTATAGCCCGTTGCATAATACCTTCGGTGGGCTGAGGTACGCCGTCCGCAAGGATGCTGGCGTAATGGTCATCAG
CGACCCGTCTTGAAACACGGACCAAGGAGTCAACCTTATGTGCGAGTGTTTGGGTGTTAAACCCCTACGCGTAATGAAAG
T
```

(2) Sequence ID to Taxonomy Mapping File

```
Z18982.1        k__Fungi; p__Ascomycota; c__Sordariomycetes; o__Glomerellales; f__Glomerellaceae; g__Colletotrichum; s__Colletotrichum_truncatum
```

This file contains one entry per sequence, with two columns per line:

* First column: Sequence ID (e.g., Z18982.1);
* Second column: Taxonomic lineage, where each rank is separated by ; (e.g., kingdom, phylum, class, etc.).

## Key Files

To construct an nt/nr sub-database, the first step is to download the complete nt/nr databases.

**nt/nr**

* nt database: [ftp://ftp.ncbi.nlm.nih.gov/blast/db/FASTA/nt.gz](ftp://ftp.ncbi.nlm.nih.gov/blast/db/FASTA/nt.gz)
* nr database: [ftp://ftp.ncbi.nlm.nih.gov/blast/db/FASTA/nr.gz](ftp://ftp.ncbi.nlm.nih.gov/blast/db/FASTA/nr.gz)

As of April 15, 2025, the nt database is 378 GB, and the nr database is 186 GB. Due to their large size, it is recommended to use Aspera Connect for faster downloads.

These files are in FASTA format, containing sequence IDs and sequence data. However, two key questions arise:

* How do we determine which sequences belong to a specific taxon (e.g., Colletotrichum)?
* How do we confirm whether a given species falls under Colletotrichum?

This is where the NCBI Taxonomy Database comes into play.

The NCBI Taxonomy Database is one of the most widely used taxonomic databases. Maintained by the National Center for Biotechnology Information (NCBI), it provides a hierarchical classification system for all known biological species.

Each taxon is assigned a unique identifier (TaxID) and includes taxonomic information such as family, genus, and species. The database is updated daily and made available for download at: [ftp://ftp.ncbi.nlm.nih.gov/pub/taxonomy](ftp://ftp.ncbi.nlm.nih.gov/pub/taxonomy).

Several key files help solve our problem:

**accession2taxid Files**

NCBI sequence IDs are called accessions, and the accession2taxid files provide a mapping between sequence IDs and TaxIDs. Each entry contains four columns:

* accession – Sequence ID (e.g., AAAA00000000)
* accession.version – Versioned sequence ID (e.g., AAAA00000000.2)
* taxid – Taxonomic ID (e.g., 39946)
* gi – GenInfo Identifier (deprecated, but still present)

Two versions are available:

* Nucleotide sequences: [ftp://ftp.ncbi.nlm.nih.gov/pub/taxonomy/accession2taxid/nucl_gb.accession2taxid.gz](ftp://ftp.ncbi.nlm.nih.gov/pub/taxonomy/accession2taxid/nucl_gb.accession2taxid.gz)
* Protein sequences: [ftp://ftp.ncbi.nlm.nih.gov/pub/taxonomy/accession2taxid/prot.accession2taxid.gz](ftp://ftp.ncbi.nlm.nih.gov/pub/taxonomy/accession2taxid/prot.accession2taxid.gz)

**nodes.dmp File**

Download: [ftp://ftp.ncbi.nlm.nih.gov/pub/taxonomy/taxdmp.zip](ftp://ftp.ncbi.nlm.nih.gov/pub/taxonomy/taxdmp.zip) (inside the ZIP file)

This is a tab-delimited file (separator: `\t|\t`, line ending: `\t|\n`) with 13 columns, the most important being:

* tax_id – Unique identifier for the taxon.
* parent tax_id – Parent taxon’s TaxID.
* rank – Taxonomic rank (e.g., species, genus, family).

This file allows us to reconstruct the taxonomic tree.

**names.dmp File**

Also inside taxdmp.zip, this file maps TaxIDs to taxon names. Key columns:

* tax_id – Taxonomic identifier.
* name_txt – Name of the taxon.
* name_class – Type of name (e.g., scientific name, synonym, common name).

## Implementation Approach

With these files, constructing a sub-database becomes straightforward. The pseudocode is as follows:

```pseudocode
# Step 1: Build the taxonomic tree using names.dmp and nodes.dmp  
tree = build_species_tree('names.dmp', 'nodes.dmp')  

# Step 2: Retrieve all TaxIDs under Colletotrichum (TaxID: 5455)  
taxids = get_taxids_under('5455', tree)  

# Step 3: Find all accessions linked to these TaxIDs  
accessions = find_accessions('nucl_gb.accession2taxid', taxids)  

# Step 4: Build an index for fast sequence extraction (since nt is huge)  
indexes = build_index('nt')  

# Step 5: Output taxonomic info and sequences  
for idx in indexes:  
    if idx.accession in accessions:  
        taxid = accessions[idx.accession]  
        taxon = tree.get(taxid)  
        write_taxon(accession, taxon.lineage())  
        seq = extract_seq('nt', idx.pos, idx.size)  
        write_seq(seq)  
```

This tool will efficiently extract and organize sequences from large NCBI databases, providing researchers with a streamlined way to work with specific taxonomic groups. 

## Installation

```shell
cd /usr/local  # Preparing to install in /usr/local
mkdir -p cellar/subnx/1.0.0  # Creating installation directory
curl -L -o cellar/subnx/1.0.0/subnx https://github.com/lawley0316/subnx/releases/download/v1.0.0/subnx-linux-x64  # Downloading executable
chmod a+x cellar/subnx/1.0.0/subnx  # Making executable for all users
cd bin && ln -s ../cellar/subnx/1.0.0/subnx .  # Adding executable to PATH
```

## Usage

**Downloading Required Data**

* Download and decompress taxdmp.zip

```shell
~/.aspera/connect/bin/ascp -i ~/.aspera/connect/etc/aspera_tokenauth_id_rsa -T -k 1 -l 100m anonftp@ftp.ncbi.nlm.nih.gov:/pub/taxonomy/taxdmp.zip .  # Download
mkdir taxdmp  # Create directory
unzip -d taxdmp taxdmp.zip  # Extract
```

* Download and decompress accession2taxid data

```shell
~/.aspera/connect/bin/ascp -i ~/.aspera/connect/etc/aspera_tokenauth_id_rsa -T -k 1 -l 100m anonftp@ftp.ncbi.nlm.nih.gov:/pub/taxonomy/accession2taxid/nucl_gb.accession2taxid.gz .  # Download
gzip -d -k nucl_gb.accession2taxid.gz  # Extract
```

* Download and decompress nt database

```shell
~/.aspera/connect/bin/ascp -i ~/.aspera/connect/etc/aspera_tokenauth_id_rsa -T -k 1 -l 100m anonftp@ftp.ncbi.nlm.nih.gov:/blast/db/FASTA/nt.gz .  # Download
gzip -d -k nt.gz  # Extract
```

**Constructing Colletotrichum Sub-Database**

```shell
subnx -i 5455 -t taxdmp -a nucl_gb.accession2taxid -n nt -s seqs.fa -T tax.txt
```

Parameter Explanation:

* `-i`: TaxID (e.g., 5455 for Colletotrichum)
* `-t`: Decompressed taxdmp directory
* `-a`: Decompressed accession2taxid file
* `-n`: Decompressed nt/nr file
* `-f`: Output full lineage information (default: principal ranks only)
* `-s`: Output sequence file (optional)
* `-T`: Output taxonomic information file