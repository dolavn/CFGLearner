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
    scoresMap(std::unordered_map<intPair, int, pair_hash> map):map(map){}
    int& operator[](intPair);
private:

    std::unordered_map<intPair, int, pair_hash> map;
};

class TreeComparator{
public:
    TreeComparator(scoresMap, int);
    TreeComparator(std::unordered_map<intPair, int, pair_hash>, int);
    int compare(const ParseTree&, const ParseTree&);
private:
    typedef std::unordered_map<const ParseTree*, int> treeToIndMap;
    typedef std::vector<std::vector<int>> alignmentTable;
    int alignInnerNodes(const ParseTree&, const ParseTree&, treeToIndMap&, treeToIndMap&, alignmentTable&);
    int indelScore;
    scoresMap scores;
};

#endif //CFGLEARNER_TREECOMPARATOR_H
