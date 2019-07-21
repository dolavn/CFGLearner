#include "Matrix.h"
#include "VectorNumpy.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

using namespace std;
namespace py = pybind11;

py::object np = py::module::import("numpy");
py::object linalg = np.attr("linalg");
py::object inverse = linalg.attr("inv");

Matrix::Matrix(ssize_t shape1, ssize_t shape2):shape({shape1, shape2}), data(shape1, VectorNumpy(shape2)){
}

Matrix::Matrix(ssize_t shape1, ssize_t shape2, const vector<double>& data):shape({shape1, shape2}),
data(shape1, VectorNumpy(shape2)){
    fillData(data);
}

VectorNumpy& Matrix::operator[](ssize_t ind){
    return data[ind];
}

Matrix Matrix::getInverse(){
    py::object mat = get_np_matrix();
    py::object inv = inverse(mat);
    cout << inv.attr("__str__")() << endl;
    return Matrix(2,2);
}

py::object Matrix::get_np_matrix(){
    ssize_t ndim = 2;
    std::vector<double> arr;
    for(ssize_t i=0;i<shape[0];++i){
        for(ssize_t j=0;j<shape[1];++j){
            arr.push_back(data[i][j]);
        }
    }
    std::vector<ssize_t> strides = { sizeof(double), sizeof(double)*shape[0] };
    auto tmp = py::array(py::buffer_info(
            arr.data(),                             /* data as contiguous array  */
            sizeof(double),                          /* size of one scalar        */
            py::format_descriptor<double>::format(), /* data type                 */
            ndim,                                    /* number of dimensions      */
            shape,                                   /* shape of the matrix       */
            strides                                  /* strides for each axis     */
    ));
}

void Matrix::fillData(const vector<double>& newData){
    ssize_t curr_ind = 0;
    for(ssize_t i=0;i<shape[0];++i){
        for(ssize_t j=0;j<shape[1];++j){
            data[i][j] = newData[curr_ind++];
        }
    }
}