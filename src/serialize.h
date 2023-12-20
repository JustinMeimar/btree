#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "btree.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Serializer {
public:
    json btree; // serializer state 
    void serialzie(const std::unique_ptr<BTree> &tree);
};

#endif