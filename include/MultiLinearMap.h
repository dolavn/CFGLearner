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
    MultiLinearMap(int, int);
    floatVec operator()(const std::vector<floatVec>&);

    void setParam(float, const intVec&);
    float getParam(const intVec&);
private:
    void initParams();
    bool testLocation(const intVec&);
    int convertInd(const IndexArray&);
    int convertInd(const intVec&);

    int d,p;
    floatVec params;
};

#endif //CFGLEARNER_MULTILINEARMAP_H
