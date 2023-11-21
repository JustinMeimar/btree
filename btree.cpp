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
            pushedNode->insert(record);
        }     
    }
    return;
}

void InternalNode::remove(Record record) {
    // Implementation
}

void InternalNode::print() {
    std::cout << "<" << id << "," << curCap << ">" << "[" << ltChildPtr->id;
    for (const auto& child : children) {
        std::cout << " | " << child.record.key << "* | " << child.gtChildPtr->id;
    }
    std::cout << "]" << std::endl;
}

void InternalNode::copyUp(std::shared_ptr<LeafNode> leaf) {
    printf("--- COPY UP ---\n");
    // Implementation
    Record firstRecord = leaf->elements.at(0);
    InternalRecord intRecord = {
        firstRecord,
        leaf
    }; 
    printf("Copy %d into %d\n", firstRecord, id);
    addChild(intRecord); 
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
    // TODO: REFACTOR
    printf("--- PUSH UP ---\n"); 
    if (!parent) {
        auto newParent = std::make_shared<InternalNode>();
        newParent->ltChildPtr = shared_from_this();
        parent = newParent;       
    }
    if (!parent->canInsert()) {
        printf("HANDLE A FULL GRAND PARENT\n");
        return nullptr;
    }
    auto splitNode = std::make_shared<InternalNode>();
    auto it = children.begin() + (curCap / 2);
    InternalRecord middleRecord = *it;
    it++;

    splitNode->ltChildPtr = middleRecord.gtChildPtr;
    middleRecord.gtChildPtr = splitNode;

    parent->addChild(middleRecord);
    splitNode->parent = parent;

    int index = std::distance(children.begin(), it); // Calculate the current index
    while (it != children.end()) {
        InternalRecord splitRecord = *it;
        splitNode->addChild(splitRecord); 
        removeChild(splitRecord);
        it->gtChildPtr->parent = splitNode;
        it = children.begin() + index;
    }
    removeChild(middleRecord); // wait until the end to not mess up iterators
    return parent;
}

void InternalNode::merge() {
    // Implementation
}

LeafNode::LeafNode(uint64_t maxCapacity = LEAF_NODE_CAP) : Node(maxCapacity) {}

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
    std::cout << "<" << id << "," << curCap <<  "," << parent->id << ">" << "[";
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
    tree->insert(Record {97});
    tree->insert(Record {99});
    
    tree->insert(Record {54});
    tree->insert(Record {57});
    tree->insert(Record {50});
    tree->insert(Record {51});

    tree->printTree(tree);
    // tree->insert(Record {29});
    // tree->insert(Record {18});
    
    // tree->printTree(tree);

    return 0;
}

/** 
    // std::shared_ptr<LeafNode> leafs[4];
    // leafs[0] = std::make_shared<LeafNode>();
    // leafs[1] = std::make_shared<LeafNode>();
    // leafs[2] = std::make_shared<LeafNode>();
    // leafs[3] = std::make_shared<LeafNode>();

    // for (int i = 0 ; i < 4; i++) { leafs[i]->print(); }
    
    // auto internalNode = std::make_shared<InternalNode>();

    // printf("\n==============\n");

    // internalNode->ltChildPtr = leafs[0];
    // internalNode->print();
    // internalNode->addChild(InternalRecord {Record{25}, leafs[1]});
    // internalNode->print();
    // internalNode->addChild(InternalRecord {Record{15}, leafs[2]});
    // internalNode->print();
    // internalNode->addChild(InternalRecord {Record{90}, leafs[3]});
    // internalNode->print();
    // internalNode->removeChild(InternalRecord {Record{15}, leafs[2]});
    // internalNode->print();
    // internalNode->removeChild(InternalRecord {Record{90}, leafs[3]});
    // internalNode->print();
    // internalNode->removeChild(InternalRecord {Record{25}, leafs[1]});
    // internalNode->print();
*/