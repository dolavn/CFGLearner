//
// Created by dolavn@wincs.cs.bgu.ac.il on 7/21/19.
//

#ifndef CFGLEARNER_OBSERVATIONTABLE_H
#define CFGLEARNER_OBSERVATIONTABLE_H

#include <vector>
#include <unordered_map>
#include <set>
#include <armadillo>
#include <string>
#include "IndexArray.h"
#include "TreeAcceptor.h"
#include "TreesIterator.h"

class Teacher;
class MultiplicityTeacher;
class ParseTree;

class BaseTable{
public:
    BaseTable();
    BaseTable(const BaseTable&)=delete;
    BaseTable& operator=(const BaseTable&)=delete;
    BaseTable(BaseTable&&)=delete;
    BaseTable& operator=(BaseTable&&)=delete;
    virtual ~BaseTable();
    void addTree(const ParseTree&);
    void addContext(const ParseTree&);
    bool hasTree(const ParseTree&) const;
    virtual bool hasContext(const ParseTree&) const;
    bool treeInS(const ParseTree& tree);
    const ParseTree& getTreeS(int ind) const;
    inline const std::vector<ParseTree*>& getR(){return r;}
    inline const std::vector<ParseTree*>& getS(){return s;}
    inline const std::vector<ParseTree*>& getC(){return c;}
protected:
    virtual void completeTree(ParseTree*)=0;
    virtual void completeContextS(ParseTree*)=0;
    virtual void completeContextR(ParseTree*)=0;
    virtual bool checkTableComplete(ParseTree*)=0;
    int getIndInS(const ParseTree&) const;
    int getIndInR(const ParseTree&) const;
    std::vector<ParseTree*> s;
    std::vector<int> sNew;
    std::vector<ParseTree*> r;
    std::vector<int> rNew;
    std::vector<ParseTree*> c;
private:
    void clear();
};

class ObservationTable: public BaseTable{
public:
    explicit ObservationTable(const Teacher& teacher);
    ObservationTable(const ObservationTable& other)=delete;
    ObservationTable& operator=(const ObservationTable& other)=delete;
    ObservationTable(ObservationTable&& other)=delete;
    ObservationTable& operator=(ObservationTable&& other)=delete;
    virtual ~ObservationTable()=default;
    std::vector<bool> getObs(const ParseTree& tree);
    int getSObsInd(const ParseTree& tree);
    void printTable();
    std::string getTableLatex();
    const std::vector<ParseTree*> getRNew();
    const std::vector<ParseTree*> getSNew();

private:
    const Teacher& teacher;
    std::unordered_map<ParseTree*,std::vector<bool>> obs;
    void completeTree(ParseTree* tree);
    void completeContextS(ParseTree*);
    void completeContextR(ParseTree*);
    bool checkTableComplete(ParseTree*);
};

class MultiplicityTreeAcceptor;
class MultiLinearMap;

class HankelMatrix: public BaseTable{
public:
    explicit HankelMatrix(const MultiplicityTeacher&);
    HankelMatrix(const HankelMatrix&)=delete;
    HankelMatrix& operator=(const HankelMatrix&)=delete;
    HankelMatrix(HankelMatrix&&)=delete;
    HankelMatrix& operator=(HankelMatrix&&)=delete;
    std::vector<double> test();
    std::vector<double> getObs(const ParseTree&) const;
    virtual MultiplicityTreeAcceptor getAcceptor() const;
    virtual void closeTable();
    virtual void makeConsistent();
    virtual bool hasContext(const ParseTree&) const;
    bool checkClosed() const;
    std::string getTableLatex();
    void printTable() const;
    virtual void giveCounterExample(const ParseTree&);
    TreesIterator getSuffixIterator() const;
protected:
    std::set<rankedChar> alphabet;
    const MultiplicityTeacher& teacher;
    arma::mat getSMatrix(bool) const;
    std::vector<rankedChar> getAlphabetVec() const;
    void fillMatLastRow(arma::mat&, const ParseTree&) const;
    bool checkIsTreeZero(const ParseTree&) const;
    MultiplicityTreeAcceptor getAcceptorTemp() const;
    virtual arma::vec getCoefficients(const ParseTree&, const arma::mat&) const;
    std::unordered_map<ParseTree*,std::vector<double>> obs;
private:
    std::vector<arma::Row<double>> base;
    arma::vec getObsVec(const ParseTree&) const;
    arma::mat getSInv() const;
    virtual void completeTree(ParseTree*);
    void updateTransition(MultiLinearMap&, const ParseTree&, const std::vector<rankedChar>&, const arma::mat&) const;
    virtual void completeContextS(ParseTree*);
    virtual void completeContextR(ParseTree*);
    bool checkTableComplete(ParseTree*);
};

class ScalarHankelMatrix: public HankelMatrix{
public:
    explicit ScalarHankelMatrix(const MultiplicityTeacher&);
    ScalarHankelMatrix(const ScalarHankelMatrix&)=delete;
    ScalarHankelMatrix& operator=(const ScalarHankelMatrix&)=delete;
    ScalarHankelMatrix(ScalarHankelMatrix&&)=delete;
    ScalarHankelMatrix& operator=(ScalarHankelMatrix&&)=delete;
    virtual void giveCounterExample(const ParseTree&) override;
    virtual void makeConsistent() override;
    virtual MultiplicityTreeAcceptor getAcceptor() const override;
    virtual void closeTable() override;
    void complete();
    int getSObsInd(const ParseTree&) const;
    int getZeroVecInd() const;
    std::vector<const ParseTree*> getTreesInClass(int) const;
    std::vector<std::pair<const ParseTree*,const ParseTree*>> getPairsVec(std::vector<const ParseTree*>) const;

private:
    typedef std::pair<ParseTree, int> extension;
    virtual arma::vec getCoefficients(const ParseTree&, const arma::mat&) const override;
    void updateTransition(MultiLinearMap&, const ParseTree&, const std::vector<rankedChar>&) const;
    void completeTree(ParseTree*);
    void completeContextS(ParseTree*);
    void completeContextR(ParseTree*);
    double getCoeff(const ParseTree&, const ParseTree&) const;
    std::vector<double> getRow(const ParseTree&) const;
    std::vector<ParseTree> getSExtensions() const;
    bool checkTableComplete(const ParseTree&);
    bool checkIsSExtension(const ParseTree&) const;
    bool checkExtension(extension, extension, const ParseTree&, double);
    std::vector<extension> getExtensions(const ParseTree&) const;
};


class PositiveHankelMatrix: public HankelMatrix{
public:
    explicit PositiveHankelMatrix(const MultiplicityTeacher&);
    PositiveHankelMatrix(const PositiveHankelMatrix&)=delete;
    PositiveHankelMatrix& operator=(const PositiveHankelMatrix&)=delete;
    PositiveHankelMatrix(PositiveHankelMatrix&&)=delete;
    PositiveHankelMatrix& operator=(PositiveHankelMatrix&&)=delete;
private:
    virtual arma::vec getCoefficients(const ParseTree&, const arma::mat&) const override;
    void completeTree(ParseTree*);
    void completeContextS(ParseTree*);
    void completeContextR(ParseTree*);
    bool checkTableComplete(ParseTree*);
};


#endif //CFGLEARNER_OBSERVATIONTABLE_H
