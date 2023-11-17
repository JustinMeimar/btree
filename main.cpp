#include <iostream>
#include <memory>
#include <cstdint>
#include <vector>
#include <algorithm>

#define LEAF_NODE_CAP 5
#define INTERNAL_NODE_CAP 3

struct Element
{
    uint64_t id;
    uint64_t val;

    bool operator<(const Element& other) const {
        return val < other.val;
    }

    bool operator==(const Element& other) const {
        return val == other.val;
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
    virtual std::shared_ptr<Node> split() = 0;

    void insertSorted(std::vector<Element>& vec, const Element& el) {
        auto it = std::lower_bound(vec.begin(), vec.end(), el);
        vec.insert(it, el);
    } 

};


// TODO: incorporate this custom type
struct InternalElement
{
    Element el;
    std::shared_ptr<Node> gtChild;
};

struct InternalNode : Node
{
    std::shared_ptr<InternalNode> parent;
    std::vector<std::shared_ptr<Node>> children;

    // std::vector<InternalElement> children;
    std::shared_ptr<Node> ltChild; // asymetric less than child

    uint64_t ceilCap;

    InternalNode(uint64_t maxCapacity) : Node(maxCapacity) {
        ceilCap = ((maxCapacity + 2 -1) / 2);
    }

    std::shared_ptr<Node> split() override
    {
        std::shared_ptr<InternalNode> splitNode = std::make_shared<InternalNode>(INTERNAL_NODE_CAP);
        
        return splitNode;
    }

    bool insert(Element insertEl) override
    { 
        printf("internal insert: %d\n", insertEl.val);
        int i = 0;
        int cap = nodeEl.size();

        bool result = false;
        
        for (auto el : nodeEl) {
            if (insertEl.val < el.val) {
                result = children.at(i)->insert(insertEl);
                // printf("child node to insert: %d\n", children.at(i)->nodeEl.at(0).val);
                // printf("child node to insert: %p\n", children.at(i));
                // printf("internal insert found: %d\n", result);
            }    
            i++; 
        }   

        printf("i: %d, cap: %d\n", i, cap);
        if (!result && i == cap) {
            result = children.at(cap)->insert(insertEl);
            printf("internal insert found: %d\n", result);
        }
        
        return result;     
    }

    bool remove(Element value) override
    {
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

    bool copyUp(std::shared_ptr<InternalNode> internalNode)
    {
        Element copyEl = nodeEl.at(0);
        // internalNode();
    }

    std::shared_ptr<Node> split() override
    {
        std::shared_ptr<LeafNode> splitNode = std::make_shared<LeafNode>(LEAF_NODE_CAP);

        int i = 0;
        for (auto el: nodeEl)
        {
            if (i >= (curCap / 2)) {
                remove(el);
                splitNode->insert(el);
            }
            i++;
        }
        return splitNode;
    }

    bool insert(Element value) override 
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
    bool remove(Element el) override
    { 
        auto it = std::find(nodeEl.begin(), nodeEl.end(), el);

        if (it != nodeEl.end()) {
            nodeEl.erase(it);
            curCap--;
            return true; 
        }
        return false; 
    }
};

struct RootNode
{
    std::shared_ptr<Node> sourceNode;

    RootNode() {
        sourceNode = std::make_shared<LeafNode>(5);
    }    

    bool insert(Element el) {
        
        if (!sourceNode->insert(el)) {

            printf("begin split\n");
            std::shared_ptr<InternalNode> newParent = std::make_shared<InternalNode>(LEAF_NODE_CAP);
            newParent->children.push_back(sourceNode);

            std::shared_ptr<LeafNode> splitNode = std::dynamic_pointer_cast<LeafNode>(sourceNode->split());
            // splitNode->copyUp();
            
            Element copyEl = splitNode->nodeEl.at(0); 
            newParent->children.push_back(splitNode);
            newParent->nodeEl.push_back(copyEl); 
             
            sourceNode = newParent;
            sourceNode->insert(el);
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
    tree->populate(9);
    tree->populate(3);
    tree->populate(7);
    tree->populate(2);

    return 0;
}
