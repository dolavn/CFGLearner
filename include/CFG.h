//
// Created by dolavn@wincs.cs.bgu.ac.il on 1/30/19.
//

#ifndef CFGLEARNER_CFG_H
#define CFGLEARNER_CFG_H

#include <vector>
#include <string>
#include <unordered_map>

struct symbol{
    int sign;
    explicit symbol(int sign):sign(sign){}

    friend bool operator==(const symbol& lhs,const symbol& rhs){return lhs.sign==rhs.sign;}
};

struct nonTerminal: public symbol{
    explicit nonTerminal(int sign):symbol(sign){}
};

struct terminal: public symbol{
    explicit terminal(int sign):symbol(sign){}
};

struct derivation{
    nonTerminal nt;
    std::vector<symbol> rhs;

    derivation(nonTerminal nt, std::vector<symbol> rhs):nt(nt),rhs(rhs){}
};

class TreeAcceptor;

class CFG{
public:
    explicit CFG(int);
    explicit CFG(const TreeAcceptor&);
    void addNonTerminal(int);
    void addTerminal(int);
    void addDerivation(int,std::vector<int>);
    std::string getRepr() const;
private:
    void initFromAcceptor(const TreeAcceptor&);
    std::vector<nonTerminal> nonTerminals;
    std::vector<terminal> terminals;
    std::vector<derivation> derivations;
    std::unordered_map<int,int> symbolsMapping;

    bool checkSymbolExists(int);
};

#endif //CFGLEARNER_CFG_H
