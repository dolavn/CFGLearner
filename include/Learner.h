//
// Created by dolavn@wincs.cs.bgu.ac.il on 1/10/19.
//

#ifndef CFGLEARNER_LEARNER_H
#define CFGLEARNER_LEARNER_H

#include "TreeAcceptor.h"

class Teacher;

TreeAcceptor learn(const Teacher&);

#endif //CFGLEARNER_LEARNER_H
