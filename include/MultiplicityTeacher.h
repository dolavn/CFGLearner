//
// Created by dolavn on 7/20/19.
//

#ifndef CFGLEARNER_MULTIPLICITYTEACHER_H
#define CFGLEARNER_MULTIPLICITYTEACHER_H

#include <vector>
#include <functional>
#include <set>
#include "RankedChar.h"
#include "TreesIterator.h"

class ParseTree;
class TreeComparator;
class MultiplicityTreeAcceptor;

class MultiplicityTeacher{
public:
    virtual double membership(const ParseTree&) const=0;
    virtual ParseTree* equivalence(const MultiplicityTreeAcceptor&) const=0;
    virtual double getDefaultValue() const=0;
    virtual std::set<rankedChar> getAlphabet() const=0;
    virtual std::string toString() const;
    virtual void printDesc() const;
    virtual double getError() const=0;
    friend std::ostream& operator<<(std::ostream&,const MultiplicityTeacher&);
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
    virtual std::set<rankedChar> getAlphabet() const {return alphabet;};
    virtual std::string toString() const;
    virtual void printDesc() const;
    virtual double getError() const{return error;}
private:
    void clear();
    void copy(const SimpleMultiplicityTeacher&);
    double epsilon;
    double defaultValue;
    mutable double error;
    std::set<rankedChar> alphabet;
    std::vector<ParseTree*> trees;
};

class DifferenceMultiplicityTeacher: public MultiplicityTeacher{
public:
    DifferenceMultiplicityTeacher(TreeComparator&, int, double, double);
    DifferenceMultiplicityTeacher(const DifferenceMultiplicityTeacher&);
    DifferenceMultiplicityTeacher& operator=(const DifferenceMultiplicityTeacher&)=delete;
    DifferenceMultiplicityTeacher(DifferenceMultiplicityTeacher&&);
    DifferenceMultiplicityTeacher& operator=(DifferenceMultiplicityTeacher&&)=delete;
    ~DifferenceMultiplicityTeacher();

    virtual double membership(const ParseTree&) const;
    virtual ParseTree* equivalence(const MultiplicityTreeAcceptor&) const;
    virtual double getDefaultValue() const{return 0;}
    virtual std::set<rankedChar> getAlphabet() const{return alphabet;};
    virtual double getError() const{return 0;}
    void addExample(const ParseTree&);
private:
    void copy(const DifferenceMultiplicityTeacher&);
    void clear();
    std::vector<ParseTree*> trees;
    std::set<rankedChar> alphabet;
    mutable std::vector<ParseTree*> cache;
    TreeComparator& cmp;
    int maxDiff;
    double decayFactor;
    double epsilon;
};

class FunctionalMultiplicityTeacher: public MultiplicityTeacher{
public:
    explicit FunctionalMultiplicityTeacher(double, double,
            std::function<double(const ParseTree&)>, const TreesIterator&);

    inline double getDefaultValue() const{return defaultValue;}

    virtual double membership(const ParseTree&) const;
    virtual ParseTree* equivalence(const MultiplicityTreeAcceptor&) const;
    virtual std::set<rankedChar> getAlphabet() const;
    virtual std::string toString() const;
    virtual double getError() const{return error;}
private:
    double epsilon;
    double defaultValue;
    mutable double error;
    std::function<double(const ParseTree&)> func;
    mutable TreesIterator it;
};


#endif //CFGLEARNER_MULTIPLICITYTEACHER_H
