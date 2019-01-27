#include "gtest/gtest.h"
#include "../../include/TreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/Teacher.h"


using namespace std;


TEST(teacher_test,basic_check){
    ParseTree t(2);
    ParseTree t2(3);
    SimpleTeacher teacher;
    teacher.addPositiveExample(t);
    ASSERT_EQ(teacher.membership(t),true);
    ASSERT_EQ(teacher.membership(t2),false);
    teacher.addNegativeExample(t2);
    ASSERT_EQ(teacher.membership(t2),false);
}

TEST(teacher_test,equivalence_check){
    rankedChar l{0,0};
    rankedChar a{1,2};
    rankedChar b{2,2};
    set<rankedChar> alphabet = {a,b,l};
    TreeAcceptor acc(alphabet,3);
    acc.setAccepting(1,true);
    acc.addTransition({},l,0);
    acc.addTransition({0,0},a,1);
    acc.addTransition({0,0},b,2);
    acc.addTransition({1,0},a,1);
    acc.addTransition({1,0},b,1);
    acc.addTransition({0,1},a,1);
    acc.addTransition({0,1},b,1);
    acc.addTransition({1,2},a,1);
    acc.addTransition({1,2},b,1);
    acc.addTransition({2,1},a,1);
    acc.addTransition({2,1},b,1);
    acc.addTransition({2,0},a,1);
    acc.addTransition({2,0},b,2);
    acc.addTransition({0,2},a,1);
    acc.addTransition({0,2},b,2);
    acc.addTransition({2,2},a,1);
    acc.addTransition({2,2},b,2);
    acc.addTransition({1,1},a,1);
    acc.addTransition({1,1},b,1);
    ParseTree t(1,{ParseTree(0),ParseTree(0)});
    ParseTree t2(2,{ParseTree(0),ParseTree(0)});
    ParseTree t3(2,{ParseTree(2,{ParseTree(0),ParseTree(0)}),ParseTree(0)});
    ParseTree t4(2,{ParseTree(1,{ParseTree(0),ParseTree(0)}),ParseTree(0)});
    SimpleTeacher teacher;
    teacher.addPositiveExample(t);
    teacher.addPositiveExample(t4);
    teacher.addNegativeExample(t2);
    teacher.addNegativeExample(t3);
    ASSERT_EQ(teacher.equivalence(acc),nullptr);
    SimpleTeacher teacher2;
    teacher2.addNegativeExample(t);
    ParseTree* ce = teacher2.equivalence(acc);
    ASSERT_FALSE(ce==nullptr);
    if(ce){
        delete(ce);
        ce=nullptr;
    }
}