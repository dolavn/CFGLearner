#include "MultiplicityTeacher.h"
#include "TreeComparator.h"
#include <functional>
#include <math>

using namespace std;

ProbabilityTeacher::ProbabilityTeacher(TreeComparator& cmp, double decayFactor, function<int(float)> countingFunc,
        double epsilon):ComparatorTeacher(cmp, epsilon), decayFactor(decayFactor),countingFunc(move(countingFunc)){

}

ProbabilityTeacher::ProbabilityTeacher(const ProbabilityTeacher& other):ComparatorTeacher(other),
decayFactor(other.decayFactor),countingFunc(other.countingFunc){

}

ProbabilityTeacher::ProbabilityTeacher(ProbabilityTeacher&& other):ComparatorTeacher(move(other)),
decayFactor(other.decayFactor),countingFunc(move(other.countingFunc)){

}

double ProbabilityTeacher::calcNewProb(const ParseTree& tree1, const ParseTree& tree2, TreeComparator& cmp) const{
    float result = cmp.compare(tree2, tree1);
    int numTreesWithResult = countingFunc(result);
    double p = tree2.getProb();
    p = p*std::pow(decayFactor, result);
    p = p / numTreesWithResult;
    return p;
}