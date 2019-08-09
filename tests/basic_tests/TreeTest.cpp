//
// Created by dolavn@wincs.cs.bgu.ac.il on 1/16/19.
//

#include "gtest/gtest.h"
#include "../../include/ParseTree.h"
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>

using namespace std;

TEST(tree_test,basic_check){
    ParseTree t(2);
    ParseTree t2(3);
    t.setProb(0.2);
    ASSERT_EQ(t.getProb(), 0.2);
    t2.setSubtree(ParseTree(2),0);
    t2.setSubtree(ParseTree(3),1);
    ParseTree t3(4);
    t.setSubtree(t2,0);
    t.setSubtree(t3,1);
    vector<int> dataList={2,3,2,3,4};
    int currInd=0;
    for(auto it=t.getIterator();it.hasNext();++it){
        ASSERT_EQ(*it,dataList[currInd++]);
    }
    ASSERT_EQ(t.isLeaf(),false);
    vector<int> ind = {0,0};
    ASSERT_EQ(t[ind].isLeaf(),true);
}

TEST(tree_test,skeleton){
    ParseTree t(2);
    ParseTree t2(3);
    t2.setSubtree(ParseTree(2),0);
    t2.setSubtree(ParseTree(3),1);
    ParseTree t3(4);
    t.setSubtree(t2,0);
    t.setSubtree(t3,1);
    ParseTree t4 = t.getSkeleton();
    vector<int> dataList={-1,-1,-1,-1,-1};
    int currInd=0;
    for(auto it=t4.getIterator();it.hasNext();++it){
        ASSERT_EQ(*it,dataList[currInd++]);
    }
    ASSERT_EQ(t4.isLeaf(),false);
    vector<int> ind = {0,0};
    ASSERT_EQ(t4[ind].isLeaf(),true);
}

TEST(tree_test,index_iterator){
    ParseTree t(2);
    ParseTree t2(3);
    t2.setSubtree(ParseTree(2),0);
    t2.setSubtree(ParseTree(3),1);
    ParseTree t3(4);
    t.setSubtree(t2,0);
    t.setSubtree(t3,1);
    vector<vector<int>> indList={{},{0},{0,0},{0,1},{1}};
    int currInd=0;
    for(auto it=t.getIndexIterator();it.hasNext();++it){
        ASSERT_EQ(*it,indList[currInd++]);
    }
}

TEST(tree_test,leaf_iterator){
    ParseTree t(2);
    ParseTree t2(3);
    t2.setSubtree(ParseTree(2),0);
    t2.setSubtree(ParseTree(3),1);
    ParseTree t3(4);
    t.setSubtree(t2,0);
    t.setSubtree(t3,1);
    vector<vector<int>> indList={{0,0},{0,1},{1}};
    int currInd=0;
    for(auto it=t.getLeafIterator();it.hasNext();++it){
        ASSERT_EQ(*it,indList[currInd++]);
    }
    ParseTree t4(2);
    for(auto it=t4.getLeafIterator();it.hasNext();++it){
        ASSERT_EQ(*it,vector<int>({}));
    }
    ParseTree t5(1);
    t5.setSubtree(ParseTree(0),0);
    t5.setSubtree(ParseTree(0),1);
    currInd = 0;
    indList={{0},{1}};
    for(auto it=t5.getLeafIterator();it.hasNext();++it){
        ASSERT_EQ(*it,indList[currInd++]);
    }
}

TEST(tree_test,copy_constructor){
    double probs[] = {0.2, 0.5};
    ParseTree t(2);
    t.setProb(probs[0]);
    ParseTree t2(3);
    t2.setProb(probs[1]);
    t.setSubtree(t2,1);
    ParseTree t3(t);
    int n = 2;
    double* p = probs;
    for(auto it=t3.getIndexIterator();it.hasNext();++it){
        ASSERT_EQ(t3[*it].getData(),n++);
        ASSERT_EQ(t3[*it].getProb(),*p);
        p++;
    }
}


TEST(tree_test,assignment_operator){
    double probs[] = {0.2, 0.5};
    ParseTree t(2);
    t.setProb(probs[0]);
    ParseTree t2(3);
    t2.setProb(probs[1]);
    t.setSubtree(t2,1);
    ParseTree t3(t2);
    t3 = t;
    int n = 2;
    double* p = probs;
    for(auto it=t3.getIndexIterator();it.hasNext();++it){
        ASSERT_EQ(t3[*it].getData(),n++);
        ASSERT_EQ(t3[*it].getProb(),*p);
        p++;
    }
}

TEST(tree_test,move_constructor){
    double probs[] = {0.2, 0.5};
    ParseTree t(2);
    t.setProb(probs[0]);
    ParseTree t2(3);
    t2.setProb(probs[1]);
    t.setSubtree(t2,1);
    ParseTree t3(std::move(t));
    int n = 2;
    double* p = probs;
    for(auto it=t3.getIndexIterator();it.hasNext();++it){
        ASSERT_EQ(t3[*it].getData(),n++);
        ASSERT_EQ(t3[*it].getProb(),*p);
        p++;
    }
}

TEST(tree_test,move_assignment){
    double probs[] = {0.2, 0.5};
    ParseTree t(2);
    t.setProb(probs[0]);
    ParseTree t2(3);
    t2.setProb(probs[1]);
    t.setSubtree(t2,1);
    ParseTree t3(t2);
    t3 = std::move(t);
    int n = 2;
    double* p = probs;
    for(auto it=t3.getIndexIterator();it.hasNext();++it){
        ASSERT_EQ(t3[*it].getData(),n++);
        ASSERT_EQ(t3[*it].getProb(),*p);
        p++;
    }
}

TEST(tree_test,access_operator){
    ParseTree t(2);
    ParseTree t2(3);
    t2.setSubtree(ParseTree(2),0);
    t2.setSubtree(ParseTree(3),1);
    ParseTree t3(4);
    t.setSubtree(t2,0);
    t.setSubtree(t3,1);
    ASSERT_EQ(t[{}].getData(),2);
    ASSERT_EQ(t[{0}].getData(),3);
    ASSERT_EQ(t[{1}].getData(),4);
    vector<int> ind({0,0});
    ASSERT_EQ(t[ind].getData(),2);
    ind = vector<int>({0,1});
    ASSERT_EQ(t[ind].getData(),3);
    try{
        t[{0,0,0}];
        ASSERT_EQ(1,0);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("Index out of bounds"));
    }
}

TEST(tree_test,make_context){
    ParseTree t(2);
    ParseTree t2(3);
    t2.setSubtree(ParseTree(2),0);
    t2.setSubtree(ParseTree(3),1);
    ParseTree t3(4);
    t.setSubtree(t2,0);
    t.setSubtree(t3,1);
    std::pair<ParseTree*,ParseTree*> contextPair = t.makeContext({0});
    ParseTree* context = contextPair.first;
    ParseTree* tree = contextPair.second;
    ASSERT_EQ((*context)[{}].getContextLoc(),vector<int>({0}));
    ASSERT_EQ((*context)[{}].getIsContext(),true);
    ASSERT_EQ((*context)[{}].getData(),2);
    ASSERT_EQ((*context)[{0}].getData(),-1);
    ASSERT_EQ((*context)[{0}].getContextLoc(),vector<int>({}));
    ASSERT_EQ((*context)[{0}].getIsContext(),true);
    ASSERT_EQ((*context)[{1}].getData(),4);
    ASSERT_EQ((*context)[{1}].getContextLoc(),vector<int>({}));
    ASSERT_EQ((*context)[{1}].getIsContext(),false);
    ASSERT_EQ((*tree)[{}].getData(),3);
    ASSERT_EQ((*tree)[{}].getContextLoc(),vector<int>({}));
    ASSERT_EQ((*tree)[{}].getIsContext(),false);
    ASSERT_EQ((*tree)[{0}].getData(),2);
    ASSERT_EQ((*tree)[{0}].getContextLoc(),vector<int>({}));
    ASSERT_EQ((*tree)[{0}].getIsContext(),false);
    ASSERT_EQ((*tree)[{1}].getData(),3);
    ASSERT_EQ((*tree)[{1}].getContextLoc(),vector<int>({}));
    ASSERT_EQ((*tree)[{1}].getIsContext(),false);
    delete(context);
    delete(tree);
}

TEST(tree_test,context_merge_test){
    ParseTree t(2);
    ParseTree t2(3);
    t2.setSubtree(ParseTree(2),0);
    t2.setSubtree(ParseTree(3),1);
    ParseTree t3(4);
    t.setSubtree(t2,0);
    t.setSubtree(t3,1);
    std::pair<ParseTree*,ParseTree*> contextPair = t.makeContext({0});
    ParseTree* context = contextPair.first;
    ParseTree* tree = contextPair.second;
    ParseTree* merged = context->mergeContext(*tree);
    ParseTree* merged2 = context->mergeContext(ParseTree(4,{ParseTree(2),ParseTree(1)}));
    ASSERT_EQ(*merged==t,true);
    ASSERT_EQ(merged->getIsContext(),false);
    ASSERT_EQ(*merged2==t,false);
    ASSERT_EQ(*merged2==ParseTree(2,{ParseTree(4,{ParseTree(2),ParseTree(3)}),ParseTree(4)}),false);
    ASSERT_EQ(*merged2==ParseTree(2,{ParseTree(4,{ParseTree(2),ParseTree(1)}),ParseTree(4)}),true);
    try{
        merged->mergeContext(t);
        ASSERT_EQ(1,0);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("Tree must be a context"));
    }
    delete(merged);
    delete(merged2);
    delete(context);
    delete(tree);
}

TEST(tree_test,equivalence_test){
    ParseTree t(2);
    ParseTree t2(2);
    ParseTree t3(3);
    ASSERT_EQ(t==t2,true);
    ASSERT_EQ(t==t3,false);
    ASSERT_EQ(t!=t2,false);
    ASSERT_EQ(t!=t3,true);
    ParseTree t4(2,{t,t2});
    ParseTree t5(2,{t,t3});
    ASSERT_EQ(t4==t5,false);
    ASSERT_EQ(t4!=t5,true);
    ParseTree t6(2);
    ParseTree t7(2);
    t6.setSubtree(t2,0);
    t7.setSubtree(t3,0);
    ASSERT_EQ(t6==t7,false);
    ASSERT_EQ(t6!=t7,true);
    ParseTree t8(2);
    ParseTree t9(2);
    t8.setSubtree(t2,1);
    t9.setSubtree(t2,1);
    ASSERT_EQ(t8==t9,true);
    ASSERT_EQ(t8!=t9,false);
}

TEST(tree_test, topology_test){
    ParseTree t(2);
    ASSERT_EQ(t.sameTopology(t.getSkeleton()),true);
    ParseTree t2(2);
    ParseTree t4(2,{t,t2});
    ASSERT_EQ(t4.sameTopology(t4.getSkeleton()),true);
    ASSERT_EQ(t4.sameTopology(t),false);
}

TEST(tree_test, advanced_topology_test){
    ParseTree t(2);
    ParseTree t2(4);
    ParseTree* curr = &t;
    ParseTree* curr2 = &t2;
    for(int i=0;i<100;++i){
        curr->setSubtree(ParseTree(2),0);
        curr->setSubtree(ParseTree(2),1);
        curr2->setSubtree(ParseTree(3),0);
        curr2->setSubtree(ParseTree(4),1);
        ParseTree* next = curr->getSubtrees()[1];
        ASSERT_EQ(t.sameTopology(t.getSkeleton()),true);
        ASSERT_EQ(t.sameTopology(t2),true);
        ASSERT_EQ(t.sameTopology(*next),false);
        curr = curr->getSubtrees()[0];
        curr2 = curr2->getSubtrees()[0];
    }
}

TEST(tree_test, apply_weights_test){
    srand(time(NULL));
    int w = rand()%10+1;
    ParseTree t(0);
    ParseTree t2(w);
    ParseTree* curr = &t;
    ParseTree* curr2 = &t2;
    for(int i=0;i<100;++i){
        curr->setSubtree(ParseTree(0),0);
        curr->setSubtree(ParseTree(0),1);
        curr2->setSubtree(ParseTree(rand()%10+1),0);
        curr2->setSubtree(ParseTree(rand()%10+1),1);
        curr = curr->getSubtrees()[0];
        curr2 = curr2->getSubtrees()[0];
    }
    t.applyWeights(t2);
    for(auto it=t.getIndexIterator();it.hasNext();++it){
        ASSERT_EQ(t[*it].getWeight(),t2[*it].getData());
    }
    try{
        t.applyWeights(t2[{0}]);
        ASSERT_EQ(true,false);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("Weights tree must have the same topology as this tree!"));
    }
}

TEST(tree_test, difference_test){
    ParseTree t(2);
    ParseTree t2(2);
    ASSERT_EQ(t-t2,0);
    t2 = ParseTree(3);
    t2.setWeight(5);
    t.setWeight(2);
    ASSERT_EQ(t-t2,7);
}


TEST(tree_test, pointer_list_test){
    ParseTree t(1);
    ParseTree t2(2);
    ParseTree t3(3);
    t.setPointer(&t2, 0);
    t.setPointer(&t3, 1);
    vector<const ParseTree*> v = t.getInOrderPtrList();
    ASSERT_EQ(3, v.size());
    ASSERT_EQ(&t2, v[0]);
    ASSERT_EQ(&t3, v[1]);
    ASSERT_EQ(&t, v[2]);
    t.setPointer(nullptr, 0);
    t.setPointer(nullptr, 1);
}

vector<ParseTree*> createPtrVec(int numNodes){
    vector<ParseTree*> ans;
    if(numNodes==0){
        return ans;
    }
    int lNum = numNodes%2==0?(numNodes)/2:(numNodes-1)/2;
    int rNum = numNodes%2==0?lNum-1:lNum;
    vector<ParseTree*> lVec = createPtrVec(lNum);
    vector<ParseTree*> rVec = createPtrVec(rNum);
    auto root = new ParseTree(1);
    if(!lVec.empty()){root->setPointer(lVec[lVec.size()-1], 0);}
    if(!rVec.empty()){root->setPointer(rVec[rVec.size()-1], 1);}
    for(auto ptr: lVec){
        ans.push_back(ptr);
    }
    for(auto ptr: rVec){
        ans.push_back(ptr);
    }
    ans.push_back(root);
}


TEST(tree_test, pointer_list_test_advanced){
    for(int nodesNum=1; nodesNum<100; nodesNum++) {
        vector<ParseTree*> ptrVec = createPtrVec(nodesNum);
        ParseTree *tree = ptrVec[nodesNum - 1];
        vector<const ParseTree*> v = tree->getInOrderPtrList();
        for (unsigned int i = 0; i < nodesNum; ++i) {
            ASSERT_EQ(ptrVec[i], v[i]);
        }
        delete (tree);
    }
}


TEST(tree_test, advanced_difference_test){
    ParseTree t(2);
    ParseTree t2(2);
    ParseTree left(1);
    ParseTree center1(4);
    ParseTree center2(5);
    ParseTree right(6);
    left.setWeight(5);
    t.setWeight(4);
    t2.setWeight(16);
    center1.setWeight(4);
    center2.setWeight(2);
    right.setWeight(4);
    t.setSubtree(left, 0); t2.setSubtree(left, 0);
    t.setSubtree(center1, 1); t2.setSubtree(center2, 1);
    t2.setSubtree(right, 2);
    ASSERT_EQ(t-t2,10);
}

TEST(tree_test, advanced_difference_test2){
    ParseTree t(0), w(5);
    t.setSubtree(ParseTree(1), 0);
    w.setSubtree(ParseTree(1), 0);
    t.setSubtree(ParseTree(0, {ParseTree(1), ParseTree(2)}), 1);
    w.setSubtree(ParseTree(4, {ParseTree(2), ParseTree(2)}), 1);
    t.applyWeights(w);
    ParseTree t2(0), w2(5);
    t2.setSubtree(ParseTree(0, {ParseTree(2), ParseTree(2)}), 0);
    w2.setSubtree(ParseTree(1, {ParseTree(1), ParseTree(1)}), 0);
    t2.setSubtree(ParseTree(0, {ParseTree(1), ParseTree(2)}), 1);
    w2.setSubtree(ParseTree(4, {ParseTree(2), ParseTree(2)}), 1);
    t2.applyWeights(w2);
    ParseTree tTest(0), wTest(5);
    tTest.setSubtree(ParseTree(0, {ParseTree(1), ParseTree(1)}), 0);
    wTest.setSubtree(ParseTree(1, {ParseTree(1), ParseTree(1)}), 0);
    tTest.setSubtree(ParseTree(0, {ParseTree(1), ParseTree(2)}), 1);
    wTest.setSubtree(ParseTree(4, {ParseTree(2), ParseTree(2)}), 1);
    tTest.applyWeights(wTest);
    ParseTree tTest2(0), wTest2(5);
    tTest2.setSubtree(ParseTree(0, {ParseTree(2), ParseTree(2)}), 0);
    wTest2.setSubtree(ParseTree(1, {ParseTree(1), ParseTree(1)}), 0);
    tTest2.setSubtree(ParseTree(0, {ParseTree(2), ParseTree(2)}), 1);
    wTest2.setSubtree(ParseTree(4, {ParseTree(2), ParseTree(2)}), 1);
    tTest2.applyWeights(wTest2);
}

TEST(tree_test, get_all_contexts_test){
    ParseTree l(0);
    ParseTree t(1, {l, l});
    ParseTree t2(1, {t, l});
    vector<ParseTree*> v = l.getAllContexts();
    ASSERT_EQ(v.size(), 1);
    ASSERT_EQ(v[0]->getIsContext(), true);
    ASSERT_EQ(v[0]->getContextLoc(), vector<int>({}));
    delete(v[0]); v[0]=nullptr;
    v = t.getAllContexts();
    ASSERT_EQ(v.size(), 3);
    ASSERT_EQ(v[0]->getIsContext(), true);
    ASSERT_EQ(v[0]->getContextLoc(), vector<int>({}));
    ASSERT_EQ(v[1]->getIsContext(), true);
    ASSERT_EQ(v[1]->getContextLoc(), vector<int>({0}));
    ASSERT_EQ(v[2]->getIsContext(), true);
    ASSERT_EQ(v[2]->getContextLoc(), vector<int>({1}));
    for(auto& ptr: v){
        if(ptr){
            delete(ptr);
            ptr=nullptr;
        }
    }
    v = t2.getAllContexts();
    ASSERT_EQ(v.size(), 5);
    ASSERT_EQ(v[0]->getIsContext(), true);
    ASSERT_EQ(v[0]->getContextLoc(), vector<int>({}));
    ASSERT_EQ(v[1]->getIsContext(), true);
    ASSERT_EQ(v[1]->getContextLoc(), vector<int>({0}));
    ASSERT_EQ(v[2]->getIsContext(), true);
    ASSERT_EQ(v[2]->getContextLoc(), vector<int>({0, 0}));
    ASSERT_EQ(v[3]->getIsContext(), true);
    ASSERT_EQ(v[3]->getContextLoc(), vector<int>({0, 1}));
    ASSERT_EQ(v[4]->getIsContext(), true);
    ASSERT_EQ(v[4]->getContextLoc(), vector<int>({1}));
    for(auto& ptr: v){
        if(ptr){
            delete(ptr);
            ptr=nullptr;
        }
    }
}


TEST(tree_test,latex_tree_test){
    ParseTree t(2);
    ASSERT_EQ(t.getLatexTree(), std::string("\\Tree [.2 ]"));
    t.setSubtree(ParseTree(0), 0);
    t.setSubtree(ParseTree(1), 1);
    ASSERT_EQ(t.getLatexTree(), std::string("\\Tree [.2 0 1 ]"));
    t.setSubtree(ParseTree(0, {ParseTree(1), ParseTree(2)}), 0);
    ASSERT_EQ(t.getLatexTree(), std::string("\\Tree [.2 [.0 1 2 ] 1 ]"));
}

TEST(tree_test,hash_test){
    ParseTree t(2);
    ParseTree t2(2);
    ParseTree t3(3);
    ASSERT_EQ(t.getHash()==t2.getHash(),true);
    ParseTree t8(2);
    ParseTree t9(2);
    t8.setSubtree(t,1);
    t9.setSubtree(t,1);
    ASSERT_EQ(t8.getHash()==t9.getHash(),true);
}