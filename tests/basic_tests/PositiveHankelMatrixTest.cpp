#include "gtest/gtest.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/MultiplicityTeacher.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ObservationTable.h"
#include "../../include/TreesIterator.h"
#include "../../include/Learner.h"

extern rankedChar a;
extern rankedChar b;
extern rankedChar l;
extern rankedChar l1;
extern rankedChar l2;
extern rankedChar inner;

using namespace std;

set<rankedChar> getAlphabet();
set<rankedChar> getAlphabetProb();
set<rankedChar> getAlphabetSmall();
MultiplicityTreeAcceptor getCountingAcc();
SimpleMultiplicityTeacher getMultiplicityTeacher();
SimpleMultiplicityTeacher getMultiplicityProbTeacher();
FunctionalMultiplicityTeacher getFuncTeacherProb();

FunctionalMultiplicityTeacher getFuncTeacherNeg(){
    ParseTree t1(1); ParseTree t2(2);
    TreesIterator it({&t1, &t2}, getAlphabetProb(), 2);
    FunctionalMultiplicityTeacher teacher(0.05, 0, [](const ParseTree& tree){
        return -1;
    }, it);
    return teacher;
}

SimpleMultiplicityTeacher getProbTeacherPositiveTest(){
    set<rankedChar> alphabet = getAlphabetProb();
    ParseTree i12(0, {ParseTree(1), ParseTree(2)});
    ParseTree i21(0, {ParseTree(2), ParseTree(1)});
    ParseTree t1(0, {ParseTree(1), i12});
    t1.setProb(0.09);
    ParseTree t2(0, {ParseTree(2), i12});
    t2.setProb(0.36);
    ParseTree t3(0, {ParseTree(1), i21});
    t3.setProb(0.09);
    ParseTree t4(0, {ParseTree(2), i21});
    t4.setProb(0.36);
    SimpleMultiplicityTeacher teacher(0.05, 0.0);
    teacher.addExample(t1); teacher.addExample(t2);
    teacher.addExample(t3); teacher.addExample(t4);
    return teacher;
}

TEST(positive_hankel_matrix_test,basic_check){
    set<rankedChar> alphabet = getAlphabet();
    FunctionalMultiplicityTeacher teacher = getFuncTeacherProb();
    PositiveHankelMatrix h(teacher);
    ParseTree leaf(1);
    ParseTree t(1, {ParseTree(1), ParseTree(2)});
    ParseTree t2(1, {t, t});
    pair<ParseTree*, ParseTree*> pair1 = t.makeContext({});
    pair<ParseTree*, ParseTree*> pair2 = t.makeContext({0});
    ParseTree* emptyContext = pair1.first;
    ParseTree* secondContext = pair2.first;
    h.addContext(*emptyContext);
    h.addContext(*secondContext);
    delete(emptyContext); delete(secondContext); delete(pair1.second); delete(pair2.second);
    h.addTree(leaf);
    h.addTree(t);
    h.addTree(t2);
    ASSERT_EQ(h.getS().size(), 2);
    ASSERT_EQ(h.getR().size(), 1);
}

TEST(positive_hankel_matrix_test, exception_check){
    set<rankedChar> alphabet = getAlphabet();
    FunctionalMultiplicityTeacher teacher = getFuncTeacherNeg();
    PositiveHankelMatrix h(teacher);
    ParseTree leaf(1);
    ParseTree t(1, {ParseTree(1), ParseTree(2)});
    ParseTree t2(1, {t, t});
    pair<ParseTree*, ParseTree*> pair1 = t.makeContext({});
    ParseTree* emptyContext = pair1.first;
    h.addContext(*emptyContext);
    delete(emptyContext); delete(pair1.second);
    ASSERT_ANY_THROW(h.addTree(leaf));
    ASSERT_ANY_THROW(h.addTree(t));
    ASSERT_ANY_THROW(h.addTree(t2));
}


TEST(positive_hankel_matrix_test, learn_test){
    set<rankedChar> alphabet = getAlphabet();
    FunctionalMultiplicityTeacher teacher = getFuncTeacherProb();
    PositiveHankelMatrix h(teacher);
    h.setVerbose(true);
    learn(teacher, h);
}