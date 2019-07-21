//
// Created by dolavn@wincs.cs.bgu.ac.il on 7/21/19.
//

#ifndef CFGLEARNER_VECTORNUMPY_H
#define CFGLEARNER_VECTORNUMPY_H

#include <vector>

typedef long int ssize_t;

class VectorNumpy{
public:
    explicit VectorNumpy(ssize_t);
    explicit VectorNumpy(std::vector<double>);

    inline double& operator[](ssize_t ind){return data[ind];}
private:
    ssize_t size;
    std::vector<double> data;
};

#endif //CFGLEARNER_VECTORNUMPY_H
