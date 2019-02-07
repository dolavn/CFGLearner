#include "gtest/gtest.h"
#include "../../include/TreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/Teacher.h"

TEST(frequency_teacher_test,basic_check){
    ParseTree t(2);
    ParseTree t2(3);
    FrequencyTeacher teacher;
    teacher.addPositiveExample(t);
    ASSERT_EQ(teacher.membership(t),true);
    ASSERT_EQ(teacher.membership(t2),false);
    teacher.addNegativeExample(t2);
    ASSERT_EQ(teacher.membership(t2),false);
}
