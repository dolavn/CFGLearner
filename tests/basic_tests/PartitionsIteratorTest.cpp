//
// Created by dolavn on 2/26/20.
//

#include "gtest/gtest.h"
#include "TreesGenerator.h"
#include "TestsHelperFunctions.h"


using namespace std;

void printVec(vector<int> v){
    cout << "[";
    for(int i=0;i<v.size();++i){
        cout << v[i] << ",";
    }
    cout << "]" << endl;
}

TEST(partitions_iterator_test,basic_check){
    PartitionsIterator it(1, 1, 1);
    vector<vector<int>> results = {{1}};
    int i =0;
    TEST_VEC(it, results, i);
    it = PartitionsIterator(2, 2);
    results = {{0, 2}, {1 ,1}, {2, 0}};
    TEST_VEC(it, results, i);
    it = PartitionsIterator(3, 2);
    results = {{0, 3}, {1 ,2}, {2, 1}, {3, 0}};
    TEST_VEC(it, results, i);
    it = PartitionsIterator(2, 3);
    results = {{0, 0, 2}, {0, 1, 1}, {0, 2, 0}, {1, 0, 1}, {1, 1, 0}, {2, 0, 0}};
    TEST_VEC(it, results, i);
    it = PartitionsIterator(3, 3);
    results = {{0, 0, 3}, {0, 1, 2}, {0, 2, 1}, {0, 3, 0}, {1, 0, 2}, {1, 1, 1},
               {1, 2, 0}, {2, 0, 1}, {2, 1, 0}, {3, 0, 0}};
    TEST_VEC(it, results, i);
}

TEST(partitions_iterator_test,test_max_in_set){
    PartitionsIterator it(3, 3, 1);
    vector<vector<int>> results = {{1, 1, 1}};
    int i =0;
    TEST_VEC(it, results, i);
    it = PartitionsIterator(3, 3, 2);
    results = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 1, 1},
               {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};
    TEST_VEC(it, results, i);
    it = PartitionsIterator(4, 4, 1);
    results = {{1, 1, 1, 1}};
    TEST_VEC(it, results, i);
}

TEST(partitions_iterator_test,test_free_index){
    PartitionsIterator it(3, 3, 1, 0);
    vector<vector<int>> results = {{1, 1, 1}, {2, 0, 1}, {2, 1, 0}, {3, 0, 0}};
    int i =0;
    TEST_VEC(it, results, i);
    it = PartitionsIterator(3, 3, 1, 1);
    results = {{0, 2, 1}, {0, 3, 0}, {1, 1, 1}, {1, 2, 0}};
    TEST_VEC(it, results, i);
    it = PartitionsIterator(3, 3, 1, 2);
    results = {{0, 0, 3}, {0, 1, 2}, {1, 0, 2}, {1, 1, 1}};
    TEST_VEC(it, results, i);
    ASSERT_ANY_THROW(it=PartitionsIterator(3, 3, 1, 3));
    it = PartitionsIterator(4, 3, 2, 2);
    for(;it.hasNext();++it){
        cout << "[";
        for(auto i:*it){
            cout << i << ",";
        }
        cout << "]" << endl;
    }
}