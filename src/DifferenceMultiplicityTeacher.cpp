#include "MultiplicityTeacher.h"
#include "TreeComparator.h"
#include "ParseTree.h"
#include "MultiplicityTreeAcceptor.h"
#include <math.h>
#include <functional>

DifferenceMultiplicityTeacher::DifferenceMultiplicityTeacher(TreeComparator& cmp, int maxDiff, double decayFactor,
        double epsilon):
trees(),alphabet(),cache(),cmp(cmp),maxDiff(maxDiff),decayFactor(decayFactor),epsilon(epsilon){

}

DifferenceMultiplicityTeacher::DifferenceMultiplicityTeacher(const DifferenceMultiplicityTeacher& other):trees(),
cache(),alphabet(other.alphabet),cmp(other.cmp),maxDiff(other.maxDiff),decayFactor(other.decayFactor),
epsilon(other.epsilon){
    copy(other);
}

DifferenceMultiplicityTeacher::DifferenceMultiplicityTeacher(DifferenceMultiplicityTeacher&& other):
trees(std::move(other.trees)),alphabet(std::move(other.alphabet)),cache(std::move(other.cache)),cmp(other.cmp),
maxDiff(other.maxDiff),decayFactor(other.decayFactor),epsilon(other.epsilon){

}

DifferenceMultiplicityTeacher::~DifferenceMultiplicityTeacher(){
    clear();
}

void DifferenceMultiplicityTeacher::addExample(const ParseTree& tree){
    for(auto currTree: trees){
        if(*currTree == tree){
            throw std::invalid_argument("Tree already in data");
        }
    }
    if(!cache.empty()){
        throw std::invalid_argument("Cache must be empty");
    }
    auto copy = new ParseTree(tree);
    trees.push_back(copy);
    for(auto it=tree.getIndexIterator();it.hasNext();++it){
        int c = (*copy)[*it].getData();
        int rank = (int)((*copy)[*it].getSubtrees().size());
        rankedChar newC = {c, rank};
        alphabet.insert(newC);
    }
}

double DifferenceMultiplicityTeacher::membership(const ParseTree& tree) const{
    for(auto& currTree: trees){
        if(*currTree==tree){
            return currTree->getProb();
        }
    }
    double maxVal=0;
    for(auto& currTree: trees){
        int diff = cmp.compare(*currTree, tree);
        if(diff>maxDiff){continue;}
        double p = currTree->getProb();
        maxVal = maxVal>=p-std::pow(decayFactor, diff)?maxVal:p-std::pow(decayFactor, diff);
    }
    if(maxVal!=0){
        auto treeCopy = new ParseTree(tree);
        treeCopy->setProb(maxVal);
        cache.push_back(treeCopy);
    }
    return maxVal;
}


ParseTree* DifferenceMultiplicityTeacher::equivalence(const MultiplicityTreeAcceptor& acc) const{
    std::function<bool(const ParseTree&)> testFunc = [&acc,this](const ParseTree& t1){
        double val = acc.run(t1);
        double prob = t1.getProb();
        return std::abs(val-prob)>epsilon;
    };
    for(auto& currTree: trees){
        if(testFunc(*currTree)){
            return new ParseTree(*currTree);
        }
    }
    for(auto& currTree: cache){
        if(testFunc(*currTree)){
            return new ParseTree(*currTree);
        }
    }
    return nullptr;
}


void DifferenceMultiplicityTeacher::copy(const DifferenceMultiplicityTeacher& other){
    for(auto& tree: other.trees){
        trees.push_back(new ParseTree(*tree));
    }
    for(auto& tree: other.cache){
        cache.push_back(new ParseTree(*tree));
    }
}

void DifferenceMultiplicityTeacher::clear(){
    for(auto& tree: trees){
        if(tree){
            delete(tree);
            tree = nullptr;
        }
    }
    trees.clear();
    for(auto& tree: cache){
        if(tree){
            delete(tree);
            tree = nullptr;
        }
    }
    cache.clear();
}