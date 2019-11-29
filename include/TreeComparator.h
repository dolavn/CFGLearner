#ifndef CFGLEARNER_TREECOMPARATOR_H
#define CFGLEARNER_TREECOMPARATOR_H

#include <unordered_map>
#include <vector>
#include <boost/functional/hash.hpp>
#include "Definitions.h"

class ParseTree;


typedef std::vector<int> intVec;
struct scoresMap{
public:
    scoresMap():map(){}
    //scoresMap(std::unordered_map<intPair, int, pair_hash> map):map(map){}
    int& operator[](intPair);
private:

    std::unordered_map<intPair, int, pair_hash> map;
};

class TreeComparator{
public:
    virtual int compare(const ParseTree&, const ParseTree&)=0;
protected:
    typedef std::unordered_map<const ParseTree*, int> treeToIndMap;
    typedef std::vector<std::vector<int>> alignmentTable;
};

class TreeAligner: public TreeComparator{
public:
    TreeAligner(scoresMap, int);
    TreeAligner(int, int, int);
    int compare(const ParseTree&, const ParseTree&);
private:
    int getScore(int, int);
    int alignInnerNodes(const ParseTree&, const ParseTree&, treeToIndMap&, treeToIndMap&, alignmentTable&);
    int indelScore;
    int innerNode;
    int replaceScore;
    scoresMap scores;
};

class SwapComparator: public TreeComparator{
public:
    SwapComparator(int, int);
    int compare(const ParseTree&, const ParseTree&);
private:
    int replaceScore;
    int swapScore;
};

int safeAdd(int, int);

#endif //CFGLEARNER_TREECOMPARATOR_H
