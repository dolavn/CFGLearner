//
// Created by dolavn on 7/20/19.
//

#ifndef CFGLEARNER_MULTIPLICITYTEACHER_H
#define CFGLEARNER_MULTIPLICITYTEACHER_H

#include <vector>

class ParseTree;
class MultiplicityTreeAcceptor;

typedef std::pair<ParseTree,double> example;

class MultiplicityTeacher{
public:
    virtual double membership(const ParseTree&)=0;
    virtual example* equivalence(const MultiplicityTreeAcceptor&)=0;
};

class SimpleMultiplicityTeacher: public MultiplicityTeacher{
public:
    explicit SimpleMultiplicityTeacher(double, double);

    void addExample(const ParseTree&);

    virtual double membership(const ParseTree&);
    virtual example* equivalence(const MultiplicityTreeAcceptor&);
private:
    double epsilon;
    double defaultValue;
    std::vector<ParseTree*> trees;
};

#endif //CFGLEARNER_MULTIPLICITYTEACHER_H
