#include "gtest/gtest.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/MultiplicityTeacher.h"
#include "../../include/TreesIterator.h"
#include "TestsHelperFunctions.h"

using namespace std;

TEST(functional_teacher_test,basic_check){
    ParseTree t(1);
    ParseTree t2(2);
    ParseTree t3(0, {t, t});
    ParseTree t4(0, {t2, t});
    ParseTree t5(0, {t4, t3});
    ParseTree t6(0, {t, t5});
    FunctionalMultiplicityTeacher teacher = getFuncTeacher();
    ASSERT_EQ(teacher.membership(t), 1);
    ASSERT_EQ(teacher.membership(t2), 0);
    ASSERT_EQ(teacher.membership(t3), 2);
    ASSERT_EQ(teacher.membership(t4), 1);
    ASSERT_EQ(teacher.membership(t5), 3);
    ASSERT_EQ(teacher.membership(t6), 4);
}

TEST(functional_teacher_test,prob_test){
    ParseTree t(1);
    ParseTree t2(2);
    ParseTree t3(0, {t, t});
    ParseTree t4(0, {t2, t});
    ParseTree t5(0, {t4, t3});
    ParseTree t6(0, {t, t5});
    FunctionalMultiplicityTeacher teacher = getFuncTeacherProb();
    ASSERT_EQ(teacher.membership(t), 0.9);
    ASSERT_EQ(teacher.membership(t2), 0.1);
    ASSERT_EQ(teacher.membership(t3), 0.9);
    ASSERT_EQ(teacher.membership(t4), 0.5);
    ASSERT_EQ(teacher.membership(t5), 0.5);
    ASSERT_EQ(teacher.membership(t6), 0.9);
}

TEST(functional_teacher_test,equivalence_test){
    FunctionalMultiplicityTeacher teacher = getFuncTeacher();
    MultiplicityTreeAcceptor acc = getCountingAcceptor();
    ASSERT_EQ(teacher.equivalence(acc), nullptr);
}