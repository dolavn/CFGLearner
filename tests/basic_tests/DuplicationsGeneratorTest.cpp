//
// Created by dolavn on 2/27/20.
//

#include "gtest/gtest.h"
#include "TreesGenerator.h"
#include "ParseTree.h"
#include "TestsHelperFunctions.h"
#include <vector>

using namespace std;

TEST(duplications_generator_test,basic_check){
    ParseTree t(1);
    vector<ParseTree> v = {t};
    DuplicationsGenerator gen(v, 1);
    vector<ParseTree> res = {t, ParseTree(0, {ParseTree(1), ParseTree(1)})};
    int ind=0;
    TEST_VEC(gen, res, ind);
    v = {t, ParseTree(0, {ParseTree(1), ParseTree(2)})};
    gen = DuplicationsGenerator(v, 1);
    res = {t, ParseTree(0, {ParseTree(1), ParseTree(1)}), ParseTree(0, {ParseTree(1), ParseTree(2)}),
           ParseTree(0, {ParseTree(1), ParseTree(0, {ParseTree(2), ParseTree(2)})}),
           ParseTree(0, {ParseTree(0, {ParseTree(1), ParseTree(1)}), ParseTree(2)}),
           ParseTree(0, {ParseTree(0, {ParseTree(1), ParseTree(1)}), ParseTree(0, {ParseTree(2), ParseTree(2)})})};
    TEST_VEC(gen, res, ind);
}

TEST(duplications_generator_test,depth_check){
    ParseTree t(1), t2(2);
    vector<ParseTree> v = {t};
    DuplicationsGenerator gen(v, 2);
    vector<ParseTree> res = {t, ParseTree(0, {t, t}), ParseTree(0, {ParseTree(0, {t, t}), t}),
                             ParseTree(0,{t, ParseTree(0, {t, t})})};
    int ind=0;
    TEST_VEC(gen, res, ind);
    v = {t, ParseTree(0, {t, t2})};
    gen = DuplicationsGenerator(v, 2);
    res = {t, ParseTree(0, {t, t}), ParseTree(0, {ParseTree(0, {t, t}), t}),
           ParseTree(0,{t, ParseTree(0, {t, t})}), ParseTree(0, {t, t2}),
           ParseTree(0, {t, ParseTree(0, {t2, t2})}),
           ParseTree(0, {t, ParseTree(0, {ParseTree(0, {t2, t2}), t2})}),
           ParseTree(0, {t, ParseTree(0, {t2, ParseTree(0, {t2, t2})})}),
           ParseTree(0, {ParseTree(0, {t, t}), t2}),
           ParseTree(0, {ParseTree(0, {t, t}), ParseTree(0, {t2, t2})}),
           ParseTree(0, {ParseTree(0, {t, t}), ParseTree(0, {ParseTree(0, {t2, t2}), t2})}),
           ParseTree(0, {ParseTree(0, {t, t}), ParseTree(0, {t2, ParseTree(0, {t2, t2})})}),
           ParseTree(0, {ParseTree(0, {ParseTree(0, {t, t}), t}), t2}),
           ParseTree(0, {ParseTree(0, {t, ParseTree(0, {t, t})}), t2}),
           ParseTree(0, {ParseTree(0, {ParseTree(0, {t, t}), t}), ParseTree(0, {t2, t2})}),
           ParseTree(0, {ParseTree(0, {t, ParseTree(0, {t, t})}), ParseTree(0, {t2, t2})}),
           ParseTree(0, {ParseTree(0, {ParseTree(0, {t, t}), t}), ParseTree(0, {ParseTree(0, {t2, t2}), t2})}),
           ParseTree(0, {ParseTree(0, {t, ParseTree(0, {t, t})}), ParseTree(0, {t2, ParseTree(0, {t2, t2})})})};
    TreesGenerator& treeGen = gen;
    TEST_VEC(treeGen, res, ind);
}

