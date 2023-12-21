#include "tests.h"
#include "btree.h"
#include "serialize.h"
#include <chrono>
#include <nlohmann/json.hpp>

const std::string helpMessage = R"(
B+ Tree Program Usage:

  Modes of Operation:
    1. Interactive Mode: Run in interactive mode 
       Usage: ./btree -i

    2. Run Benchmarks: Fill the tree with indexes from a file.
       Usage: ./btree -b <file_name>
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

void runBenchmarks(const std::unique_ptr<BTree> &tree, std::ifstream &file) {
    std::string line;
    if (file.is_open()) {
        if (getline(file, line)) {
            int numIndicies = std::stoi(line);
            std::streampos secondLinePos = file.tellg();


            // Measure time for testInsert
            auto start = std::chrono::high_resolution_clock::now();
            testInsert(tree, numIndicies, file);
            auto stop = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> insert_duration = stop - start;
            std::cout << "Insert benchmark took " << insert_duration.count() << " milliseconds.\n";
            file.seekg(secondLinePos);

            // Measure time for testLookUp
            // start = std::chrono::high_resolution_clock::now();
            // testLookUp(tree, numIndicies, file);
            // stop = std::chrono::high_resolution_clock::now();
            // std::chrono::duration<double, std::milli> lookup_duration = stop - start;
            // std::cout << "LookUp benchmark took " << lookup_duration.count() << " milliseconds.\n";
            // file.seekg(secondLinePos);

            // Measure time for testLeafChain
            start = std::chrono::high_resolution_clock::now();
            testLeafChain(tree);
            stop = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> leafchain_duration = stop - start;
            std::cout << "LeafChain benchmark took " << leafchain_duration.count() << " milliseconds.\n";
 
            testRemove(tree); 
        }
    }
}

/**
 * Start program in Interactive mode with -i 
 */
using json = nlohmann::json;


void handleInteractiveMode(const std::unique_ptr<BTree> &tree) {
    json msg;
    std::string line;

    while (std::getline(std::cin, line)) {
        auto result = json::parse(line, nullptr, false);
        if (result.is_discarded()) {
            std::cout << "{\"error\": \"invalid json\"}" << std::endl;
            continue;  // Skip the rest of the loop and wait for next input
        } else {
            msg = result;
            if (msg.contains("insert")) {
                int value = msg["insert"];
                tree->insert(value); 
            } else if (msg.contains("command")) {
                std::string command = msg["command"];
                if (command == "json_state") {
                    std::cout << serialize(tree) << std::endl;
                }
            } else {
                std::cout << "{\"error\": \"unrecognized command\"}" << std::endl;
            }
        }
        msg.clear();
    }
}

int main(int argc, char **argv) {
    
    std::unique_ptr<BTree> tree = std::make_unique<BTree>(); 
     
    if (argc >= 2) {
        std::string flag = argv[1];
        
        if (flag == "-i") {
            handleInteractiveMode(tree); 
        } else if (flag == "-h") {
            std::cerr << helpMessage;
        } else if (flag == "-t" && argc == 3) { 
            std::string file_name = argv[2];
            std::ifstream file(file_name);
            handleTests(tree, file);
        }  else if (flag == "-b" && argc == 3) {
            std::string file_name = argv[2];
            std::ifstream file(file_name);
            runBenchmarks(tree, file);
        } else {
            std::cerr << "Unknown flag: " << flag << std::endl;
        }
    } else {
        std::cerr << "Invalid argument configuration.\n" << helpMessage;
    }
    
    return 0;
}