//
// Created by dolavn@wincs.cs.bgu.ac.il on 1/10/19.
//

#ifndef CFGLEARNER_TEACHER_H
#define CFGLEARNER_TEACHER_H

#include <unordered_map>
#include <vector>
#include <functional>

class ParseTree;
class TreeAcceptor;

class Teacher{
public:
    virtual bool membership(const ParseTree&) const=0;
    virtual ParseTree* equivalence(const TreeAcceptor&) const=0;
    virtual Teacher* clone()=0;
    virtual ~Teacher()=default;
};

class SimpleTeacher: public Teacher{
public:
    SimpleTeacher();
    SimpleTeacher(const SimpleTeacher&);
    SimpleTeacher& operator=(const SimpleTeacher&);
    SimpleTeacher(SimpleTeacher&&) noexcept;
    SimpleTeacher& operator=(SimpleTeacher&&) noexcept;
    ~SimpleTeacher() override;

    void addPositiveExample(const ParseTree&);
    void addNegativeExample(const ParseTree&);
    int getPosNum() const{return positiveExamples.size();}
    int getNegNum() const{return negativeExamples.size();}

    bool membership(const ParseTree&) const override;
    ParseTree* equivalence(const TreeAcceptor&) const override;
    Teacher* clone() override;
private:
    void copy(const SimpleTeacher&);
    void clear();
    bool hasExample(const ParseTree&);
    std::vector<ParseTree*> positiveExamples;
    std::vector<ParseTree*> negativeExamples;
};

#endif //CFGLEARNER_TEACHER_H
