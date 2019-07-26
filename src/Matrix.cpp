#include "Matrix.h"
#include "VectorNumpy.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <iostream>
#include <armadillo>

using namespace std;
using namespace arma;
namespace py = pybind11;


Matrix::Matrix(ssize_t shape1, ssize_t shape2):shape({shape1, shape2}), data(shape1, VectorNumpy(shape2)){
}

Matrix::Matrix(ssize_t shape1, ssize_t shape2, const vector<double>& data):shape({shape1, shape2}),
data(shape1, VectorNumpy(shape2)){
    fillData(data);
}

Matrix::Matrix(const vector<VectorNumpy>& vec):shape({0, 0}), data(){
    if(!checkVec(vec)){
        throw std::invalid_argument("Dimensions of input vector are incorrect");
    }
    shape[0] = vec.size();
    shape[1] = vec[0].getSize();
    data = vec;
}

VectorNumpy& Matrix::operator[](ssize_t ind){

    return data[ind];
}

VectorNumpy Matrix::operator*(const VectorNumpy& v){
    VectorNumpy ans(shape[0]);
    py::object np = py::module::import("numpy");
    py::object np_vec = v.getNpArr();
    py::object dot = np.attr("dot");
    py::object mat = get_np_matrix();
    py::object v_ans = dot(mat, np_vec);
    ans.fillDataFromNpVec(v_ans);
    return ans;
}

Matrix Matrix::getInverse(){
    Matrix ans(shape[0], shape[1]);
    py::object np = py::module::import("numpy");
    py::object linalg = np.attr("linalg");
    py::object inv = linalg.attr("inv");
    py::object mat = get_np_matrix();
    py::object inverse = inv(mat);
    ans.fillDataFromNpMat(inverse);
    return ans;
}

int Matrix::getRank(){
    py::object np = py::module::import("numpy");
    py::object linalg = np.attr("linalg");
    py::object matrix_rank = linalg.attr("matrix_rank");
    py::object mat = get_np_matrix();
    py::object rank = matrix_rank(mat);
    py::int_ ranki = rank;
    return ranki;
}

py::object Matrix::get_np_matrix(){
    ssize_t ndim = 2;
    std::vector<double> arr;
    for(ssize_t i=0;i<shape[0];++i){
        for(ssize_t j=0;j<shape[1];++j){
            arr.push_back(data[i][j]);
        }
    }
    ssize_t stride = shape[0];
    stride = stride*sizeof(double);
    std::vector<ssize_t> strides = { sizeof(double), stride };
    auto ans = py::array(py::buffer_info(
            arr.data(),                              /* data as contiguous array  */
            sizeof(double),                          /* size of one scalar        */
            py::format_descriptor<double>::format(), /* data type                 */
            ndim,                                    /* number of dimensions      */
            shape,                                   /* shape of the matrix       */
            strides                                  /* strides for each axis     */
    ));
    return ans;
}

void Matrix::fillData(const vector<double>& newData){
    ssize_t curr_ind = 0;
    for(ssize_t i=0;i<shape[0];++i){
        for(ssize_t j=0;j<shape[1];++j){
            data[i][j] = newData[curr_ind++];
        }
    }
}

void Matrix::fillDataFromNpMat(py::object& mat){
    py::array a = mat;
    auto p = (const double*)(a.data());
    for(int i=0;i<shape[0];++i){
        for(int j=0;j<shape[1];++j){
            double curr = *p;
            data[i][j] = curr;
            p++;
        }
    }
}

bool Matrix::checkVec(const vector<VectorNumpy>& vec){
    if(vec.empty()){
        return false;
    }
    ssize_t size = vec[0].getSize();
    for(auto& v: vec){
        if(v.getSize()!=size){
            return false;
        }
    }
    return true;
}

