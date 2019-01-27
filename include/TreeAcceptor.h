//
// Created by dolavn@wincs.cs.bgu.ac.il on 1/10/19.
//

#ifndef CFGLEARNER_TREEACCEPTOR_H
#define CFGLEARNER_TREEACCEPTOR_H

#include <unordered_map>
#include <vector>
#include "ParseTree.h"
#include <set>
#include <boost/functional/hash.hpp>

struct rankedChar{
    int c;
    int rank;

    bool operator==(const rankedChar& other){
        return c==other.c && rank==other.rank;
    }

    friend inline bool operator<(const rankedChar& lhs,const rankedChar& rhs){
        if(lhs.c==rhs.c){
            return lhs.rank<rhs.rank;
        }
        return lhs.c<rhs.c;
    }
};

class TreeAcceptor{
public:
    TreeAcceptor(std::set<rankedChar>); //Empty constructor
    TreeAcceptor(std::set<rankedChar>, int); //Constructs a tree acceptor with n states
    void setAccepting(int,bool);
    inline int getStatesNum(){return statesNum;}
    bool isAccepting(int) const; //TODO: make private
    int nextState(std::vector<int>, rankedChar) const; //TODO: make private
    void addTransition(std::vector<int>,rankedChar,int);
    bool run(const ParseTree&) const;

private:
    typedef std::vector<int> intVec;
    template <typename Container>
    struct container_hash {
        std::size_t operator()(Container const& c) const {
            return boost::hash_range(c.begin(), c.end());
        }
    };
    struct transition{
        const intVec statesSeq;
        const rankedChar c;
        const int targetState;
    };
    typedef std::pair<rankedChar,transition> transitionPair;
    typedef std::vector<transitionPair> transitionPairVec;
    int statesNum;
    std::set<int> acceptingStates;
    std::set<rankedChar> alphabet;
    std::unordered_map<intVec, transitionPairVec, container_hash<intVec>> transitions;

    static int hasTransition(transitionPairVec,rankedChar);
};

#endif //CFGLEARNER_TREEACCEPTOR_H
