#include "TreesGenerator.h"
#include "ParseTree.h"
#include "utility.h"

using namespace std;

DuplicationsGenerator::DuplicationsGenerator(vector<ParseTree*> trees, int depth):trees(),depth(depth){
    for(auto tree: trees){
        this->trees.push_back(new ParseTree(*tree));
    }
}

DuplicationsGenerator::DuplicationsGenerator(const DuplicationsGenerator& other):trees(),depth(other.depth){
    copy(other);
}

DuplicationsGenerator& DuplicationsGenerator::operator=(const DuplicationsGenerator& other){
    if(this==&other){
        return *this;
    }
    clear();
    copy(other);
    depth = other.depth;
}

DuplicationsGenerator::DuplicationsGenerator(DuplicationsGenerator&& other):trees(move(other.trees)),depth(other.depth){

}

DuplicationsGenerator& DuplicationsGenerator::operator=(DuplicationsGenerator&& other){
    if(this==&other){
        return *this;
    }
    clear();
    trees = move(other.trees);
    depth = other.depth;
}

bool DuplicationsGenerator::hasNext() const{
    return false;
}

ParseTree DuplicationsGenerator::getNext(){
    return ParseTree();
}

void DuplicationsGenerator::copy(const DuplicationsGenerator& other){
    function<ParseTree*(const ParseTree&)> copy_func = [](const ParseTree& t){return new ParseTree(t);};
    deep_copy_vec(other.trees, trees, copy_func);
}

void DuplicationsGenerator::clear(){
    clear_vec(trees);
}


