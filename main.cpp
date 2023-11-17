#include <iostream>
#include <memory>
#include <cstdint>
#include <vector>


struct Element
{
    uint64_t id;
    uint64_t val;

    bool operator<(const Element& other) const {
        return val < other.val;
    }
};

void printVec(const std::vector<Element>& vec)
{
    for (const Element &val : vec)
    {
        printf("%ld,", val.val);
    }
    std::cout << std::endl;
}

struct Node 
{
    std::vector<Element> nodeEl;
    uint64_t curCap;
    uint64_t maxCap;
    
    Node (uint64_t maxCap) : maxCap(maxCap) {}
    virtual ~Node () {}

    virtual bool insert(Element el) = 0;
    virtual bool remove(Element el) = 0;
    virtual std::shared_ptr<Node> split(Element insertEl) = 0;

    void insertSorted(std::vector<Element>& vec, const Element& el) {
        auto it = std::lower_bound(vec.begin(), vec.end(), el);
        vec.insert(it, el);
    } 

};

struct InternalNode : Node
{
    std::shared_ptr<InternalNode> parent;
    std::vector<std::shared_ptr<Node>> children;

    InternalNode(uint64_t maxCapacity) : Node(maxCapacity) {}

    std::shared_ptr<Node> split(Element insertEl) override
    {
        std::shared_ptr<InternalNode> splitNode = std::make_shared<InternalNode>(3);
        
        return splitNode;
    }

    bool insert(Element insertVal) {

        if (nodeEl.size() == 0)
            printf("first insert");
        printf("insert called\n");
        for (Element val: nodeEl)
        {
            printf("[%d] ", val.val);
            // if (insertVal )
        }
        return true;
    }

    bool remove(Element value) {
        return true;
    }
};

struct LeafNode : Node
{
    std::shared_ptr<InternalNode> parent;
    uint64_t ceilCap;

    LeafNode(uint64_t maxCapacity) : Node(maxCapacity) {
        ceilCap = ((maxCapacity + 2 -1) / 2);
    }

    std::shared_ptr<Node> split(Element insertEl) override
    {
        std::shared_ptr<LeafNode> splitNode = std::make_shared<LeafNode>(3);

        // TODO: split the vector in two, copy the RHS into splitNode, return split node.

        return splitNode;
    }

    bool insert(Element value) 
    {   
        if (curCap < ceilCap) {
            printf("insert");
            insertSorted(nodeEl, value);
            printVec(nodeEl);
            
            curCap++;
            return true;
        } 
        return false; 
    }
    bool remove(Element value) { return true; }
};

struct RootNode
{
    std::shared_ptr<Node> sourceNode;

    RootNode() {
        sourceNode = std::make_shared<LeafNode>(3);
    }    

    bool insert(Element el) {
        printf("insert from root node\n");
        
        if (!sourceNode->insert(el)) {
            std::shared_ptr<InternalNode> newParent = std::make_shared<InternalNode>(3);
            std::shared_ptr<Node> splitNode = sourceNode->split(el);
        
            sourceNode = newParent;

        } 

        return true;
    }
};

class BTree 
{
    public:
        std::shared_ptr<RootNode> rootNode;

        BTree()
        {
            std::shared_ptr<RootNode> root = std::make_shared<RootNode>(); 
            rootNode = root; 
        }

        void populate(uint64_t val) 
        {
            static uint64_t elementId;
            
            rootNode->insert(Element {elementId++, val});
        }
};

int main() {

    std::unique_ptr<BTree> tree = std::make_unique<BTree>(); 

    tree->populate(1);
    tree->populate(5);
    tree->populate(3);

    return 0;
}