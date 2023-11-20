#include "btree.hpp" 

static uint64_t nodeId;

// Constructor and Desctructor
Node::Node(uint64_t maxCapacity) : maxCap(maxCapacity), id(nodeId++), ceilCap(CEIL_CAP(maxCapacity)) {}
Node::~Node() {}

InternalNode::InternalNode(uint64_t maxCapacity = INTERNAL_NODE_CAP) : Node(maxCapacity) {}

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

    while (!child->isLeaf()) {
        std::shared_ptr<InternalNode> internalNode = std::dynamic_pointer_cast<InternalNode>(child);
        if (!internalNode) {
            std::cerr << "Error: Expected InternalNode" << std::endl;
            return;
        }
        child = internalNode->findChildPtr(record);
    }

    if (child->isLeaf()) {
        std::shared_ptr<LeafNode> leafNode = std::dynamic_pointer_cast<LeafNode>(child);
        
        if (leafNode->canInsert()) {
            leafNode->insert(record);
        } else {
            auto internalParent = leafNode->parent;
            if (internalParent->canInsert()) {
                leafNode->insert(record);
                std::shared_ptr<LeafNode> splitNode = leafNode->split();
                internalParent->copyUp(splitNode);
            } else {
                auto pushedNode = internalParent->pushUp();
                printf("pushed node: %d\n", pushedNode->id);
                // internalParent->insert(record);
                // insert(record);
                // internalParent->parent->insert(record);
            }     
        }
    }    
}

void InternalNode::remove(Record record) {
    // Implementation
}

void InternalNode::print() {
    std::cout << "<" << id << ">" << "[" << ltChildPtr->id;
    for (const auto& child : children) {
        std::cout << " | " << child.record.key << "* | " << child.gtChildPtr->id;
    }
    std::cout << "]" << std::endl;
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

void InternalNode::addChild(InternalRecord child) {
    auto it = std::lower_bound(children.begin(), children.end(), child,
        [](const InternalRecord& lhs, const InternalRecord& rhs) {
            return lhs.record.key < rhs.record.key;
        });

    children.insert(it, child);
    curCap++;
}

void InternalNode::removeChild(const InternalRecord& targetChild) {
    auto newEnd = std::remove_if(children.begin(), children.end(),
        [&targetChild](const InternalRecord& child) {
            return child.record.key == targetChild.record.key;
        });

    if (newEnd != children.end()) {
        children.erase(newEnd, children.end());
        curCap--;
    }
}

std::shared_ptr<InternalNode> InternalNode::pushUp() {
    // TODO: implement
    // ACUTAL TODO: use the removeChild and addChild methods to 
    // properly make the pushUp to manage the curCap of all nodes involved 
    if (!parent) {
        auto newParent = std::make_shared<InternalNode>();
        auto splitNode = std::make_shared<InternalNode>();

        auto it = children.begin() + (curCap / 2);
        InternalRecord middleRecord = *it;

        // newParent->ltChildPtr = shared_from_this();
        // splitNode->ltChildPtr = middleRecord.gtChildPtr;
        // middleRecord.gtChildPtr = splitNode;

        // newParent->addChild(middleRecord);

        // parent = newParent;
        // splitNode->parent = newParent;

        // // splitNode->children.insert(splitNode->children.begin(), it + 1, children.end());
        // // children.erase(it, children.end()); 
        // removeChild();
        return newParent;
    } else {
        // TODO: handle level 4 .. k
        return shared_from_this();
    }
}

void InternalNode::merge() {
    // Implementation
}

LeafNode::LeafNode(uint64_t maxCapacity) : Node(maxCapacity) {}

void LeafNode::insert(Record record) {
    auto it = std::lower_bound(elements.begin(), elements.end(), record);
    elements.insert(it, record);
    curCap++;
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
    std::cout << "<" << id << ">" << "[";
    for (auto el : elements) { std::cout << el.key << "*"; }
    std::cout << "] ";
}

std::shared_ptr<LeafNode> LeafNode::split() {
    // 
    std::shared_ptr<LeafNode> splitNode = std::make_shared<LeafNode>(LEAF_NODE_CAP); 
    int i = 0;
    for (auto el: elements) {
        if (i >= (curCap / 2)) {
            remove(el);
            splitNode->insert(el);
        }
        i++;
    }
    splitNode->parent = this->parent;
    return splitNode;
}

BTree::BTree() : rootNode(std::make_shared<LeafNode>(LEAF_NODE_CAP)) {}
BTree::~BTree() {}

void BTree::insert(Record record) {
            
    if (rootNode->isLeaf()) {
        std::shared_ptr<LeafNode> leafRoot = std::dynamic_pointer_cast<LeafNode>(rootNode);

        if (leafRoot->canInsert()) {
            // simple insert
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

    if (rootNode->parent != nullptr) {
        rootNode = rootNode->parent;
    }
}

void BTree::remove(Record record) {
    // Implementation
}

void BTree::printTree(const std::unique_ptr<BTree>& tree) {
    printf("\n================\n");
    
    if (!tree || !tree->rootNode) {
        std::cout << "Tree is empty" << std::endl;
        return;
    }

    std::queue<std::shared_ptr<Node>> nodesQueue;
    nodesQueue.push(tree->rootNode);

    while (!nodesQueue.empty()) {
        std::shared_ptr<Node> currentNode = nodesQueue.front();
        nodesQueue.pop();

        currentNode->print();

        if (!currentNode->isLeaf()) {
            std::shared_ptr<InternalNode> internalNode = std::dynamic_pointer_cast<InternalNode>(currentNode);

            if (internalNode->ltChildPtr) {
                nodesQueue.push(internalNode->ltChildPtr);
            }

            for (const auto& child : internalNode->children) {
                if (child.gtChildPtr) {
                    nodesQueue.push(child.gtChildPtr);
                }
            }
        }
    }
}

int main() {
    
    std::unique_ptr<BTree> tree = std::make_unique<BTree>(); 

    tree->insert(Record {21});
    tree->insert(Record {9});
    tree->insert(Record {28});
    tree->insert(Record {14});
    tree->insert(Record {49});
    tree->insert(Record {22});
    tree->insert(Record {89});
    // tree->printTree(tree);
    tree->insert(Record {29});
    tree->insert(Record {18});
    
    tree->printTree(tree);

    return 0;
}