#include "Teacher.h"
#include "ParseTree.h"
#include "TreeAcceptor.h"
#include <iostream> //TODO: delete
#define DEFAULT_MIN_FREQ 0.5f
#define DEFAULT_MIN_COUNT 0


using namespace std;

FrequencyTeacher::FrequencyTeacher():minCount(DEFAULT_MIN_COUNT),minFreq(DEFAULT_MIN_FREQ),map(){

}

FrequencyTeacher::FrequencyTeacher(int minCount, float minFreq):minCount(minCount),minFreq(minFreq),map(){
    if(minFreq<0 || minFreq>1 || minCount<0){
        throw invalid_argument("Invalid parameters");
    }
}

FrequencyTeacher::FrequencyTeacher(const FrequencyTeacher& other):minCount(other.minCount),minFreq(other.minFreq),map(){
    copy(other);
}

FrequencyTeacher& FrequencyTeacher::operator=(const FrequencyTeacher& other){
    if(this==&other){
        return *this;
    }
    minCount = other.minCount;
    minFreq = other.minFreq;
    clear();
    copy(other);
    return *this;
}

FrequencyTeacher::FrequencyTeacher(FrequencyTeacher&& other) noexcept:minCount(other.minCount),
minFreq(other.minFreq),map(move(other.map)){

}

FrequencyTeacher& FrequencyTeacher::operator=(FrequencyTeacher&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    minCount = other.minCount;
    minFreq = other.minFreq;
    clear();
    map = move(other.map);
    return *this;
}

FrequencyTeacher::~FrequencyTeacher(){
    clear();
}


pair<int,int>& FrequencyTeacher::getPair(const ParseTree& tree){
    TreePointer temp(&tree);
    if(map.find(temp)==map.end()){
        map[temp] = pair<int,int>(0,0);
    }
    return map[temp];
}

bool FrequencyTeacher::membership(const ParseTree& tree) const{
    return inLanguage(tree);
}

ParseTree* FrequencyTeacher::equivalence(const TreeAcceptor& acc) const{
    for(auto pair:map){
        TreePointer ptr = pair.first;
        ParseTree& tree = *ptr.ptr;
        if(acc.run(tree)!=inLanguage(tree)){
            return new ParseTree(tree);
        }
    }
    return nullptr;
}

Teacher* FrequencyTeacher::clone(){
    return new FrequencyTeacher(*this);
}

void FrequencyTeacher::addPositiveExample(const ParseTree& tree){
    auto& p = getPair(tree);
    p.first++;
}

void FrequencyTeacher::addNegativeExample(const ParseTree& tree){
    auto& p = getPair(tree);
    p.second++;
}

bool FrequencyTeacher::inLanguage(const ParseTree& tree) const{
    TreePointer temp(&tree);
    if(map.find(temp)==map.end()){
        return false;
    }
    auto p = map.find(temp)->second;
    float freq = (float)(p.first)/(float)(p.first+p.second);
    int count = p.first;
    return freq>this->minFreq && count>this->minCount;
}


size_t TreeHasher::operator()(const TreePointer& t) const{
    if(!t.ptr){
        return 0;
    }
    return t.ptr->getHash();
}


void FrequencyTeacher::copy(const FrequencyTeacher& other){
    for(auto& key: other.map){
        auto newTree = new ParseTree(*key.first.ptr);
        TreePointer treePtr(newTree);
        int pos = key.second.first;
        int neg = key.second.second;
        map[treePtr] = {pos,neg};
    }
}

void FrequencyTeacher::clear(){
    for(auto& key: map){
        if(key.first.ptr!=nullptr){
            delete(key.first.ptr);
        }
    }
}