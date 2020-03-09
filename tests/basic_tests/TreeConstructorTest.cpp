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
    float score = c.createTree(s1);
    ASSERT_EQ(score, 5);
    ParseTree ans1(0, {ParseTree(0, {ParseTree(1), ParseTree(2)}), ParseTree(2)});
    ParseTree ans2(0, {ParseTree(2), ParseTree(0, {ParseTree(1), ParseTree(2)})});
    ParseTree tree = c.getTree();
    ASSERT_EQ(tree, ans1);
    score = c.createTree(s2);
    ASSERT_EQ(score, 5);
    tree = c.getTree();
    ASSERT_EQ(tree, ans2);
}

TEST(constructor_test,advanced_check){
    scoreTable table;
    table[{1, 1}] = 1.0f;
    table[{1, 2}] = 2.0f;
    table[{2, 1}] = 2.0f;
    table[{2, 2}] = 0.0f;
    TreeConstructor c(table);
    Sequence s1({1, 2, 2, 2, 2});
    float score = c.createTree(s1);
    ASSERT_EQ(score, 2);
}

TEST(constructor_test,lambda_param_basic_test){
    scoreTable t;
    t[{1, 2}] = 10;
    t[{1, 2, 3}] = 6;
    t[{3, 4}] = 4;
    TreeConstructor c(t);
    c.setLambda(1.0);
    Sequence s1({1, 2, 3, 4});
    float score = c.createTree(s1);
    ASSERT_EQ(score, 16);
    ParseTree ans1(0, {ParseTree(0, {ParseTree(0, {ParseTree(1), ParseTree(2)}), ParseTree(3)}),
                       ParseTree(4)});
    ParseTree tree = c.getTree();
    ASSERT_EQ(tree, ans1);
    c.setLambda(0.5);
    ParseTree ans2(0, {ParseTree(0, {ParseTree(1), ParseTree(2)}),
                       ParseTree(0, {ParseTree(3), ParseTree(4)})});
    score = c.createTree(s1);
    ASSERT_EQ(score, 14);
    tree = c.getTree();
    ASSERT_EQ(tree, ans2);
}




