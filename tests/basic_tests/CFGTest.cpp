#include "gtest/gtest.h"
#include "../../include/TreeAcceptor.h"
#include "../../include/CFG.h"

TEST(CFG_test, basic_test){
    CFG c(1);
    try{
        c.addNonTerminal(1);
        ASSERT_EQ(1,2);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("Symbol already exist in grammar"));
    }
    try{
        c.addTerminal(1);
        ASSERT_EQ(1,2);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("Symbol already exist in grammar"));
    }
    c.addTerminal(2);
    c.addDerivation(1,{2});
    c.addDerivation(1,{1,2});
    try{
        c.addDerivation(2,{1});
        ASSERT_EQ(1,2);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("LHS must be a non-terminal symbol"));
    }
    try{
        c.addDerivation(1,{4});
        ASSERT_EQ(1,2);
    }catch(std::invalid_argument& e){
        ASSERT_EQ(e.what(),std::string("Not all symbols in RHS exist in grammar"));
    }
}