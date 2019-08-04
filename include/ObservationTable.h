//
// Created by dolavn@wincs.cs.bgu.ac.il on 7/21/19.
//

#ifndef CFGLEARNER_OBSERVATIONTABLE_H
#define CFGLEARNER_OBSERVATIONTABLE_H

#include <vector>
#include <unordered_map>
#include <set>
#include <armadillo>
#include "IndexArray.h"
#include "TreeAcceptor.h"

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
    bool hasTree(const ParseTree&);
    bool treeInS(const ParseTree& tree);
    inline const std::vector<ParseTree*>& getR(){return r;}
    inline const std::vector<ParseTree*>& getS(){return s;}
    inline const std::vector<ParseTree*>& getC(){return c;}
protected:
    virtual void completeTree(ParseTree*)=0;
    virtual void completeContextS(ParseTree*)=0;
    virtual void completeContextR(ParseTree*)=0;
    virtual bool checkTableComplete(ParseTree*)=0;
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
    const ParseTree& getTreeS(int ind) const;
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

class HankelMatrix: public BaseTable{
public:
    explicit HankelMatrix(const MultiplicityTeacher&, const std::set<rankedChar>&);
    HankelMatrix(const HankelMatrix&)=delete;
    HankelMatrix& operator=(const HankelMatrix&)=delete;
    HankelMatrix(HankelMatrix&&)=delete;
    HankelMatrix& operator=(HankelMatrix&&)=delete;

    std::vector<double> getObs(const ParseTree&) const;
    MultiplicityTreeAcceptor getAcceptor() const;

    class suffixIterator{
    public:
        suffixIterator(HankelMatrix&);
        bool hasNext();
        ParseTree operator*() const;
        suffixIterator& operator++();
        suffixIterator operator++(int){
            suffixIterator ans(*this);
            ++(*this);
            return ans;
        }
    private:
        void incChar();
        HankelMatrix& mat;
        std::vector<rankedChar> alphabet;
        int currChar;
        IndexArray arr;
    };

    suffixIterator getSuffixIterator();
private:
    const MultiplicityTeacher& teacher;
    std::set<rankedChar> alphabet;
    std::vector<arma::Row<double>> base;
    arma::mat sInv;
    std::unordered_map<ParseTree*,std::vector<double>> obs;
    arma::mat getSMatrix(bool);
    void updateSInv();
    void fillMatLastRow(arma::mat&, ParseTree*);
    void completeTree(ParseTree*);
    void completeContextS(ParseTree*);
    void completeContextR(ParseTree*);
    bool checkTableComplete(ParseTree*);
};
#endif //CFGLEARNER_OBSERVATIONTABLE_H
