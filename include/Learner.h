//
// Created by dolavn@wincs.cs.bgu.ac.il on 1/10/19.
//

#ifndef CFGLEARNER_LEARNER_H
#define CFGLEARNER_LEARNER_H

#include <vector>
#include "TreeAcceptor.h"
#include "MultiplicityTreeAcceptor.h"

class Teacher;
class MultiplicityTeacher;

TreeAcceptor learn(const Teacher&);
MultiplicityTreeAcceptor learn(const MultiplicityTeacher&);

//Methods for testing here.
//TODO: delete
void initLearner(const Teacher&);
void learnerAddTree(const ParseTree&);
void learnerAddContext(const ParseTree&);
void learnerExtend(const ParseTree&);
std::vector<bool> learnerGetObs(const ParseTree&);
void clearLearner();
TreeAcceptor learnerSynthesize();
std::pair<ParseTree*,ParseTree*> learnerDecompose(ParseTree&);

#endif //CFGLEARNER_LEARNER_H
