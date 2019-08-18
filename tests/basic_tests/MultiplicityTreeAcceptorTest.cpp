#include "gtest/gtest.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/IndexArray.h"

#include <iostream>

using namespace std;

extern rankedChar l;
extern rankedChar a;
extern rankedChar b;
extern rankedChar l1;
extern rankedChar l2;
extern rankedChar inner;

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

TEST(multiplicity_tree_acceptor_test,is_pos){
    set<rankedChar> alphabet =  set<rankedChar>({inner, l1, l2});
    MultiplicityTreeAcceptor acc(alphabet, 3);
    acc.setLambda({0.05, 0, 1});
    MultiLinearMap m_l1(3, 0), m_l2(3, 0), m_inner(3, 2);
    m_l1.setParam(-1.0f, {0}); m_l2.setParam(1.0, {1});
    m_inner.setParam(1.0, {2,0,0}); m_inner.setParam(3.5, {2,0,1});
    m_inner.setParam(3.5, {2,1,0}); m_inner.setParam(1.5, {2,1,1});
    acc.addTransition(m_l1, l1); acc.addTransition(m_l2, l2);
    acc.addTransition(m_inner, inner);
    ASSERT_ANY_THROW(acc.getNormalizedAcceptor(false));
    m_l1.setParam(1.0f, {0});
    acc.addTransition(m_l1, l1);
    ASSERT_NO_THROW(acc.getNormalizedAcceptor(false));
    acc.setLambda({-0.05, 0, 1});
    ASSERT_ANY_THROW(acc.getNormalizedAcceptor(false));
    acc.setLambda({0.05, 0, 1});
    ASSERT_NO_THROW(acc.getNormalizedAcceptor(false));
}

TEST(multiplicity_tree_acceptor_test,normalize_test){
    set<rankedChar> alphabet =  set<rankedChar>({inner, l1, l2});
    MultiplicityTreeAcceptor acc(alphabet, 3);
    acc.setLambda({0.05, 0, 1});
    MultiLinearMap m_l1(3, 0), m_l2(3, 0), m_inner(3, 2);
    m_l1.setParam(1.0, {0}); m_l2.setParam(1.0, {1});
    m_inner.setParam(1.0, {2,0,0}); m_inner.setParam(3.5, {2,0,1});
    m_inner.setParam(3.5, {2,1,0}); m_inner.setParam(1.5, {2,1,1});
    acc.addTransition(m_l1, l1); acc.addTransition(m_l2, l2);
    acc.addTransition(m_inner, inner);
    MultiplicityTreeAcceptor accNorm = acc.getNormalizedAcceptor(false);
    floatVec lambdaNorm = accNorm.getLambda();
    floatVec targetLambda = {0.0476, 0, 0.952};
    MultiLinearMap target_l1(3, 0), target_l2(3, 0), target_inner(3, 2);
    target_l1.setParam(1.0, {0}); target_l2.setParam(1.0, {1});
    target_inner.setParam(0.1052, {2,0,0}); target_inner.setParam(0.3684, {2,0,1});
    target_inner.setParam(0.3684, {2,1,0}); target_inner.setParam(0.1578, {2,1,1});
    for(int i=0;i<3;++i){
        ASSERT_LE(abs(lambdaNorm[i]-targetLambda[i]), 0.001);
    }
    MultiLinearMap normalizedMap = accNorm.getMap(inner);
    for(IndexArray ind({3, 3, 3});!ind.getOverflow();++ind){
        vector<int> currInd = ind.getIntVector();
        ASSERT_LE(abs(normalizedMap.getParam(currInd)-target_inner.getParam(currInd)), 0.001);
    }
}


TEST(multiplicity_tree_acceptor_test,normalize_test_sm){
    set<rankedChar> alphabet =  set<rankedChar>({inner, l1, l2});
    MultiplicityTreeAcceptor acc(alphabet, 3);
    acc.setLambda({0.05, 0, 1});
    MultiLinearMap m_l1(3, 0), m_l2(3, 0), m_inner(3, 2);
    m_l1.setParam(1.0, {0}); m_l2.setParam(1.0, {1});
    m_inner.setParam(1.0, {2,0,0}); m_inner.setParam(3.5, {2,0,1});
    m_inner.setParam(3.5, {2,1,0}); m_inner.setParam(1.5, {2,1,1});
    acc.addTransition(m_l1, l1); acc.addTransition(m_l2, l2);
    acc.addTransition(m_inner, inner);
    MultiplicityTreeAcceptor accNorm = acc.getNormalizedAcceptor(true);
    floatVec lambdaNorm = accNorm.getLambda();
    floatVec targetLambda = {0.2204, 0.2096, 0.5699};
    MultiLinearMap target_l1(3, 0), target_l2(3, 0), target_inner(3, 2);
    target_l1.setParam(0.2137, {0});target_l1.setParam(0.0786, {1});target_l1.setParam(0.0124, {2});
    target_l2.setParam(0.0786, {0});target_l2.setParam(0.2137, {1});target_l2.setParam(0.0124, {2});
    for(IndexArray ind({3, 3});!ind.getOverflow();++ind){
        target_inner.setParam(0.0786, {0, ind.get(0), ind.get(1)});
        target_inner.setParam(0.0786, {1, ind.get(0), ind.get(1)});
        target_inner.setParam(0.0124, {2, ind.get(0), ind.get(1)});
    }
    target_inner.setParam(0.0337, {2,0,0}); target_inner.setParam(0.4117, {2,0,1});
    target_inner.setParam(0.4117, {2,1,0}); target_inner.setParam(0.0557, {2,1,1});
    for(int i=0;i<3;++i){
        ASSERT_LE(abs(lambdaNorm[i]-targetLambda[i]), 0.001);
    }
    MultiLinearMap normalizedMap = accNorm.getMap(inner);
    for(IndexArray ind({3, 3, 3});!ind.getOverflow();++ind){
        vector<int> currInd = ind.getIntVector();
        ASSERT_LE(abs(normalizedMap.getParam(currInd)-target_inner.getParam(currInd)), 0.001);
    }
}


