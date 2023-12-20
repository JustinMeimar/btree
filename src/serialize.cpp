#include "serialize.h"

void Serializer::serialzie(const std::unique_ptr<BTree> &tree) {
    // walk the tree, serialzie into btree json object
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