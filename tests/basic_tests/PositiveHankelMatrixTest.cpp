#include "gtest/gtest.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/MultiplicityTeacher.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ObservationTable.h"
#include "../../include/TreesIterator.h"
#include "../../include/Learner.h"
#include "../../include/TreeComparator.h"
#include "../../include/TreeConstructor.h"
#include "TestsHelperFunctions.h"

extern rankedChar a;
extern rankedChar b;
extern rankedChar l;
extern rankedChar l1;
extern rankedChar l2;
extern rankedChar inner;

using namespace std;


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
    for(double totalProb: {0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4 ,0.45, 0.5}){
        SimpleMultiplicityTeacher teacher = getDistributionTeacher(totalProb);
        PositiveHankelMatrix h(teacher);
        learn(teacher, h);
    }
}

TEST(positive_hankel_matrix_test, learn_test2){
    set<rankedChar> alphabet = {{0, 2}, {1, 0}, {2, 0}};
    EqualityComparator cmp;
    ProbabilityTeacher teacher(cmp, 0, 1);
    ParseTree t1(0, {ParseTree(1), ParseTree(0, {ParseTree(1), ParseTree(2)})});
    ParseTree t2(0, {ParseTree(1), ParseTree(0, {ParseTree(1), ParseTree(1)})});
    ParseTree t3(0, {ParseTree(1), ParseTree(1)});
    t1.setProb(0.3); t2.setProb(0.2); t3.setProb(0.5);
    teacher.addExample(t1);teacher.addExample(t2);teacher.addExample(t3);
    scoreTable scores;
    scores[{1,2}]=5;
    TreeConstructor c(scores);
    teacher.setupConstructorGenerator(c, 5, 1000);
    ScalarHankelMatrix h(teacher);
    learn(teacher, h);
}