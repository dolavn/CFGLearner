#include "MultiplicityTeacher.h"
#include "MultiplicityTreeAcceptor.h"
#include "ParseTree.h"

#define ABS(x) x>=0?x:-x

SimpleMultiplicityTeacher::SimpleMultiplicityTeacher(double epsilon, double defaultVal):epsilon(epsilon),
defaultValue(defaultVal),alphabet(), trees(){

}

SimpleMultiplicityTeacher::SimpleMultiplicityTeacher(const SimpleMultiplicityTeacher& other):epsilon(other.epsilon),
defaultValue(other.defaultValue),alphabet(other.alphabet),trees(){
    copy(other);
}

SimpleMultiplicityTeacher& SimpleMultiplicityTeacher::operator=(const SimpleMultiplicityTeacher& other) {
    if(this==&other){
        return *this;
    }
    clear();
    epsilon = other.epsilon;
    defaultValue = other.defaultValue;
    alphabet = other.alphabet;
    copy(other);
    return *this;
}

SimpleMultiplicityTeacher::SimpleMultiplicityTeacher(SimpleMultiplicityTeacher&& other):epsilon(other.epsilon),
defaultValue(other.defaultValue),alphabet(std::move(other.alphabet)),trees(std::move(other.trees)){

}

SimpleMultiplicityTeacher& SimpleMultiplicityTeacher::operator=(SimpleMultiplicityTeacher&& other) {
    if(this==&other){
        return *this;
    }
    clear();
    epsilon = other.epsilon;
    defaultValue = other.defaultValue;
    trees = std::move(other.trees);
    alphabet = std::move(other.alphabet);
    return *this;
}

void SimpleMultiplicityTeacher::addExample(const ParseTree& t){
    auto tNew = new ParseTree(t);
    trees.push_back(tNew);
    for(auto it=t.getIndexIterator();it.hasNext();++it){
        int c = (*tNew)[*it].getData();
        int rank = (int)((*tNew)[*it].getSubtrees().size());
        rankedChar newC = {c, rank};
        alphabet.insert(newC);
    }
}

double SimpleMultiplicityTeacher::membership(const ParseTree& tree) const{
    for(ParseTree* example: trees){
        if(*example==tree){
            return example->getProb();
        }
    }
    return defaultValue;
}

ParseTree* SimpleMultiplicityTeacher::equivalence(const MultiplicityTreeAcceptor& acc) const{
    for(auto& tree: trees){
        double calculatedValue = acc.run(*tree);
        if(ABS(calculatedValue-tree->getProb())<epsilon){
            auto ans = new ParseTree(*tree);
            return ans;
        }
    }
    return nullptr;
}

void SimpleMultiplicityTeacher::copy(const SimpleMultiplicityTeacher& other){
    for(auto& tree: other.trees){
        trees.push_back(new ParseTree(*tree));
    }
}

void SimpleMultiplicityTeacher::clear(){
    for(auto& tree: trees){
        if(tree){
            delete(tree);
            tree = nullptr;
        }
    }
    trees.clear();
}

SimpleMultiplicityTeacher::~SimpleMultiplicityTeacher(){
    clear();
}