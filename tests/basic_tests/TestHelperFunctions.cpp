#include "TestsHelperFunctions.h"
#include "ParseTree.h"
#include "MultiLinearMap.h"
#include "MultiplicityTreeAcceptor.h"
#include <stack>

using namespace std;

rankedChar l{0,0};
rankedChar a{1,2};
rankedChar b{2,2};
rankedChar inner{0, 2};
rankedChar l1{1, 0};
rankedChar l2{2, 0};

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


MultiplicityTreeAcceptor getCountingAcceptor(){
    MultiplicityTreeAcceptor acc(getAlphabetCounting(), 2);
    MultiLinearMap m1(2, 0);
    MultiLinearMap m2(2, 0);
    MultiLinearMap m3(2, 2);
    m1.setParam(1.0, {0}); m1.setParam(0.0, {1});
    m2.setParam(0.0, {0}); m2.setParam(1.0, {1});
    m3.setParam(2.0, {0, 0, 0}); m3.setParam(-1.0f, {1, 0, 0});
    m3.setParam(1.0, {0, 0, 1}); m3.setParam(0.0, {1, 0, 1});
    m3.setParam(1.0, {0, 1, 0}); m3.setParam(0.0, {1, 1, 0});
    m3.setParam(0.0, {0, 1, 1}); m3.setParam(1.0, {1, 1, 1});
    acc.addTransition(m1, l1); acc.addTransition(m2, l2);
    acc.addTransition(m3, inner);
    acc.setLambda({1.0, 0.0});
    return acc;
}
FunctionalMultiplicityTeacher getFuncTeacherNeg(){
    ParseTree t1(1); ParseTree t2(2);
    TreesIterator it({&t1, &t2}, getAlphabetProb(), 2);
    FunctionalMultiplicityTeacher teacher(0.05, 0, [](const ParseTree& tree){
        return -1;
    }, it);
    return teacher;
}

SimpleMultiplicityTeacher getMultiplicityTeacher2(){

}

SimpleMultiplicityTeacher getProbTeacherPositiveTest(){
    set<rankedChar> alphabet = getAlphabetProb();
    ParseTree i12(0, {ParseTree(1), ParseTree(2)});
    ParseTree i21(0, {ParseTree(2), ParseTree(1)});
    ParseTree t1(0, {ParseTree(1), i12});
    t1.setProb(0.09);
    ParseTree t2(0, {ParseTree(2), i12});
    t2.setProb(0.36);
    ParseTree t3(0, {ParseTree(1), i21});
    t3.setProb(0.09);
    ParseTree t4(0, {ParseTree(2), i21});
    t4.setProb(0.36);
    SimpleMultiplicityTeacher teacher(0.05, 0.0);
    teacher.addExample(t1); teacher.addExample(t2);
    teacher.addExample(t3); teacher.addExample(t4);
    return teacher;
}


set<rankedChar> getAlphabetCounting(){
    return set<rankedChar>({inner, l1, l2});
}



FunctionalMultiplicityTeacher getFuncTeacher(){
    TreesIterator it(getAlphabetCounting(), 3);
    FunctionalMultiplicityTeacher teacher(0.1, 0, [](const ParseTree& tree){
        int c = 0;
        auto it = tree.getIterator();
        while(it.hasNext()){
            if(*it==l1.c){
                c++;
            }
            ++it;
        }
        return c;
    }, it);
    return teacher;
}

FunctionalMultiplicityTeacher getFuncTeacherProb(){
    ParseTree t1(1); ParseTree t2(2);
    TreesIterator it({&t1, &t2}, getAlphabetProb(), 2);
    FunctionalMultiplicityTeacher teacher(0.05, 0, [](const ParseTree& tree){
        unsigned long maxLenL=0; int leftChar=tree.getData();
        unsigned long maxLenR=0; int rightChar=tree.getData();
        auto indexIt = tree.getIndexIterator();
        while(indexIt.hasNext()){
            vector<int> ind = *indexIt;
            bool left=true, right=true;
            for(auto elem: ind){
                if(elem!=0){left=false;}
                if(elem!=1){right=false;}
            }
            if(left && ind.size()>maxLenL){leftChar=tree.getNode(*indexIt).getData();maxLenL=ind.size();}
            if(right && ind.size()>maxLenR){rightChar=tree.getNode(*indexIt).getData();maxLenR=ind.size();}
            ++indexIt;
        }
        if(leftChar==1 && rightChar==1){return 0.9;}
        if(leftChar==1 && rightChar==2){return 0.5;}
        if(leftChar==2 && rightChar==1){return 0.5;}
        if(leftChar==2 && rightChar==2){return 0.1;}
        return 0.0;
    }, it);
    return teacher;
}


set<rankedChar> getAlphabet(){
    set<rankedChar> alphabet = {a,b,l};
    return alphabet;
}

set<rankedChar> getAlphabetProb(){
    set<rankedChar> alphabet = {inner,l1,l2};
    return alphabet;
}

SimpleMultiplicityTeacher getDistributionTeacher(double totalDist){
    vector<ParseTree> trees;
    vector<rankedChar> leaves = {l1, l2};
    double prob = totalDist/pow(2, 4);
    for(IndexArray arr({2, 2, 2, 2});!arr.getOverflow();++arr){
        vector<int> leavesTags;
        for(int i=0;i<arr.getDimensions();++i){
            leavesTags.push_back(leaves[arr[i]].c);
        }
        ParseTree t(0, {ParseTree(leavesTags[0]), ParseTree(0,
                {ParseTree(leavesTags[1]), ParseTree(0,
                        {ParseTree(leavesTags[2]), ParseTree(leavesTags[3])})})});
        t.setProb(prob);
        trees.push_back(t);
    }
    double defaultVal = (1-totalDist)/20;
    SimpleMultiplicityTeacher ans(0.001, defaultVal);
    for(const auto& tree: trees){
        ans.addExample(tree);
    }
    return ans;
}