#ifndef CFGLEARNER_TREECOMPARATOR_H
#define CFGLEARNER_TREECOMPARATOR_H

#include <unordered_map>
#include <vector>
#include <boost/functional/hash.hpp>

class ParseTree;


struct scoresMap{
public:
    int& operator[](std::vector<int>);
private:
    template <typename Container>
    struct container_hash {
        std::size_t operator()(Container const& c) const {
            return boost::hash_range(c.begin(), c.end());
        }
    };
    typedef std::vector<int> intVec;
    std::unordered_map<intVec, int, container_hash<intVec>> map;
};

class TreeComparator{
public:
    TreeComparator(scoresMap);

    int compare(const ParseTree&, const ParseTree&);
private:
    scoresMap scores;
};

#endif //CFGLEARNER_TREECOMPARATOR_H
