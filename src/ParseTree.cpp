#include "ParseTree.h"
#include <stdexcept>
#include <iostream> //TODO: Delete
#include <ParseTree.h>


using namespace std;


ParseTree::ParseTree():empty(true),data(0),leftSubtree(nullptr), rightSubtree(nullptr){}

ParseTree::ParseTree(bool context,bool context2):empty(true),data(0),isContext(context),leftSubtree(nullptr),rightSubtree(nullptr){

}

ParseTree::ParseTree(long data):empty(false),data(data), isContext(false),leftSubtree(nullptr), rightSubtree(nullptr){}


ParseTree::ParseTree(long data, bool context):empty(false),data(data), isContext(context),leftSubtree(nullptr), rightSubtree(nullptr){}

ParseTree::ParseTree(const ParseTree& other):data(other.data),isContext(other.isContext),size(other.size),leftSubtree(nullptr), rightSubtree(nullptr){
    copy(other);
}

ParseTree& ParseTree::operator=(const ParseTree& other){
    if(this==&other){
        return *this;
    }
    clear();
    size = other.size;
    isContext = other.isContext;
    copy(other);
    return *this;
}

ParseTree::ParseTree(ParseTree&& other) noexcept:leftSubtree(other.leftSubtree),rightSubtree(other.rightSubtree),data(other.data),size(other.size),isContext(other.isContext){
    other.leftSubtree = nullptr;
    other.rightSubtree = nullptr;
    other.size = 0;
}

ParseTree& ParseTree::operator=(ParseTree&& other) noexcept{
    if(this==&other){
        return *this;
    }
    clear();
    leftSubtree = other.leftSubtree;
    rightSubtree = other.rightSubtree;
    isContext = other.isContext;
    size = other.size;
    data = other.data;
    other.leftSubtree = nullptr;
    other.rightSubtree = nullptr;
    return *this;
}

ParseTree::~ParseTree(){
    clear();
}

ParseTree::iterator ParseTree::getIterator(){
    return ParseTree::iterator(*this);
}

ParseTree::indexIterator ParseTree::getIndexIterator(){
    return ParseTree::indexIterator(*this);
}

void ParseTree::copy(const ParseTree& other){
    typedef std::pair<ParseTree*,const ParseTree*> treePair;
    stack<treePair> callStack;
    callStack.emplace(this,&other);
    while(!callStack.empty()){
        treePair currPair = callStack.top();
        callStack.pop();
        ParseTree* currTree = currPair.first;
        const ParseTree* otherTree = currPair.second;
        currTree->data = otherTree->data;
        if(otherTree->rightSubtree){
            auto newRight = new ParseTree();
            currTree->rightSubtree = newRight;
            callStack.emplace(newRight, otherTree->rightSubtree);
        }
        if(otherTree->leftSubtree){
            auto newLeft = new ParseTree();
            currTree->leftSubtree = newLeft;
            callStack.emplace(newLeft, otherTree->leftSubtree);
        }
    }
}

void ParseTree::clear(){
    stack<ParseTree*> callStack;
    callStack.push(this);
    if(rightSubtree){
        delete(rightSubtree);
        rightSubtree = nullptr;
    }
    if(leftSubtree){
        delete(leftSubtree);
        leftSubtree = nullptr;
    }
}

void ParseTree::removeLeftSubtree(){
    if(leftSubtree){
        delete(leftSubtree);
        leftSubtree = nullptr;
    }
}

void ParseTree::removeRightSubtree(){
    if(rightSubtree){
        delete(rightSubtree);
        rightSubtree = nullptr;
    }
}

ParseTree* ParseTree::getSubtree(string string){
    stack<ParseTree*> stack;
    stack.push(this);
    const char* currStr = string.c_str();
    while(!stack.empty()){
        ParseTree* currTree = stack.top();
        stack.pop();
        if(!*currStr){return currTree;}
        ParseTree* next = *currStr=='1'?currTree->rightSubtree:currTree->leftSubtree;
        if(!next){
            throw std::invalid_argument("Index out of bounds");
        }
        currStr++;
        stack.push(next);
    }
}

const ParseTree& ParseTree::operator[](string string){
    return *getSubtree(std::move(string));
}

std::pair<ParseTree*,ParseTree*> ParseTree::makeContext(std::string str){
    std::string loc;
    auto context = new ParseTree(true,true);
    auto tree = new ParseTree();
    stack<stackElem> stack;
    stack.emplace(context,this,"");
    while(!stack.empty()){
        stackElem top = stack.top();
        stack.pop();
        ParseTree* currTree = top.curr;
        const ParseTree* otherTree = top.other;
        string curr_str = top.str;
        if(str==curr_str){
            currTree->data = -1;
            delete(tree);
            tree = new ParseTree(*otherTree);
            continue;
        }
        currTree->data = otherTree->data;
        if(otherTree->rightSubtree){
            auto newRight = new ParseTree(true,true);
            currTree->rightSubtree = newRight;
            stack.emplace(newRight, otherTree->rightSubtree, curr_str + "1");
        }
        if(otherTree->leftSubtree){
            auto newLeft = new ParseTree(true,true);
            currTree->leftSubtree = newLeft;
            stack.emplace(newLeft, otherTree->leftSubtree, curr_str + "0");
        }
    }
    return {context,tree};
}

ParseTree::stackPair ParseTree::incStack(stack<stackPair>& stack, stackPair& currPair){
    ParseTree* tree = currPair.first;
    std::string currStr = currPair.second;
    if(tree==nullptr){
        throw runtime_error("No next node!");
    }
    if(tree->rightSubtree){
        stack.emplace(tree->rightSubtree, currStr+'1');
    }
    if(tree->leftSubtree){
        stack.emplace(tree->leftSubtree, currStr+'0');
    }
    ParseTree* ansNode = nullptr;
    std::string ansStr;
    if(!stack.empty()){
        ansNode = stack.top().first;
        ansStr = stack.top().second;
        stack.pop();
    }
    return stackPair(ansNode,ansStr);
}

void ParseTree::setRightSubtree(const ParseTree & right) {
    if(this->empty){
        throw std::invalid_argument("Can't add subtree to en empty tree");
    }
    if(rightSubtree){
        delete(rightSubtree);
        rightSubtree=nullptr;
    }
    rightSubtree = new ParseTree(right);
}

void ParseTree::setLeftSubtree(const ParseTree & left) {
    if(this->empty){
        throw std::invalid_argument("Can't add subtree to en empty tree");
    }
    if(leftSubtree){
        delete(leftSubtree);
        leftSubtree=nullptr;
    }
    leftSubtree = new ParseTree(left);
}

ParseTree::iterator::iterator(ParseTree& tree):currNode(&tree),stack(),currStr(){
}


bool ParseTree::iterator::hasNext(){
    return currNode!=nullptr;
}

ParseTree::iterator& ParseTree::iterator::operator++(){
    stackPair p(currNode, currStr);
    p = incStack(stack, p);
    currNode = p.first;
    currStr = p.second;
    return *this;
}

long ParseTree::iterator::operator*() const{
    if(currNode==nullptr){
        throw runtime_error("No next node!");
    }
    return currNode->data;
}

long* ParseTree::iterator::operator->() const{
    if(currNode==nullptr){
        throw runtime_error("No next node!");
    }
    return &currNode->data;
}

ParseTree::indexIterator::indexIterator(ParseTree& tree):currNode(&tree),currStr(){

}

bool ParseTree::indexIterator::hasNext(){
    return currNode!=nullptr;
}

ParseTree::indexIterator& ParseTree::indexIterator::operator++(){
    stackPair p(currNode, currStr);
    p = incStack(stack, p);
    currNode = p.first;
    currStr = p.second;
    return *this;
}

std::string ParseTree::indexIterator::operator*() const{
    if(currNode==nullptr){
        throw runtime_error("No next node!");
    }
    return currStr;
}