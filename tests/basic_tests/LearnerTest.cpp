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