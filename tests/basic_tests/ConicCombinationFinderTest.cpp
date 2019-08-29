#include "gtest/gtest.h"
#include "../../include/ConicCombinationFinder.h"
#include <armadillo>
#include <iostream>

using namespace std;



TEST(conic_combination_finder_test,basic_check){
    arma::mat m(3, 5);
    m(0, 0)=3;m(0, 1)=0; m(0, 2)=0; m(0, 3)=3; m(0, 4)=6;
    m(1, 0)=3;m(1, 1)=1; m(1, 2)=0; m(1, 3)=2; m(1, 4)=5;
    m(2, 0)=3;m(2, 1)=0; m(2, 2)=1; m(2, 3)=2; m(2, 4)=4;
    ConicCombinationFinder finder(m);
    finder.solve(0);
    ConicCombinationFinder::Status s = finder.getStatus();
    ASSERT_EQ(s, ConicCombinationFinder::SOLVED);
    finder.solve(1);
    s = finder.getStatus();
    ASSERT_EQ(s, ConicCombinationFinder::NO_SOL);
    finder.solve(2);
    s = finder.getStatus();
    ASSERT_EQ(s, ConicCombinationFinder::NO_SOL);
    finder.solve(3);
    s = finder.getStatus();
    ASSERT_EQ(s, ConicCombinationFinder::NO_SOL);
    finder.solve(4);
    s = finder.getStatus();
    ASSERT_EQ(s, ConicCombinationFinder::SOLVED);
}
