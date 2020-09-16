#include "TreeConstructor.h"
#include "Sequence.h"
#include "ParseTree.h"

#include <iostream>
#include <limits>
#include <stack>

#define NEG_INF -100

using namespace std;
using namespace Trees;


TreeConstructor::TreeConstructor(scoreTable table):table(std::move(table)), calcTable(),seq(),lambda(0),
concatDuplications(false),concatDirection(false){

}

float TreeConstructor::createTree(const Sequence& seq){
    this->seq = seq;
    //cout << seq << endl;
    //dpTable = Trees::dpTable(seq.getLength(), seq.getLength());
    dpTable dpTable(this->seq.getLength(), this->seq.getLength());
    if(concatDuplications){
        concat(dpTable);
    }
    //printTable();
    for(int i=0;i<seq.getLength();++i){    /*Table initialization*/
        dpTable[i][i] = 0;
        if(i<this->seq.getLength()-1){
            dpTable[i][i+1] = table[this->seq.subseq(i, i+2)];
        }
    }
    for(int currLength=3;currLength<=this->seq.getLength();currLength++){
        for(int i=0;i<this->seq.getLength()-currLength+1;++i){
            int j = i + currLength - 1;
            float max = NEG_INF;
            int argmax = i;
            for(int k=i;k<j;k++){
                //cout << "i, k, j" << i << "," << k << "," << j << endl;
                //cout << dpTable.getVal(i,k) << " , " << dpTable.getVal(k+1, j) << endl;
                float curr = dpTable.getVal(i, k) + dpTable.getVal(k+1, j);
                if(curr>max){
                    max = curr;
                    argmax = k;
                }
            }
            //cout << "(" << i << "," << j << ")=" <<  argmax << endl;
            dpTable[j][i] = argmax;
            dpTable[i][j] = max+lambda*table[this->seq.subseq(i, j+1)];
        }
    }
    this->calcTable = move(dpTable);
    return this->calcTable[0][this->seq.getLength()-1];
}

float TreeConstructor::createTree(const vector<int>& seq){
    Sequence seq2(seq);
    return createTree(seq2);
}

void TreeConstructor::setLambda(float lambda) {
    this->lambda = lambda;
}

void TreeConstructor::setConcat(bool concat){
    this->concatDuplications = concat;
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
        /*cout << currTup.first << "," << currTup.second << endl;
        cout << "[";
        for(auto elem: seq.subseq(currTup.first, currTup.second+1)){
            cout << elem << ",";
        }
        cout << "]" << endl;
        cout << "visiting" << endl;*/
        buildStack.pop();
        if(currTup.first!=currTup.second){ //Node is an internal node
            int k = currTup.second==currTup.first+1?currTup.first:table.getTrace(currTup.first, currTup.second);
            //cout << "[" << currTup.first << "," << k << "] [" << k+1 << "," << currTup.second << "]" << endl;
            auto rightNode = new ParseTree(0);
            auto leftNode = new ParseTree(0);
            node->setPointer(rightNode,1); node->setPointer(leftNode,0);
            buildStack.emplace(leftNode, intTuple(currTup.first, k));
            buildStack.emplace(rightNode, intTuple(k+1, currTup.second));
        }else{ //Node is a leaf
            node->setData(seq.getValue(currTup.first));
            //cout << "data:" << node->getData() << "addr:" << node <<  endl;
        }
    }
    return root;
}

vector<pair<int,int>> TreeConstructor::getDuplicationIndices() const {
    vector<pair<int, int>> ans;
    int lastChar = seq.getValue(0);
    int lastInd = 0;
    for(int i=1;i<seq.getLength();++i){
        if(seq.getValue(i)!=lastChar){
            ans.emplace_back(lastInd, i);
            lastInd = i;
            lastChar = seq.getValue(i);
        }
    }
    ans.emplace_back(lastInd, seq.getLength());
    return ans;
}


void TreeConstructor::concat(dpTable& table){
    vector<pair<int,int>> dupIndices = getDuplicationIndices();
    vector<int> updatedSeqVec;
    duplications.clear();
    for(auto& p: dupIndices){
        duplications.emplace_back(p, seq.getValue(p.first));
        updatedSeqVec.push_back(seq.getValue(p.first));
    }
    Sequence updatedSeq = Sequence(updatedSeqVec);
    this->originalSeq = this->seq;
    this->seq = updatedSeq;
}

ParseTree TreeConstructor::getDuplicatedTree(int symbol, int len) const{
    if(len<=0){throw std::invalid_argument("len must be at least 1");}
    ParseTree currTree(symbol);
    while(len>1){
        if(concatDirection){ //left
            currTree = ParseTree(0, {ParseTree(symbol), currTree});
        }else{ //right
            currTree = ParseTree(0, {currTree, ParseTree(symbol)});
        }

        len=len-1;
    }
    return currTree;
}

ParseTree TreeConstructor::postProcessTree(const ParseTree& tree) const{
    int currInd = 0;
    ParseTree ans = tree;
    for(auto it=tree.getLeafIterator();it.hasNext();++it){
        if(currInd>=duplications.size()){
            throw std::invalid_argument("index out of bounds");
        }
        pair<int,int> currRange = duplications[currInd].first;
        int currSym = duplications[currInd].second;
        int currLen = currRange.second-currRange.first;
        currInd++;
        ans[*it] = getDuplicatedTree(currSym, currLen);
    }
    return ans;
}



ParseTree TreeConstructor::getTree(){
    ParseTree* root = traceback(calcTable, seq);
    ParseTree ans(*root);
    delete(root);
    if(concatDuplications){
        ans = postProcessTree(ans);
    }
    return ans;
}

void TreeConstructor::printTable(){
    for(auto elem: table){
        cout << "["; for(auto e: elem.first){ cout << e << ",";} cout << "]=>" << elem.second << endl;
    }
}


namespace Trees{
    dpTable::dpTable():table(),len1(0),len2(0){}
    dpTable::dpTable(int len1, int len2):table(len1, vector<float>(len2)),len1(len1),len2(len2){
    }

/*
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
        table = new float[other.len1*other.len2];
        len1 = other.len1;
        len2 = other.len2;
        for(int i=0;i<len1*len2;++i){table[i] = other.table[i];}
    }
*/
    float dpTable::getVal(int i, int j) const{
        return table[i][j];
    }

    int dpTable::getTrace(int i, int j) const{
        return int(table[j][i]);
    }

    void dpTable::printTable(){
        for(int i=0;i<len1;++i){
            for(int j=0;j<len2;++j){
                std::cout << "[" << i << "," << j << "]=" << table[i][j] << std::endl;
            }
        }
    }

    vector<float>& dpTable::operator[](int ind){
        return table[ind];
    }

/*
    dpTable::dpRow dpTable::operator[](int ind){
        return {*this, ind};
    }

    dpTable::dpRow::dpRow(const dpTable& table, int ind1):table(table.table),start(ind1*table.len2){
        cout << "row " << ind1 << " , " << table.len2 << endl;
    }

    float& dpTable::dpRow::operator[](int ind2) {
        return table[start+ind2];
    }

    dpTable::~dpTable(){
        clear();
    }
*/
}
