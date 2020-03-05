#include "MultiplicityTeacher.h"
#include "TreeComparator.h"
#include "ParseTree.h"
#include "MultiplicityTreeAcceptor.h"
#include "Logger.h"
#include <math.h>
#include <functional>
#include <iostream>


DifferenceMultiplicityTeacher::DifferenceMultiplicityTeacher(TreeComparator& cmp, int maxDiff, double decayFactor,
        double epsilon):ComparatorTeacher(cmp, epsilon),maxDiff(maxDiff),decayFactor(decayFactor){

}

DifferenceMultiplicityTeacher::DifferenceMultiplicityTeacher(const DifferenceMultiplicityTeacher& other):
ComparatorTeacher(other),maxDiff(other.maxDiff),decayFactor(other.decayFactor){

}

DifferenceMultiplicityTeacher::DifferenceMultiplicityTeacher(DifferenceMultiplicityTeacher&& other):
ComparatorTeacher(std::move(other)),maxDiff(other.maxDiff),decayFactor(other.decayFactor){

}

double DifferenceMultiplicityTeacher::calcNewProb(const ParseTree& tree1, const ParseTree& tree2,
        TreeComparator& cmp) const{
    double ans;
    float diff = cmp.compare(tree2, tree1);
    double p = tree2.getProb();
    ans = p-std::pow(decayFactor, diff);
    return ans;
}
