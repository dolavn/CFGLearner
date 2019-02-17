//
// Created by dolavn@wincs.cs.bgu.ac.il on 1/17/19.
//

#include <iostream> //TODO: delete
#include <sstream>
#include "TreeAcceptor.h"

using namespace std;

TreeAcceptor::
TreeAcceptor(set<rankedChar> alphabet):statesNum(0),acceptingStates(),alphabet(std::move(alphabet)),transitions(){

}

TreeAcceptor::TreeAcceptor(set<rankedChar> alphabet, int statesNum):statesNum(statesNum),acceptingStates(),alphabet(std::move(alphabet)),transitions(){

}

int TreeAcceptor::addState(){
    int ans = statesNum++;
    return ans;
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

void TreeAcceptor::addChar(rankedChar c){
    alphabet.insert(c);
}

void TreeAcceptor::addTransition(const std::vector<int>& states, rankedChar c, int targetState) {
    if(c.rank!=(int)(states.size())){
        throw std::invalid_argument("Rank of character and number of states don't match!");
    }
    transition t{states,c,targetState};
    transitionPairVec& v = transitions[states];
    for(auto& pair: v){
        if(pair.first==c){
            pair.second = t;
            return;
        }
    }
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
    stack<vector<int>> stack;
    for(auto it = runTree.getLeafIterator();it.hasNext();++it){
        stack.push(*it);
    }
    while(!stack.empty()){
        vector<int> nodeInd = stack.top();
        stack.pop();
        const ParseTree& node = t.getNode(nodeInd);
        vector<int> states;
        int rank = 0;
        for(int i=0;i<runTree[nodeInd].getChildrenNum();++i){
            if(node.hasSubtree(i)){
                states.push_back(runTree[nodeInd][{i}].getData());
                rank++;
            }
        }
        runTree[nodeInd].setData(nextState(states,{node.getData(),rank}));
        if(runTree[nodeInd].getData()==-1){ //no transition
            return false;
        }
        if(nodeInd.empty()){break;}
        vector<int> fatherInd = vector<int>(nodeInd.begin(),nodeInd.begin()+nodeInd.size()-1);
        ParseTree& fatherNode = runTree[fatherInd];
        bool climb=true;
        for(int i=0;i<fatherNode.getChildrenNum();++i){
            if(fatherNode.hasSubtree(i) && fatherNode[{i}].getData()==-1){climb=false;break;}
        }
        if(climb) {
            stack.push(fatherInd);
        }
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

vector<rankedChar> TreeAcceptor::getAlphabet() const{
    vector<rankedChar> ans;
    for(rankedChar c:alphabet){
        ans.push_back(c);
    }
    return ans;
}

vector<transition> TreeAcceptor::getTransitions() const{
    vector<transition> ans;
    for(auto& states: transitions){
        for(auto& p: states.second){
            ans.push_back(p.second);
        }
    }
    return ans;
}

void TreeAcceptor::printDescription() const{
    std::cout << "States num : " << statesNum << std::endl;
    for(int i=0;i<statesNum;++i) {
        string acc = isAccepting(i) ? "Accepting" : "Not Accepting";
        std::cout << "State : " << i << " " << acc << std::endl;
    }
    std::cout << "Transitions:" << std::endl;
    for(auto& t: transitions){
        for(auto& p: t.second){
            stringstream stream;
            stream << "<";
            for(unsigned int i=0;i<t.first.size();++i){
                stream << t.first[i];
                if(i<t.first.size()-1){stream << ",";}
            }
            stream << ">";
            std::cout << "delta(" << stream.str() << "," << p.second.c.c << ")=" << p.second.targetState << std::endl;
        }
    }
}