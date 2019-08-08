//
// Created by dolavn on 7/20/19.
//

#ifndef CFGLEARNER_MULTIPLICITYTEACHER_H
#define CFGLEARNER_MULTIPLICITYTEACHER_H

#include <vector>
#include <functional>
#include <set>

class ParseTree;
class MultiplicityTreeAcceptor;
class TreesIterator;


class MultiplicityTeacher{
public:
    virtual double membership(const ParseTree&) const=0;
    virtual ParseTree* equivalence(const MultiplicityTreeAcceptor&) const=0;
    virtual double getDefaultValue() const=0;
};

class SimpleMultiplicityTeacher: public MultiplicityTeacher{
public:
    explicit SimpleMultiplicityTeacher(double, double);
    SimpleMultiplicityTeacher(const SimpleMultiplicityTeacher&);
    SimpleMultiplicityTeacher& operator=(const SimpleMultiplicityTeacher&);
    SimpleMultiplicityTeacher(SimpleMultiplicityTeacher&&);
    SimpleMultiplicityTeacher& operator=(SimpleMultiplicityTeacher&&);
    virtual ~SimpleMultiplicityTeacher();

    void addExample(const ParseTree&);

    inline double getDefaultValue() const{return defaultValue;}

    virtual double membership(const ParseTree&) const;
    virtual ParseTree* equivalence(const MultiplicityTreeAcceptor&) const;


private:
    void clear();
    void copy(const SimpleMultiplicityTeacher&);
    double epsilon;
    double defaultValue;
    std::vector<ParseTree*> trees;
};

class FunctionalMultiplicityTeacher: public MultiplicityTeacher{
public:
    explicit FunctionalMultiplicityTeacher(double, double, std::function<double(const ParseTree&)>, TreesIterator&);

    inline double getDefaultValue() const{return defaultValue;}

    virtual double membership(const ParseTree&) const;
    virtual ParseTree* equivalence(const MultiplicityTreeAcceptor&) const;


private:

    double epsilon;
    double defaultValue;
    std::function<double(const ParseTree&)> func;
    TreesIterator& it;
};


#endif //CFGLEARNER_MULTIPLICITYTEACHER_H
