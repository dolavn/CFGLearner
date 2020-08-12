#include "ObservationTable.h"
#include "ConicCombinationFinder.h"
#include "MultiplicityTeacher.h"
#include "MultiplicityTreeAcceptor.h"
#include "Logger.h"
#include "utility.h"
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
        if(arma::rank(currMat)<=1){
            return true;
        }
    }
    return false;
}

void ScalarHankelMatrix::giveCounterExample(const ParseTree& counterExample){
    for(auto& context: counterExample.getAllContexts()){
        if(!hasContext(*context)){
            addContext(*context);
        }
        SAFE_DELETE(context)
    }
}

double ScalarHankelMatrix::getCoeff(const ParseTree& t1, const ParseTree& t2) const{
    vector<double> row1 = getRow(t1);
    vector<double> row2 = getRow(t2);
    for(int i=0;i<row1.size();++i){
        if(row2[i]!=0){
            return row1[i]/row2[i];
        }
    }
    return 0;
}

vector<double> ScalarHankelMatrix::getRow(const ParseTree& tree) const{
    for(auto treePtr: s){
        if(*treePtr==tree){
            auto it = obs.find(treePtr);
            if(it==obs.end()){
                throw std::invalid_argument("tree not in map");
            }
            return it->second;
        }
    }
    for(auto treePtr: r){
        if(*treePtr==tree){
            auto it = obs.find(treePtr);
            if(it==obs.end()){
                throw std::invalid_argument("tree not in map");
            }
            return it->second;
        }
    }
    throw std::invalid_argument("Tree not in map");
}


void ScalarHankelMatrix::makeConsistent(){
    Logger& logger = Logger::getLogger();
    closeTable();
    logger.setLoggingLevel(Logger::LOG_DETAILS);
    logger << "consistent" << logger.endline;
    for(int i=0;i<s.size();++i){
        vector<const ParseTree*> treesInClass = getTreesInClass(i);
        vector<pair<const ParseTree*, const ParseTree*>> pairs = getPairsVec(treesInClass);
        for(auto treePair: pairs){
            double alpha = getCoeff(*treePair.first, *treePair.second);
            vector<pair<ParseTree, int>> extensions1 = getExtensions(*treePair.first);
            vector<pair<ParseTree, int>> extensions2 = getExtensions(*treePair.second);
            for(int j=0;j<extensions1.size();++j){
                for(auto context: c){
                    ParseTree* mergedContext1 = context->mergeContext(extensions1[j].first);
                    ParseTree* mergedContext2 = context->mergeContext(extensions2[j].first);
                    if(ABS(teacher.membership(*mergedContext1)/teacher.membership(*mergedContext2)-alpha)<EPSILON){
                        auto contextTreePair = extensions1[j].first.makeContext({extensions1[j].second});
                        auto newContext = context->mergeContext(*contextTreePair.first);
                        addContext(*newContext);
                        SAFE_DELETE(contextTreePair.first); SAFE_DELETE(contextTreePair.second);
                        SAFE_DELETE(newContext);
                    }
                    SAFE_DELETE(mergedContext1); SAFE_DELETE(mergedContext2);
                }
            }
        }
    }
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

bool isZeroVec(vector<double>& vec){
    for(auto elem: vec){if(elem!=0){return false;}}
    return true;
}

int ScalarHankelMatrix::getSObsInd(const ParseTree& tree) const{
    mat sMat = getSMatrix(false);
    mat currMat(2, c.size());
    vector<double> observation = getObs(tree);
    bool zeroVec = isZeroVec(observation);
    for(int i=0;i<c.size();++i){
        currMat(0,i) = observation[i];
    }
    for(int i=0; i<sMat.n_rows;++i){
        bool currRowIsZero=true;
        for(int j=0;j<c.size();++j){
            if(sMat(i,j)!=0){currRowIsZero=false;}
            currMat(1,j) = sMat(i,j);
        }
        if(zeroVec&&currRowIsZero){return i;}
        if(arma::rank(currMat)==1){
            return i;
        }
    }
    throw std::invalid_argument("Shouldn't happen");
}

vector<const ParseTree*> ScalarHankelMatrix::getTreesInClass(int ind) const{
    vector<const ParseTree*> ans;
    for(auto t: s){
        if(getSObsInd(*t)==ind){ans.emplace_back(t);}
    }
    for(auto t: r){
        if(getSObsInd(*t)==ind){ans.emplace_back(t);}
    }
    return ans;
}

vector<pair<const ParseTree*, const ParseTree*>> ScalarHankelMatrix::getPairsVec(vector<const ParseTree*> vec) const{
    vector<pair<const ParseTree*, const ParseTree*>> ans;

    for(int i=0;i<vec.size();++i){
        for(int j=0;j<vec.size();++j){
            if(i==j){continue;}
            ans.emplace_back(vec[i], vec[j]);
        }
    }

    return ans;
}

vector<pair<ParseTree, int>> ScalarHankelMatrix::getExtensions(const ParseTree& tree) const{
    vector<ParseTree> trees;
    for(auto tree: s){
        trees.emplace_back(*tree);
    }
    for(auto tree: r){
        trees.emplace_back(*tree);
    }
    return extendSet(tree, trees, alphabet);
}


vector<pair<ParseTree, int>> extendSet(const ParseTree& tree, vector<ParseTree> treeSet, set<rankedChar> alphabet){
    vector<pair<ParseTree,int>> ans;
    for(auto c: alphabet){
        if(c.rank==0){continue;}
        IndexArray arr(vector<int>(c.rank-1, treeSet.size()));
        while(!arr.getOverflow()){
            for(int k=0;k<c.rank;++k){
                vector<ParseTree> children;
                ParseTree currTree(c.c);
                currTree.setSubtree(tree, k);
                for(int currInd=0;currInd<c.rank-1;++currInd){
                    int treeIndex = arr[currInd];
                    ParseTree& child = treeSet[treeIndex];
                    currTree.setSubtree(child, currInd+(currInd>=k?1:0));
                }
                ans.emplace_back(currTree, k);
            }
            ++arr;
        }
    }
    return ans;
}