//
// Created by dolavn@wincs.cs.bgu.ac.il on 1/16/19.
//

#include "gtest/gtest.h"
#include "../../include/ParseTree.h"
#include <iostream>
#include <vector>

using namespace std;

TEST(basic_check, tree_test){
    ParseTree t(2);
    ParseTree t2(3);
    t2.setLeftSubtree(ParseTree(2));
    t2.setRightSubtree(ParseTree(3));
    ParseTree t3(4);
    t.setLeftSubtree(t2);
    t.setRightSubtree(t3);
    vector<long> dataList={2,3,2,3,4};
    int currInd=0;
    for(auto it=t.getIterator();it.hasNext();++it){
        ASSERT_EQ(*it,dataList[currInd++]);
    }
}

TEST(index_iterator, tree_test){
    ParseTree t(2);
    ParseTree t2(3);
    t2.setLeftSubtree(ParseTree(2));
    t2.setRightSubtree(ParseTree(3));
    ParseTree t3(4);
    t.setLeftSubtree(t2);
    t.setRightSubtree(t3);
    vector<string> stringList={"","0","00","01","1"};
    int currInd=0;
    for(auto it=t.getIndexIterator();it.hasNext();++it){
        ASSERT_EQ(*it,stringList[currInd++]);
    }
}

TEST(copy_constructor, tree_test){
    ParseTree t(2);
    ParseTree t2(3);
    t.setRightSubtree(t2);
    ParseTree t3(t);
    int p = 2;
    for(auto it=t3.getIterator();it.hasNext();++it){
        ASSERT_EQ(*it,p++);
    }
}

TEST(move_constructor, tree_test){
    ParseTree t(2);
    ParseTree t2(3);
    t.setRightSubtree(t2);
    ParseTree t3(std::move(t));
    int p = 2;
    for(auto it=t3.getIterator();it.hasNext();++it){
        ASSERT_EQ(*it,p++);
    }
}

TEST(access_operator, tree_test){
    ParseTree t(2);
    ParseTree t2(3);
    t2.setLeftSubtree(ParseTree(2));
    t2.setRightSubtree(ParseTree(3));
    ParseTree t3(4);
    t.setLeftSubtree(t2);
    t.setRightSubtree(t3);
    ASSERT_EQ(t[""].getData(),2);
    ASSERT_EQ(t["0"].getData(),3);
    ASSERT_EQ(t["1"].getData(),4);
    ASSERT_EQ(t["00"].getData(),2);
    ASSERT_EQ(t["01"].getData(),3);
    try{
        t["000"];
        ASSERT_EQ(1,0);
    }catch(std::invalid_argument e){
        ASSERT_EQ(e.what(),std::string("Index out of bounds"));
    }
}

TEST(make_context, tree_test){
    ParseTree t(2);
    ParseTree t2(3);
    t2.setLeftSubtree(ParseTree(2));
    t2.setRightSubtree(ParseTree(3));
    ParseTree t3(4);
    t.setLeftSubtree(t2);
    t.setRightSubtree(t3);
    std::pair<ParseTree*,ParseTree*> contextPair = t.makeContext("0");
    ParseTree* context = contextPair.first;
    ParseTree* tree = contextPair.second;
    ASSERT_EQ((*context)[""].getData(),2);
    ASSERT_EQ((*context)["0"].getData(),-1);
    ASSERT_EQ((*context)["1"].getData(),4);
    ASSERT_EQ((*tree)[""].getData(),3);
    ASSERT_EQ((*tree)["0"].getData(),2);
    ASSERT_EQ((*tree)["1"].getData(),3);
    delete(context);
    delete(tree);
}