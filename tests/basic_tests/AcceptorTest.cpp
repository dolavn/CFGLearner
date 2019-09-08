//
// Created by dolavn@wincs.cs.bgu.ac.il on 1/17/19.
//

#include "gtest/gtest.h"
#include "../../include/TreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "TestsHelperFunctions.h"

using namespace std;

TEST(acceptor_test,basic_check){
    set<rankedChar> alphabet = getAlphabet();
    TreeAcceptor acc(alphabet);
    ASSERT_EQ(acc.getStatesNum(),0);
    acc = TreeAcceptor(alphabet, 2);
    ASSERT_EQ(acc.getStatesNum(),2);
}

TEST(acceptor_test,accepting_states){
    set<rankedChar> alphabet = getAlphabet();
    TreeAcceptor acc(alphabet);
    try{
        acc.setAccepting(0,true);
        ASSERT_EQ(1,0);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("No such state!"));
    }
    acc = TreeAcceptor(alphabet, 2);
    acc.setAccepting(0,true);
    ASSERT_EQ(acc.isAccepting(0),true);
    ASSERT_EQ(acc.isAccepting(1),false);
    acc.setAccepting(0,false);
    acc.setAccepting(1,true);
    ASSERT_EQ(acc.isAccepting(0),false);
    ASSERT_EQ(acc.isAccepting(1),true);
}

TEST(acceptor_test,transition){
    set<rankedChar> alphabet = getAlphabet();
    TreeAcceptor acc(alphabet,2);
    acc.addTransition({0,0},a,1);
    acc.addTransition({0,1},b,1);
    acc.addTransition({1,0},b,1);
    acc.addTransition({1,1},a,0);
    acc.addTransition({},l,0);
    try{
        acc.addTransition({1,1},l,0);
        ASSERT_EQ(1,0);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("Rank of character and number of states don't match!"));
    }
    try{
        acc.addTransition({},a,0);
        ASSERT_EQ(1,0);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("Rank of character and number of states don't match!"));
    }
    ASSERT_EQ(acc.nextState({0,0},a),1);
    ASSERT_EQ(acc.nextState({0,1},b),1);
    ASSERT_EQ(acc.nextState({1,0},b),1);
    ASSERT_EQ(acc.nextState({1,1},a),0);
    ASSERT_EQ(acc.nextState({1,1},b),-1);
    ASSERT_EQ(acc.nextState({0,0},b),-1);
    ASSERT_EQ(acc.nextState({},l),0);
}

TEST(acceptor_test,run_basic){
    set<rankedChar> alphabet = getAlphabet();
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
    ASSERT_EQ(acc.run(t),true);
    ASSERT_EQ(acc.run(t2),false);
    ASSERT_EQ(acc.run(t3),false);
    ASSERT_EQ(acc.run(t4),true);
}

TEST(acceptor_test,run_advanced){
    set<rankedChar> alphabet = getAlphabet();
    TreeAcceptor acc(alphabet,4);
    acc.setAccepting(0,true);
    acc.setAccepting(1,true);
    acc.setAccepting(2,true);
    acc.setAccepting(3,true);
    acc.addTransition({},l,0);
    for(int x: {0,1}){
        for(int y: {0, 1}){
            acc.addTransition({x,y},a,1);
        }
    }
    for(int x: {0,2}){
        for(int y: {0, 2}){
            acc.addTransition({x,y},b,2);
        }
    }
    for(int x: {0,1}){
        for(int y: {2, 3}){
            acc.addTransition({x,y},a,3);
            acc.addTransition({y,x},a,3);
        }
    }
    ParseTree t(1,{ParseTree(0),ParseTree(0)});
    ParseTree t2(2,{ParseTree(1,{ParseTree(0),ParseTree(0)}),ParseTree(0)});
    ParseTree t3(1,{ParseTree(2,{ParseTree(0),ParseTree(0)}),ParseTree(1,{ParseTree(1,{ParseTree(0),ParseTree(0)}),ParseTree(1,{ParseTree(2,{ParseTree(0),ParseTree(0)}),ParseTree(0)})})});
    ParseTree t4(1,{ParseTree(2,{ParseTree(0),ParseTree(0)}),ParseTree(0)});
    ASSERT_EQ(acc.run(t),true);
    ASSERT_EQ(acc.run(t2),false);
    ASSERT_EQ(acc.run(t3),false);
    ASSERT_EQ(acc.run(t4),true);
}