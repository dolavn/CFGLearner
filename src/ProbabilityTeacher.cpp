#include "MultiplicityTeacher.h"
#include "TreeComparator.h"
#include "ParseTree.h"
#include "MultiplicityTreeAcceptor.h"
#include <functional>
#include "Logger.h"
#include "math.h"

using namespace std;

ProbabilityTeacher::ProbabilityTeacher(TreeComparator& cmp, double decayFactor, double epsilon):
ComparatorTeacher(cmp, epsilon),decayFactor(decayFactor),countingFunc(){

}

ProbabilityTeacher::ProbabilityTeacher(TreeComparator& cmp, double decayFactor, countingFunction countingFunc,
        double epsilon):ComparatorTeacher(cmp, epsilon), decayFactor(decayFactor),countingFunc(move(countingFunc)){
    if(decayFactor>=1 || decayFactor<=0){
        throw invalid_argument("Decay factor must be between 0 and 1");
    }
}

ProbabilityTeacher::ProbabilityTeacher(const ProbabilityTeacher& other):ComparatorTeacher(other),
decayFactor(other.decayFactor),countingFunc(other.countingFunc){

}

ProbabilityTeacher::ProbabilityTeacher(ProbabilityTeacher&& other):ComparatorTeacher(move(other)),
decayFactor(other.decayFactor),countingFunc(move(other.countingFunc)){

}

double ProbabilityTeacher::calcNewProb(const ParseTree& tree1, const ParseTree& tree2, TreeComparator& cmp) const{
    float result = cmp.compare(tree2, tree1);
    int numTreesWithResult = countingFunc?countingFunc(result, tree2.getLeavesNum()):1;
    double p = tree2.getProb();
    p = p*std::pow(decayFactor, result);
    p = p / numTreesWithResult;
    return p;
}

double ProbabilityTeacher::membership(const ParseTree& tree) const{
    double ans = ComparatorTeacher::membership(tree);
    return ans*(1-decayFactor);
}


//TODO: Ugly design. Fix this.
ParseTree* ProbabilityTeacher::equivalence(const MultiplicityTreeAcceptor& acc) const{
    Logger& logger = Logger::getLogger();
    std::function<bool(const ParseTree&, bool)> testFunc = [&acc,this](const ParseTree& t1, bool cache){
        double val = acc.run(t1);
        double prob = t1.getProb();
        if(!cache){
            prob = prob*(1-decayFactor);
        }
        return std::abs(val-prob)>epsilon;
    };
    for(auto& currTree: trees){
        if(testFunc(*currTree, false)){
            return new ParseTree(*currTree);
        }
    }
    for(auto& currTree: cache){
        if(testFunc(*currTree, true)){
            return new ParseTree(*currTree);
        }
    }
    return nullptr;
}
