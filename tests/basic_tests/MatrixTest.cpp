#include "gtest/gtest.h"
#include "../../include/Matrix.h"

using namespace std;

TEST(matrix_test,basic_check){
    Matrix m(2,2);
    m.getInverse();
}