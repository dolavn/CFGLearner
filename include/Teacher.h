//
// Created by Dolav Nitay
//

#ifndef CFGLEARNER_TEACHER_H
#define CFGLEARNER_TEACHER_H

#include <unordered_map>
#include <vector>
#include <functional>

class ParseTree;
class TreeAcceptor;

/*
 * A class representing the oracle used in the L* algorithm by the learner.
 * Each implementation of this class must support membership query - Whether a given tree belongs to the language,
 * and equivalence query - Whether a given tree acceptor correctly identifies the language.
 */
class Teacher{
public:
    virtual bool membership(const ParseTree&) const=0;
    virtual ParseTree* equivalence(const TreeAcceptor&) const=0;
    virtual Teacher* clone()=0;
    virtual ~Teacher()=default;
};

/*
 * This simple teacher holds a list of positive and negative examples. It returns true to the membership query
 * iff the given tree belongs to the positive examples. And returns true to the equivalence query iff the given
 * tree acceptor returns the correct answer for all positive and negative examples.
 */
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
    std::size_t getPosNum() const{return positiveExamples.size();}
    std::size_t getNegNum() const{return negativeExamples.size();}

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



struct TreePointer{
    ParseTree* ptr;
    const ParseTree* constPtr;

    explicit TreePointer(ParseTree* ptr):ptr(ptr),constPtr(nullptr){}
    explicit TreePointer(const ParseTree* constPtr):ptr(nullptr),constPtr(constPtr){}

    friend bool operator==(const TreePointer&, const TreePointer&);
};


struct TreeHasher
{
    std::size_t operator()(const TreePointer&) const;
};

/*
 * This teacher holds a count of how many times each tree appeared as a positive example, and how many times
 * that tree appeared as a negative example. It also calculates for each tree the frequency of its appearances as
 * positive example. This teacher has two parameters, minFreq, and minCount. It regards a tree as a positive example
 * iff it appeared more than minCount times as a positive example, and the frequency of it's appearances as a positive
 * example is more than minFreq. It returns true to the membership query iff the given tree is regarded as a positive
 * example. And returns true to the equivalence query iff the given acceptor agrees with it on all examples given.
 */

class FrequencyTeacher: public Teacher{
public:
    FrequencyTeacher();
    FrequencyTeacher(int, float);
    FrequencyTeacher(const FrequencyTeacher&);
    FrequencyTeacher& operator=(const FrequencyTeacher&);
    FrequencyTeacher(FrequencyTeacher&&) noexcept;
    FrequencyTeacher& operator=(FrequencyTeacher&&) noexcept;
    ~FrequencyTeacher() override;

    void addPositiveExample(const ParseTree&);
    void addNegativeExample(const ParseTree&);
    inline int getMinCount() const{return minCount;};
    inline float getMinFreq() const{return minFreq;};

    bool membership(const ParseTree&) const override;
    ParseTree* equivalence(const TreeAcceptor&) const override;
    Teacher* clone() override;
private:
    bool inLanguage(const ParseTree&) const;
    std::pair<int,int>& getPair(const ParseTree&);
    void copy(const FrequencyTeacher&);
    void clear();

    int minCount;
    float minFreq;
    std::unordered_map<TreePointer,std::pair<int, int>,TreeHasher> map;
};



#endif //CFGLEARNER_TEACHER_H
