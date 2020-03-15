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
class TreesGenerator;
class TreeConstructor;

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

class ComparatorTeacher: public MultiplicityTeacher{
public:
    ComparatorTeacher(TreeComparator&, double);
    ComparatorTeacher(const ComparatorTeacher&);
    ComparatorTeacher& operator=(const ComparatorTeacher&)=delete;
    ComparatorTeacher(ComparatorTeacher&&);
    ComparatorTeacher& operator=(ComparatorTeacher&&)=delete;
    virtual ~ComparatorTeacher();

    virtual double membership(const ParseTree&) const;
    virtual ParseTree* equivalence(const MultiplicityTreeAcceptor&) const;
    virtual double getDefaultValue() const{return 0;}
    virtual std::set<rankedChar> getAlphabet() const{return alphabet;};
    virtual double getError() const{return 0;}
    void addExample(const ParseTree&);
protected:
    double epsilon;
    std::vector<ParseTree*> trees;
    mutable std::vector<ParseTree*> cache;
    virtual double calcNewProb(const ParseTree&, const ParseTree&, TreeComparator& cmp) const=0;
private:
    void copy(const ComparatorTeacher&);
    void clear();
    std::set<rankedChar> alphabet;
    TreeComparator& cmp;
};

class DifferenceMultiplicityTeacher: public ComparatorTeacher{
public:
    DifferenceMultiplicityTeacher(TreeComparator&, int, double, double);
    DifferenceMultiplicityTeacher(const DifferenceMultiplicityTeacher&);
    DifferenceMultiplicityTeacher(DifferenceMultiplicityTeacher&&);
    DifferenceMultiplicityTeacher& operator=(const DifferenceMultiplicityTeacher&)=delete;
    DifferenceMultiplicityTeacher& operator=(DifferenceMultiplicityTeacher&&)=delete;
private:
    virtual double calcNewProb(const ParseTree&, const ParseTree&, TreeComparator&) const;
    int maxDiff;
    double decayFactor;
};

class ProbabilityTeacher: public ComparatorTeacher{
public:
    typedef std::function<int(float,int)> countingFunction;

    ProbabilityTeacher(TreeComparator&, double, double);
    ProbabilityTeacher(TreeComparator&, double, countingFunction, double);
    ProbabilityTeacher(const ProbabilityTeacher&);
    ProbabilityTeacher(ProbabilityTeacher&&);
    ProbabilityTeacher& operator=(const ProbabilityTeacher&)=delete;
    ProbabilityTeacher& operator=(ProbabilityTeacher&&)=delete;
    virtual ~ProbabilityTeacher();

    void setTreesGenerator(const TreesGenerator&);
    void setupDuplicationsGenerator(int);
    void setupConstructorGenerator(TreeConstructor&, int, int);
    virtual double membership(const ParseTree&) const;
    virtual ParseTree* equivalence(const MultiplicityTreeAcceptor&) const;
private:
    virtual double calcNewProb(const ParseTree&, const ParseTree&, TreeComparator&) const;
    double decayFactor;
    countingFunction countingFunc;
    TreesGenerator* generator;
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
