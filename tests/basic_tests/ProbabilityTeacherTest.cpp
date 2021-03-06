#include "gtest/gtest.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/MultiplicityTeacher.h"
#include "../../include/MultiLinearMap.h"
#include "../../include/TreeComparator.h"
#include "../../include/TreesGenerator.h"
#include "../../include/TreeConstructor.h"
#include "../../include/Logger.h"

#define EPS 0.001

using namespace std;

TEST(probability_teacher_test,basic_check){
    DuplicationComparator cmp;
    ProbabilityTeacher teacher(cmp, 0.5, EPS);
    ParseTree t1(0, {ParseTree(1), ParseTree(2)});
    t1.setProb(0.5);
    teacher.addExample(t1);
    ASSERT_EQ(teacher.membership(t1), 0.5);
    ParseTree t2(0, {ParseTree(0, {ParseTree(1), ParseTree(1)}), ParseTree(2)});
    ParseTree t3(0, {ParseTree(1), ParseTree(0, {ParseTree(2), ParseTree(2)})});
    ParseTree t4(0, {ParseTree(0, {ParseTree(1), ParseTree(0, {ParseTree(1), ParseTree(1)})}), ParseTree(2)});
    ParseTree t5(0, {ParseTree(0, {ParseTree(0, {ParseTree(1), ParseTree(1)}), ParseTree(1)}), ParseTree(2)});
    ASSERT_EQ(teacher.membership(t2), 0.25);
    ASSERT_EQ(teacher.membership(t3), 0.25);
    ASSERT_EQ(teacher.membership(t4), 0.125);
    ASSERT_EQ(teacher.membership(t5), 0);
}

TEST(probability_teacher_test,equiv_test){
    DuplicationComparator cmp;
    ProbabilityTeacher teacher(cmp, 0.5, EPS);
    ParseTree t1(0, {ParseTree(1), ParseTree(2)});
    t1.setProb(0.5);
    teacher.addExample(t1);
    teacher.setupDuplicationsGenerator(1);
    set<rankedChar> alphabet = {{0, 2}, {1, 0}, {2, 0}};
    MultiplicityTreeAcceptor acc(alphabet, 3);
    acc.setLambda({0, 0, 0.5});
    MultiLinearMap m_l1(3, 0), m_l2(3, 0), m_inner(3, 2);
    m_l1.setParam(1.0, {0}); m_l2.setParam(1.0, {1});
    m_inner.setParam(1.0, {2,0,1});
    acc.addTransition(m_l1, {1, 0}); acc.addTransition(m_l2, {2, 0});
    acc.addTransition(m_inner, {0, 2});
    ParseTree* counter = teacher.equivalence(acc);
    ASSERT_TRUE(counter!=nullptr);
    SAFE_DELETE(counter);
    acc = MultiplicityTreeAcceptor(alphabet, 5);
    acc.setLambda({0, 0, 0, 0, 0.5});
    m_l1 = MultiLinearMap(5, 0); m_l2 = MultiLinearMap(5, 0);
    m_inner = MultiLinearMap(5 ,2);
    m_l1.setParam(1.0, {0}); m_l1.setParam(1.0, {2});
    m_l2.setParam(1.0, {1}); m_l2.setParam(1.0, {3});
    m_inner.setParam(0.5, {0, 2, 0});
    m_inner.setParam(0.5, {1, 3, 1});
    m_inner.setParam(1.0, {4, 0, 1});
    acc.addTransition(m_l1, {1, 0}); acc.addTransition(m_l2, {2, 0});
    acc.addTransition(m_inner, {0, 2});
    counter = teacher.equivalence(acc);
    ASSERT_TRUE(counter==nullptr);
}

TEST(probability_teacher_test,equiv_constructor_generator){
    DuplicationComparator cmp;
    ProbabilityTeacher teacher(cmp, 0.5, EPS);
    ParseTree t1(0, {ParseTree(1), ParseTree(2)});
    t1.setProb(0.5);
    teacher.addExample(t1);
    scoreTable scores;
    for(int i=0;i<4;++i){
        for(int j=0;j<4;++j){
            if(i==0&&j==0){continue;}
            vector<int> v;
            for(int k=0;k<i;++k){v.push_back(1);}
            for(int k=0;k<j;++k){v.push_back(2);}
            if(i==0||j==0){
                scores[v] = 10;
                continue;
            }
            scores[v] = 5;
        }
    }
    TreeConstructor c(scores);
    teacher.setupConstructorGenerator(c, 5, 1000);
    set<rankedChar> alphabet = {{0, 2}, {1, 0}, {2, 0}};
    MultiplicityTreeAcceptor acc(alphabet, 3);
    acc.setLambda({0, 0, 0.5});
    MultiLinearMap m_l1(3, 0), m_l2(3, 0), m_inner(3, 2);
    m_l1.setParam(1.0, {0}); m_l2.setParam(1.0, {1});
    m_inner.setParam(1.0, {2,0,1});
    acc.addTransition(m_l1, {1, 0}); acc.addTransition(m_l2, {2, 0});
    acc.addTransition(m_inner, {0, 2});
    ParseTree* counter = teacher.equivalence(acc);
    ASSERT_TRUE(counter!=nullptr);
    SAFE_DELETE(counter);

    acc = MultiplicityTreeAcceptor(alphabet, 5);
    acc.setLambda({0, 0, 0, 0, 0.5});
    m_l1 = MultiLinearMap(5, 0); m_l2 = MultiLinearMap(5, 0);
    m_inner = MultiLinearMap(5 ,2);
    m_l1.setParam(1.0, {0}); m_l1.setParam(1.0, {2});
    m_l2.setParam(1.0, {1}); m_l2.setParam(1.0, {3});
    m_inner.setParam(0.5, {0, 2, 0});
    m_inner.setParam(0.5, {1, 3, 1});
    m_inner.setParam(1.0, {4, 0, 1});
    acc.addTransition(m_l1, {1, 0}); acc.addTransition(m_l2, {2, 0});
    acc.addTransition(m_inner, {0, 2});
    counter = teacher.equivalence(acc);
    if(counter){
        cout << *counter << endl;
        cout << counter->getProb() << endl;
        cout << acc.run(*counter) << endl;
    }
    ASSERT_TRUE(counter==nullptr);
}

TEST(probability_teacher_test,equiv_constructor_generator2){
    DuplicationComparator cmp;
    ProbabilityTeacher teacher(cmp, 0.5, EPS);
    ParseTree t1(0, {ParseTree(1), ParseTree(2)});
    ParseTree t(0, {t1, ParseTree(3)});
    t.setProb(1.0);
    teacher.addExample(t);
    scoreTable scores;
    for(int i=0;i<4;++i){
        for(int j=0;j<4;++j){
            for(int p=0;p<4;++p){
                if(i==0&&j==0){continue;}
                vector<int> v;
                for(int k=0;k<i;++k){v.push_back(1);}
                for(int k=0;k<j;++k){v.push_back(2);}
                for(int k=0;k<p;++k){v.push_back(3);}
                int largerZero=0;
                for(auto& elem: {i, j, p}){
                    if(elem>0){largerZero++;}
                }
                if(largerZero==1){
                    scores[v] = 10;
                    continue;
                }
                if(p==0){
                    scores[v] = 5;
                    continue;
                }
                if(i==0){
                    scores[v] = 2;
                    continue;
                }
                if(j==0){
                    scores[v] = 0;
                    continue;
                }
                scores[v] = 1;
            }
        }
    }
    TreeConstructor c(scores);
    teacher.setupConstructorGenerator(c, 5, 1000);
    set<rankedChar> alphabet = {{0, 2}, {1, 0}, {2, 0}, {3, 0}};
    MultiplicityTreeAcceptor acc(alphabet, 5);
    acc.setLambda({0, 0, 0, 0, 1});
    MultiLinearMap m_l1(5, 0), m_l2(5, 0), m_l3(5, 0), m_inner(5, 2);
    m_l1.setParam(1.0, {0}); m_l2.setParam(1.0, {1}); m_l3.setParam(1.0, {2});
    m_inner.setParam(1.0, {3,0,1});
    m_inner.setParam(1.0, {4,3,2});
    acc.addTransition(m_l1, {1, 0}); acc.addTransition(m_l2, {2, 0});
    acc.addTransition(m_inner, {0, 2});
    ParseTree* counter = teacher.equivalence(acc);
    ASSERT_TRUE(counter!=nullptr);
    SAFE_DELETE(counter);

    acc = MultiplicityTreeAcceptor(alphabet, 8);
    acc.setLambda({1, 0, 0, 0, 0, 0, 0, 0});
    m_l1 = MultiLinearMap(8, 0); m_l2 = MultiLinearMap(8, 0); m_l3 = MultiLinearMap(8, 0);
    m_inner = MultiLinearMap(8 ,2);
    m_l1.setParam(1.0, {5}); m_l1.setParam(1.0, {1});
    m_l2.setParam(1.0, {6}); m_l2.setParam(1.0, {2});
    m_l3.setParam(1.0, {7}); m_l3.setParam(1.0, {3});
    m_inner.setParam(1.0, {0, 4, 3});
    m_inner.setParam(1.0, {4, 1, 2});
    m_inner.setParam(0.5, {3, 7, 3});
    m_inner.setParam(0.5, {1, 5, 1});
    m_inner.setParam(0.5, {2, 6, 2});
    acc.addTransition(m_l1, {1, 0}); acc.addTransition(m_l2, {2, 0}); acc.addTransition(m_l3, {3, 0});
    acc.addTransition(m_inner, {0, 2});
    counter = teacher.equivalence(acc);
    if(counter){
        cout << *counter << endl;
        cout << counter->getProb() << endl;
        cout << acc.run(*counter) << endl;
    }
    ASSERT_TRUE(counter==nullptr);
}

TEST(probability_swap_teacher_test,equiv_constructor_generator){
    SwapComparator cmp;
    ProbabilityTeacher teacher(cmp, 0.5, EPS);
    ParseTree t1(0, {ParseTree(1), ParseTree(2)});
    t1.setProb(0.5);
    teacher.addExample(t1);
    scoreTable scores;
    scores[{1,2}]=5;
    scores[{2,1}]=5;
    TreeConstructor c(scores);
    teacher.setupConstructorGenerator(c, 5, 1000);
    set<rankedChar> alphabet = {{0, 2}, {1, 0}, {2, 0}};
    MultiplicityTreeAcceptor acc(alphabet, 3);
    acc.setLambda({0, 0, 0.5});
    MultiLinearMap m_l1(3, 0), m_l2(3, 0), m_inner(3, 2);
    m_l1.setParam(1.0, {0}); m_l2.setParam(1.0, {1});
    m_inner.setParam(1.0, {2,0,1});
    acc.addTransition(m_l1, {1, 0}); acc.addTransition(m_l2, {2, 0});
    acc.addTransition(m_inner, {0, 2});
    ParseTree* counter = teacher.equivalence(acc);
    ASSERT_TRUE(counter!=nullptr);
    SAFE_DELETE(counter);
    m_inner.setParam(0.5, {2,1,0});
    acc.addTransition(m_inner, {0, 2});
    ParseTree t2(0, {ParseTree(2), ParseTree(1)});
    counter = teacher.equivalence(acc);
    ASSERT_TRUE(counter==nullptr);
}

TEST(probability_swap_teacher_test2,equiv_constructor_generator){
    SwapComparator cmp;
    ProbabilityTeacher teacher(cmp, 0.5, EPS);
    ParseTree t1(0, {ParseTree(1), ParseTree(2)});
    ParseTree t2(0, {t1, ParseTree(4)});
    t1.setProb(0.5);
    t2.setProb(0.5);
    teacher.addExample(t1);
    scoreTable scores;
    scores[{1,2}]=5;
    scores[{2,1}]=5;
    TreeConstructor c(scores);
    teacher.setupConstructorGenerator(c, 5, 1000);
    set<rankedChar> alphabet = {{0, 2}, {1, 0}, {2, 0}, {4, 0}};
    MultiplicityTreeAcceptor acc(alphabet, 5);
    acc.setLambda({0, 0, 0, 0.5, 0.5});
    MultiLinearMap m_l1(5, 0), m_l2(5, 0), m_l4(5, 0), m_inner(5, 2);
    m_l1.setParam(1.0, {0}); m_l2.setParam(1.0, {1});
    m_l4.setParam(1.0, {2});
    m_inner.setParam(1.0, {3,0,1});
    m_inner.setParam(1.0, {4,3,2});
    acc.addTransition(m_l1, {1, 0}); acc.addTransition(m_l2, {2, 0});
    acc.addTransition(m_l4, {4, 0}); acc.addTransition(m_inner, {0, 2});
    ParseTree* counter = teacher.equivalence(acc);
    ASSERT_TRUE(counter!=nullptr);
    SAFE_DELETE(counter);
    m_inner.setParam(0.5, {3,1,0});
    m_inner.setParam(0.5, {4,2,3});
    acc.addTransition(m_inner, {0, 2});
    counter = teacher.equivalence(acc);
    ASSERT_TRUE(counter==nullptr);
}