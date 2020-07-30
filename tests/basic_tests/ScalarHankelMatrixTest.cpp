#include "gtest/gtest.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/MultiplicityTeacher.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ObservationTable.h"
#include "../../include/TreesIterator.h"
#include "../../include/Learner.h"
#include "TestsHelperFunctions.h"

extern rankedChar a;
extern rankedChar b;
extern rankedChar l;
extern rankedChar l1;
extern rankedChar l2;
extern rankedChar inner;

using namespace std;


TEST(scalar_hankel_matrix_test,basic_check){
    set<rankedChar> alphabet = getAlphabet();
    FunctionalMultiplicityTeacher teacher = getFuncTeacherProb2();
    ScalarHankelMatrix h(teacher);
    ParseTree leaf(1);
    ParseTree t(0, {ParseTree(1), ParseTree(2)});
    ParseTree t2(0, {ParseTree(2), ParseTree(1)});
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
    h.printTable();
    ASSERT_EQ(h.getS().size(), 2);
    ASSERT_EQ(h.getR().size(), 1);
}

TEST(scalar_hankel_matrix_test, exception_check){
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

TEST(scalar_hankel_matrix_test,learn_check){
    FunctionalMultiplicityTeacher teacher = getFuncTeacherProb2();
    ScalarHankelMatrix h(teacher);
    MultiplicityTreeAcceptor acc = learn(teacher, h);
    ASSERT_FLOAT_EQ(acc.run(ParseTree(0, {ParseTree(1), ParseTree(1)})), 0.2);
    ASSERT_FLOAT_EQ(acc.run(ParseTree(0, {ParseTree(2), ParseTree(2)})), 0.2);
    ASSERT_FLOAT_EQ(acc.run(ParseTree(0, {ParseTree(1), ParseTree(2)})), 0.5);
    ASSERT_FLOAT_EQ(acc.run(ParseTree(0, {ParseTree(2), ParseTree(1)})), 0.1);
}