//
// Created by dolavn@wincs.cs.bgu.ac.il on 1/17/19.
//

#include <iostream> //TODO: delete

#include "TreeAcceptor.h"

using namespace std;

TreeAcceptor::TreeAcceptor(set<rankedChar> alphabet):statesNum(0),acceptingStates(),alphabet(std::move(alphabet)),transitions(){

}

TreeAcceptor::TreeAcceptor(set<rankedChar> alphabet, int statesNum):statesNum(statesNum),acceptingStates(),alphabet(std::move(alphabet)),transitions(){

}

void TreeAcceptor::setAccepting(int state, bool accepting){
    if(state>=statesNum){
        throw std::invalid_argument("No such state!");
    }
    if(accepting){
        acceptingStates.insert(state);
    }else{
        acceptingStates.erase(state);
    }
}

void TreeAcceptor::addTransition(std::vector<int> states, rankedChar c, int targetState) {
    if(c.rank!=states.size()){
        throw std::invalid_argument("Rank of character and number of states don't match!");
    }
    transition t{states,c,targetState};
    transitionPairVec& v = transitions[states];
    v.emplace_back(c,t);
}

int TreeAcceptor::nextState(std::vector<int> states, rankedChar c) const{
    auto it = transitions.find(states);
    if(it==transitions.end()){
        return -1;
    }
    transitionPairVec v = (*it).second;
    int ind = hasTransition(v,c);
    if(ind==-1){return -1;}
    return v[ind].second.targetState;
}

bool TreeAcceptor::run(const ParseTree& t) const{
    ParseTree runTree = t.getSkeleton();
    stack<string> stack;
    std::cerr << std::endl;
    for(auto it = runTree.getLeafIterator();it.hasNext();++it){
        stack.push(*it);
    }
    while(!stack.empty()){
        string nodeInd = stack.top();
        stack.pop();
        const ParseTree& node = t.getNode(nodeInd);
        vector<int> states;
        int rank = 0;
        if(node.hasLeftSubtree()){
            states.push_back(runTree[nodeInd]["0"].getData());
            rank++;
        }
        if(node.hasRightSubtree()){
            states.push_back(runTree[nodeInd]["1"].getData());
            rank++;
        }
        runTree[nodeInd].setData(nextState(states,{node.getData(),rank}));
        if(runTree[nodeInd].getData()==-1){ //no transition
            return false;
        }
        if(nodeInd.empty()){break;}
        string fatherInd = nodeInd.substr(0,nodeInd.size()-1);
        ParseTree& fatherNode = runTree[fatherInd];
        if(fatherNode.hasLeftSubtree() && fatherNode["0"].getData()==-1){continue;}
        if(fatherNode.hasRightSubtree() && fatherNode["1"].getData()==-1){continue;}
        stack.push(fatherInd);
    }
    return isAccepting(runTree.getData());
}

bool TreeAcceptor::isAccepting(int state) const{
    auto it = acceptingStates.find(state);
    return it!=acceptingStates.end();
}

int TreeAcceptor::hasTransition(TreeAcceptor::transitionPairVec v, rankedChar c) {
    int i=0;
    for(auto& pair: v){
        if(pair.first==c){
            return i;
        }
        i++;
    }
    return -1;
}
