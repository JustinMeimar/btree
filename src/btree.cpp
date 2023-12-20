#include "btree.h" 

static uint64_t nodeId;

Node::Node(uint64_t maxCapacity) : maxCap(maxCapacity), id(nodeId++), ceilCap(CEIL_CAP(maxCapacity)), curCap(0) {}
Node::~Node() {}

InternalNode::InternalNode(uint64_t maxCapacity = INTERNAL_NODE_CAP) : Node(maxCapacity) {}

std::shared_ptr<Node> InternalNode::findChildPtr(uint64_t key) {
    if (children.empty() || key < children.front().record.key) {
        return ltChildPtr;
    }
    
    auto it = std::lower_bound(children.begin(), children.end(), key, 
        [](const InternalRecord& lhs, uint64_t rhsKey) {
            return lhs.record.key <= rhsKey;
        });

    if (it != children.begin())
        --it;

    return it->gtChildPtr;
}

void InternalNode::insert(Record record) {
    // insert a record into a leaf node below
    std::shared_ptr<Node> child = findChildPtr(record.key); 
    if (!child) {
        print();
        std::cout << "child is null "<< record.key << std::endl;
    }
    while (!child->isLeaf()) {
        std::shared_ptr<InternalNode> internalNode = std::dynamic_pointer_cast<InternalNode>(child);
        if (!internalNode) {
            std::cerr << "Error: Expected InternalNode" << std::endl;
            return;
        }
        child = internalNode->findChildPtr(record.key);
    }

    std::shared_ptr<LeafNode> leafNode = std::dynamic_pointer_cast<LeafNode>(child);
    if (leafNode) {
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
    }   
    return;
}

void InternalNode::remove(uint64_t key) {
    // TODO: Implementation
}

void InternalNode::print() {
    std::string parentId = "null";
    if (parent) {
        parentId = std::to_string(parent->id);
    }
    std::cout << "<" << id << "," << curCap << "," << parent << ">" << "[" << ltChildPtr->id;
    for (const auto& child : children) {
        std::cout << " | " << child.record.key << "* | " << child.gtChildPtr->id;
    }
    std::cout << "]" << std::endl;
}



void InternalNode::copyUp(std::shared_ptr<LeafNode> leaf) {
    Record firstRecord = leaf->elements.at(0);
    InternalRecord intRecord = {
        firstRecord,
        leaf
    }; 
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

/**
 * pushUp: split the internal node in two. The middle element is pushed into the parent node.
 * The leftChildPtr of the middle node now must pont to the lhs Split Node, and the gtChildPtr
 * must point to the RHS split node.
 * 
 * @returns a pointer to the node that the split node is pushed into 
*/
std::shared_ptr<InternalNode> InternalNode::pushUp() {

    if (!parent) {
        auto newParent = std::make_shared<InternalNode>();
        newParent->ltChildPtr = shared_from_this();
        parent = newParent;       
    }
    if (!parent->canInsert()) {
        parent->pushUp();
        std::shared_ptr<InternalNode> tempParent = parent;
        while (tempParent->parent) {
            tempParent = tempParent->parent;
        }
        return tempParent;
    }
    auto splitNode = std::make_shared<InternalNode>();
    auto it = children.begin() + (curCap / 2);
    InternalRecord middleRecord = *it;
    middleRecord.gtChildPtr->parent = splitNode; // TODO: REASON
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
    // TODO: Implementation
}

LeafNode::LeafNode(uint64_t maxCapacity = LEAF_NODE_CAP) : Node(maxCapacity) {}

void LeafNode::insert(Record record) {
    // std::cout << "[leaf" << id <<  "] capacity before:" << curCap << std::endl; 
    auto it = std::lower_bound(elements.begin(), elements.end(), record);
    elements.insert(it, record);
    curCap++;
}

/**
 * Only called when we are sure we can remove without side-effects. 
*/
void LeafNode::remove(uint64_t key) {        
    auto it = std::find_if(elements.begin(), elements.end(),
        [key](const Record& record) { return record.key == key; });

    if (it != elements.end()) {
        elements.erase(it);
    }
    curCap = elements.size();
}

std::shared_ptr<LeafNode> LeafNode::split() {
    
    std::shared_ptr<LeafNode> splitNode = std::make_shared<LeafNode>(LEAF_NODE_CAP);
    size_t splitIndex = elements.size() / 2;
    std::vector<Record> elementsToMove(elements.begin() + splitIndex, elements.end());
    elements.erase(elements.begin() + splitIndex, elements.end());
    for (const auto& el : elementsToMove) {
        splitNode->insert(el);
    }

    if (nextLeaf) {
        splitNode->nextLeaf = nextLeaf;
    } 
    nextLeaf = splitNode;
    splitNode->parent = this->parent;
    splitNode->prevLeaf = shared_from_this();
    curCap = elements.size();
    
    return splitNode;
}

void LeafNode::print() {
    
    std::string parentId = "null";
    std::string nextId = "null";
    if (parent) { 
        parentId = std::to_string(parent->id);
    }
    if (nextLeaf) {
        nextId = std::to_string(nextLeaf->id);
    }
    std::cout << "<" << id << "," << curCap <<  "," << parentId << "," << nextId <<">" << "[";
    for (auto el : elements) { std::cout << el.key << "*"; }
    std::cout << "] ";
    
}

BTree::BTree() : capacity(0), rootNode(std::make_shared<LeafNode>(LEAF_NODE_CAP)) {}
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
    capacity++;
}

void BTree::remove(uint64_t key) {
    
    auto leafNode = findLeafNode(key);
    if (leafNode) {
        if (leafNode->canRemove()) {
            leafNode->remove(key);
        } else {
            // TODO: Implement
        }
    }
}

void BTree::print() {
    if (capacity == 0) {
        std::cout << "Tree is empty" << std::endl;
        return;
    }
    std::queue<std::shared_ptr<Node>> nodesQueue;
    nodesQueue.push(rootNode);

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
    std::cout << std::endl;
}

/**
 * Look up the index of a record based on the key 
*/
std::shared_ptr<LeafNode> BTree::findLeafNode(uint64_t key) {
    std::shared_ptr<Node> curNode = rootNode;

    while (curNode) {
        auto internalNode = std::dynamic_pointer_cast<InternalNode>(curNode);
        if (internalNode) {
            curNode = internalNode->findChildPtr(key);
        } else {
            auto leafNode = std::dynamic_pointer_cast<LeafNode>(curNode);
            if (leafNode) {
                return leafNode;
            } else {
                break;
            }
        }
    }
    return nullptr;
}

Record BTree::lookUp(uint64_t key) {

    auto leafNode = findLeafNode(key);

    if (leafNode) {
        auto it = std::find_if(leafNode->elements.begin(), leafNode->elements.end(),
            [key](const Record& record) { return record.key == key; });

        if (it != leafNode->elements.end()) {
            return *it;
        } 
    }

    return Record {0, false};
}

void traverseLeafChain(const std::unique_ptr<BTree> &tree) {

    std::shared_ptr<LeafNode> curLeafNode; 
    std::shared_ptr<Node> curNode = tree->rootNode;
    while (curNode) { 
        auto internalNode = std::dynamic_pointer_cast<InternalNode>(curNode);
        if (internalNode) {
            curNode = internalNode->ltChildPtr;
        } else {
            curLeafNode = std::dynamic_pointer_cast<LeafNode>(curNode);
            break;
        }
    }
    while (curLeafNode) {
        curLeafNode->print();
        curLeafNode = curLeafNode->nextLeaf;
    }
}