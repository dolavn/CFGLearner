#include "TreesGenerator.h"
#include "ParseTree.h"
#include "utility.h"

using namespace std;

DuplicationsGenerator::DuplicationsGenerator(vector<ParseTree> trees, int depth):trees(),depth(depth),currTree(0),
currIt(),directionsIterator(){
    for(const auto& tree: trees){
        this->trees.push_back(new ParseTree(tree));
    }
    createIterator();
}

DuplicationsGenerator::DuplicationsGenerator(vector<ParseTree*> trees, int depth):trees(),depth(depth),currTree(0),
                                                                                  currIt(),directionsIterator(){
    for(const auto& tree: trees){
        this->trees.push_back(new ParseTree(*tree));
    }
    createIterator();
}

DuplicationsGenerator::DuplicationsGenerator(const DuplicationsGenerator& other):trees(),depth(other.depth),
currTree(other.currTree),currIt(other.currIt),directionsIterator(other.directionsIterator){
    copy(other);
}

DuplicationsGenerator& DuplicationsGenerator::operator=(const DuplicationsGenerator& other){
    if(this==&other){
        return *this;
    }
    clear();
    copy(other);
    depth = other.depth;
    currTree = other.currTree;
    currIt = other.currIt;
    directionsIterator = other.directionsIterator;
}

DuplicationsGenerator::DuplicationsGenerator(DuplicationsGenerator&& other):trees(move(other.trees)),depth(other.depth),
currTree(other.currTree),currIt(move(other.currIt)),directionsIterator(move(other.directionsIterator)){
}

DuplicationsGenerator& DuplicationsGenerator::operator=(DuplicationsGenerator&& other){
    if(this==&other){
        return *this;
    }
    clear();
    trees = move(other.trees);
    depth = other.depth;
    currTree = other.currTree;
    currIt = move(other.currIt);
    directionsIterator = move(other.directionsIterator);
}

DuplicationsGenerator::~DuplicationsGenerator(){
    clear();
}

bool DuplicationsGenerator::hasNext() const{
    return currTree<trees.size();
}

void printVec(vector<int> v);

void DuplicationsGenerator::createIterator(){
    ParseTree& tree = *trees[currTree];
    int numLeaves = tree.getLeavesNum();
    int duplications = depth*numLeaves;
    currIt = PartitionsIterator(duplications, numLeaves+1, depth, numLeaves);
    directionsIterator = RIGHT;
}


ParseTree DuplicationsGenerator::createDuplication(const ParseTree& tree, PartitionsIterator::partition part){
    int currLeaf = 0,dirIteratorInd=0;
    ParseTree copy(tree);
    for(auto it = tree.getLeafIterator();it.hasNext();++it,++currLeaf){
        ParseTree rightTree(tree.getNode(*it).getData());
        int dupNum = part[currLeaf];
        if(dupNum==0){continue;}
        ParseTree currTree = ParseTree(0, {rightTree, rightTree});
        dupNum=dupNum-1;
        while(dupNum>0){
            ParseTree newTree = directionsIterator==RIGHT?ParseTree(0,
                    {currTree, rightTree}):ParseTree(0,{rightTree,currTree});
            currTree = move(newTree);
            dupNum--;
        }
        copy[*it] = currTree;
    }
    return copy;
}

bool DuplicationsGenerator::isDirectional(){
    for(int i=0;i<(*currIt).size()-1;++i){
        if((*currIt)[i]>1){
            return true;
        }
    }
    return false;
}

DuplicationsGenerator& DuplicationsGenerator::operator++(){
    if(isDirectional()){
        if(directionsIterator==RIGHT){
            directionsIterator=LEFT;
            return *this;
        }
    }
    ++currIt;
    directionsIterator=RIGHT;
    if(!currIt.hasNext()){
        ++currTree;
        if(currTree<trees.size()){
            createIterator();
        }
    }
    return *this;
}

void DuplicationsGenerator::reset(){
    currTree=0;
    createIterator();
}

TreesGenerator* DuplicationsGenerator::clone() const{
    return new DuplicationsGenerator(*this);
}

ParseTree DuplicationsGenerator::operator*(){
    return createDuplication(*trees[currTree], *currIt);
}

void DuplicationsGenerator::copy(const DuplicationsGenerator& other){
    deep_copy_vec(other.trees, trees);
}

void DuplicationsGenerator::clear(){
    clear_vec(trees);
}


