#include "Teacher.h"
#include "ParseTree.h"
#include "TreeAcceptor.h"
#include <limits>

#define DEFAULT_DIFF 0

DifferenceTeacher::DifferenceTeacher():SimpleTeacher(),maxDiff(DEFAULT_DIFF){

}

DifferenceTeacher::DifferenceTeacher(int maxDiff):SimpleTeacher(),maxDiff(maxDiff){

}

DifferenceTeacher::DifferenceTeacher(const DifferenceTeacher& other):SimpleTeacher(other),
maxDiff(other.maxDiff){

}

DifferenceTeacher& DifferenceTeacher::operator=(const DifferenceTeacher& other){
    if(this==&other){
        return *this;
    }
    SimpleTeacher::operator=(other);
    maxDiff = other.maxDiff;
    return *this;
}

DifferenceTeacher::DifferenceTeacher(DifferenceTeacher&& other):SimpleTeacher(other),
                                                                     maxDiff(other.maxDiff){

}

DifferenceTeacher& DifferenceTeacher::operator=(DifferenceTeacher&& other){
    if(this==&other){
        return *this;
    }
    SimpleTeacher::operator=(other);
    maxDiff = other.maxDiff;
    return *this;
}

DifferenceTeacher::~DifferenceTeacher(){

}

bool DifferenceTeacher::membership(const ParseTree& tree) const{
    int minPos = std::numeric_limits<int>::max();
    for(ParseTree* ptr: positiveExamples){
        int diff = *ptr-tree;
        if(diff<minPos){minPos=diff;}
    }
    int minNeg = std::numeric_limits<int>::max();
    for(ParseTree* ptr: negativeExamples){
        int diff = *ptr-tree;
        if(diff<minPos){minNeg=diff;}
    }
    return minPos<=minNeg && minPos<=maxDiff;
}

ParseTree* DifferenceTeacher::equivalence(const TreeAcceptor& acc) const{
    for(ParseTree* ptr: positiveExamples){
        if(!acc.run(*ptr)){
            return new ParseTree(*ptr);
        }
    }
    for(ParseTree* ptr: negativeExamples){
        if(acc.run(*ptr)){
            return new ParseTree(*ptr);
        }
    }
    return nullptr;
}

Teacher* DifferenceTeacher::clone(){
    return new DifferenceTeacher(*this);
}