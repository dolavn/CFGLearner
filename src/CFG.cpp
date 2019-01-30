#include "CFG.h"
#include "TreeAcceptor.h"
#include <sstream>
#include <iostream>

using namespace std;

#define NON_TERMINAL 1
#define TERMINAL 2

CFG::CFG(int startSymbol):nonTerminals(),terminals(),derivations(){
    nonTerminals.emplace_back(startSymbol);
    symbolsMapping[startSymbol] = NON_TERMINAL;
}

CFG::CFG(const TreeAcceptor& acc):nonTerminals(),terminals(),derivations(){
    initFromAcceptor(acc);
}

void CFG::addNonTerminal(int nt){
    if(checkSymbolExists(nt)){
        throw std::invalid_argument("Symbol already exist in grammar");
    }
    nonTerminals.push_back(nonTerminal(nt));
    symbolsMapping[nt]=NON_TERMINAL;
}

void CFG::addTerminal(int t){
    if(checkSymbolExists(t)){
        throw std::invalid_argument("Symbol already exist in grammar");
    }
    terminals.push_back(terminal(t));
    symbolsMapping[t]=TERMINAL;
}

void CFG::addDerivation(int lhs, vector<int> rhs){
    if(!checkSymbolExists(lhs)){
        throw std::invalid_argument("LHS doesn't exist in grammar");
    }
    if(symbolsMapping[lhs]==TERMINAL){
        throw std::invalid_argument("LHS must be a non-terminal symbol");
    }
    vector<symbol> rhsSymbols;
    for(int& sym: rhs){
        if(!checkSymbolExists(sym)){
            throw std::invalid_argument("Not all symbols in RHS exist in grammar");
        }
        rhsSymbols.emplace_back(sym);
    }
    derivations.emplace_back(nonTerminal(lhs),rhsSymbols);
}

bool CFG::checkSymbolExists(int symbol){
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

void CFG::initFromAcceptor(const TreeAcceptor& acc){
    addNonTerminal(0); //start symbol
    for(int i=0;i<acc.getStatesNum();++i){
        addNonTerminal(i+1);
        if(acc.isAccepting(i)){
            addDerivation(0,{i+1});
        }
    }
    for(rankedChar& c:acc.getAlphabet()){
        if(c.rank==0){
            addTerminal(c.c+acc.getStatesNum());
        }
    }
    for(transition& t:acc.getTransitions()){
        vector<int> lhs;
        if(t.statesSeq.size()>0) {
            for (int state: t.statesSeq) {
                lhs.push_back(state+1);
            }
        }else{
            lhs.push_back(t.c.c+acc.getStatesNum());
        }
        addDerivation(t.targetState+1,lhs);
    }
}
