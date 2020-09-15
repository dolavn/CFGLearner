
#ifndef CFGLEARNER_COLINEARHANKELMATRIX_H
#define CFGLEARNER_COLINEARHANKELMATRIX_H

#include <vector>
#include <unordered_map>
#include <set>
#include "TreesIterator.h"
#include "ParseTree.h"
#include "RankedChar.h"

class Teacher;
class MultiplicityTeacher;
class MultiplicityTreeAcceptor;
class MultiLinearMap;

class ColinearHankelMatrix{
public:
    explicit ColinearHankelMatrix(const MultiplicityTeacher&);
    ColinearHankelMatrix(const ColinearHankelMatrix&)=delete;
    ColinearHankelMatrix& operator=(const ColinearHankelMatrix&)=delete;
    ColinearHankelMatrix(ColinearHankelMatrix&&)=delete;
    ColinearHankelMatrix& operator=(ColinearHankelMatrix&&)=delete;
    void giveCounterExample(const ParseTree&);
    void makeConsistent();
    MultiplicityTreeAcceptor getAcceptor() const;
    void closeTable();
    void complete();
    int getClassInd(const ParseTree&) const;
    int getZeroVecInd() const;
    std::vector<int> getTreesInClass(int) const;
    std::vector<std::pair<int,int>> getPairsVec(std::vector<int>) const;
    void printTable() const;
    bool hasTree(const ParseTree&) const;
    bool hasContext(const ParseTree&) const;
    void addTree(const ParseTree&);
    void addContext(const ParseTree&);
    std::vector<ParseTree> getTrees(){return trees;}//todo : delete
private:
    const MultiplicityTeacher& teacher;
    std::vector<ParseTree> trees;
    std::vector<ParseTree> contexts;
    struct tree_hash {
        std::size_t operator()(const ParseTree& t) const {
            return t.getHash();
        }
    };
    std::unordered_map<ParseTree,std::vector<double>,tree_hash> obs;
    std::unordered_map<int, int> classesRepresentatives;
    std::set<rankedChar> alphabet;
    int numClasses;
    std::vector<double> getObsVec(const ParseTree&) const;
    typedef std::pair<ParseTree, int> extension;
    void updateTransition(MultiLinearMap&, const ParseTree&, const std::vector<rankedChar>&) const;
    void initTable();
    bool makeZeroConsistent();
    void completeTree(const ParseTree&);
    void completeContext(const ParseTree&);
    std::vector<rankedChar> getAlphabetVec() const;
    IteratorWrapper getSuffixIterator() const;
    double getCoeff(const ParseTree&, const ParseTree&) const;
    double getCoeffLazy(const ParseTree&, const ParseTree&) const;
    bool hasTreeInTable(const ParseTree&) const;
    std::vector<double> getRow(const ParseTree&) const;
    std::vector<ParseTree> getExtensions() const;
    bool checkIsExtension(const ParseTree&) const;
    bool checkExtension(extension, extension, const ParseTree&, double);
    std::vector<extension> getExtensions(const ParseTree&) const;
};



bool isZeroVec(std::vector<double>&);
double getNorm(std::vector<double>&);
double getCosVectors(std::vector<double>&, std::vector<double>&);

std::vector<std::pair<ParseTree, int>> extendSet(const ParseTree&, std::vector<ParseTree>, std::set<rankedChar>);

#endif //CFGLEARNER_COLINEARHANKELMATRIX_H
