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

    friend inline bool operator==(const rankedChar& lhs,const rankedChar& rhs){
        return lhs.c==rhs.c && lhs.rank==rhs.rank;
    }

    friend inline bool operator<(const rankedChar& lhs,const rankedChar& rhs){
        if(lhs.c==rhs.c){
            return lhs.rank<rhs.rank;
        }
        return lhs.c<rhs.c;
    }
};

typedef std::vector<int> intVec;

struct transition{
    intVec statesSeq;
    rankedChar c;
    int targetState;

    inline friend bool operator==(const transition& lhs, const transition& rhs){
        return lhs.statesSeq==rhs.statesSeq && lhs.c==rhs.c && lhs.targetState == rhs.targetState;
    }
};

class TreeAcceptor{
public:
    explicit TreeAcceptor(std::set<rankedChar>); //Empty constructor
    TreeAcceptor(std::set<rankedChar>, int); //Constructs a tree acceptor with n states
    int addState();
    void addChar(rankedChar);
    void setAccepting(int,bool);
    inline int getStatesNum() const{return statesNum;}
    bool isAccepting(int) const;
    int nextState(std::vector<int>, rankedChar) const; //TODO: make private
    void addTransition(const std::vector<int>&,rankedChar,int);
    bool run(const ParseTree&) const;
    void printDescription() const;
    std::string getLatexDescription() const;
    std::vector<rankedChar> getAlphabet() const;
    std::vector<transition> getTransitions() const;

private:
    template <typename Container>
    struct container_hash {
        std::size_t operator()(Container const& c) const {
            return boost::hash_range(c.begin(), c.end());
        }
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
