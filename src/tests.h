#ifndef TESTS_H
#define TESTS_H

#include "btree.h"
/**
 * Fails when insertion can not complete. Correctness of insertion validated in later tests. 
*/
bool testInsert(const std::unique_ptr<BTree> &tree, int numIndicies, std::ifstream &file);

/**
 * Assert that each index we inserted into the tree can be found by searching from the
 * root of the tree.
*/
bool testLookUp(const std::unique_ptr<BTree> &tree, int numIndicies, std::ifstream &file);

/*
 * Every index inserted into a B+ Tree is unique. Regardless of order,
 * once N insertions are made the indicices in adjacent leaf nodes should
 * be monotonically increasing. This is what makes B+ Trees good for range queries.
*/
bool testLeafChain(const std::unique_ptr<BTree> &tree);

/** 
 * TODO: actually implement remove...
*/
bool testRemove(const std::unique_ptr<BTree> &tree);

#endif