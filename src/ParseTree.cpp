#include "ParseTree.h"
#include <stdexcept>
#include <iostream> //TODO: Delete
#include <ParseTree.h>

#define IS_NULL(A)  A==nullptr

using namespace std;


ParseTree::ParseTree():empty(true),data(0),size(0),isContext(false),contextLoc(),leftSubtree(nullptr), rightSubtree(nullptr){}

ParseTree::ParseTree(bool context,std::string contextLoc):empty(true),data(0),size(0),isContext(context),contextLoc(contextLoc),leftSubtree(nullptr),rightSubtree(nullptr){
    if(!context){contextLoc="";}
}

ParseTree::ParseTree(int data):empty(false),data(data),size(1),isContext(false),contextLoc(),leftSubtree(nullptr), rightSubtree(nullptr){}

ParseTree::ParseTree(int data, vector<ParseTree> v):empty(false),data(data),size(1),isContext(false),contextLoc(),leftSubtree(new ParseTree(v[0])), rightSubtree(new ParseTree(v[1])){}

ParseTree::ParseTree(int data, bool context, std::string contextLoc):empty(false),data(data),size(1),isContext(context),contextLoc(contextLoc),leftSubtree(nullptr), rightSubtree(nullptr){
    if(!context){contextLoc="";}
}

ParseTree::ParseTree(const ParseTree& other):empty(other.empty),data(other.data),size(other.size),isContext(other.isContext),contextLoc(other.contextLoc),leftSubtree(nullptr), rightSubtree(nullptr){
    copy(other);
}

ParseTree& ParseTree::operator=(const ParseTree& other){
    if(this==&other){
        return *this;
    }
    clear();
    size = other.size;
    isContext = other.isContext;
    contextLoc = other.contextLoc;
    copy(other);
    return *this;
}

ParseTree::ParseTree(ParseTree&& other) noexcept:empty(other.empty),data(other.data),size(other.size),isContext(other.isContext),contextLoc(std::move(other.contextLoc)),leftSubtree(other.leftSubtree),rightSubtree(other.rightSubtree){
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

ParseTree::indexIterator ParseTree::getLeafIterator(){
    return ParseTree::indexIterator(*this,true);
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
    throw std::invalid_argument("Index out of bounds");
}

ParseTree& ParseTree::operator[](string str){
    return *getSubtree(std::move(str));
}

const ParseTree& ParseTree::getNode(string str) const{
    stack<const ParseTree*> stack;
    stack.push(this);
    const char* currStr = str.c_str();
    while(!stack.empty()){
        const ParseTree& currTree = *stack.top();
        stack.pop();
        if(!*currStr){return currTree;}
        ParseTree* next = *currStr=='1'?currTree.rightSubtree:currTree.leftSubtree;
        if(!next){
            throw std::invalid_argument("Index out of bounds");
        }
        currStr++;
        stack.push(next);
    }
    throw std::invalid_argument("Index out of bounds");
}

vector<ParseTree*> ParseTree::getSubtrees() const{
    return {leftSubtree,rightSubtree};
}

std::pair<ParseTree*,ParseTree*> ParseTree::makeContext(std::string str) const{
    std::string loc;
    auto context = new ParseTree(true,str);
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
            bool isContext = str.substr(0,curr_str.size()+1)==curr_str + "1";
            ParseTree* newRight;
            if(isContext){
                newRight = new ParseTree(true,str.substr(curr_str.size()+1));
            }else{
                newRight = new ParseTree();
            }
            currTree->rightSubtree = newRight;
            stack.emplace(newRight, otherTree->rightSubtree, curr_str + "1");
        }
        if(otherTree->leftSubtree){
            bool isContext = str.substr(0,curr_str.size()+1)==curr_str + "0";
            ParseTree* newLeft;
            if(isContext){
                newLeft = new ParseTree(true,str.substr(curr_str.size()+1));
            }else{
                newLeft = new ParseTree();
            }
            currTree->leftSubtree = newLeft;
            stack.emplace(newLeft, otherTree->leftSubtree, curr_str + "0");
        }
    }
    return {context,tree};
}

ParseTree* ParseTree::mergeContext(const ParseTree& other) const{
    if(!this->isContext){
        throw std::invalid_argument("Tree must be a context");
    }
    auto ans = new ParseTree(*this);
    ans->isContext = false;
    ans->contextLoc = "";
    (*ans)[contextLoc].setData(other.getData());
    if(other.leftSubtree){
        (*ans)[contextLoc].setLeftPointer(new ParseTree(*other.leftSubtree));
    }
    if(other.rightSubtree){
        (*ans)[contextLoc].setRightPointer(new ParseTree(*other.rightSubtree));
    }
    return ans;
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

bool ParseTree::isLeaf() const{
    return leftSubtree==nullptr && rightSubtree==nullptr;
}

ParseTree ParseTree::getSkeleton() const{
     ParseTree ans(*this);
     for(auto it=ans.getIndexIterator();it.hasNext();++it){
         (ans)[*it].setData(-1);
     }
     return ans;
}

ParseTree::iterator::iterator(ParseTree& tree):stack(),currNode(&tree),currStr(){
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

int ParseTree::iterator::operator*() const{
    if(currNode==nullptr){
        throw runtime_error("No next node!");
    }
    return currNode->data;
}

int* ParseTree::iterator::operator->() const{
    if(currNode==nullptr){
        throw runtime_error("No next node!");
    }
    return &currNode->data;
}

ParseTree::indexIterator::indexIterator(ParseTree& tree):currNode(&tree),currStr(),leaves(false){

}

ParseTree::indexIterator::indexIterator(ParseTree& tree,bool leaves):currNode(&tree),currStr(),leaves(leaves){
    if(leaves){
        if(!currNode->isLeaf()){
            operator++();
        }
    }
}

bool ParseTree::indexIterator::hasNext(){
    return currNode!=nullptr;
}

ParseTree::indexIterator& ParseTree::indexIterator::operator++(){
    stackPair p(currNode, currStr);
    do{
        p = incStack(stack,p);
    }while(leaves && p.first!=nullptr && !p.first->isLeaf());
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

bool operator==(const ParseTree& lhs, const ParseTree& rhs){
    if((IS_NULL(lhs.leftSubtree) ||  IS_NULL(rhs.leftSubtree)) && !(IS_NULL(lhs.leftSubtree) && IS_NULL(rhs.leftSubtree))){ //only one left son is null
        return false;
    }
    if((IS_NULL(lhs.rightSubtree) ||  IS_NULL(rhs.rightSubtree)) && !(IS_NULL(lhs.rightSubtree) && IS_NULL(rhs.rightSubtree))){ //only one right son is null
        return false;
    }
    bool leftSon = IS_NULL(lhs.leftSubtree) || *lhs.leftSubtree==*rhs.leftSubtree;
    bool rightSon = IS_NULL(lhs.rightSubtree) || *lhs.rightSubtree==*rhs.rightSubtree;
    return lhs.data == rhs.data && leftSon && rightSon;
}

ostream& operator<<(ostream& output, const ParseTree& tree){
    output << "(" << tree.data;
    if(tree.leftSubtree!=nullptr){
        output << " " << *tree.leftSubtree;
    }
    if(tree.rightSubtree!=nullptr){
        output << " " << *tree.rightSubtree;
    }
    output << ")";
    return output;
}