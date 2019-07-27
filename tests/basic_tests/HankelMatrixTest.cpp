#include "gtest/gtest.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/MultiplicityTeacher.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ObservationTable.h"

extern rankedChar a;
extern rankedChar b;
extern rankedChar l;

using namespace std;

set<rankedChar> getAlphabet();

MultiplicityTreeAcceptor getCountingAcc();

TEST(hankel_matrix_test,basic_check){
    SimpleMultiplicityTeacher teacher(0.2, 0.0);
    set<rankedChar> alphabet = getAlphabet();
    MultiplicityTreeAcceptor acc = getCountingAcc();
    ParseTree t(1, {ParseTree(0), ParseTree(0)});
    t.setProb(2.0);
    ParseTree t2(1, {t, t});
    t2.setProb(4.0);
    ParseTree t3(1, {t2, t2});
    t3.setProb(8.0);
    teacher.addExample(t);
    teacher.addExample(t2);
    teacher.addExample(t3);
    HankelMatrix h(teacher);
    pair<ParseTree*, ParseTree*> pair = t.makeContext({});
    ParseTree* emptyContext = pair.first;
    h.addContext(*emptyContext);
    delete(pair.first);
    delete(pair.second);
    pair = {nullptr, nullptr};
    h.addTree(t);
    h.addTree(t2);
    vector<double> obs = h.getObs(t);
    ASSERT_EQ(obs.size(), 1);
    ASSERT_EQ(obs[0], 2.0);
    obs = h.getObs(t2);
    ASSERT_EQ(obs.size(), 1);
    ASSERT_EQ(obs[0], 4.0);
}