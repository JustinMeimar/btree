#include <iostream>
#include <memory>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <assert.h>

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
    virtual void remove(Record record) = 0;
    virtual void print() = 0;
    virtual bool isLeaf() = 0;
};

class InternalNode : public Node {
public:
    InternalNode(uint64_t maxCapacity);

    std::shared_ptr<Node> split();

    void insert(Record record) override;
    void remove(Record record) override;
    void print() override;
    bool isLeaf() override { return false; };
    
    void copyUp(std::shared_ptr<LeafNode> leaf);
    void pushUp(std::shared_ptr<InternalNode> internal);
    void merge();
    std::shared_ptr<Node> findChildPtr(const Record& record); // helper
 
    std::vector<InternalRecord> children;
    std::shared_ptr<InternalRecord> parent;
    std::shared_ptr<Node> ltChildPtr; // asymmetric less than child
};

class LeafNode : public Node {
public:
    LeafNode(uint64_t maxCapacity);

    void insert(Record record) override;
    void remove(Record record) override;
    void print() override;
    bool isLeaf() override { return true; };

    inline bool canInsert() { return (curCap < ceilCap ? true : false); } 
    inline bool canRemove() { return (curCap > 1); }
   
    std::shared_ptr<LeafNode> split();
};

class BTree {
public:
    BTree();
    ~BTree();
    std::shared_ptr<Node> rootNode;

    void insert(Record record);
    void remove(Record record);
};

void printTree(const std::unique_ptr<BTree>& tree);