#include "CFG.h"
#include "TreeAcceptor.h"
#include <sstream>
#include <iostream>

using namespace std;

#define NON_TERMINAL 1
#define TERMINAL 2

CFG::CFG(string startSymbol):nonTerminals(),startSymbol(startSymbol),terminals(),derivations(){
    nonTerminals.emplace_back(startSymbol);
    symbolsMapping[startSymbol] = NON_TERMINAL;
}

CFG::CFG(const TreeAcceptor& acc):nonTerminals(),startSymbol(""),terminals(),derivations(){
    unordered_map<int, string> temp;
    initFromAcceptor(acc,temp);
}

CFG::CFG(const TreeAcceptor& acc, unordered_map<int,string>& map):nonTerminals(),startSymbol(""),
terminals(),derivations(){
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

bool CFG::checkSymbolExists(string symbol) const{
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
    startSymbol = nonTerminal("S");
    unordered_map<int, int> leafStates;
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
    for(int i=0;i<acc.getStatesNum();++i){
        stringstream stream;
        vector<int> currTerminals;
        for(transition& t: acc.getTransitions()){
            if(t.targetState==i && t.statesSeq.empty()){
                currTerminals.push_back(t.c.c);
            }
        }
        stream << "N" << i;
        addNonTerminal(stream.str());
        for(int terminal: currTerminals){
            stringstream stream2;
            if(cogMap.find(terminal)!=cogMap.end()){
                stream2 << cogMap[terminal];
            }else {
                stream2 << terminal;
            }
            addDerivation(stream.str(), {stream2.str()});
        }
        if(acc.isAccepting(i)){
            addDerivation("S",{stream.str()});
        }
    }
    for(transition& t:acc.getTransitions()){
        vector<string> lhs;
        if(!t.statesSeq.empty()) {
            for (int state: t.statesSeq) {
                stringstream stream;
                stream << "N" << state;
                lhs.push_back(stream.str());
            }
        }else{
            continue;
        }
        stringstream stream;
        stream << "N" << t.targetState;
        addDerivation(stream.str(),lhs);
    }
    cout << getRepr() << endl;
    simplifyGrammar();
}

bool CFG::hasNonTerminal(string nt) const{
    return checkSymbolExists(nt) && symbolsMapping.find(nt)->second==NON_TERMINAL;
}

int CFG::getDerivationsNum(string nt) const{
    int ans = 0;
    for(auto der: derivations){
        if(der.nt.sign==nt){ans++;}
    }
    return ans;
}

bool CFG::isSimple() const{
    for(auto nt: nonTerminals){
        if(getDerivationsNum(nt.sign)!=1){
            return false;
        }
    }
    return true;
}

void printRHS(vector<symbol> rhs){
    cout << "{";
    for(int i=0;i<rhs.size();++i){
        cout << rhs[i].sign;
        if(i<rhs.size()-1){cout << ",";}
    }
    cout << "}" << endl;
}

void printDerivation(derivation d){
    cout << d.nt.sign << " -> ";
    printRHS(d.rhs);
}

void CFG::simplifyGrammar(){
    for(auto it=nonTerminals.begin();it!=nonTerminals.end();){
        auto& nt = *it;
        int appearNum = getDerivationsNum(nt.sign);
        if(nt.sign==startSymbol.sign){++it;continue;}
        if(appearNum==1){
            vector<symbol> rhs;
            for(auto it=derivations.begin();it!=derivations.end();++it){
                auto derivation = *it;
                if(derivation.nt.sign==nt.sign){
                    derivations.erase(it);
                    rhs = derivation.rhs;
                    break;
                }
            }
            swapSymbolRhs(nt, rhs);
            removeNonterminal(nt.sign);
        }else{
            ++it;
        }
    }
}

void CFG::removeNonterminal(string str){
    symbolsMapping.erase(symbolsMapping.find(str));
    for(auto it=nonTerminals.begin();it!=nonTerminals.end();++it){
        auto nt = *it;
        if(nt.sign==str){
            nonTerminals.erase(it);
            break;
        }
    }
}

void CFG::swapSymbolRhs(symbol sym, vector<symbol> targetRhs){
    for(auto& der: derivations){
        for(auto it=der.rhs.begin();it!=der.rhs.end();++it){
            auto currSym = *it;
            if(currSym.sign==sym.sign){
                der.rhs.erase(it);
                der.rhs.insert(it, targetRhs.begin(), targetRhs.end());
                it = der.rhs.begin();
            }
        }
    }
}