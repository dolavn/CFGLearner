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
    delete(emptyContext); delete(secondContext);
    h.addTree(leaf);
    h.addTree(t);
    h.addTree(t2);
}