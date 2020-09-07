#include "TreesGenerator.h"
#include "TreeConstructor.h"
#include "ParseTree.h"
#include "Logger.h"
#include "utility.h"
#include <random>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <chrono>
#include <iostream>
#include <fstream>



typedef std::chrono::high_resolution_clock myclock;
using namespace std;
using namespace std::chrono;


static ofstream myfile;

ConstructorGenerator::ConstructorGenerator(TreeConstructor& con, int maxLen, vector<int> alphabet):
        constructor(con),maxLen(maxLen), numTrees(-1), alphabet(move(alphabet)),currLen(1),
        currArr({(int)(alphabet.size())}),treesGenerated(0), currTree(nullptr){
    generateTree();
    myfile.open("trees.txt");
    myfile << "alphabet:[";
    for(auto a: this->alphabet){
        myfile << a << ",";
    }
    myfile << "]" << endl;
}

ConstructorGenerator::ConstructorGenerator(TreeConstructor& con, int maxLen, int numTrees, vector<int> alphabet):
constructor(con),maxLen(maxLen), numTrees(numTrees), alphabet(move(alphabet)),currLen(1),
currArr({(int)(alphabet.size())}),treesGenerated(0), currTree(nullptr){
    generateTree();
    myfile.open("trees.txt");
    myfile << "alphabet:[";
    for(auto a: this->alphabet){
        myfile << a << ",";
    }
    myfile << "]" << endl;
}

ConstructorGenerator::ConstructorGenerator(const ConstructorGenerator& other):constructor(other.constructor),
maxLen(other.maxLen),numTrees(other.numTrees),alphabet(other.alphabet),treesGenerated(other.treesGenerated),
currTree(nullptr){
    cout << "copy" << endl;
    copy(other);
}

ConstructorGenerator::ConstructorGenerator(ConstructorGenerator&& other):constructor(other.constructor),
maxLen(other.maxLen),numTrees(other.numTrees),alphabet(move(other.alphabet)),treesGenerated(other.treesGenerated),
currTree(other.currTree){
    other.currTree = nullptr;
}

ConstructorGenerator::~ConstructorGenerator(){
    myfile.close();
    clear();
}

unsigned int ConstructorGenerator::convertSampleToLen(unsigned int sample) const{
    auto sigma = (unsigned int)alphabet.size();
    int curr_sum = 0;
    unsigned int ans=0;
    while(curr_sum<sample){
        curr_sum = curr_sum + int(pow(sigma, ans));
        ans++;
    }
    return ans;
}

unsigned int ConstructorGenerator::generateSeqLen() const{
    std::random_device rd;
    std::default_random_engine generator(rd());
    unsigned long sigma = alphabet.size();
    double total = pow(sigma, maxLen)-1; total = total/(sigma-1); //total = total*sigma;
    uniform_int_distribution<int> distribution(1,int(total));
    unsigned int sample = distribution(generator);
    //return sample;
    return convertSampleToLen(sample);
}

vector<int> ConstructorGenerator::generateSeqDeterministic() const{
    vector<int> ans(currLen);
    myfile << "[";
    for(unsigned int i=0;i<currLen;++i){ans[i] = alphabet[currArr.get(i)]; myfile << ans[i] << ",";}
    myfile << "]" << endl;
    return ans;
}

vector<int> ConstructorGenerator::generateSeq() const{
    if(numTrees==-1){return generateSeqDeterministic();}
    myclock::time_point beginning = myclock::now();
    milliseconds ms = duration_cast< milliseconds >(
            system_clock::now().time_since_epoch()
    );
    std::random_device rd;
    std::default_random_engine generator(rd());
    unsigned int len = generateSeqLen();
    vector<int> ans(len);
    myfile << "currlen:" << len << " maxlen:" << maxLen << endl;
    for(int i=0;i<len;++i) {
        ms = duration_cast<milliseconds>(
                system_clock::now().time_since_epoch()
        );
        uniform_int_distribution<int> distribution = uniform_int_distribution<int>(0, int(alphabet.size()) - 1);
        ans[i] = alphabet[distribution(generator)];
        myfile << ans[i] << ",";
    }
    myfile << endl;
    return ans;
}

void ConstructorGenerator::generateTree(){
    Logger& logger = Logger::getLogger();
    logger.setLoggingLevel(Logger::LOG_DEBUG);
    SAFE_DELETE(currTree);
    vector<int> seq = generateSeq();
    constructor.createTree(seq);
    currTree = new ParseTree(constructor.getTree());
}

TreesGenerator* ConstructorGenerator::clone() const{
    return new ConstructorGenerator(*this);
}

void ConstructorGenerator::reset(){
    if(numTrees==-1){
        currLen=1;
        currArr = IndexArray({(int)(alphabet.size())});
    }
    treesGenerated = 0;
}

bool ConstructorGenerator::hasNext() const{
    if(numTrees==-1){
        return currLen<maxLen;
    }
    return treesGenerated<numTrees;
}

ParseTree ConstructorGenerator::operator*(){
    if(!currTree){
        throw std::out_of_range("No current tree");
    }
    return *currTree;
}

ConstructorGenerator& ConstructorGenerator::operator++(){
    Logger& logger = Logger::getLogger();
    logger.setLoggingLevel(Logger::LOG_DEBUG);
    //logger << "inc" << logger.endline;
    if(numTrees==-1){
        currArr++;
        if(currArr.getOverflow()){
            currLen++;
            vector<int> sizes; for(int i=0;i<currLen;++i){sizes.push_back((int)(alphabet.size()));}
            currArr = IndexArray(sizes);
        }

    }else{
        treesGenerated++;
    }
    generateTree();
    //logger << "tree generated" << logger.endline;
}

void ConstructorGenerator::copy(const ConstructorGenerator& other){
    if(other.currTree){
        this->currTree = new ParseTree(*other.currTree);
    }else{
        this->currTree = nullptr;
    }
}

void ConstructorGenerator::clear(){
    SAFE_DELETE(this->currTree);
}