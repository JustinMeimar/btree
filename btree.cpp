#include "btree.hpp" 

static uint64_t nodeId;

std::string InternalRecord::dumpJSON() {
    json recordJSON;
    recordJSON["key"] = record.key;
    recordJSON["gt_child"] = gtChildPtr->id;

    return recordJSON.dump();
}

std::string Record::dumpJSON() {
    json recordJSON;
    recordJSON["key"] = key;

    return recordJSON.dump();
}

// Constructor and Desctructor
Node::Node(uint64_t maxCapacity) : maxCap(maxCapacity), id(nodeId++), ceilCap(CEIL_CAP(maxCapacity)), curCap(0) {}
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
    if (leafNode) {
        if (leafNode->canInsert()) {
            leafNode->insert(record);
        } else {
            auto internalParent = leafNode->parent;
            if (internalParent->canInsert()) {
                std::cout << "parent can insert with capacity: " << internalParent->curCap << std::endl;
                leafNode->insert(record);
                std::shared_ptr<LeafNode> splitNode = leafNode->split();
                internalParent->copyUp(splitNode);
            } else {
                std::cout << "parent can NOT insert with capacity: " << internalParent->curCap << std::endl;
                auto pushedNode = internalParent->pushUp();
                std::cout << "pushed node: " << pushedNode << std::endl;  
                pushedNode->insert(record);
            }     
        }
    }  
    
    return;
}

void InternalNode::remove(Record record) {
    // Implementation
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

std::string InternalNode::dumpJSON() {

    json nodeJSON;

    nodeJSON["id"] = id;
    nodeJSON["parent"] = parent->id;
    nodeJSON["cur_cap"] = curCap;

    json childArray = json::array(); 

    for (InternalRecord child: children) {    
        childArray.push_back(child.dumpJSON());
    }

    nodeJSON["children"] = childArray;

    return nodeJSON.dump();
}

std::string LeafNode::dumpJSON() {
    json leafJSON;
    leafJSON["id"] = id;
    leafJSON["parent"] = parent->id;
    leafJSON["cur_cap"] = parent->id;

    json recordArray = json::array(); 

    for (Record leafRecord: elements) {    
        recordArray.push_back(leafRecord.dumpJSON());
    }

    leafJSON["records"] = recordArray;

    return leafJSON.dump();
}

void InternalNode::copyUp(std::shared_ptr<LeafNode> leaf) {
    // printf("--- COPY UP ---\n");
    // Implementation
    Record firstRecord = leaf->elements.at(0);
    InternalRecord intRecord = {
        firstRecord,
        leaf
    }; 
    // printf("Copy %d into %d\n", firstRecord, id);
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
    std::cout << "[leaf" << id <<  "] capacity before:" << curCap << std::endl; 
    auto it = std::lower_bound(elements.begin(), elements.end(), record);
    elements.insert(it, record);
    curCap++;
}

void LeafNode::remove(Record record) {
    auto it = std::find(elements.begin(), elements.end(), record);
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

    splitNode->parent = this->parent;
    curCap = elements.size();
    std::cout << "ELEMENTS::SIZE() PREV" << curCap << "ELEMENTS::SIZE SPLIT" << splitNode->curCap << std::endl; 
    return splitNode;
}

void LeafNode::print() {
    
    std::string parentId = "null";
    if (parent) { 
        parentId = std::to_string(parent->id);
    }

    std::cout << "<" << id << "," << curCap <<  "," << parentId << ">" << "[";
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

void BTree::remove(Record record) {
    // TODO: Implementation
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
}

std::string BTree::serializeToJSON()
{
    json btree;
    btree["nodes"] = {1, 5, 6, 8};
    std::string jsonString = btree.dump();
    return jsonString;
}

int main(int argc, char **argv) {
    
    std::unique_ptr<BTree> tree = std::make_unique<BTree>(); 

    std::string filename = "in.txt"; // default 
    if (argc > 1) {
        filename = argv[1];
    }
    std::ifstream file(filename);
    std::string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            uint64_t number = static_cast<uint64_t>(std::stoul(line));
            if (number) {
                // printf("\n===============\n"); 
                // std::cout << std::endl << "insert: " << number << std::endl;
                tree->insert( Record {number});
                tree->print();
            }
        }
    }   
    return 0;
}