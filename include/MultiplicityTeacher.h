//
// Created by dolavn on 7/20/19.
//

#ifndef CFGLEARNER_MULTIPLICITYTEACHER_H
#define CFGLEARNER_MULTIPLICITYTEACHER_H

#include <vector>

class ParseTree;
class MultiplicityTreeAcceptor;

typedef std::pair<ParseTree,double> example;
typedef std::pair<ParseTree,ParseTree> taggedTree;

class MultiplicityTeacher{
public:
    virtual double membership(const ParseTree&)=0;
    virtual example* equivalence(const MultiplicityTreeAcceptor&)=0;
};

class SimpleMultiplicityTeacher: public MultiplicityTeacher{
public:
    explicit SimpleMultiplicityTeacher(double);

    void addExample(taggedTree);

    virtual double membership(const ParseTree&);
    virtual example* equivalence(const MultiplicityTreeAcceptor&);
private:
    double epsilon;
    std::vector<taggedTree> trees;
};

#endif //CFGLEARNER_MULTIPLICITYTEACHER_H
