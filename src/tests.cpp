#include "tests.h"
/**
 * Fails when insertion can not complete. Correctness of insertion validated in later tests. 
*/
bool testInsert(const std::unique_ptr<BTree> &tree, int numIndicies, std::ifstream &file) {

    try {
        std::string line; 
        while (getline(file, line)) {
            uint64_t index = static_cast<uint64_t>(std::stoul(line));
            if (index) {
                tree->insert( Record {index});
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return false;
    }   
    return true;     
}

/**
 * Assert that each index we inserted into the tree can be found by searching from the
 * root of the tree.
*/
bool testLookUp(const std::unique_ptr<BTree> &tree, int numIndicies, std::ifstream &file) {
    std::string line;
    for (int i =1; i < numIndicies; i++) {
        if (!tree->lookUp(i).valid) {
            return false;
        }
    }
    if (tree->lookUp(numIndicies++).valid) {
        return false;
    }

    return true;    
}

/*
 * Every index inserted into a B+ Tree is unique. Regardless of order,
 * once N insertions are made the indicices in adjacent leaf nodes should
 * be monotonically increasing. This is what makes B+ Trees good for range queries.
*/
bool testLeafChain(const std::unique_ptr<BTree> &tree) {
    /**
     * TODO: implement this test
     *
     * */
    std::shared_ptr<LeafNode> curLeafNode; 
    std::shared_ptr<Node> curNode = tree->rootNode;
    while (curNode) { 
        auto internalNode = std::dynamic_pointer_cast<InternalNode>(curNode);
        if (internalNode) {
            curNode = internalNode->ltChildPtr;
        } else {
            curLeafNode = std::dynamic_pointer_cast<LeafNode>(curNode);
            break;
        }
    }

    static uint64_t prevKey = 0;
    while (curLeafNode) {
        for (auto record : curLeafNode->elements) {
            if (record.key != (prevKey + 1)) {
                return false;
            }
            prevKey = record.key; 
        }
        curLeafNode = curLeafNode->nextLeaf;
    }

    return true;    
}

/** 
 * TODO: actually implement remove...
*/
bool testRemove(const std::unique_ptr<BTree> &tree) {
    return false;    
}