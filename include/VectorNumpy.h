//
// Created by dolavn@wincs.cs.bgu.ac.il on 7/21/19.
//

#ifndef CFGLEARNER_VECTORNUMPY_H
#define CFGLEARNER_VECTORNUMPY_H

#include <vector>

typedef long int ssize_t;

namespace pybind11{
    class object;
}

class VectorNumpy{
public:
    explicit VectorNumpy(ssize_t);
    explicit VectorNumpy(const std::vector<double>&);

    inline double& operator[](ssize_t ind){return data[ind];}
    inline double getValue(ssize_t ind) const{return data[ind];}
    pybind11::object getNpArr() const;
    void fillDataFromNpVec(pybind11::object&);
    inline ssize_t getSize() const{return size;}
private:
    ssize_t size;
    std::vector<double> data;

};

#endif //CFGLEARNER_VECTORNUMPY_H
