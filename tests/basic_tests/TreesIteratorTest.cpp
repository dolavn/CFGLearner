#include "gtest/gtest.h"
#include "../../include/ParseTree.h"
#include "../../include/TreesIterator.h"

extern rankedChar a;
extern rankedChar b;
extern rankedChar l;

using namespace std;

set<rankedChar> getAlphabet();


bool vecContained(const vector<ParseTree>& v1, const vector<ParseTree>& v2){
    for(auto t: v1){
        bool inv2 = false;
        for(auto t2: v2){
            if(t==t2){
                inv2=true;
                break;
            }
        }
        if(!inv2){return false;}
    }
    return true;
}

bool vecSameElem(const vector<ParseTree>& v1, const vector<ParseTree>& v2){
    return vecContained(v1, v2) && vecContained(v2, v1);
}

TEST(trees_iterator_test, vecContainedTest){
    ParseTree leaf(l.c);
    ParseTree t(a.c, {leaf, leaf});
    vector<ParseTree> v1 = {leaf};
    vector<ParseTree> v2 = {leaf, t};
    ASSERT_EQ(vecContained(v1, v2), true);
    ASSERT_EQ(vecContained(v2, v1), false);
    vector<ParseTree> v3 = {t, leaf};
    ASSERT_EQ(vecContained(v1, v3), true);
    ASSERT_EQ(vecContained(v2, v3), true);
    ASSERT_EQ(vecContained(v3, v2), true);
}

TEST(trees_iterator_test,basic_check){
    set<rankedChar> alphabet = getAlphabet();
    ParseTree leaf = ParseTree(l.c);
    vector<ParseTree> treesVec = {leaf};
    vector<ParseTree> itVec;
    TreesIterator it(alphabet, 1);
    while(it.hasNext()){
        ParseTree t = *it;
        itVec.push_back(t);
        it++;
    }
    ASSERT_EQ(vecSameElem(treesVec, itVec), true);
}

TEST(trees_iterator_test,basic_check2){
    set<rankedChar> alphabet = getAlphabet();
    ParseTree leaf = ParseTree(l.c);
    vector<ParseTree> itVec;
    ParseTree aTree = ParseTree(a.c, {leaf, leaf});
    ParseTree bTree = ParseTree(b.c, {leaf, leaf});
    vector<ParseTree> treesVec = {leaf, aTree, bTree};
    TreesIterator it(alphabet, 2);
    while(it.hasNext()){
        ParseTree t = *(it++);
        itVec.push_back(t);
    }
    ASSERT_EQ(vecSameElem(treesVec, itVec), true);
    itVec.clear();
    it = TreesIterator(alphabet, 3);
    for(auto c: {a.c, b.c}){
        treesVec.push_back(ParseTree(c, {aTree, leaf}));
        treesVec.push_back(ParseTree(c, {leaf, aTree}));
        treesVec.push_back(ParseTree(c, {bTree, leaf}));
        treesVec.push_back(ParseTree(c, {leaf, bTree}));
        treesVec.push_back(ParseTree(c, {aTree, bTree}));
        treesVec.push_back(ParseTree(c, {bTree, aTree}));
        treesVec.push_back(ParseTree(c, {bTree, bTree}));
        treesVec.push_back(ParseTree(c, {aTree, aTree}));
    }
    while(it.hasNext()){
        ParseTree t = *(it++);
        cout << t << endl;
        itVec.push_back(t);
    }
    ASSERT_EQ(vecSameElem(treesVec, itVec), true);
}
