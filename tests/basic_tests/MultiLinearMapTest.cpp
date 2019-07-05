//
// Created by dolavn on 7/5/19.
//

#include "gtest/gtest.h"
#include "../../include/MultiLinearMap.h"

using namespace std;

TEST(multi_linear_map_test,basic_check){
    MultiLinearMap m(1, 1);
    for(int i=0;i<100;++i){
        auto currFloat = (float)(i);
        m.setParam(currFloat, intVec({0, 0}));
        ASSERT_EQ(floatVec({currFloat}), m({{1.0}}));
    }
}

TEST(multi_linear_map_test,basic_check_2scalars){
    MultiLinearMap m(1, 2); //v1*v2
    m.setParam(1.0, intVec({0, 0, 0}));
    for(int i=0;i<100;++i){
        for(int j=0;j<100;++j){
            auto iFloat = (float)(i);
            auto jFloat = (float)(j);
            ASSERT_EQ(floatVec({iFloat*jFloat}), m({{iFloat},{jFloat}}));
        }
    }
}

TEST(multi_linear_map_test,2d_basic_check){
    MultiLinearMap m1(2, 1); //Identity
    m1.setParam(1.0, intVec({0, 0})); m1.setParam(1.0, intVec({1, 1}));
    MultiLinearMap m2(2, 1); //Switch X, Y
    m2.setParam(1.0, intVec({0, 1})); m2.setParam(1.0, intVec({1, 0}));
    MultiLinearMap m3(2, 1); //2*Identity
    m3.setParam(2.0, intVec({0, 0})); m3.setParam(2.0, intVec({1, 1}));
    MultiLinearMap m4(2, 1); //(x+y,y-x)
    m4.setParam(1.0, intVec({0, 0})); m4.setParam(1.0, intVec({0, 1}));
    m4.setParam(-1.0f, intVec({1, 0})); m4.setParam(1.0f, intVec({1, 1}));
    for(int i=0;i<100;++i){
        for(int j=0;j<100;++j){
            auto iFloat = (float)(i);
            auto jFloat = (float)(j);
            floatVec currVec = floatVec({iFloat, jFloat});
            ASSERT_EQ(currVec, m1({currVec}));
            ASSERT_EQ(floatVec({jFloat, iFloat}), m2({currVec}));
            ASSERT_EQ(floatVec({2*iFloat, 2*jFloat}), m3({currVec}));
            ASSERT_EQ(floatVec({iFloat+jFloat, jFloat-iFloat}), m4({currVec}));
        }
    }
}

TEST(multi_linear_map_test,2d_two_vecs){
    MultiLinearMap m1(2, 2); //Element-wise-multiplication
    m1.setParam(1.0, intVec({0, 0, 0}));
    m1.setParam(1.0, intVec({1, 1, 1}));
    MultiLinearMap m2(2, 2); //(a[0]*b[1],a[1]*b[0])
    m2.setParam(1.0, intVec({0, 0, 1}));
    m2.setParam(1.0, intVec({1, 1, 0}));
    MultiLinearMap m3(2, 2); //(a[0]*b[0]+a[1]*b[1],0)
    m3.setParam(1.0, intVec({0, 0, 0}));
    m3.setParam(1.0, intVec({0, 1, 1}));
    MultiLinearMap m4(2, 2); //(a[0]*b[0]+a[1]*b[1],a[0]*b[0]+a[1]*b[1])
    m4.setParam(1.0, intVec({0, 0, 0}));
    m4.setParam(1.0, intVec({0, 1, 1}));
    m4.setParam(1.0, intVec({1, 0, 0}));
    m4.setParam(1.0, intVec({1, 1, 1}));
    for(int i=0;i<10;++i){
        for(int j=0;j<10;++j){
            for(int k=0;k<10;++k){
                for(int p=0;p<10;++p){
                    auto iFloat = (float)(i);
                    auto jFloat = (float)(j);
                    auto kFloat = (float)(k);
                    auto pFloat = (float)(p);
                    floatVec currVec1 = floatVec({iFloat, jFloat});
                    floatVec currVec2 = floatVec({kFloat, pFloat});
                    ASSERT_EQ(floatVec({iFloat*kFloat, jFloat*pFloat}), m1({currVec1, currVec2}));
                    ASSERT_EQ(floatVec({iFloat*pFloat, jFloat*kFloat}), m2({currVec1, currVec2}));
                    ASSERT_EQ(floatVec({iFloat*kFloat+jFloat*pFloat,0}), m3({currVec1, currVec2}));
                    ASSERT_EQ(floatVec({iFloat*kFloat+jFloat*pFloat,
                                        iFloat*kFloat+jFloat*pFloat}), m4({currVec1, currVec2}));
                }
            }
        }
    }
}

TEST(multi_linear_map_test,exception_test){
    MultiLinearMap m(1, 1);
    ASSERT_THROW(m.setParam(1.0, intVec({1, 1})), invalid_argument);
}