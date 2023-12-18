#ifndef BTREE_H
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
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#define LEAF_NODE_CAP 3
#define INTERNAL_NODE_CAP 3
#define CEIL_CAP(maxCapacity) (((maxCapacity) + 1) / 2)

// Forward Declarations
class Node;
class InternalNode;
class LeafNode;

struct Record {
    uint64_t key;

    bool operator<(const Record& other) const {
        return key < other.key;
    }

    bool operator==(const Record& other) const {
        return key == other.key;
    }

    std::string dumpJSON();
};

struct InternalRecord {
    Record record;
    std::shared_ptr<Node> gtChildPtr;

    std::string dumpJSON();
};


class Node {
public:
    uint64_t id, curCap, maxCap, ceilCap; 
    std::vector<Record> elements;
    std::shared_ptr<InternalNode> parent;

    Node(uint64_t maxCapacity);

    virtual ~Node();

    virtual void insert(Record record) = 0;
    virtual void remove(Record record) = 0;
    virtual void print() = 0;
    virtual bool isLeaf() = 0;
    virtual std::string dumpJSON() = 0;
};

class InternalNode : 
    public Node,
    public std::enable_shared_from_this<InternalNode> // necessary for self mutating pushUp()
{
public:
    InternalNode(uint64_t maxCapacity);

    std::shared_ptr<Node> split();

    void insert(Record record) override;
    void remove(Record record) override;
    void print() override;
    bool isLeaf() override { return false; };
    std::string dumpJSON() override;
    
    void copyUp(std::shared_ptr<LeafNode> leaf);
    // void pushUp();
    std::shared_ptr<InternalNode> pushUp();
    void merge();
    inline bool canInsert() { return (curCap < maxCap ? true : false); } 
    inline bool canRemove() { return (curCap > 1); }
    
    void addChild(InternalRecord child); //helper
    void removeChild(const InternalRecord& child); //helper
    // std::shared_ptr<Node> findChildPtr(const Record& record); // helper
    std::shared_ptr<Node> findChildPtr(uint64_t key); // helper
 
    std::vector<InternalRecord> children;
    std::shared_ptr<Node> ltChildPtr; // asymmetric less than child
};

class LeafNode : public Node {
public:
    LeafNode(uint64_t maxCapacity);

    void insert(Record record) override;
    void remove(Record record) override;
    void print() override;
    bool isLeaf() override { return true; };
    std::string dumpJSON() override;

    inline bool canInsert() { return (curCap < ceilCap ? true : false); } 
    inline bool canRemove() { return (curCap > 1); }
   
    std::shared_ptr<LeafNode> split();
};

class BTree {
public:
    BTree();
    ~BTree();
    uint64_t capacity; 
    std::shared_ptr<Node> rootNode;

    std::string serializeToJSON(); 
    Record lookUp(uint64_t key);
    void print();
    void insert(Record record);
    void remove(Record record);
};

#endif