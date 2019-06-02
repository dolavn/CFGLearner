#ifndef CFGLEARNER_TREECOMPARATOR_H
#define CFGLEARNER_TREECOMPARATOR_H

#include <unordered_map>
class ParseTree;

class TreeComparator{
public:
    TreeComparator(std::unordered_map<int, int>);

    int compare(const ParseTree&, const ParseTree&);
private:
    std::unordered_map<int, int> scores;
};

#endif //CFGLEARNER_TREECOMPARATOR_H
