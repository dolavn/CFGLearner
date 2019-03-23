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