#include "MultiLinearMap.h"
#include "IndexArray.h"
#include <iostream> //TODO: Delete
#include <math.h>

using namespace std;

void printVec(const floatVec& vec){ //TODO: Delete
    cout << "[";
    for(int k=0;k<vec.size();k++){
        cout << vec[k];
        if(k<vec.size()-1){
            cout << " , ";
        }
    }
    cout << "]" << endl;
}

MultiLinearMap::MultiLinearMap():dim(1),paramNum(0){
    initParams();
}

MultiLinearMap::MultiLinearMap(int dim, int paramNum):dim(dim),paramNum(paramNum){
    initParams();
}


floatVec MultiLinearMap::operator()(const vector<floatVec>& input) {
    if(!testInput(input)){
        throw std::invalid_argument("Invalid input dimensions");
    }
    floatVec y(dim);
    if(dim==0){return y;}
    intVec maxLengths(paramNum+1, dim);
    IndexArray ind(maxLengths);
    for(;!ind.getOverflow();++ind){
        int i = ind[0];
        float toAdd = params[convertInd(ind)];
        for(int j=1;j<paramNum+1;++j){
            toAdd = toAdd * input[j-1][ind[j]];
        }
        y[i] = y[i] + toAdd;
    }
    return y;
}

void MultiLinearMap::setParam(float param, const intVec& location){
    if(!testLocation(location)){
        throw std::invalid_argument("Parameters index out of bounds");
    }
    params[convertInd(location)] = param;
}

float MultiLinearMap::getParam(const intVec& location){
    if(!testLocation(location)){
        throw std::invalid_argument("Parameters index out of bounds");
    }
    return params[convertInd(location)];
}

int MultiLinearMap::convertInd(const IndexArray& index) const{
    int ind = 0;
    int currFactor = 1;
    for (int i = paramNum; i >= 0; i--) {
        ind = ind + currFactor*index.get(i);
        currFactor = currFactor*index.getMax(i);
    }
    return ind;
}

int MultiLinearMap::convertInd(const intVec& index) const{
    int ind = 0;
    int currFactor = 1;
    for (int i = paramNum; i >= 0; i--) {
        ind = ind + currFactor*index[i];
        currFactor = currFactor*dim;
    }
    return ind;
}


void MultiLinearMap::initParams(){
    if(dim==0){return;}
    params = floatVec((unsigned long)(pow(dim, paramNum+1)));
    intVec maxLengths(paramNum+1, dim);
    IndexArray ind(maxLengths);
    for(;!ind.getOverflow();++ind){
        params[convertInd(ind)] = 0;
    }
}


bool MultiLinearMap::testLocation(const intVec& ind){
    if(dim==0){
        return false;
    }
    if(ind.size()!=paramNum+1){
        return false;
    }
    for(int i=0;i<paramNum+1;++i){
        if(ind[i]>=dim){
            return false;
        }
    }
    return true;
}

bool MultiLinearMap::testInput(const vector<floatVec>& input){
    if(input.size()!=paramNum){
        return false;
    }
    for(int i=0;i<paramNum;++i){
        if(input[i].size()!=dim){
            return false;
        }
    }
    return true;
}

void MultiLinearMap::printDesc() const{
    cout << "output:" << dim << endl;
    cout << "paramNum:" << paramNum << endl;
    intVec maxLengths(paramNum+1, dim);
    IndexArray ind(maxLengths);
    for(;!ind.getOverflow();++ind){
        int i = ind[0];
        float toAdd = params[convertInd(ind)];
        cout << ind << " " << toAdd << endl;
    }
}
