#ifndef BTREE_H
#define BTREE_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <memory>
#include <cstdint>
#include <queue>
#include <vector>
#include <algorithm>
#include <assert.h>
// #include <nlohmann/json.hpp>

#define LEAF_NODE_CAP 5
#define INTERNAL_NODE_CAP 3
#define CEIL_CAP(maxCap) (((maxCap) + 1) / 2)

// Forward Declarations
class Node;
class InternalNode;
class LeafNode;

struct Record {
    uint64_t key;
    bool valid;
    
    Record(uint64_t k, bool v=true) : key(k), valid(v){}
    
    bool operator<(const Record& other) const {
        return key < other.key;
    }
    bool operator==(const Record& other) const {
        return key == other.key;
    }
};

struct InternalRecord {
    Record record;
    std::shared_ptr<Node> gtChildPtr;
};

class Node {
public:
    uint64_t id, curCap, maxCap, ceilCap; 
    std::vector<Record> elements;
    std::shared_ptr<InternalNode> parent;

    Node(uint64_t maxCapacity);

    virtual ~Node();
    virtual void insert(Record record) = 0;
    virtual void remove(uint64_t key) = 0;
    virtual void print() = 0;
    virtual bool isLeaf() = 0;
};

class InternalNode : 
    public Node,
    public std::enable_shared_from_this<InternalNode> // necessary for self mutating pushUp()
{
public:
    InternalNode(uint64_t maxCapacity);
    
    std::vector<InternalRecord> children;
    std::shared_ptr<Node> ltChildPtr; // asymmetric less than child
    
    void insert(Record record) override;
    void remove(uint64_t key) override;
    void print() override;
    bool isLeaf() override { return false; };
    
    void copyUp(std::shared_ptr<LeafNode> leaf);
    std::shared_ptr<InternalNode> pushUp();
    void merge();
    inline bool canInsert() { return (curCap < maxCap ? true : false); } 
    inline bool canRemove() { return (curCap > 1); }
    
    void addChild(InternalRecord child); //helper
    void removeChild(const InternalRecord& child); //helper
    std::shared_ptr<Node> findChildPtr(uint64_t key); // helper
    std::shared_ptr<Node> split();  
};

class LeafNode : 
    public Node,
    public std::enable_shared_from_this<LeafNode> // necessary for assigning self to splitNode previous leaf
{
public:
    LeafNode(uint64_t maxCapacity);

    std::shared_ptr<LeafNode> nextLeaf;
    std::shared_ptr<LeafNode> prevLeaf;

    void insert(Record record) override;
    void remove(uint64_t key) override;
    void merge();
    void print() override;
    bool isLeaf() override { return true; };

    inline bool canInsert() { return (curCap < ceilCap ? true : false); } 
    inline bool canRemove() { return (curCap >= ceilCap); }

    std::shared_ptr<LeafNode> mergeWithLeftNeighbor();
    std::shared_ptr<LeafNode> mergeWithRightNeighbor();
    std::shared_ptr<LeafNode> split();
};

class BTree {
public:
    BTree();
    ~BTree();
    uint64_t capacity; 
    std::shared_ptr<Node> rootNode;

    Record lookUp(uint64_t key);
    std::shared_ptr<LeafNode> findLeafNode(uint64_t key);
    void print();
    void insert(Record record);
    void remove(uint64_t key);
};

#endif