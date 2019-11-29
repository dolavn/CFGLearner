#include <unordered_map>
#include <limits>
#include <iostream>

#include "gtest/gtest.h"
#include "../../include/ParseTree.h"
#include "../../include/TreeComparator.h"

#define INDEL 2

using namespace std;

scoresMap getScores(){
    scoresMap scores;
    for(int i=0;i<=4;++i){
        scores[{i, i}] = 0;
        if(i!=0){
            scores[{i, 0}] = numeric_limits<int>::max();
            scores[{0, i}] = numeric_limits<int>::max();
        }
    }
    scores[{1, 2}] = 1; scores[{2, 1}] = 1;
    scores[{1, 3}] = 3; scores[{3, 1}] = 3;
    scores[{1, 4}] = 2; scores[{4, 1}] = 2;
    scores[{2, 3}] = 2; scores[{3, 2}] = 2;
    scores[{2, 4}] = 2; scores[{4, 2}] = 2;
    scores[{3, 4}] = 1; scores[{4, 3}] = 1;
    return scores;
}


TEST(tree_aligner_test,basic_check){
    ParseTree t(0);
    ParseTree t2(0);
    ParseTree t3(0);
    ParseTree t4(0);
    ParseTree t5(0);
    t2.setSubtree(ParseTree(1), 0);
    t2.setSubtree(ParseTree(2), 1);
    t.setSubtree(ParseTree(1), 0);
    t.setSubtree(ParseTree(4), 1);
    t4.setSubtree(t, 0);
    t4.setSubtree(t, 1);
    t5.setSubtree(t2, 0);
    t5.setSubtree(t, 1);
    scoresMap scores = getScores();
    TreeAligner c(scores, INDEL);
    ASSERT_EQ(2,c.compare(t, t2));
    ASSERT_EQ(2,c.compare(t4, t5));
    ASSERT_EQ(2*INDEL,c.compare(t, t3));
}

TEST(tree_aligner_test,advanced_check){
    ParseTree t6(ParseTree(0, {ParseTree(0,{ParseTree(0, {ParseTree(3253)
                                                , ParseTree(3252)}), ParseTree(0, {ParseTree(3254)})}),
                                                        ParseTree(0, {ParseTree(3257)})}));
    ParseTree t8(0,{ParseTree(3252), ParseTree(0, {ParseTree(2077)})});
    scoresMap scores = getScores();
    TreeAligner c(scores, INDEL);
    ASSERT_EQ(c.compare(t6, t8), 4);
}

TEST(swap_comparator,basic_check){
    ParseTree t(0);
    ParseTree t2(0);
    ParseTree t3(0);
    ParseTree t4(0);
    t.setSubtree(ParseTree(1), 0);
    t.setSubtree(ParseTree(2), 1);
    t2.setSubtree(ParseTree(2), 0);
    t2.setSubtree(ParseTree(1), 1);
    t3.setSubtree(t, 0);
    t3.setSubtree(ParseTree(1), 1);
    t4.setSubtree(ParseTree(1), 0);
    t4.setSubtree(t2, 1);
    scoresMap scores = getScores();
    SwapComparator c(4, 1);
    ASSERT_EQ(1, c.compare(t, t2));
    ASSERT_EQ(2, c.compare(t3, t4));
}