#include "gtest/gtest.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/MultiplicityTeacher.h"
#include "../../include/TreesIterator.h"


using namespace std;

rankedChar inner{0, 2};
rankedChar l1{1, 0};
rankedChar l2{2, 0};

set<rankedChar> getAlphabetCounting(){
    return set<rankedChar>({inner, l1, l2});
}


FunctionalMultiplicityTeacher getFuncTeacher(){
    TreesIterator it(getAlphabetCounting(), 3);
    FunctionalMultiplicityTeacher teacher(0.1, 0, [](const ParseTree& tree){
        int c = 0;
        auto it = tree.getIterator();
        while(it.hasNext()){
            if(*it==l1.c){
                c++;
            }
            ++it;
        }
        return c;
    }, it);
    return teacher;
}


MultiplicityTreeAcceptor getCountingAcceptor(){
    MultiplicityTreeAcceptor acc(getAlphabetCounting(), 2);
    MultiLinearMap m1(2, 0);
    MultiLinearMap m2(2, 0);
    MultiLinearMap m3(2, 2);
    m1.setParam(1.0, {0}); m1.setParam(0.0, {1});
    m2.setParam(0.0, {0}); m2.setParam(1.0, {1});
    m3.setParam(2.0, {0, 0, 0}); m3.setParam(-1.0f, {1, 0, 0});
    m3.setParam(1.0, {0, 0, 1}); m3.setParam(0.0, {1, 0, 1});
    m3.setParam(1.0, {0, 1, 0}); m3.setParam(0.0, {1, 1, 0});
    m3.setParam(0.0, {0, 1, 1}); m3.setParam(1.0, {1, 1, 1});
    acc.addTransition(m1, l1); acc.addTransition(m2, l2);
    acc.addTransition(m3, inner);
    acc.setLambda({1.0, 0.0});
    return acc;
}

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

TEST(functional_teacher_test,equivalence_test){
    FunctionalMultiplicityTeacher teacher = getFuncTeacher();
    MultiplicityTreeAcceptor acc = getCountingAcceptor();
    ASSERT_EQ(teacher.equivalence(acc), nullptr);
}