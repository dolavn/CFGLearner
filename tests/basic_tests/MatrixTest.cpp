#include "gtest/gtest.h"
#include "../../include/Matrix.h"
#include <armadillo>

using namespace std;


TEST(matrix_test, basic_check){
    arma::mat a = arma::zeros(2, 2);
    a(0, 0) = 1;
    a(1, 1) = 1;
    ASSERT_EQ(2, arma::rank(a));
}

/*

TEST(matrix_test, basic_check2){
    py::initialize_interpreter();
    Matrix m(2,2);
    m[0][0] = 2;
    m[1][1] = 2;
    ASSERT_EQ(m.getRank(),2);
    VectorNumpy v({1, 1});
    Matrix inv = m.getInverse();
    ASSERT_EQ(0.5, inv[0][0]);
    ASSERT_EQ(0.5, inv[1][1]);
    ASSERT_EQ(0, inv[0][1]);
    ASSERT_EQ(0, inv[1][0]);
    VectorNumpy v2 = m*v;
    ASSERT_EQ(v2[0], 2);
    ASSERT_EQ(v2[1], 2);
    VectorNumpy v_mat1({1, 0});
    VectorNumpy v_mat2({2, 0});
    Matrix m2({v_mat1, v_mat2});
    ASSERT_EQ(m2.getRank(),1);
    VectorNumpy v_mat11({1, 0});
    VectorNumpy v_mat12({2, 1});
    Matrix m3({v_mat11, v_mat12});
    ASSERT_EQ(m3.getRank(),2);
    py::finalize_interpreter();
}
 */