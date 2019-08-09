#include "ParseTree.h"
#include <stdexcept>
#include <iostream> //TODO: Delete
#include <sstream>
#include <unordered_map>
#include <ParseTree.h>

#define IS_NULL(A)  (A==nullptr)

using namespace std;


ParseTree::ParseTree():empty(true),data(0),size(0),weight(0),prob(0),isContext(false),contextLoc(),
                       subtrees(){}

ParseTree::ParseTree(bool context,vector<int> contextLoc):empty(true),data(0),size(0),weight(0),prob(0),
isContext(context),contextLoc(std::move(contextLoc)),subtrees(){
    if(!context){contextLoc = vector<int>();}
}

ParseTree::ParseTree(int data):empty(false),data(data),size(1),weight(0),
prob(0),isContext(false),contextLoc(),subtrees(){}

ParseTree::ParseTree(int data, vector<ParseTree> v):empty(false),data(data),size(1),weight(0),prob(0),
                                                    isContext(false),contextLoc(),subtrees(v.size()){
    for(unsigned int i=0;i<v.size();++i){
        subtrees[i] = new ParseTree(v[i]);
    }
}

ParseTree::ParseTree(int data, bool context, vector<int> contextLoc):empty(false),data(data),size(1),weight(0),prob(0),
                                                                     isContext(context),
                                                                     contextLoc(std::move(contextLoc)),subtrees(){
    if(!context){contextLoc=vector<int>();}
}

ParseTree::ParseTree(const ParseTree& other):empty(other.empty),data(other.data),size(other.size),weight(other.weight),
                                             prob(other.prob),isContext(other.isContext),contextLoc(other.contextLoc),
                                             subtrees(){
    copy(other);
}

ParseTree& ParseTree::operator=(const ParseTree& other){
    if(this==&other){
        return *this;
    }
    clear();
    size = other.size;
    weight = other.weight;
    prob = other.prob;
    isContext = other.isContext;
    contextLoc = other.contextLoc;
    copy(other);
    return *this;
}

ParseTree::ParseTree(ParseTree&& other) noexcept:empty(other.empty),data(other.data),
                                                 size(other.size),weight(other.weight),prob(other.prob),
                                                 isContext(other.isContext),contextLoc(std::move(other.contextLoc)),
                                                 subtrees(std::move(other.subtrees)){
    other.size = 0;
}

ParseTree& ParseTree::operator=(ParseTree&& other) noexcept{
    if(this==&other){
        return *this;
    }
    clear();
    subtrees = std::move(other.subtrees);
    isContext = other.isContext;
    size = other.size;
    weight = other.weight;
    prob = other.prob;
    data = other.data;
    return *this;
}

ParseTree::~ParseTree(){
    clear();
}

ParseTree::iterator ParseTree::getIterator() const{
    return ParseTree::iterator(*this);
}

ParseTree::indexIterator ParseTree::getIndexIterator() const{
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
        currTree->weight = otherTree->weight;
        currTree->prob = otherTree->prob;
        currTree->subtrees = vector<ParseTree*>(otherTree->subtrees.size());
        for(auto i=(int)(otherTree->subtrees.size()-1);i>=0;--i){
            if(!otherTree->subtrees[i]){continue;}
            auto newTree = new ParseTree();
            currTree->subtrees[i]=newTree;
            callStack.emplace(newTree, otherTree->subtrees[i]);
        }
    }
}

void ParseTree::clear(){
    for(auto& tree: subtrees){
        if(tree) {
            delete (tree);
            tree = nullptr;
        }
    }
}

ParseTree* ParseTree::getSubtree(const vector<int>& loc){
    ParseTree* currTree = this;
    for(auto& elem: loc){
        if(elem>=(int)(currTree->subtrees.size())){
            throw std::invalid_argument("Index out of bounds");
        }
        ParseTree* next = currTree->subtrees[elem];
        if(!next){
            throw std::invalid_argument("Index out of bounds");
        }
        currTree = next;
    }
    return currTree;
}

ParseTree& ParseTree::operator[](const vector<int>& loc){
    return *getSubtree(loc);
}

const ParseTree& ParseTree::getNode(const vector<int>& loc) const{
    const ParseTree* currTree = this;
    for(auto& elem: loc){
        if(elem>=(int)(currTree->subtrees.size())){
            throw std::invalid_argument("Index out of bounds");
        }
        ParseTree* next = currTree->subtrees[elem];
        if(!next){
            throw std::invalid_argument("Index out of bounds");
        }
        currTree = next;
    }
    return *currTree;
}

vector<ParseTree*> ParseTree::getSubtrees() const{
    return subtrees;
}

std::pair<ParseTree*,ParseTree*> ParseTree::makeContext(vector<int> loc) const{
    auto context = new ParseTree(true,loc);
    auto tree = new ParseTree();
    stack<stackElem> stack;
    stack.emplace(context,this,vector<int>());
    while(!stack.empty()){
        stackElem top = stack.top();
        stack.pop();
        ParseTree* currTree = top.curr;
        const ParseTree* otherTree = top.other;
        vector<int> curr_loc = top.loc;
        if(loc==curr_loc){
            currTree->data = -1;
            delete(tree);
            tree = new ParseTree(*otherTree);
            continue;
        }
        currTree->data = otherTree->data;
        for(auto i=(int)(otherTree->subtrees.size()-1);i>=0;--i){
            if(!otherTree->subtrees[i]){continue;}
            bool isContext = isPrefix(curr_loc,loc) && loc.size()>curr_loc.size() && loc[curr_loc.size()]==i;
            ParseTree* newTree;
            if(isContext){
                vector<int> newVecLoc = vector<int>(loc.begin()+curr_loc.size()+1,loc.end());
                newTree = new ParseTree(true, newVecLoc);
            }else{
                newTree = new ParseTree();
            }
            while((int)(currTree->subtrees.size())<i+1){currTree->subtrees.push_back(nullptr);}
            currTree->subtrees[i] = newTree;
            vector<int> locCopy = curr_loc;
            locCopy.push_back(i);
            stack.emplace(newTree,otherTree->subtrees[i],locCopy);
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
    ans->contextLoc = vector<int>();
    (*ans)[contextLoc].setData(other.getData());
    (*ans)[contextLoc].subtrees = vector<ParseTree*>(other.subtrees.size());
    for(unsigned int i=0;i<other.subtrees.size();++i){
        if(!other.subtrees[i]){continue;}
        (*ans)[contextLoc].subtrees[i] = new ParseTree(*other.subtrees[i]);
    }
    return ans;
}

ParseTree::stackPair ParseTree::incStack(stack<stackPair>& stack, stackPair& currPair){
    const ParseTree* tree = currPair.first;
    vector<int> currLoc = currPair.second;
    if(tree==nullptr){
        throw runtime_error("No next node!");
    }
    for(auto i=(int)(tree->subtrees.size()-1);i>=0;--i){
        if(!tree->subtrees[i]){continue;}
        vector<int> copyLoc = currLoc; copyLoc.push_back(i);
        stack.emplace(tree->subtrees[i],copyLoc);
    }
    const ParseTree* ansNode = nullptr;
    vector<int> ansLoc;
    if(!stack.empty()){
        ansNode = stack.top().first;
        ansLoc = stack.top().second;
        stack.pop();
    }
    return stackPair(ansNode,ansLoc);
}

void ParseTree::setSubtree(const ParseTree & tree,unsigned int ind) {
    if(this->empty){
        throw std::invalid_argument("Can't add subtree to en empty tree");
    }
    while(subtrees.size()<ind+1){subtrees.push_back(nullptr);}
    if(hasSubtree(ind)){
        delete(subtrees[ind]);
        subtrees[ind]=nullptr;
    }
    subtrees[ind] = new ParseTree(tree);
}

bool ParseTree::isLeaf() const{
    for(auto& tree: subtrees){
        if(tree){
            return false;
        }
    }
    return true;
}

int ParseTree::getWeight() const{
    return weight;
}

void ParseTree::setWeight(int weight){
    this->weight = weight;
}

double ParseTree::getProb() const{
    return prob;
}

void ParseTree::setProb(double prob){
    this->prob = prob;
}

void ParseTree::applyWeights(const ParseTree& weightsTree){
    if(!this->sameTopology(weightsTree)){
        throw std::invalid_argument("Weights tree must have the same topology as this tree!");
    }
    weight = weightsTree.getData();
    for(int i=0;i<subtrees.size();i++){
        if(!subtrees[i]){continue;}
        subtrees[i]->applyWeights(*weightsTree.getSubtrees()[i]);
    }
}

bool ParseTree::sameTopology(const ParseTree& other){
    if(subtrees.size()!=other.subtrees.size()){
        return false;
    }
    for(int i=0;i<subtrees.size();i++){
        if((subtrees[i]==nullptr) != (other.subtrees[i]==nullptr)){
            return false;
        }
        if(subtrees[i]==nullptr){
            continue;
        }
        if(!subtrees[i]->sameTopology(*other.subtrees[i])){
            return false;
        }
    }
    return true;
}

ParseTree ParseTree::getSkeleton() const{
    ParseTree ans(*this);
    for(auto it=ans.getIndexIterator();it.hasNext();++it){
        (ans)[*it].setData(-1);
    }
    return ans;
}

vector<ParseTree*> ParseTree::getAllContexts() const{
    auto it = getIndexIterator();
    vector<ParseTree*> ans;
    while(it.hasNext()){
        auto ind = *it;
        pair<ParseTree*, ParseTree*> contextTreePair = makeContext(ind);
        if(contextTreePair.second){
            delete(contextTreePair.second);
            contextTreePair.second=nullptr;
        }
        ans.push_back(contextTreePair.first);
        ++it;
    }
    return ans;
}

std::string ParseTree::latexTreeRecursive() const{
    stringstream stream;
    for(unsigned int i=0;i<subtrees.size();++i){
        auto t = subtrees[i];
        if(t->subtrees.size()==0){
            if(t->data==-1){
                stream << "$\\square$";
            }else {
                stream << t->data;
            }
        }else{
            stream << t->latexTreeRecursive();
        }
        if(i<subtrees.size()-1) {
            stream << " ";
        }
    }
    stringstream labelStream;
    if(this->data==-1){
        labelStream << "$\\square$";
    }else {
        labelStream << this->data;
    }
    if(stream.str().size()>0){
        labelStream << " ";
    }
    return "[." + labelStream.str() + stream.str() + " ]";
}

std::string ParseTree::getLatexTree() const{
    return "\\Tree " + this->latexTreeRecursive();
}

ParseTree::iterator::iterator(const ParseTree& tree):stack(),currNode(&tree),currLoc(){
}


bool ParseTree::iterator::hasNext(){
    return currNode!=nullptr;
}

ParseTree::iterator& ParseTree::iterator::operator++(){
    stackPair p(currNode, currLoc);
    p = incStack(stack, p);
    currNode = p.first;
    currLoc = p.second;
    return *this;
}

int ParseTree::iterator::operator*() const{
    if(currNode==nullptr){
        throw runtime_error("No next node!");
    }
    return currNode->data;
}

const int* ParseTree::iterator::operator->() const{
    if(currNode==nullptr){
        throw runtime_error("No next node!");
    }
    return &currNode->data;
}

ParseTree::indexIterator::indexIterator(const ParseTree& tree):currNode(&tree),currLoc(),leaves(false){

}

ParseTree::indexIterator::indexIterator(ParseTree& tree,bool leaves):currNode(&tree),currLoc(),leaves(leaves){
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
    stackPair p(currNode, currLoc);
    do{
        p = incStack(stack,p);
    }while(leaves && p.first!=nullptr && !p.first->isLeaf());
    currNode = p.first;
    currLoc = p.second;
    return *this;
}

std::vector<int> ParseTree::indexIterator::operator*() const{
    if(currNode==nullptr){
        throw runtime_error("No next node!");
    }
    return currLoc;
}

size_t ParseTree::getHash() const{
    size_t ans = hash<int>()(data);
    for(auto tree: subtrees){
        if(tree){
            ans = ans ^ (hash<int>()(tree->getData()) << 1);
        }
        ans = ans >> 1;
    }
    return ans;
}

bool operator==(const ParseTree& lhs, const ParseTree& rhs){
    if(lhs.data!=rhs.data){return false;}
    if(lhs.subtrees.size()!=rhs.subtrees.size()){return false;}
    for(unsigned int i=0;i<lhs.subtrees.size();i++){
        if((IS_NULL(lhs.subtrees[i]) || IS_NULL(rhs.subtrees[i])) && !(IS_NULL(lhs.subtrees[i]) && IS_NULL(rhs.subtrees[i]))){
            return false;
        }
        if(!IS_NULL(lhs.subtrees[i]) && *lhs.subtrees[i]!=*rhs.subtrees[i]){
            return false;
        }
    }
    return true;
}

int operator-(const ParseTree& lhs, const ParseTree& rhs){
    if(lhs.data!=rhs.data){return lhs.weight+rhs.weight;}
    int diff = 0;
    unsigned int i=0;
    const ParseTree& bigger = lhs.subtrees.size()>rhs.subtrees.size()?lhs:rhs;
    for(;i<min(lhs.subtrees.size(),rhs.subtrees.size());i++){
        if(!IS_NULL(lhs.subtrees[i]) && !IS_NULL(rhs.subtrees[i])){
            diff = diff + (*lhs.subtrees[i]-*rhs.subtrees[i]);
            continue;
        }
        if(!IS_NULL(lhs.subtrees[i])){
            diff = diff + lhs.subtrees[i]->getWeight();
            continue;
        }
        if(!IS_NULL(rhs.subtrees[i])){
            diff = diff + rhs.subtrees[i]->getWeight();
            continue;
        }
    }
    for(;i<max(lhs.subtrees.size(),rhs.subtrees.size());i++){
        diff = diff + bigger.subtrees[i]->getWeight();
    }
    return diff;
}

ostream& operator<<(ostream& output, const ParseTree& tree){
    output << "( [" << tree.data << "," << tree.weight << "]";
    for(auto& subtree: tree.subtrees){
        if(subtree){
            output << " " << *subtree;
        }
    }
    output << ")";
    return output;
}

bool ParseTree::isPrefix(const vector<int>& v1, const vector<int>& v2){
    if(v1.size()>v2.size()){return false;}
    for(unsigned int i=0;i<v1.size();++i){
        if(v1[i]!=v2[i]){return false;}
    }
    return true;
}


vector<const ParseTree*> ParseTree::getInOrderPtrList() const{
    vector<const ParseTree*> ans;
    unordered_map<const ParseTree*, bool> seenMap;
    stack<const ParseTree*> stack;
    stack.push(this);
    seenMap[this] = false;
    while(!stack.empty()){
        const ParseTree* top = stack.top();
        if(seenMap[top]){
            stack.pop();
            ans.push_back(top);
        }else{
            seenMap[top] = true;
            for(long i=top->subtrees.size()-1;i>=0;i--){
                if(top->subtrees[i]) {
                    stack.push(top->subtrees[i]);
                }
            }
        }
    }
    return ans;
}