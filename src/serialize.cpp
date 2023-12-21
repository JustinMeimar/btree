#include "serialize.h"

std::string serializeLeaf(std::shared_ptr<LeafNode> leaf) {
    json node;
    // node["id"] = leaf->id;
    // node["records"] = 

    return "this is a string";
}

std::string serialize(const std::unique_ptr<BTree> &tree) {
    // walk the tree, serialzie into btree json object

    json tree_repr;

    // set up initail json feilds
    tree_repr["height"] = 5;
    tree_repr["total_capacity"] = 10; 
    tree_repr["nodes"] = "{}"; // nodes feild starts empty
    // tree["nodes"]["root_node"] = serialize(tree->rootNode);
    tree_repr["nodes"][""];
    
    // if (tree->capacity == 0) {
    //     std::cout << "Tree is empty" << std::endl;
    //     return "{}";
    // }

    // std::queue<std::shared_ptr<Node>> nodesQueue;
    // nodesQueue.push(tree->rootNode);

    // while (!nodesQueue.empty()) {
    //     std::shared_ptr<Node> currentNode = nodesQueue.front();
    //     nodesQueue.pop();

    //     serializeNode(currentNode); 

    //     if (!currentNode->isLeaf()) {
    //         std::shared_ptr<InternalNode> internalNode = std::dynamic_pointer_cast<InternalNode>(currentNode);
    //         if (internalNode->ltChildPtr) {
    //             nodesQueue.push(internalNode->ltChildPtr);
    //         }
    //         for (const auto& child : internalNode->children) {
    //             if (child.gtChildPtr) {
    //                 nodesQueue.push(child.gtChildPtr);
    //             }
    //         }
    //     }
    // }

    return "this is a std::string";
}

// std::string BTree::serializeToJSON()
// {
//     json btree;
//     btree["nodes"] = {1, 5, 6, 8};
//     std::string jsonString = btree.dump();
//     return jsonString;
// }

// std::string InternalNode::dumpJSON() {

//     json nodeJSON;

//     nodeJSON["id"] = id;
//     nodeJSON["parent"] = parent->id;
//     nodeJSON["cur_cap"] = curCap;

//     json childArray = json::array(); 

//     for (InternalRecord child: children) {    
//         childArray.push_back(child.dumpJSON());
//     }

//     nodeJSON["children"] = childArray;

//     return nodeJSON.dump();
// }

// std::string LeafNode::dumpJSON() {
//     json leafJSON;
//     leafJSON["id"] = id;
//     leafJSON["parent"] = parent->id;
//     leafJSON["cur_cap"] = parent->id;

//     json recordArray = json::array(); 

//     for (Record leafRecord: elements) {    
//         recordArray.push_back(leafRecord.dumpJSON());
//     }

//     leafJSON["records"] = recordArray;

//     return leafJSON.dump();
// }

// std::string InternalRecord::dumpJSON() {
//     json recordJSON;
//     recordJSON["key"] = record.key;
//     recordJSON["gt_child"] = gtChildPtr->id;

//     return recordJSON.dump();
// }

// std::string Record::dumpJSON() {
//     json recordJSON;
//     recordJSON["key"] = key;

//     return recordJSON.dump();
// }