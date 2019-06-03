#include "Teacher.h"
#include "ParseTree.h"
#include "TreeAcceptor.h"
#include "TreeComparator.h"
#include <limits>

#define DEFAULT_DIFF 0

using namespace std;

function<int(const ParseTree&, const ParseTree&)> defaultFunc = [](const ParseTree& t1, const ParseTree& t2){
    return t1-t2;
};

DifferenceTeacher::DifferenceTeacher():SimpleTeacher(),maxDiff(DEFAULT_DIFF),cmpFunc(defaultFunc){

}

DifferenceTeacher::DifferenceTeacher(int maxDiff):SimpleTeacher(),maxDiff(maxDiff),cmpFunc(defaultFunc){

}

DifferenceTeacher::DifferenceTeacher(const DifferenceTeacher& other):SimpleTeacher(other),
maxDiff(other.maxDiff), cmpFunc(defaultFunc){
    copyCache(other);
}

DifferenceTeacher& DifferenceTeacher::operator=(const DifferenceTeacher& other){
    if(this==&other){
        return *this;
    }
    SimpleTeacher::operator=(other);
    maxDiff = other.maxDiff;
    cmpFunc = other.cmpFunc;
    clearCache();
    copyCache(other);
    return *this;
}

DifferenceTeacher::DifferenceTeacher(DifferenceTeacher&& other) noexcept:SimpleTeacher(std::move(other)),
positiveCache(std::move(other.positiveCache)),negativeCache(std::move(other.negativeCache)),
maxDiff(other.maxDiff),cmpFunc(other.cmpFunc){

}

DifferenceTeacher& DifferenceTeacher::operator=(DifferenceTeacher&& other){
    if(this==&other){
        return *this;
    }
    SimpleTeacher::operator=(other);
    clearCache();
    positiveCache = std::move(other.positiveCache);
    negativeCache = std::move(other.negativeCache);
    maxDiff = other.maxDiff;
    cmpFunc = other.cmpFunc;
    return *this;
}

DifferenceTeacher::~DifferenceTeacher(){
    clearCache();
}

bool DifferenceTeacher::membership(const ParseTree& tree) const{
    int minPos = std::numeric_limits<int>::max();
    ParseTree* exp = nullptr;
    for(ParseTree* ptr: positiveCache){
        if(tree==*ptr){
            return true;
        }
    }
    for(ParseTree* ptr: negativeCache){
        if(tree==*ptr){
            return false;
        }
    }
    for(ParseTree* ptr: positiveExamples){
        int diff = cmpFunc(*ptr, tree);
        if(diff<minPos){minPos=diff;exp=ptr;}
    }
    int minNeg = std::numeric_limits<int>::max();
    for(ParseTree* ptr: negativeExamples){
        int diff = cmpFunc(*ptr, tree);
        if(diff<minPos){minNeg=diff;}
    }
    //std::cout << "minPos:" << minPos << std::endl;
    //std::cout << tree << std::endl;
    //std::cout << *exp << std::endl;
    if(minPos<=minNeg && minPos<=maxDiff){
        positiveCache.push_back(new ParseTree(tree));
        return true;
    }else{
        negativeCache.push_back(new ParseTree(tree));
        return false;
    }
}

ParseTree* DifferenceTeacher::equivalence(const TreeAcceptor& acc) const{
    for(ParseTree* ptr: positiveExamples){
        if(!acc.run(*ptr)){
            return new ParseTree(*ptr);
        }
    }
    for(ParseTree* ptr: positiveCache){
        if(!acc.run(*ptr)){
            return new ParseTree(*ptr);
        }
    }
    for(ParseTree* ptr: negativeExamples){
        if(acc.run(*ptr)){
            return new ParseTree(*ptr);
        }
    }
    for(ParseTree* ptr: negativeCache){
        if(acc.run(*ptr)){
            return new ParseTree(*ptr);
        }
    }
    return nullptr;
}

void DifferenceTeacher::addPositiveExample(const ParseTree& tree){
    clearCache();
    SimpleTeacher::addPositiveExample(tree);
}


void DifferenceTeacher::addNegativeExample(const ParseTree& tree){
    clearCache();
    SimpleTeacher::addNegativeExample(tree);
}

void DifferenceTeacher::setTreeComparator(TreeComparator& cmp){
    cmpFunc = [&cmp](const ParseTree& t1, const ParseTree& t2){
        return cmp.compare(t1, t2);
    };
}


void DifferenceTeacher::copyCache(const DifferenceTeacher& other){
    for(ParseTree* ptr: other.positiveCache){
        positiveCache.push_back(new ParseTree(*ptr));
    }
    for(ParseTree* ptr: other.negativeCache){
        negativeCache.push_back(new ParseTree(*ptr));
    }
}


void DifferenceTeacher::clearCache(){
    for(ParseTree* ptr : positiveCache){
        delete(ptr);
    }
    positiveCache.clear();
    for(ParseTree* ptr : negativeCache){
        delete(ptr);
    }
    negativeCache.clear();
}

Teacher* DifferenceTeacher::clone(){
    return new DifferenceTeacher(*this);
}