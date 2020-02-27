#include "TreesGenerator.h"


using namespace std;

PartitionsIterator::PartitionsIterator():elements(0),sets(0),maxInSet(-1),freeIndex(-1),indices(){
    checkState();
}

PartitionsIterator::PartitionsIterator(int elements, int sets):elements(elements),sets(sets),maxInSet(-1),
freeIndex(-1),indices(vector<int>(sets-1, elements+1)){
    checkState();
}

PartitionsIterator::PartitionsIterator(int elements, int sets, int maxInSet):elements(elements),sets(sets),
maxInSet(maxInSet),freeIndex(-1),indices(vector<int>(sets-1, elements+1)){
    checkState();
    while(!testIndexVec(indices)){
        ++indices;
    }
}

PartitionsIterator::PartitionsIterator(int elements, int sets, int maxInSet, int freeIndex):elements(elements),
sets(sets),maxInSet(maxInSet),freeIndex(freeIndex),indices(vector<int>(sets-1, elements+1)){
    checkState();
    while(!testIndexVec(indices)){
        ++indices;
    }
}

bool PartitionsIterator::testIndexVec(const IndexArray& ind){ /*Tests that this is a valid index vector for a partition */
    if(ind.getOverflow()){
        return true;
    }
    if(sets==1){
        return true;
    }
    if(maxInSet!=-1 && freeIndex!=0){
        if(ind.get(0)>maxInSet){
            return false;
        }
    }
    for(int i=0;i<ind.getDimensions()-1;++i){
        if(ind.get(i)>ind.get(i+1)){
            return false;
        }
        if(maxInSet!=-1 && freeIndex!=i+1 && ind.get(i+1)-ind.get(i)>maxInSet){
            return false;
        }
    }
    if(maxInSet!=-1 && freeIndex!=ind.getDimensions()){
        if(elements-ind.get(ind.getDimensions()-1)>maxInSet){
            return false;
        }
    }
    return true;
}

PartitionsIterator& PartitionsIterator::operator++(){
    do{
        ++indices;
    }while(!testIndexVec(indices));
}

bool PartitionsIterator::hasNext() const{
    return !indices.getOverflow();
}

PartitionsIterator::partition PartitionsIterator::operator*(){
    vector<int> ans(sets);
    int curr = 0;
    for(int i=0;i<sets-1;++i){
        ans[i] = indices[i]-curr;
        curr = indices[i];
    }
    ans[sets-1] = elements-curr;
    return ans;
}

#define ASSERT(cond, err)    if(!(cond)){throw invalid_argument(err);}

void PartitionsIterator::checkState(){
    ASSERT(elements>=0, "negative elements")
    ASSERT(sets>=0, "negative number of sets")
    ASSERT(maxInSet==-1 || maxInSet>0, "negative non -1 maxInSet")
    ASSERT(freeIndex==-1 || (freeIndex>=0 && freeIndex<sets), "freeIndex out of bounds")
}