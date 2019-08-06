//
// Created by dolavn on 7/5/19.
//

#ifndef CFGLEARNER_MULTILINEARMAP_H
#define CFGLEARNER_MULTILINEARMAP_H

#include <vector>

typedef std::vector<float> floatVec;
typedef std::vector<int> intVec;


class IndexArray;

class MultiLinearMap{
public:
    MultiLinearMap();
    MultiLinearMap(int, int);
    floatVec operator()(const std::vector<floatVec>&);

    void setParam(float, const intVec&);
    inline int getVDim() const{return dim;};
    inline int getParamNum() const{return paramNum;};
    float getParam(const intVec&);
    void printDesc() const;
private:
    void initParams();
    bool testLocation(const intVec&);
    bool testInput(const std::vector<floatVec>&);
    int convertInd(const IndexArray&) const;
    int convertInd(const intVec&) const;

    int dim,paramNum;
    floatVec params;
};

#endif //CFGLEARNER_MULTILINEARMAP_H
