//
// Created by dolavn on 6/3/19.
//

#ifndef CFGLEARNER_DEFINITIONS_H
#define CFGLEARNER_DEFINITIONS_H

template <typename Container>
struct container_hash {
    std::size_t operator()(Container const& c) const {
        return boost::hash_range(c.begin(), c.end());
    }
};

struct pair_hash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        std::vector<int> c = {p.first, p.second};
        return boost::hash_range(c.begin(), c.end());
    }
};

typedef std::pair<int, int> intPair;

#endif //CFGLEARNER_DEFINITIONS_H
