//
// Created by dolavn@wincs.cs.bgu.ac.il on 1/10/19.
//

#ifndef CFGLEARNER_LEARNER_H
#define CFGLEARNER_LEARNER_H

#include <vector>
#include "TreeAcceptor.h"

class Teacher;

TreeAcceptor learn(const Teacher&);


//Methods for testing here.
//TODO: delete
void initLearner(const Teacher&);
void learnerAddTree(const ParseTree&);
void learnerAddContext(const ParseTree&);
std::vector<bool> learnerGetObs(const ParseTree&);
void clearLearner();
TreeAcceptor learnerSynthesize();


#endif //CFGLEARNER_LEARNER_H
