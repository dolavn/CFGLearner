#include "gtest/gtest.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ParseTree.h"
#include "../../include/MultiplicityTeacher.h"
#include "../../include/MultiplicityTreeAcceptor.h"
#include "../../include/ObservationTable.h"
#include "../../include/TreesIterator.h"
#include "../../include/Learner.h"
#include "../../include/Logger.h"

extern rankedChar a;
extern rankedChar b;
extern rankedChar l;
extern rankedChar l1;
extern rankedChar l2;
extern rankedChar inner;

using namespace std;

set<rankedChar> getAlphabet();
set<rankedChar> getAlphabetProb();

set<rankedChar> getAlphabetSmall(){
    return set<rankedChar>({a, l});
}

MultiplicityTreeAcceptor getCountingAcc();

SimpleMultiplicityTeacher getMultiplicityTeacher(){
    SimpleMultiplicityTeacher teacher(0.2, 0.0);
    set<rankedChar> alphabet = getAlphabet();
    ParseTree t(1, {ParseTree(0), ParseTree(0)});
    t.setProb(2.0);
    ParseTree t2(1, {t, t});
    t2.setProb(4.0);
    ParseTree t3(1, {t2, t2});
    t3.setProb(8.0);
    ParseTree t4(1, {t2, t});
    t4.setProb(6.0);
    ParseTree t5(1, {t, t2});
    t5.setProb(6.0);
    ParseTree t6(1, {ParseTree(0), t});
    t6.setProb(3.0);
    ParseTree t7(1, {t, ParseTree(0)});
    t7.setProb(3.0);
    ParseTree t8(1, {ParseTree(0), t2});
    t8.setProb(5.0);
    ParseTree t9(1, {t2, ParseTree(0)});
    t9.setProb(5.0);
    ParseTree leaf(0);
    leaf.setProb(1.0);
    teacher.addExample(leaf);
    teacher.addExample(t);
    teacher.addExample(t2);
    teacher.addExample(t3);
    teacher.addExample(t4);
    teacher.addExample(t5);
    teacher.addExample(t6);
    teacher.addExample(t7);
    teacher.addExample(t8);
    teacher.addExample(t9);
    return teacher;
}

SimpleMultiplicityTeacher getMultiplicityProbTeacher(){
    SimpleMultiplicityTeacher teacher(0.05, 0.0);
    set<rankedChar> alphabet = getAlphabetProb();
    ParseTree t(0, {ParseTree(1), ParseTree(1)});
    t.setProb(0.9);
    ParseTree t2(0, {ParseTree(1), ParseTree(2)});
    t2.setProb(0.5);
    ParseTree t3(0, {ParseTree(2), ParseTree(1)});
    t3.setProb(0.5);
    ParseTree t4(0, {ParseTree(2), ParseTree(2)});
    t4.setProb(0.1);
    ParseTree t5(0, {t, t2});
    t5.setProb(0.5);
    ParseTree t6(0, {t2, t});
    t6.setProb(0.9);
    ParseTree t7(0, {ParseTree(2), t});
    t7.setProb(0.5);
    ParseTree t8(0, {t, ParseTree(2)});
    t8.setProb(0.5);
    ParseTree t9(0, {ParseTree(1), t});
    teacher.addExample(t);
    teacher.addExample(t2);
    teacher.addExample(t3);
    teacher.addExample(t4);
    teacher.addExample(t5);
    teacher.addExample(t6);
    teacher.addExample(t7);
    teacher.addExample(t8);
    teacher.addExample(t9);
    return teacher;
}

FunctionalMultiplicityTeacher getFuncTeacherProb();

TEST(hankel_matrix_test,basic_check){
    set<rankedChar> alphabet = getAlphabet();
    SimpleMultiplicityTeacher teacher = getMultiplicityTeacher();
    HankelMatrix h(teacher);
    ParseTree leaf(0);
    ParseTree t(1, {ParseTree(0), ParseTree(0)});
    ParseTree t2(1, {t, t});
    pair<ParseTree*, ParseTree*> pair1 = t.makeContext({});
    pair<ParseTree*, ParseTree*> pair2 = t.makeContext({0});
    ParseTree* emptyContext = pair1.first;
    ParseTree* secondContext = pair2.first;
    ASSERT_EQ(h.hasContext(*emptyContext), false);
    ASSERT_EQ(h.hasContext(*secondContext), false);
    h.addContext(*emptyContext);
    ASSERT_EQ(h.hasContext(*emptyContext), true);
    h.addContext(*secondContext);
    ASSERT_EQ(h.hasContext(*emptyContext), true);
    ASSERT_ANY_THROW(h.addContext(*secondContext));
    delete(pair1.first);
    delete(pair1.second);
    pair1 = {nullptr, nullptr};
    delete(pair2.first);
    delete(pair2.second);
    pair2 = {nullptr, nullptr};
    h.addTree(leaf);
    h.addTree(t);
    h.addTree(t2);
    vector<double> obs = h.getObs(leaf);
    ASSERT_EQ(obs.size(), 2);
    ASSERT_EQ(obs[0], 1.0);
    ASSERT_EQ(obs[1], 2.0);
    obs = h.getObs(t);
    ASSERT_EQ(obs.size(), 2);
    ASSERT_EQ(obs[0], 2.0);
    ASSERT_EQ(obs[1], 3.0);
    obs = h.getObs(t2);
    ASSERT_EQ(obs.size(), 2);
    ASSERT_EQ(obs[0], 4.0);
    ASSERT_EQ(obs[1], 5.0);
    const vector<ParseTree*> r = h.getR();
    const vector<ParseTree*> s = h.getS();
    ASSERT_EQ(r.size(), 1);
    ASSERT_EQ(s.size(), 2);
    ASSERT_TRUE(*s[0]==leaf);
    ASSERT_TRUE(*s[1]==t);
    ASSERT_TRUE(*r[0]==t2);
}

TEST(hankel_matrix_test,context_check){
    SimpleMultiplicityTeacher teacher = getMultiplicityTeacher();
    set<rankedChar> alphabet = getAlphabet();
    HankelMatrix h(teacher);
    ParseTree leaf(0);
    ParseTree t(1, {leaf, leaf});
    ParseTree t2(1, {t, t});
    pair<ParseTree*, ParseTree*> pair1 = t.makeContext({});
    pair<ParseTree*, ParseTree*> pair2 = t.makeContext({0});
    ParseTree* emptyContext = pair1.first;
    ParseTree* secondContext = pair2.first;
    h.addContext(*emptyContext);
    delete(pair1.first);
    delete(pair1.second);
    pair1 = {nullptr, nullptr};
    h.addTree(leaf);
    h.addTree(t);
    h.addTree(t2);
    const vector<ParseTree*> rInit = h.getR();
    const vector<ParseTree*> sInit = h.getS();
    ASSERT_EQ(rInit.size(), 2);
    ASSERT_EQ(sInit.size(), 1);
    ASSERT_TRUE(*sInit[0]==leaf);
    ASSERT_TRUE(*rInit[0]==t);
    ASSERT_TRUE(*rInit[1]==t2);
    h.addContext(*secondContext);
    delete(pair2.first);
    delete(pair2.second);
    pair2 = {nullptr, nullptr};
    const vector<ParseTree*> r = h.getR();
    const vector<ParseTree*> s = h.getS();
    ASSERT_EQ(r.size(), 1);
    ASSERT_EQ(s.size(), 2);
    ASSERT_TRUE(*s[0]==leaf);
    ASSERT_TRUE(*s[1]==t);
    ASSERT_TRUE(*r[0]==t2);
}

TEST(hankel_matrix_test,suffixIterator){
    set<rankedChar> alphabet = getAlphabetSmall();
    SimpleMultiplicityTeacher teacher = getMultiplicityTeacher();
    HankelMatrix h(teacher);
    ParseTree leaf(0);
    ParseTree t(1, {ParseTree(0), ParseTree(0)});
    ParseTree t2(1, {t, t});
    pair<ParseTree*, ParseTree*> pair1 = t.makeContext({});
    pair<ParseTree*, ParseTree*> pair2 = t.makeContext({0});
    ParseTree* emptyContext = pair1.first;
    ParseTree* secondContext = pair2.first;
    h.addContext(*emptyContext);
    h.addContext(*secondContext);
    delete(pair1.first);
    delete(pair1.second);
    pair1 = {nullptr, nullptr};
    delete(pair2.first);
    delete(pair2.second);
    pair2 = {nullptr, nullptr};
    h.addTree(leaf);
    h.addTree(t);
    h.addTree(t2);
    h.addTree(ParseTree(1, {leaf, t}));
    h.addTree(ParseTree(1, {t, leaf}));
    h.getAcceptor();
    auto it = h.getSuffixIterator();
    vector<ParseTree*> s = h.getS();
    vector<ParseTree*> trees;
    vector<rankedChar> nodes = {a, b};
    for(auto c: nodes){
        for(int i=0;i<s.size();++i){
            for(int j=0;j<s.size();++j){
                auto t = new ParseTree(c.c);
                t->setSubtree(*s[i], 0);
                t->setSubtree(*s[j], 1);
                trees.push_back(t);
            }
        }
    }
    int i=0;
    while(it.hasNext()){
        ASSERT_EQ(*(it++), *trees[i++]);

    }
    for(auto& tree: trees){
        if(tree){
            delete(tree);
            tree = nullptr;
        }
    }
}


TEST(hankel_matrix_test,acceptor_test){
    set<rankedChar> alphabet = getAlphabet();
    SimpleMultiplicityTeacher teacher = getMultiplicityTeacher();
    HankelMatrix h(teacher);
    ParseTree leaf(0);
    ParseTree t(1, {ParseTree(0), ParseTree(0)});
    ParseTree t2(1, {t, t});
    pair<ParseTree*, ParseTree*> pair1 = t.makeContext({});
    pair<ParseTree*, ParseTree*> pair2 = t.makeContext({0});
    ParseTree* emptyContext = pair1.first;
    ParseTree* secondContext = pair2.first;
    h.addContext(*emptyContext);
    h.addContext(*secondContext);
    delete(pair1.first);
    delete(pair1.second);
    pair1 = {nullptr, nullptr};
    delete(pair2.first);
    delete(pair2.second);
    pair2 = {nullptr, nullptr};
    h.addTree(leaf);
    //h.addTree(t);
    //h.addTree(t2);
    h.makeConsistent();
    //h.closeTable();
    MultiplicityTreeAcceptor acc = h.getAcceptor();
    ASSERT_EQ(acc.run(leaf), 1);
    ASSERT_EQ(acc.run(t), 2);
    ASSERT_EQ(acc.run(t2), 4);
}


TEST(hankel_matrix_test,learner_test){
    set<rankedChar> alphabet = getAlphabet();
    SimpleMultiplicityTeacher teacher = getMultiplicityTeacher();
    ParseTree leaf(0);
    ParseTree t(1, {ParseTree(0), ParseTree(0)});
    ParseTree t2(1, {t, t});
    HankelMatrix h(teacher);
    MultiplicityTreeAcceptor acc = learn(teacher, h);
    ASSERT_EQ(acc.run(leaf), 1);
    ASSERT_EQ(acc.run(t), 2);
    ASSERT_EQ(acc.run(t2), 4);
}

TEST(hankel_matrix_test,learner_test2){
    set<rankedChar> alphabet = getAlphabetProb();
    SimpleMultiplicityTeacher teacher = getMultiplicityProbTeacher();
    ParseTree t(0, {ParseTree(1), ParseTree(2)});
    ParseTree t2(0, {ParseTree(2), ParseTree(1)});
    HankelMatrix h(teacher);
    MultiplicityTreeAcceptor acc = learn(teacher, h);
    ASSERT_EQ(acc.run(t), 0.5);
    ASSERT_EQ(acc.run(t2), 0.5);
}

TEST(hankel_matrix_test,learner_test3){
    set<rankedChar> alphabet = getAlphabetProb();
    SimpleMultiplicityTeacher teacher(0.005, 0);
    ParseTree t(1); t.setProb(0.05);
    ParseTree t2(2); t2.setProb(0);
    ParseTree t3(0, {ParseTree(1), ParseTree(1)}); t3.setProb(0.1);
    ParseTree t4(0, {ParseTree(2), ParseTree(2)}); t4.setProb(0.15);
    ParseTree t5(0, {ParseTree(1), ParseTree(2)}); t5.setProb(0.35);
    ParseTree t6(0, {ParseTree(2), ParseTree(1)}); t6.setProb(0.35);
    teacher.addExample(t); teacher.addExample(t2); teacher.addExample(t3);
    teacher.addExample(t4); teacher.addExample(t5); teacher.addExample(t6);
    HankelMatrix h(teacher);
    MultiplicityTreeAcceptor acc = learn(teacher, h);
    acc.printDesc();
    ASSERT_FLOAT_EQ(acc.run(t), 0.05);
    ASSERT_FLOAT_EQ(acc.run(t2), 0);
    ASSERT_FLOAT_EQ(acc.run(t3), 0.1);
    ASSERT_FLOAT_EQ(acc.run(t4), 0.15);
    ASSERT_FLOAT_EQ(acc.run(t5), 0.35);
    ASSERT_FLOAT_EQ(acc.run(t6), 0.35);
}

TEST(hankel_matrix_test,learner_test4){
    //Logger& logger = Logger::getLogger();
    //logger.setPrintLevel(Logger::LOG_DETAILS);
    set<rankedChar> alphabet = getAlphabetProb();
    FunctionalMultiplicityTeacher teacher = getFuncTeacherProb();
    HankelMatrix h(teacher);
    MultiplicityTreeAcceptor acc = learn(teacher, h);
    ParseTree t1(1); ParseTree t2(2);
    ParseTree t3(0, {t1, t2});
    ParseTree t4(0, {t3, ParseTree(1)});
    ParseTree t5(0, {t1, t4});
    ParseTree t6(0, {t2, t5});
    ParseTree t7(0, {t6, t6});
    ParseTree t8(0, {ParseTree(2), t7});
    ASSERT_FLOAT_EQ(acc.run(t1), 0.9);
    ASSERT_FLOAT_EQ(acc.run(t2), 0.1);
    ASSERT_FLOAT_EQ(acc.run(t3), 0.5);
    ASSERT_FLOAT_EQ(acc.run(t4), 0.9);
    ASSERT_FLOAT_EQ(acc.run(t5), 0.9);
    ASSERT_FLOAT_EQ(acc.run(t6), 0.5);
    ASSERT_FLOAT_EQ(acc.run(t7), 0.5);
    ASSERT_FLOAT_EQ(acc.run(t8), 0.5);
}