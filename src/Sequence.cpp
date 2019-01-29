#include "Sequence.h"
#include <iostream>
#include <vector>
#include "Learner.h"
#include "Teacher.h"
#include "ParseTree.h"
#include "TreeAcceptor.h"


using namespace std;

Teacher* getTeacher(){
    ParseTree t(1,{ParseTree(0),ParseTree(0)});
    ParseTree t2(1,{ParseTree(0),ParseTree(1)});
    ParseTree t3(2,{ParseTree(2,{ParseTree(0),ParseTree(0)}),ParseTree(0)});
    ParseTree t4(2,{ParseTree(1,{ParseTree(0),ParseTree(0)}),ParseTree(0)});
    ParseTree t5(1,{ParseTree(0),ParseTree(1,{ParseTree(0),ParseTree(0)})});
    ParseTree t6(1,{ParseTree(0),ParseTree(1,{ParseTree(0),ParseTree(0)})});
    ParseTree t7(1);
    auto teacher = new SimpleTeacher();
    teacher->addPositiveExample(t);
    teacher->addPositiveExample(t4);
    teacher->addPositiveExample(t5);
    teacher->addNegativeExample(t2);
    teacher->addNegativeExample(t3);
    teacher->addPositiveExample(t7);
    return teacher;
}

Teacher* getTeacher2(){
    ParseTree t(0,{ParseTree(1),ParseTree(2)});
    ParseTree t2(0,{ParseTree(1),ParseTree(0,{ParseTree(0,{ParseTree(1),ParseTree(2)}),ParseTree(2)})});
    ParseTree t3(0,{ParseTree(1),ParseTree(0,{ParseTree(0,{ParseTree(1),ParseTree(0,{ParseTree(0,{ParseTree(1),ParseTree(2)}),ParseTree(2)})}),ParseTree(2)})});
    ParseTree t4(0,{ParseTree(2),ParseTree(2)});
    ParseTree t5(0,{ParseTree(2),ParseTree(0,{ParseTree(0,{ParseTree(1),ParseTree(2)}),ParseTree(2)})});
    ParseTree t6(0,{ParseTree(2),ParseTree(0,{ParseTree(0,{ParseTree(1),ParseTree(0,{ParseTree(0,{ParseTree(1),ParseTree(2)}),ParseTree(2)})}),ParseTree(2)})});
    auto teacher = new SimpleTeacher();
    teacher->addPositiveExample(t);
    teacher->addPositiveExample(t2);
    teacher->addPositiveExample(t3);
    teacher->addNegativeExample(t4);
    teacher->addNegativeExample(t5);
    teacher->addNegativeExample(t6);
    return teacher;
}


ostream& operator<<(ostream& stream, const Sequence& seq){
    stream << seq.toString();
    return stream;
}

vector<int> Sequence::subseq(int begin, int end) const{
    return vector<int>(seq.begin()+begin, seq.begin()+end);
}

int main(int argc, char** argv){
    Teacher* t = getTeacher2();
    TreeAcceptor acc = learn(*t);
    acc.printDescription();
    delete(t);
    return 0;
}
