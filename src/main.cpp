#include "tests.h"
#include "btree.h"
#include "serialize.h"

const std::string helpMessage = R"(
B+ Tree Program Usage:

  Modes of Operation:
    1. Interactive Mode: Run in interactive mode 
       Usage: ./btree -i

    2. File Mode: Fill the tree with indexes from a file.
       Usage: ./btree -f <file_name>
       File Format: See tests

    3. Test Mode:
       Usage: ./btree -t <file_name>
)";

/**
 * Start program in testing mode with -t 
 */
void handleTests(const std::unique_ptr<BTree> &tree, std::ifstream &file) {
    
    std::string line;
    std::cout << "{" << std::endl; 
    if (file.is_open()) {
        if (getline(file, line)) {
            int numIndicies = std::stoi(line);
            
            std::streampos secondLinePos = file.tellg();
            std::cout << "\t\"testInsert\":" << testInsert(tree, numIndicies, file) << "," << std::endl; 
            file.seekg(secondLinePos);  

            std::cout << "\t\"testLookUp\":" << testLookUp(tree, numIndicies, file) << "," << std::endl;
            file.seekg(secondLinePos); 

            std::cout << "\t\"testLeafChain\":" << testLeafChain(tree) << "," << std::endl;
            std::cout << "\t\"testRemove\":" << testRemove(tree) << std::endl;
            
        }
    }
    std::cout << "}" << std::endl; 
}

/**
 * Start program in Interactive mode with -i 
 */
void handleInteractiveMode(const std::unique_ptr<BTree> &tree) {
    uint64_t index;
    while (1) {
        while (1) {
            scanf("%lu", &index);
            if (index == 0) {
                std::cout << "switch to delete mode" << std::endl; 
                break;
            }
            tree->insert(Record{index});
            tree->print(); 
        }
        while (1) {
            scanf("%lu", &index);
            if (index == 0) {
                std::cout << "switch to insert mode" << std::endl;
                break; 
            } 
            tree->remove(index);
            tree->print();
        }
    }
}

int main(int argc, char **argv) {
    
    std::unique_ptr<BTree> tree = std::make_unique<BTree>(); 
     
    if (argc >= 2) {
        std::string flag = argv[1];

        if (flag == "-t" && argc == 3) { 
            std::string file_name = argv[2];
            std::ifstream file(file_name);
            handleTests(tree, file);
        } else if (flag == "-h") {
            std::cerr << helpMessage;
        } else if (flag == "i") {
            handleInteractiveMode(tree); 
        } else {
            std::cerr << "Unknown flag: " << flag << std::endl;
        }
    } else {
        std::cerr << "Invalid argument configuration.\n" << helpMessage;
    }
    
    return 0;
}