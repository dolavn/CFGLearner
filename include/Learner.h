//
// Created by dolavn@wincs.cs.bgu.ac.il on 1/10/19.
//

#ifndef CFGLEARNER_LEARNER_H
#define CFGLEARNER_LEARNER_H

#include <set>
#include "ParseTree.h"
#include "TreeAcceptor.h"

class Teacher;

class Learner{
public:
private:
    Teacher* teacher;
    TreeAcceptor acceptor;
    std::set<ParseTree> r;
    std::set<ParseTree> s;
    std::set<ParseTree> c;
};

#endif //CFGLEARNER_LEARNER_H
