#include "gtest/gtest.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/MultiplicityTeacher.h"
#include "../../include/MultiLinearMap.h"
#include "../../include/TreeComparator.h"

using namespace std;

TEST(multilplicity_difference_teacher_test,basic_check){
    SwapComparator cmp(4, 1);
    DifferenceMultiplicityTeacher teacher(cmp, 2, 0.25, 0.1);
    ParseTree t1(0, {ParseTree(1), ParseTree(2)});
    ParseTree t2(0, {ParseTree(2), ParseTree(1)});
    ParseTree t3(0, {ParseTree(2), ParseTree(2)});
    t1.setProb(0.5);
    teacher.addExample(t1);
    ASSERT_EQ(teacher.membership(t1), 0.5);
    ASSERT_EQ(teacher.membership(t2), 0.25);
    ASSERT_ANY_THROW(teacher.addExample(t1));
    ASSERT_ANY_THROW(teacher.addExample(t3));
}

TEST(multilplicity_difference_teacher_test,equivalence_check){
    SwapComparator cmp(4, 1);
    DifferenceMultiplicityTeacher teacher(cmp, 2, 0.25, 0.01);
    ParseTree t1(0, {ParseTree(1), ParseTree(2)});
    ParseTree t2(0, {ParseTree(2), ParseTree(1)});
    t1.setProb(0.5);
    teacher.addExample(t1);
    ASSERT_EQ(teacher.membership(t1), 0.5);
    set<rankedChar> alphabet = teacher.getAlphabet();
    MultiplicityTreeAcceptor acc(alphabet, 3);
    acc.setLambda({0,0,0.5});
    MultiLinearMap m1(3, 0);
    m1.setParam(1.0,{0});
    MultiLinearMap m2(3, 0);
    m2.setParam(1.0,{1});
    MultiLinearMap m0(3, 2);
    m0.setParam(1.0, {2, 0, 1});  m0.setParam(0.4, {2, 1, 0});
    acc.addTransition(m1, rankedChar{1, 0});
    acc.addTransition(m2, rankedChar{2, 0});
    acc.addTransition(m0, rankedChar{0, 2});
    ASSERT_EQ(teacher.equivalence(acc), nullptr);
    teacher.membership(t2);
    ParseTree* counterExample = teacher.equivalence(acc);
    ASSERT_NE(counterExample, nullptr);
    ASSERT_EQ(*counterExample, t2);
    delete(counterExample);
    m0.setParam(0.5, {2, 1, 0});
    acc.addTransition(m0, rankedChar{0, 2});
    ASSERT_EQ(teacher.equivalence(acc), nullptr);
}

TEST(multiplicity_difference_teacher_test, copy_test){
    SwapComparator cmp(4, 1);
    auto teacher = new DifferenceMultiplicityTeacher(cmp, 2, 0.25, 0.1);
    ParseTree t1(0, {ParseTree(1), ParseTree(2)});
    ParseTree t2(0, {ParseTree(2), ParseTree(1)});
    t1.setProb(0.5);
    teacher->addExample(t1);
    DifferenceMultiplicityTeacher teacher2 = *teacher;
    delete(teacher);
    ASSERT_EQ(teacher2.membership(t1), 0.5);
    ASSERT_EQ(teacher2.membership(t2), 0.25);
}
