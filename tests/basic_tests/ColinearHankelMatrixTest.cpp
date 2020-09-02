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
#include "../../include/Logger.h"
#include "TestsHelperFunctions.h"


using namespace std;


TEST(colinear_hankel_matrix_test ,basic_check){
    set<rankedChar> alphabet = getAlphabet();
    FunctionalMultiplicityTeacher teacher = getFuncTeacherProb2();
    ColinearHankelMatrix h(teacher);
    h.complete();
    h.printTable();
    MultiplicityTreeAcceptor acc = h.getAcceptor();
    acc.printDesc();
}

TEST(colinear_hankel_matrix_test, vec_test){
    vector<double> v1 = {0, 0.2};
    ASSERT_FLOAT_EQ(getNorm(v1), 0.2);
    v1 = {1.01, 2};
    vector<double> v2 = {2, 3.999};
    cout << getCosVectors(v1, v2) << endl;
}

TEST(colinear_hankel_matrix_test ,learner_test){
    set<rankedChar> alphabet = getAlphabet();
    FunctionalMultiplicityTeacher teacher = getFuncTeacherProb2();
    ColinearHankelMatrix h(teacher);
    MultiplicityTreeAcceptor acc = learnColin(teacher, h);
    acc.printDesc();
}