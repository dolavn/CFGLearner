//
// Created by dolavn on 7/5/19.
//

#ifndef CFGLEARNER_MULTIPLICITYTREEACCEPTOR_H
#define CFGLEARNER_MULTIPLICITYTREEACCEPTOR_H

#include "TreeAcceptor.h"
#include "MultiLinearMap.h"


class MultiplicityTreeAcceptor{
public:
    MultiplicityTreeAcceptor(const std::set<rankedChar>&, int); //Empty constructor

    void addTransition(const MultiLinearMap&, const rankedChar&);
    void setLambda(const floatVec&);

    void printDesc() const;

    float run(const ParseTree&) const;
private:
    struct ranked_char_hash {
        std::size_t operator()(const rankedChar& c) const {
            std::vector<int> v({c.c, c.rank});
            return boost::hash_range(v.begin(), v.end());
        }
    };
    template <typename Container>
    struct container_hash {
        std::size_t operator()(Container const& c) const {
            return boost::hash_range(c.begin(), c.end());
        }
    };
    bool testMap(const MultiLinearMap&, const rankedChar&);
    std::set<rankedChar> alphabet;
    int dim;
    std::unordered_map<rankedChar, MultiLinearMap, ranked_char_hash> transitions;
    floatVec lambda;
};

#endif //CFGLEARNER_MULTIPLICITYTREEACCEPTOR_H
