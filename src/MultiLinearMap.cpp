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

MultiLinearMap::MultiLinearMap(int d, int p):d(d),p(p){
    initParams();
}


floatVec MultiLinearMap::operator()(const vector<floatVec>& input) {
    floatVec y(d);
    intVec maxLengths(p+1, d);
    IndexArray ind(maxLengths);
    for(;!ind.getOverflow();++ind){
        int i = ind[0];
        float param = params[convertInd(ind)];
        float toAdd = params[convertInd(ind)];
        for(int j=1;j<p+1;++j){
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

int MultiLinearMap::convertInd(const IndexArray& index){
    int ind = 0;
    int currFactor = 1;
    for (int i = p; i >= 0; i--) {
        ind = ind + currFactor*index.get(i);
        currFactor = currFactor*index.getMax(i);
    }
    return ind;
}

int MultiLinearMap::convertInd(const intVec& index){
    int ind = 0;
    int currFactor = 1;
    for (int i = p; i >= 0; i--) {
        ind = ind + currFactor*index[i];
        currFactor = currFactor*d;
    }
    return ind;
}


void MultiLinearMap::initParams(){
    params = floatVec((unsigned long)(pow(d, p+1)));
    intVec maxLengths(p+1, d);
    IndexArray ind(maxLengths);
    for(;!ind.getOverflow();++ind){
        params[convertInd(ind)] = 0;
    }
}


bool MultiLinearMap::testLocation(const intVec& ind){
    if(ind.size()!=p+1){
        return false;
    }
    for(int i=0;i<p+1;++i){
        if(ind[i]>=d){
            return false;
        }
    }
    return true;
}
