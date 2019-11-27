#include "MultiplicityTeacher.h"
#include "MultiplicityTreeAcceptor.h"
#include "ParseTree.h"
#include "utility.h"
#include "Logger.h"
#include <sstream>
#include <iostream> //todo:delete

using namespace std;

ostream& operator<<(ostream& output, const MultiplicityTeacher& teacher){
    output << teacher.toString();
    return output;
}

SimpleMultiplicityTeacher::SimpleMultiplicityTeacher(double epsilon, double defaultVal):epsilon(epsilon),
defaultValue(defaultVal),alphabet(), trees(){

}

SimpleMultiplicityTeacher::SimpleMultiplicityTeacher(const SimpleMultiplicityTeacher& other):epsilon(other.epsilon),
defaultValue(other.defaultValue),alphabet(other.alphabet),trees(){
    copy(other);
}

SimpleMultiplicityTeacher& SimpleMultiplicityTeacher::operator=(const SimpleMultiplicityTeacher& other) {
    if(this==&other){
        return *this;
    }
    clear();
    epsilon = other.epsilon;
    defaultValue = other.defaultValue;
    alphabet = other.alphabet;
    copy(other);
    return *this;
}

SimpleMultiplicityTeacher::SimpleMultiplicityTeacher(SimpleMultiplicityTeacher&& other):epsilon(other.epsilon),
defaultValue(other.defaultValue),alphabet(std::move(other.alphabet)),trees(std::move(other.trees)){

}

SimpleMultiplicityTeacher& SimpleMultiplicityTeacher::operator=(SimpleMultiplicityTeacher&& other) {
    if(this==&other){
        return *this;
    }
    clear();
    epsilon = other.epsilon;
    defaultValue = other.defaultValue;
    trees = std::move(other.trees);
    alphabet = std::move(other.alphabet);
    return *this;
}

void SimpleMultiplicityTeacher::addExample(const ParseTree& t){
    for(auto currTree: trees){
        if(*currTree == t){
            throw std::invalid_argument("Tree already in data");
        }
    }
    auto tNew = new ParseTree(t);
    trees.push_back(tNew);
    for(auto it=t.getIndexIterator();it.hasNext();++it){
        int c = (*tNew)[*it].getData();
        int rank = (int)((*tNew)[*it].getSubtrees().size());
        rankedChar newC = {c, rank};
        alphabet.insert(newC);
    }
}

double SimpleMultiplicityTeacher::membership(const ParseTree& tree) const{
    for(ParseTree* example: trees){
        if(*example==tree){
            return example->getProb();
        }
    }
    return defaultValue;
}

ParseTree* SimpleMultiplicityTeacher::equivalence(const MultiplicityTreeAcceptor& acc) const{
    ParseTree* ans = nullptr;
    Logger& logger = Logger::getLogger();
    int wrong = 0;
    for(auto& tree: trees){
        double calculatedValue = acc.run(*tree);
        if(ABS(calculatedValue-tree->getProb())>epsilon){
            wrong = wrong+1;
            if(!ans){
                logger.setLoggingLevel(Logger::LOG_DEBUG);
                logger << "accVal:" << calculatedValue << logger.endline;
                //logger << "prob:" << tree->getProb() << logger;
                ans = new ParseTree(*tree);
            }
        }
    }
    error = wrong/(int)(trees.size());
    return ans;
}

void SimpleMultiplicityTeacher::copy(const SimpleMultiplicityTeacher& other){
    for(auto& tree: other.trees){
        trees.push_back(new ParseTree(*tree));
    }
}

void SimpleMultiplicityTeacher::clear(){
    for(auto& tree: trees){
        if(tree){
            delete(tree);
            tree = nullptr;
        }
    }
    trees.clear();
}

string MultiplicityTeacher::toString() const{
    return "MultiplicityTeacher";
}

void MultiplicityTeacher::printDesc() const{
    cout << "MultiplicityTeacher" << endl;
}

string SimpleMultiplicityTeacher::toString() const{
    stringstream stream;
    stream << "SimpleMultiplicityTeacher eps=" << epsilon << " examplesNum:" << trees.size();
    return stream.str();
}

void SimpleMultiplicityTeacher::printDesc() const{
    cout << "SimpleMultiplicityTeacher eps=" << epsilon << " examplesNum:" << trees.size() << endl;
    for(auto tree: trees){
        cout << *tree << " - " << tree->getProb() << endl;
    }
}

SimpleMultiplicityTeacher::~SimpleMultiplicityTeacher(){
    clear();
}