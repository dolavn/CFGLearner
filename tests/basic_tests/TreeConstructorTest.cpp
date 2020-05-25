//
// Created by dolavn on 1/10/20.
//

#include "gtest/gtest.h"
#include "../../include/TreeConstructor.h"
#include "../../include/ParseTree.h"

#define EPSILON 0.00001

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
    ASSERT_NEAR(score, 2, EPSILON);
    table[{4, 4}] = 0.0f;
    table[{4, 2}] = 0.0f;
    table[{4, 4, 4}] = 0.0f;
    table[{4, 4, 2}] = 0.0f;
    c = TreeConstructor(table);
    s1 = Sequence({4, 4, 4, 2});
    score = c.createTree(s1);
    ASSERT_NEAR(score, 0, EPSILON);
    ParseTree tree = c.getTree();
}

TEST(constructor_test,concat_test){
    scoreTable table;
    table[{1, 2}] = 5.0f;
    TreeConstructor c(table);
    c.setConcat(true);
    c.setLambda(1.0f);
    Sequence s({1, 1, 2});
    float score = c.createTree(s);
    ASSERT_NEAR(score, 5, EPSILON);
    ParseTree expected(0, {ParseTree(0, {ParseTree(1), ParseTree(1)}), ParseTree(2)});
    ASSERT_EQ(c.getTree(), expected);
    Sequence s2({1, 1, 1, 2});
    score = c.createTree(s2);
    expected = ParseTree(0, {ParseTree(0, {ParseTree(1), ParseTree(0, {ParseTree(1), ParseTree(1)})}), ParseTree(2)});
    ASSERT_NEAR(score, 5, EPSILON);
    ASSERT_EQ(c.getTree(), expected);
    Sequence s3({1, 1, 1, 2, 1, 2});
    score = c.createTree(s3);
    expected = ParseTree(0, {ParseTree(0, {ParseTree(0, {ParseTree(1), ParseTree(0, {ParseTree(1), ParseTree(1)})}),
                                           ParseTree(2)}), ParseTree(0, {ParseTree(1), ParseTree(2)})});
    c.getTree();
    ASSERT_NEAR(score, 10, EPSILON);
    ASSERT_EQ(c.getTree(), expected);
    c.setConcat(false);
    score = c.createTree(s);
    ASSERT_NEAR(score, 5, EPSILON);
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








