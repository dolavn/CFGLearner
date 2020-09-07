#include "gtest/gtest.h"
#include "TreesGenerator.h"
#include "ParseTree.h"
#include "TestsHelperFunctions.h"
#include "TreeConstructor.h"
#include <vector>

using namespace std;

TEST(constructor_generator_test,basic_check){
    scoreTable table;
    table[{1, 1}] = 1.0f;
    table[{1, 2}] = 2.0f;
    table[{2, 1}] = 2.0f;
    table[{2, 2}] = 1.0f;
    TreeConstructor c(table);
    int maxLeaves = 5;
    int numTrees = 100;
    ConstructorGenerator gen(c, maxLeaves, numTrees, {1, 2});
    int createdTrees = 0;
    for(;gen.hasNext();++gen){
        ParseTree tree = *gen;
        cout << tree << endl;
        ASSERT_TRUE(tree.getLeavesNum()<=maxLeaves);
        createdTrees++;
    }
    ASSERT_EQ(createdTrees, numTrees);
}

TEST(constructor_generator_test,exhustive_test){
    scoreTable table;
    table[{1, 1}] = 1.0f;
    table[{1, 2}] = 2.0f;
    table[{2, 1}] = 2.0f;
    table[{2, 2}] = 1.0f;
    TreeConstructor c(table);
    int maxLeaves = 5;
    ConstructorGenerator gen(c, maxLeaves, {1, 2});
    gen.reset();
    for(;gen.hasNext();++gen){
        ParseTree tree = *gen;
        cout << tree << endl;
        ASSERT_TRUE(tree.getLeavesNum()<=maxLeaves);
    }
}
