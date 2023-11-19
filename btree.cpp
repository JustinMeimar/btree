#include "btree.hpp" 

static uint64_t nodeId;

// Constructor and Desctructor
Node::Node(uint64_t maxCapacity) : maxCap(maxCapacity), id(nodeId++), ceilCap(CEIL_CAP(maxCapacity)) {}
Node::~Node() {}

InternalNode::InternalNode(uint64_t maxCapacity) : Node(maxCapacity) {
    // TODO: impl
}

std::shared_ptr<Node> InternalNode::split() {
    // Implementation
}

std::shared_ptr<Node> InternalNode::findChildPtr(const Record& record) {
    // Implementation
    if (children.empty() || record.key < children.front().record.key) {
        return ltChildPtr;
    }
    
    auto it = std::lower_bound(children.begin(), children.end(), record, 
        [](const InternalRecord& lhs, const Record& rhs) {
            return lhs.record.key < rhs.key;
        });

    if (it != children.begin())
        --it;

    return it->gtChildPtr;
}

void InternalNode::insert(Record record) {
    // insert a record into a leaf node below
    std::shared_ptr<Node> child = findChildPtr(record); 


    std::shared_ptr<LeafNode> leafNode;
    while (!std::dynamic_pointer_cast<LeafNode>(child)) {

        std::shared_ptr<InternalNode> internalNode = std::dynamic_pointer_cast<InternalNode>(child); 
        child = internalNode->findChildPtr(record);
    }

    if (leafNode->canInsert()) {
        leafNode->insert(record);
    } else {
        // Copy up with possibility of causing a push up.
    }
    
}

void InternalNode::remove(Record record) {
    // Implementation
}

void InternalNode::print() {
    // Implementation
    std::cout << "[";
    std::cout << ltChildPtr->id << "|"; 
    for (auto child : children)
    {
        std::cout << child.record.key << "*|" << child.gtChildPtr->id; 
    }
    std::cout << "]";
}

void InternalNode::copyUp(std::shared_ptr<LeafNode> leaf) {
    // Implementation
    Record firstRecord = leaf->elements.at(0);
    InternalRecord intRecord = {
        firstRecord,
        leaf
    };

    auto it = std::lower_bound(children.begin(), children.end(), intRecord,
        [](const InternalRecord& lhs, const InternalRecord& rhs) {
            return lhs.record.key < rhs.record.key;
        });

    children.insert(it, intRecord);
    curCap++;
}

void InternalNode::pushUp(std::shared_ptr<InternalNode> internal) {
    // Implementation
}

void InternalNode::merge() {
    // Implementation
}

LeafNode::LeafNode(uint64_t maxCapacity) : Node(maxCapacity) {}

void LeafNode::insert(Record record) {
    printf("leaf insert: %lu\n", record.key);
    if (curCap < ceilCap) {
        auto it = std::lower_bound(elements.begin(), elements.end(), record);
        elements.insert(it, record);
        curCap++;
    }
}

void LeafNode::remove(Record record) {
    // remove node impl...
    auto it = std::find(elements.begin(), elements.end(), record);
    if (it != elements.end()) {
        elements.erase(it);
        curCap--;
    }
}

void LeafNode::print() {
    std::cout << "[";
    for (auto el : elements)
    {
        std::cout << el.key << "*";
    }
    std::cout << "]";
}

std::shared_ptr<LeafNode> LeafNode::split() {
    std::shared_ptr<LeafNode> splitNode = std::make_shared<LeafNode>(LEAF_NODE_CAP);
    
    int i = 0;
    for (auto el: elements) {
        if (i >= (curCap / 2)) {
            remove(el);
            splitNode->insert(el);
        }
        i++;
    }
    return splitNode;
}

BTree::BTree() : rootNode(std::make_shared<LeafNode>(LEAF_NODE_CAP)) {}
BTree::~BTree() {}

void BTree::insert(Record record) {
            
    if (rootNode->isLeaf()) {
        std::shared_ptr<LeafNode> leafRoot = std::dynamic_pointer_cast<LeafNode>(rootNode);

        if (leafRoot->canInsert()) {
            // simple insert to start
            rootNode->insert(record);
        } else {
            std::shared_ptr<InternalNode> newInternalRoot = std::make_shared<InternalNode>(INTERNAL_NODE_CAP);
            std::shared_ptr<LeafNode> splitNode = leafRoot->split();

            splitNode->parent = newInternalRoot;
            rootNode->parent = newInternalRoot;
            newInternalRoot->ltChildPtr = leafRoot;
            newInternalRoot->copyUp(splitNode);

            rootNode = newInternalRoot; // assign root to be newly created internal node.
            rootNode->insert(record); 
        }
    } else {
        rootNode->insert(record);
    }
}

void BTree::remove(Record record) {
    // Implementation
}

void printTree(const std::unique_ptr<BTree>& tree) {
    // Implementation   
    if (tree) {
        std::cout << std::endl; 
        std::shared_ptr<Node> root = tree->rootNode;
        if (root->isLeaf()) {
            printf("root is leaf\n");
            std::shared_ptr<LeafNode> leafRoot = std::dynamic_pointer_cast<LeafNode>(root); 
            leafRoot->print();
        } else {
            printf("root is internal\n");
            std::shared_ptr<InternalNode> internalRoot = std::dynamic_pointer_cast<InternalNode>(root); 
            internalRoot->print();
            printf("\n");
            internalRoot->ltChildPtr->print();
            for (auto child: internalRoot->children)
            {
                child.gtChildPtr->print();
            }
        }
    }
}

int main() {
    
    std::unique_ptr<BTree> tree = std::make_unique<BTree>(); 

    tree->insert(Record {21});
    tree->insert(Record {9});
    tree->insert(Record {28});
    
    printTree(tree);

    return 0;
}