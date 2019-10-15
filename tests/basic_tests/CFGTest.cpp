#include "gtest/gtest.h"
#include "../../include/TreeAcceptor.h"
#include "../../include/CFG.h"

TEST(CFG_test, basic_test){
    CFG c("S");
    try{
        c.addNonTerminal("S");
        ASSERT_EQ(1,2);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("Symbol already exist in grammar"));
    }
    c.addTerminal("a");
    c.addDerivation("S",{"a"});
    c.addDerivation("S",{"S","a"});
    try{
        c.addDerivation("a",{"S"});
        ASSERT_EQ(1,2);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("LHS must be a non-terminal symbol"));
    }
    try{
        c.addDerivation("S",{"b"});
        ASSERT_EQ(1,2);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("Not all symbols in RHS exist in grammar"));
    }
}

TEST(CFG_test, simplify_grammar_test){
    CFG c("S");
    c.addNonTerminal("A");
    c.addNonTerminal("B");
    c.addTerminal("a");
    c.addDerivation("S",{"A"});
    c.addDerivation("S",{"A","A"});
    c.addDerivation("S",{"A","B"});
    c.addDerivation("A",{"B", "a"});
    c.addDerivation("B",{"a","a"});
    c.addDerivation("B",{"a"});
    ASSERT_TRUE(c.hasNonTerminal("A"));
    c.simplifyGrammar();
    ASSERT_FALSE(c.hasNonTerminal("A"));
    std::cout << c.getRepr() << std::endl;
}