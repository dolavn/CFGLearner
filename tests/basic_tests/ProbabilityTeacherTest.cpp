#include "gtest/gtest.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/MultiplicityTeacher.h"
#include "../../include/MultiLinearMap.h"
#include "../../include/TreeComparator.h"


#define EPS 0.01

using namespace std;

TEST(probability_teacher_test,basic_check){
    DuplicationComparator cmp;
    ProbabilityTeacher teacher(cmp, 0.5, EPS);
    ParseTree t1(0, {ParseTree(1), ParseTree(2)});
    t1.setProb(0.5);
    teacher.addExample(t1);
    ASSERT_EQ(teacher.membership(t1), 0.25);
    ParseTree t2(0, {ParseTree(0, {ParseTree(1), ParseTree(1)}), ParseTree(2)});
    ParseTree t3(0, {ParseTree(1), ParseTree(0, {ParseTree(2), ParseTree(2)})});
    ASSERT_EQ(teacher.membership(t2), 0.125);
    ASSERT_EQ(teacher.membership(t3), 0.125);
}