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
    teacher.addPositiveExample(t2);
    ASSERT_EQ(teacher.membership(t2),false);
    teacher.addPositiveExample(t2);
    ASSERT_EQ(teacher.membership(t2),true);
}

TEST(frequency_teacher_test,advanced_check){
    ParseTree t(2);
    ParseTree t2(3);
    FrequencyTeacher teacher(4, 0.5);
    teacher.addPositiveExamples(t, 2);
    ASSERT_EQ(teacher.membership(t),false);
    teacher.addPositiveExamples(t, 4);
    ASSERT_EQ(teacher.membership(t),true);
    ASSERT_EQ(teacher.membership(t2),false);
    teacher.addNegativeExamples(t2, 4);
    ASSERT_EQ(teacher.membership(t2),false);
    teacher.addPositiveExamples(t2, 2);
    ASSERT_EQ(teacher.membership(t2),false);
    teacher.addPositiveExamples(t2, 3);
    ASSERT_EQ(teacher.membership(t2),true);
}


TEST(frequency_teacher_test,parameters_test){
    ParseTree t(2);
    ParseTree t2(3);
    FrequencyTeacher teacher(10, 0.5f);
    teacher.addPositiveExample(t);
    ASSERT_EQ(teacher.membership(t),false);
    ASSERT_EQ(teacher.membership(t2),false);
    for(int i=0;i<10;++i){
        teacher.addPositiveExample(t);
    }
    ASSERT_EQ(teacher.membership(t),true);
    FrequencyTeacher teacher2(4, 0.75f);
    for(int i=0;i<5;++i){
        teacher2.addPositiveExample(t);
    }
    ASSERT_EQ(teacher2.membership(t),true);
    for(int i=0;i<5;++i){
        teacher2.addNegativeExample(t);
    }
    ASSERT_EQ(teacher2.membership(t),false);
    for(int i=0;i<10;++i){
        teacher2.addPositiveExample(t);
    }
    ASSERT_EQ(teacher2.membership(t),false);
    teacher2.addPositiveExample(t);
    ASSERT_EQ(teacher2.membership(t),true);
}

TEST(frequency_teacher_test,exception_tests){
    try {
        FrequencyTeacher teacher(-10, 0.5f);
        ASSERT_EQ(1,2);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("Invalid parameters"));
    }
    try {
        FrequencyTeacher teacher(1, 1.5f);
        ASSERT_EQ(1,2);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("Invalid parameters"));
    }
    try {
        FrequencyTeacher teacher(1, -0.5f);
        ASSERT_EQ(1,2);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("Invalid parameters"));
    }
}


