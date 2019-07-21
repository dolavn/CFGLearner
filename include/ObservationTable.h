//
// Created by dolavn@wincs.cs.bgu.ac.il on 7/21/19.
//

#ifndef CFGLEARNER_OBSERVATIONTABLE_H
#define CFGLEARNER_OBSERVATIONTABLE_H

#include <vector>
#include <unordered_map>

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
protected:
    virtual void completeTree(ParseTree*)=0;
    virtual void completeContext(ParseTree*)=0;
    virtual bool checkTableComplete(ParseTree*)=0;
    virtual void checkTableCompleteContext(ParseTree*)=0;
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
    inline const std::vector<ParseTree*>& getR(){return r;}
    inline const std::vector<ParseTree*>& getS(){return s;}
    inline const std::vector<ParseTree*>& getC(){return c;}
    const std::vector<ParseTree*> getRNew();
    const std::vector<ParseTree*> getSNew();

private:
    const Teacher& teacher;
    std::unordered_map<ParseTree*,std::vector<bool>> obs;
    void completeTree(ParseTree* tree);
    void completeContext(ParseTree*);
    bool checkTableComplete(ParseTree*);
    void checkTableCompleteContext(ParseTree*);
};

class HankelMatrix: public BaseTable{
public:
    explicit HankelMatrix(const MultiplicityTeacher& teacher);
private:
    const MultiplicityTeacher& teacher;
    std::unordered_map<ParseTree*,std::vector<double>> obs;
    void completeTree(ParseTree* tree);
    void completeContext(ParseTree*);
    bool checkTableComplete(ParseTree*);
    void checkTableCompleteContext(ParseTree*);
};
#endif //CFGLEARNER_OBSERVATIONTABLE_H