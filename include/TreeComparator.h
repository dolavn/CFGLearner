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
    virtual float compare(const ParseTree&, const ParseTree&)=0;
protected:
    typedef std::unordered_map<const ParseTree*, int> treeToIndMap;
    //typedef std::vector<std::vector<int>> alignmentTable;
    template <typename T>
    using alignmentTable = std::vector<std::vector<T>>;
};

class TreeAligner: public TreeComparator{
public:
    TreeAligner(scoresMap, int);
    TreeAligner(int, int, int);
    virtual float compare(const ParseTree&, const ParseTree&);
private:
    int getScore(int, int);
    int alignInnerNodes(const ParseTree&, const ParseTree&, treeToIndMap&, treeToIndMap&, alignmentTable<int>&);
    int indelScore;
    int innerNode;
    int replaceScore;
    scoresMap scores;
};

class SwapComparator: public TreeComparator{
public:
    SwapComparator(int, int);
    virtual float compare(const ParseTree&, const ParseTree&);
private:
    int replaceScore;
    int swapScore;
};


class DuplicationComparator: public TreeComparator{
public:
    DuplicationComparator();
    virtual float compare(const ParseTree&, const ParseTree&);
private:
    std::vector<std::pair<int,int>> getDupIndices(const ParseTree&);
};

template <typename T>
T safeAdd(T, T);

#endif //CFGLEARNER_TREECOMPARATOR_H
