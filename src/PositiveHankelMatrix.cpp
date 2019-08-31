#include "ObservationTable.h"
#include "ConicCombinationFinder.h"
#include "MultiplicityTeacher.h"

using namespace std;
using namespace arma;

PositiveHankelMatrix::PositiveHankelMatrix(const MultiplicityTeacher& teacher):HankelMatrix(teacher){
    if(teacher.getDefaultValue()<0){
        throw std::invalid_argument("Default value must be positive for positive Hankel Matrix");
    }
}

void PositiveHankelMatrix::completeTree(ParseTree* tree){
    obs[tree] = vector<double>();
    for(auto it=c.begin();it!=c.end();it++){
        ParseTree* context = *it;
        ParseTree* merged = context->mergeContext(*tree);
        vector<double>& observation = obs[tree];
        while(((int)observation.size())-1<(it-c.begin())){
            observation.push_back(teacher.getDefaultValue());
        }
        double val;
        if((val=teacher.membership(*merged))<0){
            delete(merged);
            throw std::invalid_argument("Must return positive value for positive Hankel Matrix");
        }
        obs[tree][it-c.begin()]=val;
        delete(merged);
    }
}

void PositiveHankelMatrix::completeContextR(ParseTree* context){
    for (auto tree: s) {
        ParseTree *merged = context->mergeContext(*tree);
        double val;
        if((val=teacher.membership(*merged))<0){
            throw std::invalid_argument("Must return positive value for positive Hankel Matrix");
        }
        obs[tree].push_back(val);
        delete (merged);
    }
}

void PositiveHankelMatrix::completeContextS(ParseTree* context){
    for (auto tree: s) {
        ParseTree *merged = context->mergeContext(*tree);
        if(teacher.membership(*merged)<0){
            throw std::invalid_argument("Must return positive value for positive Hankel Matrix");
        }
        double val;
        if((val=teacher.membership(*merged))<0){
            throw std::invalid_argument("Must return positive value for positive Hankel Matrix");
        }
        obs[tree].push_back(val);
        delete (merged);
    }
}

bool PositiveHankelMatrix::checkTableComplete(ParseTree* tree){
    mat sMat = getSMatrix(true);
    fillMatLastRow(sMat, tree);
    inplace_trans(sMat);
    ConicCombinationFinder c(sMat);
    c.solve((int)(sMat.n_cols)-1);
    return c.getStatus()==ConicCombinationFinder::SOLVED;
}