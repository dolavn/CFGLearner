//
// Created by dolavn on 9/8/19.
//

#ifndef CFGLEARNER_TESTSHELPERFUNCTIONS_H
#define CFGLEARNER_TESTSHELPERFUNCTIONS_H

#include <set>
#include "MultiplicityTeacher.h"

std::set<rankedChar> getAlphabet();
std::set<rankedChar> getAlphabetProb();
std::set<rankedChar> getAlphabetSmall();
std::set<rankedChar> getAlphabetCounting();
MultiplicityTreeAcceptor getCountingAcc();
MultiplicityTreeAcceptor getCountingAcceptor();
SimpleMultiplicityTeacher getMultiplicityTeacher();
SimpleMultiplicityTeacher getMultiplicityProbTeacher();
SimpleMultiplicityTeacher getDistributionTeacher(double);
FunctionalMultiplicityTeacher getFuncTeacherProb();
FunctionalMultiplicityTeacher getFuncTeacherNeg();
FunctionalMultiplicityTeacher getFuncTeacher();

extern rankedChar l;
extern rankedChar a;
extern rankedChar b;
extern rankedChar inner;
extern rankedChar l1;
extern rankedChar l2;

#endif //CFGLEARNER_TESTSHELPERFUNCTIONS_H
