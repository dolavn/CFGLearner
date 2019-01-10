//
// Created by dolavn@wincs.cs.bgu.ac.il on 1/10/19.
//

#ifndef CFGLEARNER_TEACHER_H
#define CFGLEARNER_TEACHER_H

class ParseTree;
class TreeAcceptor;

class Teacher{
public:
    virtual bool membership(const ParseTree&)=0;
    virtual ParseTree* equivalence(const TreeAcceptor&)=0;
};

#endif //CFGLEARNER_TEACHER_H
