//
// Created by dolavn on 1/10/20.
//

#include "gtest/gtest.h"
#include "../../include/TreeConstructor.h"
#include "../../include/ParseTree.h"

TEST(constructor_test,basic_check){
    scoreTable t;
    t[{1, 2}] = 5;
    t[{2, 2}] = 1;
    TreeConstructor c(t);
    Sequence s1({1, 2, 2});
    Sequence s2({2, 1, 2});
    c.createTree(s1);
    ParseTree ans1(0, {ParseTree(0, {ParseTree(1), ParseTree(2)}), ParseTree(2)});
    ParseTree ans2(0, {ParseTree(2), ParseTree(0, {ParseTree(1), ParseTree(2)})});
    ParseTree tree = c.getTree();
    ASSERT_EQ(tree, ans1);
    c.createTree(s2);
    tree = c.getTree();
    ASSERT_EQ(tree, ans2);
}
