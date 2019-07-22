#include "gtest/gtest.h"
#include "../../include/Matrix.h"
#include <pybind11/embed.h>


namespace py = pybind11;

using namespace std;

TEST(matrix_test,basic_check){
    py::initialize_interpreter();
    Matrix m(2,2);
    m[0][0] = 2;
    m[1][1] = 2;
    VectorNumpy v({1, 1});
    Matrix inv = m.getInverse();
    ASSERT_EQ(0.5, inv[0][0]);
    ASSERT_EQ(0.5, inv[1][1]);
    ASSERT_EQ(0, inv[0][1]);
    ASSERT_EQ(0, inv[1][0]);
    VectorNumpy v2 = m*v;
    ASSERT_EQ(v2[0], 2);
    ASSERT_EQ(v2[1], 2);
    py::finalize_interpreter();
}