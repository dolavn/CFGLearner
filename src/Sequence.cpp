#include "Sequence.h"
#include <iostream>
#include <vector>
#include <armadillo>
#include "Learner.h"
#include "Teacher.h"
#include "ParseTree.h"
#include "Logger.h"
#include "TreeAcceptor.h"
#include "CFG.h"
#include "ConicCombinationFinder.h"

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

using namespace arma;

int testCone(){
    static int dim = 5;
    static int vec_num = 1000;
    mat m(dim, vec_num,fill::randu);
    ConicCombinationFinder c(m);
    int solvedCount = 0;
    for(int i=0;i<vec_num;++i){
        c.solve(i);
        if(c.getStatus()==ConicCombinationFinder::SOLVED){
            solvedCount++;
        }
        cout << i << endl;
    }
    cout << "dim:" << dim << endl;
    cout << "vec num:" << vec_num << endl;
    cout << "solved:" << solvedCount << endl;
    return 0;
}

int testLogger(){
    Logger& l = Logger::getLogger();
    l.setPrintLevel(Logger::LOG_ERRORS);
    l << "hello, shouldn't print that";
    l.setPrintLevel(Logger::LOG_DEBUG);
    l << "this should be printed" << l.endline;
    l << "hello " << 5 << l.endline;
    return 0;
}


int main(int argc, char** argv){
    return testLogger();
    return testCone();
    Teacher* t = getTeacher2();
    TreeAcceptor acc = learn(*t);
    acc.printDescription();
    CFG c(acc);
    cout << c.getRepr() << endl;
    delete(t);
    return 0;
}
