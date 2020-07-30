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
FunctionalMultiplicityTeacher getFuncTeacherProb2();
FunctionalMultiplicityTeacher getFuncTeacherNeg();
FunctionalMultiplicityTeacher getFuncTeacher();


extern rankedChar l;
extern rankedChar a;
extern rankedChar b;
extern rankedChar inner;
extern rankedChar l1;
extern rankedChar l2;


#define TEST_VEC(it, vec, ind) \
        ASSERT_TRUE(it.hasNext()); \
        for(ind=0;it.hasNext();++it){ \
            ASSERT_TRUE(ind<vec.size()); \
            ASSERT_EQ(*it, vec[ind++]); \
        } \
        ASSERT_EQ(ind, vec.size()); \
        ASSERT_FALSE(it.hasNext())


#endif //CFGLEARNER_TESTSHELPERFUNCTIONS_H
