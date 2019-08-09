//
// Created by dolavn on 8/7/19.
//

#ifndef CFGLEARNER_TREESITERATOR_H
#define CFGLEARNER_TREESITERATOR_H

#include <vector>
#include <set>
#include "IndexArray.h"
#include "RankedChar.h"

class ParseTree;

class TreesIterator{
public:
    TreesIterator(std::vector<ParseTree*>, std::set<rankedChar>, int);
    TreesIterator(std::set<rankedChar>, int);
    TreesIterator(const TreesIterator&);
    TreesIterator& operator=(const TreesIterator&);
    TreesIterator(TreesIterator&&);
    TreesIterator& operator=(TreesIterator&&);
    ~TreesIterator();
    void setVerbose(bool verbose){this->verbose = verbose;} //todo:delete
    bool hasNext() const;
    void resetIterator();
    std::set<rankedChar> getAlphabet() const;
    ParseTree operator*() const;
    TreesIterator& operator++();
    TreesIterator operator++(int){
        TreesIterator ans(*this);
        ++(*this);
        return ans;
    }
private:
    void incLevel();
    void copy(const TreesIterator&);
    void clear();
    void copyVec(const std::vector<ParseTree*>&, std::vector<ParseTree*>&);
    void clearVec(std::vector<ParseTree*>&);
    void incChar();
    bool checkIndex(const IndexArray& arr);
    void createNewTree();
    typedef std::pair<ParseTree*, int> treeLevelPair;
    std::vector<ParseTree*> currLevel;
    std::vector<treeLevelPair> prevLevels;
    std::vector<rankedChar> alphabet;
    int currChar;
    int maxLevel;
    int remainingLevels;
    bool verbose=false; //todo:delete
    IndexArray arr;
};


#endif //CFGLEARNER_TREESITERATOR_H
