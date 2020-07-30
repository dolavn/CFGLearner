#include "ObservationTable.h"
#include "ConicCombinationFinder.h"
#include "MultiplicityTeacher.h"
#include "Logger.h"
using namespace std;
using namespace arma;

#define EPSILON (0.000001)

ScalarHankelMatrix::ScalarHankelMatrix(const MultiplicityTeacher& teacher):HankelMatrix(teacher){
    if(teacher.getDefaultValue()<0){
        throw std::invalid_argument("Default value must be positive for positive Hankel Matrix");
    }
    Logger& logger = Logger::getLogger();
    logger.setLoggingLevel(Logger::LOG_DEBUG);
    logger << "Matrix Created";
}

void ScalarHankelMatrix::completeTree(ParseTree* tree){
    Logger& logger = Logger::getLogger();
    logger.setLoggingLevel(Logger::LOG_DEBUG);
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
    logger << "finish complete" << logger.endline;
}

void ScalarHankelMatrix::completeContextR(ParseTree* context){
    auto it = r.begin();
    while(it!=r.end()){
        auto tree = *it;
        ParseTree *merged = context->mergeContext(*tree);
        double val;
        if((val=teacher.membership(*merged))<0){
            throw std::invalid_argument("Must return positive value for positive Hankel Matrix");
        }
        obs[tree].push_back(val);
        if(!checkTableComplete(tree)){
            auto itNew = rNew.begin();
            while(itNew!=rNew.end()){ //Remove tree if from rNew list if it's there.
                int& ind = *itNew;
                if(ind==(int)(it-r.begin())){
                    rNew.erase(itNew);
                    break;
                }
                itNew++;
            }
            sNew.push_back((int)(s.size()));
            s.push_back(tree);
            r.erase(it); //Remove tree from r list.
        }else{
            it++;
        }
        delete (merged);
    }
}

void ScalarHankelMatrix::completeContextS(ParseTree* context){
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

bool ScalarHankelMatrix::checkTableComplete(ParseTree* tree){
    mat sMat = getSMatrix(false);
    mat currMat(2, c.size());
    vector<double> observation = getObs(*tree);
    for(int i=0;i<c.size();++i){
        currMat(0,i) = observation[i];
    }
    for(int i=0; i<sMat.n_rows;++i){
        for(int j=0;j<c.size();++j){
            currMat(1,j) = sMat(i,j);
        }
        if(arma::rank(currMat)==1){
            return true;
        }
    }
    return false;
}

arma::vec ScalarHankelMatrix::getCoefficients(const ParseTree& tree, const arma::mat& s) const{ //CHANGE
    mat sMat = getSMatrix(false);
    mat currMat(2, c.size());
    vector<double> observation = getObs(tree);
    arma::vec ans = arma::zeros(this->s.size());
    for(int i=0;i<c.size();++i){
        currMat(0,i) = observation[i];
    }
    for(int i=0; i<sMat.n_rows;++i){
        for(int j=0;j<c.size();++j){
            currMat(1,j) = sMat(i,j);
        }
        if(arma::rank(currMat)==1){
            for(int j=0;j<c.size();++j){
                if(observation[j]!=0){
                    double factor = observation[j]/currMat(1,j);
                    ans[i] = factor;
                    break;
                }
            }
        }
    }
    return ans;
}