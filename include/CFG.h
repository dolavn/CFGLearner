/*
 * A header for the definition of a context free grammar.
 * Created by Dolav Nitay.
 */

#ifndef CFGLEARNER_CFG_H
#define CFGLEARNER_CFG_H

#include <vector>
#include <string>
#include <unordered_map>

//A symbol, can either be a terminal or a non-terminal.
struct symbol{
    std::string sign;
    explicit symbol(std::string sign):sign(std::move(sign)){}

    friend bool operator==(const symbol& lhs,const symbol& rhs){return lhs.sign==rhs.sign;}
};

//A non-terminal symbol
struct nonTerminal: public symbol{
    explicit nonTerminal(std::string sign):symbol(std::move(sign)){}
};

//A terminal symbol
struct terminal: public symbol{
    explicit terminal(std::string sign):symbol(std::move(sign)){}
};

//A derivation rule, to derive a sequence of symbols from a non-terminal one.
struct derivation{
    nonTerminal nt;
    std::vector<symbol> rhs;

    derivation(nonTerminal nt, std::vector<symbol> rhs):nt(std::move(nt)),rhs(std::move(rhs)){}
};

class TreeAcceptor;

//A class representing a context free grammar
class CFG{
public:
    explicit CFG(std::string);
    explicit CFG(const TreeAcceptor&);
    void addNonTerminal(std::string);
    void addTerminal(std::string);
    void addDerivation(std::string,std::vector<std::string>);
    std::string getRepr() const; //Returns a representation of this CFG, could be used to construct nltk cfg object.
private:
    //Creates a context free grammar from a tree acceptor
    void initFromAcceptor(const TreeAcceptor&);
    std::vector<nonTerminal> nonTerminals;
    std::vector<terminal> terminals;
    std::vector<derivation> derivations;
    /* A mapping used to decide if a given symbol is in the grammar,
     * and whether it's a terminal or a non terminal symbol. */
    std::unordered_map<std::string,int> symbolsMapping;
    //Returns true if the given symbol exists in the grammar.
    bool checkSymbolExists(std::string);
};

#endif //CFGLEARNER_CFG_H
