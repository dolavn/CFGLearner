#include "gtest/gtest.h"
#include "../../include/TreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/Teacher.h"
#include "../../include/Learner.h"

using namespace std;

Teacher* getTeacher(){
    ParseTree t(1,{ParseTree(0),ParseTree(0)});
    ParseTree t2(1,{ParseTree(0),ParseTree(1)});
    ParseTree t3(2,{ParseTree(2,{ParseTree(0),ParseTree(0)}),ParseTree(0)});
    ParseTree t4(2,{ParseTree(1,{ParseTree(0),ParseTree(0)}),ParseTree(0)});
    ParseTree t5(1,{ParseTree(0),ParseTree(1,{ParseTree(0),ParseTree(0)})});
    auto teacher = new SimpleTeacher();
    teacher->addPositiveExample(t);
    teacher->addPositiveExample(t4);
    teacher->addPositiveExample(t5);
    teacher->addNegativeExample(t2);
    teacher->addNegativeExample(t3);
    return teacher;
}

Teacher* getTeacher2(){
    ParseTree t(0,{ParseTree(1),ParseTree(2)});
    ParseTree t2(0,{ParseTree(2),ParseTree(1)});
    auto teacher = new SimpleTeacher();
    teacher->addPositiveExample(t);
    teacher->addNegativeExample(t2);
    return teacher;
}

Teacher* getTeacher3(){
    vector<ParseTree> vec(20);
    vec[0] = ParseTree(0,{ParseTree(1),ParseTree(2)});
    for(unsigned int i=1; i<20;++i){
        vec[i] = ParseTree(0,{ParseTree(1), ParseTree(0, {vec[i-1], ParseTree(2)})});
    }
    auto teacher = new SimpleTeacher();
    for(auto& tree: vec){
        teacher->addPositiveExample(tree);
    }
    return teacher;
}

Teacher* getTeacher4(){
    vector<ParseTree> vec(20);
    vec[0] = ParseTree(0,{ParseTree(1),ParseTree(2)});
    for(unsigned int i=1; i<20;++i){
        vec[i] = ParseTree(0,{ParseTree(1), ParseTree(0, {vec[i-1], ParseTree(2)})});
    }
    auto teacher = new SimpleTeacher();
    for(unsigned int i=0;i<vec.size();i++){
        if(i%2==0){
            teacher->addNegativeExample(vec[i]);
        }else{
            teacher->addPositiveExample(vec[i]);
        }
    }
    return teacher;
}

void clearTeacher(Teacher* t){
    if(t){
        delete(t);
    }
}

TEST(learner_test, observation_table_test){
    Teacher* teacher = getTeacher();
    initLearner(*teacher);
    ParseTree t(1,{ParseTree(0),ParseTree(0)});
    ParseTree t4(2,{ParseTree(1,{ParseTree(0),ParseTree(0)}),ParseTree(0)});
    pair<ParseTree*,ParseTree*> con = t.makeContext("");
    pair<ParseTree*,ParseTree*> con2 = t.makeContext("1");
    learnerAddContext(*con.first);
    learnerAddContext(*con2.first);
    learnerAddTree(*con.second);
    learnerAddTree(ParseTree(4));
    learnerAddTree(t4);
    ParseTree* alternate1 = con.first->mergeContext(ParseTree(2));
    ASSERT_EQ(learnerGetObs(*con.second),vector<bool>({true,true}));
    ASSERT_EQ(learnerGetObs(ParseTree(4)),vector<bool>({false,false}));
    ASSERT_EQ(learnerGetObs(*alternate1),vector<bool>({false,false}));
    ASSERT_EQ(learnerGetObs(t4),vector<bool>({true,false}));
    delete(con.first);
    delete(con.second);
    delete(con2.first);
    delete(con2.second);
    delete(alternate1);
    clearLearner();
    clearTeacher(teacher);
}

TEST(learner_test, synthesize_test){
    Teacher* teacher = getTeacher();
    initLearner(*teacher);
    TreeAcceptor acc = learnerSynthesize();
    ASSERT_EQ(acc.getStatesNum(),0);
    ParseTree t(4);
    ParseTree t2(2,{ParseTree(1,{ParseTree(0),ParseTree(0)}),ParseTree(0)});
    pair<ParseTree*,ParseTree*> con = t.makeContext("");
    learnerAddContext(*con.first);
    learnerAddTree(t);
    learnerAddTree(t2);
    acc = learnerSynthesize();
    ASSERT_EQ(acc.getStatesNum(),2);
    ASSERT_EQ(acc.isAccepting(0),false);
    ASSERT_EQ(acc.isAccepting(1),true);
    ASSERT_EQ(acc.run(t2),true);
    ASSERT_EQ(acc.run(ParseTree(1,{ParseTree(4),ParseTree(0)})),true);
    ASSERT_EQ(acc.run(t),false);
    delete(con.first);
    delete(con.second);
    clearLearner();
    clearTeacher(teacher);
}

TEST(learner_test, decompose_test){
    Teacher* teacher = getTeacher();
    initLearner(*teacher);
    ParseTree t(0);
    ParseTree t2(1,{ParseTree(1,{ParseTree(0),ParseTree(0)}),ParseTree(0)});
    pair<ParseTree*,ParseTree*> pObserved = learnerDecompose(t2);
    pair<ParseTree*,ParseTree*> pExpected = t2.makeContext("00");
    ASSERT_EQ(*pObserved.first,*pExpected.first);
    ASSERT_EQ(*pObserved.second,*pExpected.second);
    learnerAddTree(*pObserved.second);
    delete(pObserved.first); delete(pObserved.second);
    delete(pExpected.first); delete(pExpected.second);
    pObserved = learnerDecompose(t2);
    pExpected = t2.makeContext("0");
    ASSERT_EQ(*pObserved.first,*pExpected.first);
    ASSERT_EQ(*pObserved.second,*pExpected.second);
    learnerAddTree(*pObserved.second);
    delete(pObserved.first); delete(pObserved.second);
    delete(pExpected.first); delete(pExpected.second);
    pObserved = learnerDecompose(t2);
    pExpected = t2.makeContext("0");
    ASSERT_EQ(*pObserved.first,*pExpected.first);
    ASSERT_EQ(*pObserved.second,*pExpected.second);
    delete(pObserved.first); delete(pObserved.second);
    delete(pExpected.first); delete(pExpected.second);
    ParseTree t3(1,{ParseTree(0),ParseTree(0)});
    pair<ParseTree*,ParseTree*> context = t3.makeContext("0");
    learnerAddContext(*context.first);
    delete(context.first); delete(context.second);
    pObserved = learnerDecompose(t2);
    pExpected = t2.makeContext("");
    ASSERT_EQ(*pObserved.first,*pExpected.first);
    ASSERT_EQ(*pObserved.second,*pExpected.second);
    delete(pObserved.first); delete(pObserved.second);
    delete(pExpected.first); delete(pExpected.second);
    clearLearner();
    clearTeacher(teacher);
}

TEST(learner_test, extend_test){
    Teacher* teacher = getTeacher2();
    initLearner(*teacher);
    learnerExtend(ParseTree(0,{ParseTree(1),ParseTree(2)}));
    ASSERT_EQ(learnerGetObs(ParseTree(1)),vector<bool>{});
    learnerExtend(ParseTree(0,{ParseTree(1),ParseTree(2)}));
    ASSERT_EQ(learnerGetObs(ParseTree(1)),vector<bool>{});
    learnerExtend(ParseTree(0,{ParseTree(1),ParseTree(2)}));
    ASSERT_EQ(learnerGetObs(ParseTree(1)),vector<bool>{false});
    clearLearner();
    clearTeacher(teacher);
}

::testing::AssertionResult hasChar(TreeAcceptor& acc, rankedChar& c){
    vector<rankedChar> alphabet = acc.getAlphabet();
    for(rankedChar& curr: alphabet){
        if(curr==c){
            return ::testing::AssertionSuccess();
        }
    }
    return ::testing::AssertionFailure() << " char " << "(" << c.c << "," << c.rank << ")  doesn't exist in alphabet.";
}

int hasState(TreeAcceptor& acc, rankedChar& c, const vector<int>& states){
    vector<transition> transitions = acc.getTransitions();
    //cerr << "char:" <<  c.c << endl;
    //cerr << "expected states [";
    for(const int& state: states){
        //cerr << state << ",";
    }
    //cerr << "]" << endl;
    for(transition& t: transitions){
        //cerr << "delta(" << t.c.c;
        for(const int& state: t.statesSeq){
            //cerr << "," << state;
        }
        //cerr << ")=" << t.targetState << endl;
        if(t.c==c && t.statesSeq==states){
            return t.targetState;
        }
    }
    return -1;
}

TEST(learner_test, learn_test_basic){
    Teacher* teacher = getTeacher3();
    TreeAcceptor acc = learn(*teacher);
    rankedChar c1{1, 0};
    rankedChar c2{2, 0};
    rankedChar cInt{0, 2};
    EXPECT_TRUE(hasChar(acc, c1));
    EXPECT_TRUE(hasChar(acc, c2));
    EXPECT_TRUE(hasChar(acc, cInt));
    ASSERT_EQ(acc.getAlphabet().size(), 3);
    int q1,q2, qInt;
    EXPECT_PRED_FORMAT1([&](const char*,TreeAcceptor& acc){
        q1 = hasState(acc,c1,vector<int>());
        if(q1!=-1){
            return ::testing::AssertionSuccess();
        }
        return ::testing::AssertionFailure() << "no state for reading 1 on leaf";
    },acc);
    EXPECT_PRED_FORMAT1([&](const char*,TreeAcceptor& acc){
        q2 = hasState(acc,c2,vector<int>());
        if(q2!=-1){
            return ::testing::AssertionSuccess();
        }
        return ::testing::AssertionFailure() << "no state for reading 2 on leaf";
    },acc);
    EXPECT_PRED_FORMAT1([&](const char*,TreeAcceptor& acc){
        qInt = hasState(acc,cInt,vector<int>({q1, q2}));
        if(qInt!=-1){
            return ::testing::AssertionSuccess();
        }
        return ::testing::AssertionFailure() << "No state for interior nodes";
    },acc);
    EXPECT_TRUE(q1!=q2);
    EXPECT_TRUE(q1!=qInt);
    EXPECT_TRUE(qInt!=q2);
    EXPECT_TRUE(!acc.isAccepting(q1));
    EXPECT_TRUE(!acc.isAccepting(q2));
    EXPECT_TRUE(acc.isAccepting(qInt));
    ASSERT_EQ(acc.getStatesNum(),3);
    clearTeacher(teacher);
}

TEST(learner_test, learn_test_advanced){
    Teacher* teacher = getTeacher4();
    TreeAcceptor acc = learn(*teacher);
    rankedChar c1{1, 0};
    rankedChar c2{2, 0};
    rankedChar cInt{0, 2};
    EXPECT_TRUE(hasChar(acc, c1));
    EXPECT_TRUE(hasChar(acc, c2));
    EXPECT_TRUE(hasChar(acc, cInt));
    ASSERT_EQ(acc.getAlphabet().size(), 3);
    int q1,q2,q3,q4,q5;
    EXPECT_PRED_FORMAT1([&](const char*,TreeAcceptor& acc){
        q1 = hasState(acc,c1,vector<int>());
        if(q1!=-1){
            return ::testing::AssertionSuccess();
        }
        return ::testing::AssertionFailure() << "no state for reading 1 on leaf";
    },acc);
    EXPECT_PRED_FORMAT1([&](const char*,TreeAcceptor& acc){
        q2 = hasState(acc,c2,vector<int>());
        if(q2!=-1){
            return ::testing::AssertionSuccess();
        }
        return ::testing::AssertionFailure() << "no state for reading 2 on leaf";
    },acc);
    EXPECT_PRED_FORMAT1([&](const char*,TreeAcceptor& acc){
        if(q1!=q2){
            return ::testing::AssertionSuccess();
        }
        return ::testing::AssertionFailure() << "reading 1 and reading 2 on leaf lead to same state";
    },acc);
    EXPECT_PRED_FORMAT1([&](const char*,TreeAcceptor& acc){
        q3 = hasState(acc,cInt,vector<int>({q1, q2}));
        if(q3!=-1){
            return ::testing::AssertionSuccess();
        }
        return ::testing::AssertionFailure() << "No state for odd interior nodes";
    },acc);
    EXPECT_PRED_FORMAT1([&](const char*,TreeAcceptor& acc){
        q4 = hasState(acc,cInt,vector<int>({q3, q2}));
        if(q4!=-1){
            return ::testing::AssertionSuccess();
        }
        return ::testing::AssertionFailure() << "No state for even interior nodes";
    },acc);
    EXPECT_PRED_FORMAT1([&](const char*,TreeAcceptor& acc){
        q5 = hasState(acc,cInt,vector<int>({q1, q4}));
        if(q4!=-1){
            return ::testing::AssertionSuccess();
        }
        return ::testing::AssertionFailure() << "No accepting states";
    },acc);
    EXPECT_TRUE(q3!=q4);
    EXPECT_TRUE(q3!=q5);
    EXPECT_TRUE(q4!=q5);
    EXPECT_TRUE(!acc.isAccepting(q3));
    EXPECT_TRUE(!acc.isAccepting(q4));
    EXPECT_TRUE(acc.isAccepting(q5));
    ASSERT_EQ(acc.getStatesNum(),5);
    clearTeacher(teacher);
}



