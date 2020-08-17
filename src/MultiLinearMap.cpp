#include "MultiLinearMap.h"
#include "IndexArray.h"
#include <iostream> //TODO: Delete
#include <math.h>

using namespace std;

void printVec(const floatVec& vec){ //TODO: Delete
    cout << "[";
    for(unsigned int k=0;k<vec.size();k++){
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

float MultiLinearMap::getParam(const intVec& location) const{
    if(!testLocation(location)){
        cout << "get [";
        for(int i=0;i<location.size();++i){
            cout << location[i];
            if(i<location.size()-1){cout << ", ";}
        }
        cout << "]" << endl;
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


bool MultiLinearMap::testLocation(const intVec& ind) const{
    if(dim==0){
        cout << "err1" << endl;
        return false;
    }
    if(ind.size()!=(unsigned int)(paramNum+1)){
        cout << ind.size() << endl;
        cout << "p:" << paramNum << endl;
        cout << "err2" << endl;
        return false;
    }
    for(int i=0;i<paramNum+1;++i){
        if(ind[i]>=dim){
            cout << "err3" << endl;
            return false;
        }
    }
    return true;
}

bool MultiLinearMap::testInput(const vector<floatVec>& input){
    if(input.size()!=(unsigned int)paramNum){
        return false;
    }
    for(int i=0;i<paramNum;++i){
        if(input[i].size()!=(unsigned int)dim){
            return false;
        }
    }
    return true;
}

MultiLinearMap MultiLinearMap::removeDimension(int dimension) const{
    MultiLinearMap ans(dim-1, paramNum);
    IndexArray arr(vector<int>(paramNum+1, dim));
    while(!arr.getOverflow()){
        vector<int> newParamLocation;
        for(int i=0;i<paramNum+1;++i){
            if(arr[i]==dimension){continue;}
            if(arr[i]>dimension){
                newParamLocation.push_back(arr[i]-1);
            }else{
                newParamLocation.push_back(arr[i]);
            }
        }
        if(newParamLocation.size()!=paramNum+1){ ++arr;continue;}
        ans.setParam(getParam(arr.getIntVector()), newParamLocation);
        ++arr;
    }
    return ans;
}

void MultiLinearMap::printDesc() const{
    if(dim==0){return;}
    intVec maxLengths(paramNum+1, dim);
    IndexArray ind(maxLengths);
    for(;!ind.getOverflow();++ind){
        //int i = ind[0];
        float toAdd = params[convertInd(ind)];
        cout << ind << " " << toAdd << endl;
    }
}
