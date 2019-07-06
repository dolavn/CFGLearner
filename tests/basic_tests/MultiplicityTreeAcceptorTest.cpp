#include "gtest/gtest.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ParseTree.h"

#include <iostream>

using namespace std;

rankedChar l2{0,0};
rankedChar a2{1,2};
rankedChar b2{2,2};

set<rankedChar> getAlphabet2(){
    set<rankedChar> alphabet = {a2,b2,l2};
    return alphabet;
}

TEST(multiplicity_tree_acceptor_test,basic_check){
    set<rankedChar> alphabet = getAlphabet2();
    MultiplicityTreeAcceptor a(alphabet, 2);
    a.setLambda(floatVec({1, 0}));
    MultiLinearMap m_l(2, 0), m_a(2, 2), m_b(2, 2);
    m_l.setParam(1.0f,{0});
    m_l.setParam(1.0f,{1});
    m_a.setParam(1.0f,{0, 0, 1});
    m_a.setParam(1.0f,{0, 1, 0});
    m_a.setParam(1.0f,{1, 1, 1});
    m_b.setParam(1.0f,{0, 0, 1});
    m_b.setParam(1.0f,{0, 1, 0});
    m_b.setParam(1.0f,{1, 1, 1});
    a.addTransition(m_l, l2);
    a.addTransition(m_a, a2);
    a.addTransition(m_b, b2);
    ParseTree t(1, {ParseTree(0), ParseTree(0)});
    ASSERT_EQ(a.run(t), 2.0f);
    ParseTree t2(2, {t, t});
    ASSERT_EQ(a.run(t2), 4.0f);
}
