//
// Created by dolavn@wincs.cs.bgu.ac.il on 1/16/19.
//

#include "gtest/gtest.h"
#include "../../include/ParseTree.h"
#include <iostream>
#include <vector>

using namespace std;

TEST(tree_test,basic_check){
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
    ASSERT_EQ(t.isLeaf(),false);
    ASSERT_EQ(t["00"].isLeaf(),true);
}

TEST(tree_test,skeleton){
    ParseTree t(2);
    ParseTree t2(3);
    t2.setLeftSubtree(ParseTree(2));
    t2.setRightSubtree(ParseTree(3));
    ParseTree t3(4);
    t.setLeftSubtree(t2);
    t.setRightSubtree(t3);
    ParseTree t4 = t.getSkeleton();
    vector<long> dataList={-1,-1,-1,-1,-1};
    int currInd=0;
    for(auto it=t4.getIterator();it.hasNext();++it){
        ASSERT_EQ(*it,dataList[currInd++]);
    }
    ASSERT_EQ(t.isLeaf(),false);
    ASSERT_EQ(t["00"].isLeaf(),true);
}

TEST(tree_test,index_iterator){
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

TEST(tree_test,leaf_iterator){
    ParseTree t(2);
    ParseTree t2(3);
    t2.setLeftSubtree(ParseTree(2));
    t2.setRightSubtree(ParseTree(3));
    ParseTree t3(4);
    t.setLeftSubtree(t2);
    t.setRightSubtree(t3);
    vector<string> stringList={"00","01","1"};
    int currInd=0;
    for(auto it=t.getLeafIterator();it.hasNext();++it){
        ASSERT_EQ(*it,stringList[currInd++]);
    }
    ParseTree t4(2);
    for(auto it=t4.getLeafIterator();it.hasNext();++it){
        ASSERT_EQ(*it,"");
    }
    ParseTree t5(1);
    t5.setLeftSubtree(ParseTree(0));
    t5.setRightSubtree(ParseTree(0));
    currInd = 0;
    stringList={"0","1"};
    for(auto it=t5.getLeafIterator();it.hasNext();++it){
        ASSERT_EQ(*it,stringList[currInd++]);
    }
}

TEST(tree_test,copy_constructor){
    ParseTree t(2);
    ParseTree t2(3);
    t.setRightSubtree(t2);
    ParseTree t3(t);
    int p = 2;
    for(auto it=t3.getIterator();it.hasNext();++it){
        ASSERT_EQ(*it,p++);
    }
}


TEST(tree_test,assignment_operator){
    ParseTree t(2);
    ParseTree t2(3);
    t.setRightSubtree(t2);
    ParseTree t3(t2);
    t3 = t;
    int p = 2;
    for(auto it=t3.getIterator();it.hasNext();++it){
        ASSERT_EQ(*it,p++);
    }
}

TEST(tree_test,move_constructor){
    ParseTree t(2);
    ParseTree t2(3);
    t.setRightSubtree(t2);
    ParseTree t3(std::move(t));
    int p = 2;
    for(auto it=t3.getIterator();it.hasNext();++it){
        ASSERT_EQ(*it,p++);
    }
}

TEST(tree_test,move_assignment){
    ParseTree t(2);
    ParseTree t2(3);
    t.setRightSubtree(t2);
    ParseTree t3(t2);
    t3 = std::move(t);
    int p = 2;
    for(auto it=t3.getIterator();it.hasNext();++it){
        ASSERT_EQ(*it,p++);
    }
}

TEST(tree_test,access_operator){
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
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("Index out of bounds"));
    }
}

TEST(tree_test,make_context){
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