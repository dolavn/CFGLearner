#include "VectorNumpy.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/embed.h>


using namespace std;

namespace py = pybind11;

VectorNumpy::VectorNumpy(ssize_t size):size(size),data(size){

}

VectorNumpy::VectorNumpy(const vector<double>& data):size(data.size()),data(data){

}

py::object VectorNumpy::getNpArr() const{
    ssize_t ndim2 = 1;
    std::vector<double> arr2 = {1.0, 1.0};
    std::vector<ssize_t> shape2= {2};
    std::vector<ssize_t> strides2 = {sizeof(double)};
    auto ans2 = py::array(py::buffer_info(
            arr2.data(),                              /* data as contiguous array  */
            sizeof(double),                          /* size of one scalar        */
            py::format_descriptor<double>::format(), /* data type                 */
            ndim2,                                    /* number of dimensions      */
            shape2,                                   /* shape of the matrix       */
            strides2                                  /* strides for each axis     */
    ));
    return ans2;
}

void VectorNumpy::fillDataFromNpVec(py::object& arr) {
    py::array a = arr;
    auto p = (const double*)(a.data());
    for(int i=0;i<size;++i){
        double curr = *p;
        data[i] = curr;
        p++;
    }
}

