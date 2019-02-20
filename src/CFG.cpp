#include "CFG.h"
#include "TreeAcceptor.h"
#include <sstream>
#include <iostream>

using namespace std;

#define NON_TERMINAL 1
#define TERMINAL 2

CFG::CFG(string startSymbol):nonTerminals(),terminals(),derivations(){
    nonTerminals.emplace_back(startSymbol);
    symbolsMapping[startSymbol] = NON_TERMINAL;
}

CFG::CFG(const TreeAcceptor& acc):nonTerminals(),terminals(),derivations(){
    unordered_map<int, string> temp;
    initFromAcceptor(acc,temp);
}

CFG::CFG(const TreeAcceptor& acc, unordered_map<int,string>& map):nonTerminals(),terminals(),derivations(){
    initFromAcceptor(acc,map);
}

void CFG::addNonTerminal(string nt){
    if(checkSymbolExists(nt)){
        throw std::invalid_argument("Symbol already exist in grammar");
    }
    nonTerminals.emplace_back(nonTerminal(nt));
    symbolsMapping[nt]=NON_TERMINAL;
}

void CFG::addTerminal(string t){
    if(checkSymbolExists(t)){
        throw std::invalid_argument("Symbol already exist in grammar");
    }
    terminals.emplace_back(terminal(t));
    symbolsMapping[t]=TERMINAL;
}

void CFG::addDerivation(string lhs, vector<string> rhs){
    if(!checkSymbolExists(lhs)){
        throw std::invalid_argument("LHS doesn't exist in grammar");
    }
    if(symbolsMapping[lhs]==TERMINAL){
        throw std::invalid_argument("LHS must be a non-terminal symbol");
    }
    vector<symbol> rhsSymbols;
    for(string& sym: rhs){
        if(!checkSymbolExists(sym)){
            throw std::invalid_argument("Not all symbols in RHS exist in grammar");
        }
        rhsSymbols.emplace_back(sym);
    }
    derivations.emplace_back(nonTerminal(lhs),rhsSymbols);
}

bool CFG::checkSymbolExists(string symbol){
    return symbolsMapping.find(symbol)!=symbolsMapping.end();
}

string CFG::getRepr() const{
    stringstream stream;
    for(auto& nt: nonTerminals){
        stream << nt.sign << " -> ";
        bool first=true;
        for(derivation d: derivations){
           if(d.nt==nt){
               if(!first){
                   stream << " | ";
               }
               first = false;
               for(symbol& s: d.rhs){
                   stream << s.sign << " ";
               }
           }
        }
        stream << "\n";
    }
    return stream.str();
}

void CFG::initFromAcceptor(const TreeAcceptor& acc, unordered_map<int, string> &cogMap){
    addNonTerminal("S"); //start symbol
    unordered_map<int, int> leafStates;
    for(int i=0;i<acc.getStatesNum();++i){
        stringstream stream;
        int terminal = -1;
        for(transition& t: acc.getTransitions()){
            if(t.targetState==i && !t.statesSeq.empty()){
                break;
            }
            if(t.targetState==i && t.statesSeq.empty()){
                terminal = t.c.c;
            }
        }
        if(terminal!=-1){
            leafStates[i] = terminal;
            continue;
        }
        stream << "N" << i;
        addNonTerminal(stream.str());
        if(acc.isAccepting(i)){
            addDerivation("S",{stream.str()});
        }
    }
    for(rankedChar& c:acc.getAlphabet()){
        if(c.rank==0){
            stringstream stream;
            if(cogMap.find(c.c)!=cogMap.end()){
                stream << cogMap[c.c];
            }else {
                stream << c.c;
            }
            addTerminal(stream.str());
        }
    }
    for(transition& t:acc.getTransitions()){
        vector<string> lhs;
        if(!t.statesSeq.empty()) {
            for (int state: t.statesSeq) {
                stringstream stream;
                if(leafStates.find(state)!=leafStates.end()){
                    int c = leafStates[state];
                    if(cogMap.find(c)!=cogMap.end()){
                        stream << cogMap[c];
                    }else {
                        stream << c;
                    }
                }else{
                    stream << "N" << state;
                }
                lhs.push_back(stream.str());
            }
        }else{
            continue;
        }
        stringstream stream;
        stream << "N" << t.targetState;
        addDerivation(stream.str(),lhs);
    }
}
