//
// Created by dolavn on 7/5/19.
//

#include "gtest/gtest.h"
#include "../../include/IndexArray.h"

#include <vector>

using namespace std;

TEST(index_array_test,basic_check){
    IndexArray ind({2, 2});
    vector<int> indices_0 = {0, 0, 1, 1, 2};
    vector<int> indices_1 = {0, 1, 0, 1, 0};
    vector<int> indices[] = {indices_0, indices_1};
    int iter=0;
    for(;!ind.getOverflow();++ind){
        for(int i=0;i<2;++i){
            ASSERT_EQ(indices[i][iter], ind.get(i));
        }
        iter++;
    }
}