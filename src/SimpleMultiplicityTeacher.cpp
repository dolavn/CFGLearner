#include "MultiplicityTeacher.h"
#include "MultiplicityTreeAcceptor.h"
#include "ParseTree.h"

#define ABS(x) x>=0?x:-x

SimpleMultiplicityTeacher::SimpleMultiplicityTeacher(double epsilon, double defaultVal):epsilon(epsilon),
defaultValue(defaultVal), trees(){

}

void SimpleMultiplicityTeacher::addExample(const ParseTree& t){
    auto tNew = new ParseTree(t);
    trees.push_back(tNew);
}

double SimpleMultiplicityTeacher::membership(const ParseTree& tree){
    for(ParseTree* example: trees){
        if(*example==tree){
            return example->getProb();
        }
    }
    return defaultValue;
}

example* SimpleMultiplicityTeacher::equivalence(const MultiplicityTreeAcceptor& acc){
    for(auto& tree: trees){
        double calculatedValue = acc.run(*tree);
        if(ABS(calculatedValue-tree->getProb())<epsilon){
            auto ans = new example;
            ans->first = *tree;
            ans->second = calculatedValue;
            return ans;
        }
    }
    return nullptr;
}