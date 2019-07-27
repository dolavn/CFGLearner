#include "gtest/gtest.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ParseTree.h"

#include <iostream>

using namespace std;

extern rankedChar l;
extern rankedChar a;
extern rankedChar b;

set<rankedChar> getAlphabet();

MultiplicityTreeAcceptor getCountingAcc(){
    set<rankedChar> alphabet = getAlphabet();
    MultiplicityTreeAcceptor acc(alphabet, 2);
    acc.setLambda(floatVec({1, 0}));
    MultiLinearMap m_l(2, 0), m_a(2, 2), m_b(2, 2);
    m_l.setParam(1.0f,{0});
    m_l.setParam(1.0f,{1});
    m_a.setParam(1.0f,{0, 0, 1});
    m_a.setParam(1.0f,{0, 1, 0});
    m_a.setParam(1.0f,{1, 1, 1});
    m_b.setParam(1.0f,{0, 0, 1});
    m_b.setParam(1.0f,{0, 1, 0});
    m_b.setParam(1.0f,{1, 1, 1});
    acc.addTransition(m_l, l);
    acc.addTransition(m_a, a);
    acc.addTransition(m_b, b);
    return acc;
}

TEST(multiplicity_tree_acceptor_test,basic_check){
    set<rankedChar> alphabet = getAlphabet();
    MultiplicityTreeAcceptor acc(alphabet, 2);
    acc.setLambda(floatVec({1, 0}));
    MultiLinearMap m_l(2, 0), m_a(2, 2), m_b(2, 2);
    m_l.setParam(1.0f,{0});
    m_l.setParam(1.0f,{1});
    m_a.setParam(1.0f,{0, 0, 1});
    m_a.setParam(1.0f,{0, 1, 0});
    m_a.setParam(1.0f,{1, 1, 1});
    m_b.setParam(1.0f,{0, 0, 1});
    m_b.setParam(1.0f,{0, 1, 0});
    m_b.setParam(1.0f,{1, 1, 1});
    acc.addTransition(m_l, l);
    acc.addTransition(m_a, a);
    acc.addTransition(m_b, b);
    ParseTree t(1, {ParseTree(0), ParseTree(0)});
    ASSERT_EQ(acc.run(t), 2.0f);
    ParseTree t2(2, {t, t});
    ASSERT_EQ(acc.run(t2), 4.0f);
}
