#include "gtest/gtest.h"
#include "../../include/TreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/Teacher.h"


using namespace std;


TEST(difference_teacher_test,basic_check){
    ParseTree t(2);
    ParseTree t2(3);
    t2.setWeight(5);
    DifferenceTeacher teacher;
    teacher.addPositiveExample(t);
    ASSERT_EQ(teacher.membership(t),true);
    ASSERT_EQ(teacher.membership(t2),false);
    teacher.addNegativeExample(t2);
    ASSERT_EQ(teacher.membership(t2),false);
    DifferenceTeacher teacher2(10);
    teacher2.addPositiveExample(t);
    ASSERT_EQ(teacher2.membership(t),true);
    ASSERT_EQ(teacher2.membership(t2),true);
    teacher2.addNegativeExample(t2);
    ASSERT_EQ(teacher2.membership(t2),false);
}

TEST(difference_teacher_test,learn_test){
    ParseTree t(0), w(5);
    t.setSubtree(ParseTree(1), 0);
    w.setSubtree(ParseTree(1), 0);
    t.setSubtree(ParseTree(0, {ParseTree(1), ParseTree(2)}), 1);
    w.setSubtree(ParseTree(4, {ParseTree(2), ParseTree(2)}), 1);
    t.applyWeights(w);
    ParseTree t2(0), w2(5);
    t2.setSubtree(ParseTree(0, {ParseTree(2), ParseTree(2)}), 0);
    w2.setSubtree(ParseTree(1, {ParseTree(1), ParseTree(1)}), 0);
    t2.setSubtree(ParseTree(0, {ParseTree(1), ParseTree(2)}), 1);
    w2.setSubtree(ParseTree(4, {ParseTree(2), ParseTree(2)}), 1);
    t2.applyWeights(w2);
    ParseTree tTest(0), wTest(5);
    tTest.setSubtree(ParseTree(0, {ParseTree(1), ParseTree(1)}), 0);
    wTest.setSubtree(ParseTree(1, {ParseTree(1), ParseTree(1)}), 0);
    tTest.setSubtree(ParseTree(0, {ParseTree(1), ParseTree(2)}), 1);
    wTest.setSubtree(ParseTree(4, {ParseTree(2), ParseTree(2)}), 1);
    tTest.applyWeights(wTest);
    ParseTree tTest2(0), wTest2(5);
    tTest2.setSubtree(ParseTree(0, {ParseTree(2), ParseTree(2)}), 0);
    wTest2.setSubtree(ParseTree(1, {ParseTree(1), ParseTree(1)}), 0);
    tTest2.setSubtree(ParseTree(0, {ParseTree(2), ParseTree(2)}), 1);
    wTest2.setSubtree(ParseTree(4, {ParseTree(2), ParseTree(2)}), 1);
    tTest2.applyWeights(wTest2);
    DifferenceTeacher teacher(5);
    teacher.addPositiveExample(t);
    teacher.addPositiveExample(t2);
}