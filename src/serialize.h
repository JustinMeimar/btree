#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "btree.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string serialize(const std::unique_ptr<BTree> &tree);

#endif