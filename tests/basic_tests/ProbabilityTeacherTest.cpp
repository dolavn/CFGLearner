#include "gtest/gtest.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/MultiplicityTeacher.h"
#include "../../include/MultiLinearMap.h"
#include "../../include/TreeComparator.h"
#include "../../include/TreesGenerator.h"


#define EPS 0.01

using namespace std;

TEST(probability_teacher_test,basic_check){
    DuplicationComparator cmp;
    ProbabilityTeacher teacher(cmp, 0.5, EPS);
    ParseTree t1(0, {ParseTree(1), ParseTree(2)});
    t1.setProb(0.5);
    teacher.addExample(t1);
    ASSERT_EQ(teacher.membership(t1), 0.5);
    ParseTree t2(0, {ParseTree(0, {ParseTree(1), ParseTree(1)}), ParseTree(2)});
    ParseTree t3(0, {ParseTree(1), ParseTree(0, {ParseTree(2), ParseTree(2)})});
    ASSERT_EQ(teacher.membership(t2), 0.25);
    ASSERT_EQ(teacher.membership(t3), 0.25);
}

TEST(probability_teacher_test,equiv_test){
    DuplicationComparator cmp;
    ProbabilityTeacher teacher(cmp, 0.5, EPS);
    ParseTree t1(0, {ParseTree(1), ParseTree(2)});
    t1.setProb(0.5);
    teacher.addExample(t1);
    teacher.setupDuplicationsGenerator(1);
    set<rankedChar> alphabet = {{0, 2}, {1, 0}, {2, 0}};
    MultiplicityTreeAcceptor acc(alphabet, 3);
    acc.setLambda({0, 0, 0.5});
    MultiLinearMap m_l1(3, 0), m_l2(3, 0), m_inner(3, 2);
    m_l1.setParam(1.0, {0}); m_l2.setParam(1.0, {1});
    m_inner.setParam(1.0, {2,0,1});
    acc.addTransition(m_l1, {1, 0}); acc.addTransition(m_l2, {2, 0});
    acc.addTransition(m_inner, {0, 2});
    ParseTree* counter = teacher.equivalence(acc);
    ASSERT_TRUE(counter!=nullptr);
    SAFE_DELETE(counter);
    acc = MultiplicityTreeAcceptor(alphabet, 5);
    acc.setLambda({0, 0, 0, 0, 0.5});
    m_l1 = MultiLinearMap(5, 0); m_l2 = MultiLinearMap(5, 0);
    m_inner = MultiLinearMap(5 ,2);
    m_l1.setParam(1.0, {0}); m_l1.setParam(1.0, {2});
    m_l2.setParam(1.0, {1}); m_l2.setParam(1.0, {3});
    m_inner.setParam(0.5, {0, 2, 0});
    m_inner.setParam(0.5, {1, 3, 1});
    m_inner.setParam(1.0, {4, 0, 1});
    acc.addTransition(m_l1, {1, 0}); acc.addTransition(m_l2, {2, 0});
    acc.addTransition(m_inner, {0, 2});
    counter = teacher.equivalence(acc);
    ASSERT_TRUE(counter==nullptr);
}