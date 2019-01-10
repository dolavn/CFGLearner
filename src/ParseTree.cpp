#include "ParseTree.h"
#include <stdexcept>

using namespace std;

typedef pair<TreeNode*, TreeNode*> nodePair;

TreeNode::TreeNode(long data):rightSon(nullptr),leftSon(nullptr),data(data){}

TreeNode::TreeNode(const TreeNode& other):rightSon(nullptr),leftSon(nullptr),data(other.data){}

TreeNode& TreeNode::operator=(const TreeNode& other){
    if(this==&other){
        return *this;
    }
    rightSon = nullptr;
    leftSon = nullptr;
    data = other.data;
    return *this;
}

TreeNode::TreeNode(TreeNode&& other) noexcept:rightSon(other.rightSon),leftSon(other.leftSon),data(other.data){
    other.rightSon = nullptr;
    other.leftSon = nullptr;
}

TreeNode& TreeNode::operator=(TreeNode&& other) noexcept{
    if(this==&other){
        return *this;
    }
    rightSon = other.rightSon;
    leftSon = other.leftSon;
    other.rightSon = nullptr;
    other.leftSon = nullptr;
    data = other.data;
    return *this;
}

TreeNode::~TreeNode()=default;


ParseTree::ParseTree():root(nullptr){}

ParseTree::ParseTree(TreeNode* root):root(root){}

ParseTree::ParseTree(const ParseTree& other):root(nullptr),size(other.size){
    copy(other);
}

ParseTree& ParseTree::operator=(const ParseTree& other){
    if(this==&other){
        return *this;
    }
    clear();
    size = other.size;
    copy(other);
    return *this;
}

ParseTree::ParseTree(ParseTree&& other) noexcept:root(other.root),size(other.size){
    other.root = nullptr;
    other.size = 0;
}

ParseTree& ParseTree::operator=(ParseTree&& other) noexcept{
    if(this==&other){
        return *this;
    }
    clear();
    root = other.root;
    size = other.size;
    other.root = nullptr;
    return *this;
}

ParseTree::~ParseTree(){
    clear();
}

ParseTree::iterator ParseTree::getIterator(){
    return ParseTree::iterator(*this);
}

void ParseTree::copy(const ParseTree& other){
    stack<nodePair> callStack;
    if(other.root!=nullptr){
        root = new TreeNode(*other.root);
        callStack.push(nodePair(root, other.root));
    }
    while(!callStack.empty()){
        nodePair currPair = callStack.top();
        callStack.pop();
        TreeNode* currNode = currPair.first;
        TreeNode* otherNode = currPair.second;
        if(otherNode->getRightSon()){
            auto newRight = new TreeNode(*otherNode->getRightSon());
            currNode->setRightSon(newRight);
            callStack.push(nodePair(newRight, otherNode->getRightSon()));
        }
        if(otherNode->getLeftSon()){
            auto newLeft = new TreeNode(*otherNode->getLeftSon());
            currNode->setLeftSon(newLeft);
            callStack.push(nodePair(newLeft, otherNode->getLeftSon()));
        }
    }
}

void ParseTree::clear(){
    stack<TreeNode*> callStack;
    if(root!=nullptr){
        callStack.push(root);
        root = nullptr;
    }
    while(!callStack.empty()){
        TreeNode* curr = callStack.top();
        callStack.pop();
        if(curr->getRightSon()){
            callStack.push(curr->getRightSon());
        }
        if(curr->getLeftSon()){
            callStack.push(curr->getLeftSon());
        }
        delete(curr);
    }
}

ParseTree::iterator::iterator(const ParseTree& tree):stack(),currNode(tree.root),currStr(),locIterator(false){
}


bool ParseTree::iterator::hasNext(){
    return currNode!=nullptr;
}

ParseTree::iterator& ParseTree::iterator::operator++(){
    if(currNode==nullptr){
        throw runtime_error("No next node!");
    }
    if(currNode->getRightSon()){
        stack.emplace(currNode->getRightSon(), currStr+'1');
    }
    if(currNode->getLeftSon()){
        stack.emplace(currNode->getLeftSon(), currStr+'0');
    }
    currNode = nullptr;
    currStr = "";
    if(!stack.empty()){
        currNode = stack.top().first;
        currStr = stack.top().second;
        stack.pop();
    }
    return *this;
}

std::pair<TreeNode&, std::string> ParseTree::iterator::operator*() const{
    if(currNode==nullptr){
        throw runtime_error("No next node!");
    }
    return std::pair<TreeNode&, std::string>(*currNode,currStr);
}

TreeNode* ParseTree::iterator::operator->() const{
    if(currNode==nullptr){
        throw runtime_error("No next node!");
    }
    return currNode;
}
