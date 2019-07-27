#include "gtest/gtest.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/MultiplicityTeacher.h"
#include "../../include/MultiplicityTreeAcceptor.h"

using namespace std;

extern rankedChar l;
extern rankedChar a;
extern rankedChar b;

set<rankedChar> getAlphabet();

MultiplicityTreeAcceptor getCountingAcc();

TEST(multiplicity_teacher_test,basic_check){
    ParseTree t(2);
    t.setProb(0.2);
    ParseTree t2(3);
    t2.setProb(0.4);
    SimpleMultiplicityTeacher teacher(0.1, 0.0);
    teacher.addExample(t);
    ASSERT_EQ(teacher.membership(t),0.2);
    ASSERT_EQ(teacher.membership(t2),0.0);
    teacher.addExample(t2);
    ASSERT_EQ(teacher.membership(t2),0.4);
}

TEST(multiplicity_teacher_test, memory_check){
    ParseTree t(2);
    t.setProb(0.2);
    ParseTree t2(3);
    t2.setProb(0.4);
    SimpleMultiplicityTeacher teacher(0.1, 0.0);
    teacher.addExample(t);
    ASSERT_EQ(teacher.membership(t),0.2);
    ASSERT_EQ(teacher.membership(t2),0.0);
    teacher.addExample(t2);
    ASSERT_EQ(teacher.membership(t2),0.4);
    SimpleMultiplicityTeacher teacher2(teacher);
    ASSERT_EQ(teacher2.membership(t2),0.4);
    SimpleMultiplicityTeacher teacher3(0.1, 0.0);
    ParseTree t4(3);
    t4.setProb(0.6);
    teacher3.addExample(t4);
    ASSERT_EQ(teacher3.membership(t4),0.6);
    teacher3 = teacher2;
    ASSERT_EQ(teacher3.membership(t4),0.4);
}

TEST(multiplicity_teacher_test, equivalence_test){
    set<rankedChar> alphabet = getAlphabet();
    MultiplicityTreeAcceptor acc = getCountingAcc();
    SimpleMultiplicityTeacher teacher(0.1, 0.0);
    ParseTree t(1, {ParseTree(0), ParseTree(0)});
    t.setProb(2.0);
    ParseTree t2(1, {t, t});
    t2.setProb(4.0);
    ParseTree t3(1, {t2, t2});
    t3.setProb(4.0);
    teacher.addExample(t);
    teacher.addExample(t2);
    ASSERT_EQ(teacher.equivalence(acc), nullptr);
    teacher.addExample(t3);
    ParseTree* example = teacher.equivalence(acc);
    ASSERT_TRUE(*example==t3);
    if(example) {
        delete (example);
        example = nullptr;
    }
}
