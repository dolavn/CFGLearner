#include "TreeConstructor.h"
#include "Sequence.h"
#include "ParseTree.h"

#include <iostream>
#include <limits>
#include <stack>

using namespace std;

TreeConstructor::TreeConstructor(scoreTable table):table(std::move(table)), dpTable(),seq(){

}

int TreeConstructor::createTree(const Sequence& seq){
    this->seq = seq;
    dpTable = Trees::dpTable(seq.getLength(), seq.getLength());
    for(int i=0;i<seq.getLength();++i){    /*Table initialization*/
        dpTable[i][i] = 0;
        if(i<seq.getLength()-1){
            dpTable[i][i+1] = table[seq.subseq(i, i+2)];
        }
    }
    for(int currLength=3;currLength<=seq.getLength();currLength++){
        for(int i=0;i<seq.getLength()-currLength+1;++i){
            int j = i + currLength - 1;
            int max = numeric_limits<int>::min();
            int argmax = -1;
            for(int k=i;k<j;k++){
                int curr = dpTable[i][k] + dpTable[k+1][j];
                if(curr>max){
                    max = curr;
                    argmax = k;
                }
            }
            dpTable[j][i] = argmax;
            dpTable[i][j] = max;
        }
    }
    return dpTable[0][seq.getLength()-1];
}

ParseTree* TreeConstructor::traceback(const Trees::dpTable& table, const Sequence& seq){
    typedef pair<int, int> intTuple;
    typedef pair<ParseTree*, intTuple> stackPair;
    stack<stackPair> buildStack;
    auto root = new ParseTree(0);
    intTuple rootTup = intTuple(0, seq.getLength()-1);
    buildStack.emplace(root, rootTup);
    while(!buildStack.empty()){
        auto top = buildStack.top(); ParseTree* node = top.first; intTuple currTup = top.second;
        //cout << currTup.first << "," << currTup.second << endl;
        //cout << "visiting" << endl;
        buildStack.pop();
        if(currTup.first!=currTup.second){ //Node is an internal node
            int k = currTup.second==currTup.first+1?currTup.first:table.getTrace(currTup.first, currTup.second);
            //cout << "[" << currTup.first << "," << k << "] [" << k+1 << "," << currTup.second << "]" << endl;
            auto rightNode = new ParseTree(0);
            auto leftNode = new ParseTree(0);
            node->setRightPointer(rightNode); node->setLeftPointer(leftNode);
            buildStack.emplace(leftNode, intTuple(currTup.first, k));
            buildStack.emplace(rightNode, intTuple(k+1, currTup.second));
        }else{ //Node is a leaf
            node->setData(seq.getValue(currTup.first));
            //cout << "data:" << node->getData() << "addr:" << node <<  endl;
        }
    }
    return root;
}

ParseTree* TreeConstructor::getTree(){
    return traceback(dpTable, seq);
}

namespace Trees{
    dpTable::dpTable():table(nullptr),len1(0),len2(0){}
    dpTable::dpTable(int len1, int len2):table(nullptr),len1(len1),len2(len2){
        table = new int[len1*len2];
    }

    dpTable::dpTable(const dpTable& other):table(nullptr),len1(other.len1),len2(other.len2){copy(other);}

    dpTable& dpTable::operator=(const Trees::dpTable &other){
        if(this==&other){
            return *this;
        }
        clear();
        copy(other);
        return *this;
    }

    dpTable::dpTable(Trees::dpTable &&other) noexcept:table(other.table),len1(other.len1),len2(other.len2){
        other.table = nullptr;
        other.len1 = 0;
        other.len2 = 0;
    }

    dpTable& dpTable::operator=(dpTable&& other) noexcept{
        if(this==&other){
            return *this;
        }
        clear();
        table = other.table;
        other.table = nullptr;
        len1 = other.len1;
        len2 = other.len2;
        other.len1 = 0;
        other.len2 = 0;
        return *this;
    }

    void dpTable::clear() {
        if(table!=nullptr){
            delete[](table);
            table=nullptr;
        }
    }

    void dpTable::copy(const Trees::dpTable &other) {
        table = new int[other.len1*other.len2];
        len1 = other.len1;
        len2 = other.len2;
        for(int i=0;i<len1*len2;++i){table[i] = other.table[i];}
    }

    int dpTable::getVal(int i, int j) const{
        return table[i*len2 + j];
    }

    int dpTable::getTrace(int i, int j) const{
        return table[j*len2 + i];
    }

    void dpTable::printTable(){
        for(int i=0;i<len1*len2;++i){
            std::cout << "[" << i << "]=" << table[i] << std::endl;
        }
    }

    dpTable::dpRow dpTable::operator[](int ind){
        return {*this, ind};
    }

    dpTable::dpRow::dpRow(const dpTable& table, int ind1):table(table.table),start(ind1*table.len2){

    }

    int& dpTable::dpRow::operator[](int ind2) {
        return table[start+ind2];
    }

    dpTable::~dpTable(){
        clear();
    }
}
