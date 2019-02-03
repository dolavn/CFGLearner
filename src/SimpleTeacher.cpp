#include "Teacher.h"
#include "ParseTree.h"
#include "TreeAcceptor.h"
#include <iostream>
using namespace std;

SimpleTeacher::SimpleTeacher():positiveExamples(),negativeExamples(){

}

SimpleTeacher::SimpleTeacher(const SimpleTeacher& other):positiveExamples(),negativeExamples(){
    copy(other);
}

SimpleTeacher& SimpleTeacher::operator=(const SimpleTeacher& other){
    if(this==&other){
        return *this;
    }
    clear();
    copy(other);
    return *this;
}

SimpleTeacher::SimpleTeacher(SimpleTeacher&& other) noexcept:positiveExamples(std::move(other.positiveExamples)),negativeExamples(std::move(negativeExamples)){

}

SimpleTeacher& SimpleTeacher::operator=(SimpleTeacher&& other) noexcept{
    if(this==&other){
        return *this;
    }
    clear();
    positiveExamples = std::move(other.positiveExamples);
    negativeExamples = std::move(other.negativeExamples);
    return *this;
}

SimpleTeacher::~SimpleTeacher(){
    clear();
}

void SimpleTeacher::clear(){
    for(auto& ptr: positiveExamples){
        if(ptr){
            delete(ptr);
            ptr=nullptr;
        }
    }
    for(auto& ptr: negativeExamples){
        if(ptr){
            delete(ptr);
            ptr=nullptr;
        }
    }
}

void SimpleTeacher::copy(const SimpleTeacher& other){
    for(auto& ptr: other.positiveExamples){
        positiveExamples.push_back(new ParseTree(*ptr));
    }
    for(auto& ptr: other.negativeExamples){
        negativeExamples.push_back(new ParseTree(*ptr));
    }
}


bool SimpleTeacher::membership(const ParseTree& t) const{
    for(ParseTree* ptr: positiveExamples){
        if(*ptr==t){
            return true;
        }
    }
    return false;
}

ParseTree* SimpleTeacher::equivalence(const TreeAcceptor & acc) const{
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

void SimpleTeacher::addPositiveExample(const ParseTree& tree){
    if(hasExample(tree)){
        throw std::invalid_argument("Already has this example");
    }
    positiveExamples.push_back(new ParseTree(tree));
}

void SimpleTeacher::addNegativeExample(const ParseTree & tree){
    if(hasExample(tree)){
        throw std::invalid_argument("Already has this example");
    }
    negativeExamples.push_back(new ParseTree(tree));
}

bool SimpleTeacher::hasExample(const ParseTree& tree){
    for(auto example: positiveExamples){
        if(*example==tree){
            return true;
        }
    }
    for(auto example: negativeExamples){
        if(*example==tree){
            return true;
        }
    }
    return false;
}

Teacher* SimpleTeacher::clone(){
    return new SimpleTeacher(*this);
}