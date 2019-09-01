#include "gtest/gtest.h"
#include "../../include/ConicCombinationFinder.h"
#include <armadillo>
#include <iostream>

using namespace std;

bool test_solution(arma::mat& m, arma::vec v, int ind){
    if(m.n_cols!=v.n_rows+1){return false;}
    for(int j=0;j<v.n_rows;++j){
        if(v(j)<0){
            return false;
        }
    }
    for(int i=0;i<m.n_rows;++i){
        double curr=0;
        int vInd=0;
        for(int j=0;j<m.n_cols;++j){
            if(j==ind){continue;}
            curr = curr + m(i,j)*v(vInd);
            vInd++;
        }
        if(curr!=m(i,ind)){
            return false;
        }
    }
    return true;
}

TEST(conic_combination_finder_test,basic_check){
    arma::mat m(3, 5);
    m(0, 0)=3;m(0, 1)=0; m(0, 2)=0; m(0, 3)=3; m(0, 4)=6;
    m(1, 0)=3;m(1, 1)=1; m(1, 2)=0; m(1, 3)=2; m(1, 4)=5;
    m(2, 0)=3;m(2, 1)=0; m(2, 2)=1; m(2, 3)=2; m(2, 4)=4;
    ConicCombinationFinder finder(m);
    bool sol[] = {true, false, false, false, true};
    for(int i=0;i<5;++i){
        finder.solve(i);
        auto target = sol[i]?ConicCombinationFinder::SOLVED:ConicCombinationFinder::NO_SOL;
        ASSERT_EQ(finder.getStatus(), target);
        if(sol[i]){
            ASSERT_EQ(true, test_solution(m, finder.getSolution(), i));
        }else{
            ASSERT_ANY_THROW(finder.getSolution());
        }
    }
}
