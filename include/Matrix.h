//
// Created by dolavn@wincs.cs.bgu.ac.il on 7/21/19.
//

#ifndef CFGLEARNER_MATRIX_H
#define CFGLEARNER_MATRIX_H

#include <vector>
#include "VectorNumpy.h"

namespace pybind11{
    class object;
}

typedef long int ssize_t;

class Matrix{
public:
    Matrix(ssize_t,ssize_t);
    Matrix(ssize_t, ssize_t, const std::vector<double>&);

    VectorNumpy operator*(const VectorNumpy&);
    VectorNumpy& operator[](ssize_t ind);
    Matrix getInverse();
private:
    std::vector<ssize_t> shape;
    std::vector<VectorNumpy> data;
    pybind11::object get_np_matrix();
    void fillDataFromNpMat(pybind11::object&);
    void fillData(const std::vector<double>&);
};

#endif //CFGLEARNER_MATRIX_H
