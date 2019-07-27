#include "ObservationTable.h"
#include "ParseTree.h"
#include "MultiplicityTeacher.h"
#include <armadillo>

using namespace std;
using namespace arma;

HankelMatrix::HankelMatrix(const MultiplicityTeacher& teacher):teacher(teacher), obs(){

}

void HankelMatrix::completeTree(ParseTree* tree){
    for(auto it=c.begin();it!=c.end();it++){
        ParseTree* context = *it;
        ParseTree* merged = context->mergeContext(*tree);
        vector<double>& observation = obs[tree];
        while(((int)observation.size())-1<(it-c.begin())){
            observation.push_back(teacher.getDefaultValue());
        }
        obs[tree][it-c.begin()]=teacher.membership(*merged);
        delete(merged);
    }
}

void HankelMatrix::completeContext(ParseTree* context){
    for (auto tree: s) {
        ParseTree *merged = context->mergeContext(*tree);
        obs[tree].push_back(teacher.membership(*merged));
        delete (merged);
    }
}

bool HankelMatrix::checkTableComplete(ParseTree* tree){
    mat sMat(s.size()+1, c.size());
    for(auto it=s.begin();it!=s.end();it++){
        vector<double>& observation = obs[*it];
        for(int i=0;i<c.size();++i){
            sMat[it-s.begin(),i] = observation[i];
        }
    }
    vector<double>& observation = obs[tree];
    for(int i=0;i<c.size();++i){
        sMat[s.size(),i] = observation[i];
    }
    //cout << sMat << endl;
    //printf("rank:%ld\t s.size()=%ld\n",arma::rank(sMat), s.size());
    return arma::rank(sMat)!=s.size()+1;
}

vector<double> HankelMatrix::getObs(const ParseTree& tree) const{
    ParseTree* ptr = nullptr;
    //First look for the tree in the sets s and r
    for(auto t: s){
        if(*t==tree){
            ptr=t;
            break;
        }
    }
    if(!ptr){
        for(auto t: r){
            if(*t==tree){
                ptr=t;
                break;
            }
        }
    }
    if(!ptr){ //The tree not in s and r, need to calculate the observation for it.
        vector<double> ans(c.size());
        for(auto it=c.begin();it!=c.end();it++){
            ParseTree* context = *it;
            ParseTree* merged = context->mergeContext(tree);
            ans[it-c.begin()]=teacher.membership(*merged);
            delete(merged);
        }
        return ans;
    }else{ //Return the pre-calculated value of the tree.
        auto key = obs.find(ptr);
        return key->second;
    }
}

void HankelMatrix::checkTableCompleteContext(ParseTree* newContext){

}
