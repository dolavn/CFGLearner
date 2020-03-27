#include "MultiplicityTeacher.h"
#include "TreeComparator.h"
#include "ParseTree.h"
#include "MultiplicityTreeAcceptor.h"
#include "Logger.h"
#include <math.h>
#include <functional>


ComparatorTeacher::ComparatorTeacher(TreeComparator& cmp, double epsilon):
        trees(),alphabet(),cache(),cmp(cmp),epsilon(epsilon){

}

ComparatorTeacher::ComparatorTeacher(const ComparatorTeacher& other):trees(),cache(),alphabet(other.alphabet),
cmp(other.cmp),epsilon(other.epsilon){
    copy(other);
}

ComparatorTeacher::ComparatorTeacher(ComparatorTeacher&& other):
        trees(std::move(other.trees)),alphabet(std::move(other.alphabet)),cache(std::move(other.cache)),cmp(other.cmp),
        epsilon(other.epsilon){

}

ComparatorTeacher::~ComparatorTeacher(){
    clear();
}

void ComparatorTeacher::addExample(const ParseTree& tree){
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

double ComparatorTeacher::membership(const ParseTree& tree) const{
    Logger& logger = Logger::getLogger();
    logger.setLoggingLevel(Logger::LOG_DEBUG);
    logger << "prob membership" << logger.endline;
    double maxVal=0;
    for(auto& currTree: trees){
        logger << "currTree:" << *currTree << logger.endline;
        if(*currTree==tree){
            logger << "if" << logger.endline;
            return currTree->getProb();
        }else{
            //logger << "else" << logger.endline;
            //logger << &cmp << logger.endline;
            double p = calcNewProb(tree, *currTree, cmp);
            if(p>maxVal){maxVal=p;}
        }
        //logger << "maxVal:" << maxVal << logger.endline;
    }
    if(maxVal!=0){
        auto treeCopy = new ParseTree(tree);
        treeCopy->setProb(maxVal);
        cache.push_back(treeCopy);
    }
    return maxVal;
}


ParseTree* ComparatorTeacher::equivalence(const MultiplicityTreeAcceptor& acc) const{
    Logger& logger = Logger::getLogger();
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


void ComparatorTeacher::copy(const ComparatorTeacher& other){
    for(auto& tree: other.trees){
        trees.push_back(new ParseTree(*tree));
    }
    for(auto& tree: other.cache){
        cache.push_back(new ParseTree(*tree));
    }
}

void ComparatorTeacher::clear(){
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