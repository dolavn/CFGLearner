#include "MultiplicityTreeAcceptor.h"
#include "IndexArray.h"
#include <math.h>

using namespace std;


template<class T>
void printVec(const vector<T>& vec){ //TODO: Delete
    cout << "[";
    for(int k=0;k<vec.size();k++){
        cout << vec[k];
        if(k<vec.size()-1){
            cout << " , ";
        }
    }
    cout << "]" << endl;
}


MultiplicityTreeAcceptor::MultiplicityTreeAcceptor(const set<rankedChar>& alphabet, int dim):
alphabet(alphabet),dim(dim),transitions(),lambda(){

}

void MultiplicityTreeAcceptor::addTransition(const MultiLinearMap& map, const rankedChar& c){
    if(!testMap(map, c)){
        throw invalid_argument("Map dimensions do not fit character dimensions");
    }
    transitions[c] = map;
}

void MultiplicityTreeAcceptor::setLambda(const floatVec& v){
    lambda = v;
}

float MultiplicityTreeAcceptor::run(const ParseTree& tree) const{
    ParseTree runTree = tree.getSkeleton();
    stack<vector<int>> stack;
    unordered_map<vector<int>,floatVec, MultiplicityTreeAcceptor::container_hash<vector<int>>> vecMap;
    for(auto it = runTree.getLeafIterator();it.hasNext();++it){
        stack.push(*it);
    }
    while(!stack.empty()){
        vector<int> nodeInd = stack.top();
        stack.pop();
        const ParseTree& node = tree.getNode(nodeInd);
        vector<floatVec> params;
        int rank = 0;
        for(int i=0;i<runTree[nodeInd].getChildrenNum();++i){
            if(node.hasSubtree(i)){
                nodeInd.push_back(i);
                params.push_back(vecMap[nodeInd]);
                nodeInd.pop_back();
                rank++;
            }
        }
        rankedChar c = {node.getData(), rank};
        if(transitions.find(c) == transitions.end()){ //no transition
            return 0;
        }
        auto it = transitions.find(c);
        MultiLinearMap m;
        m = it->second;
        vecMap[nodeInd] = m(params);
        if(nodeInd.empty()){break;}
        vector<int> fatherInd = vector<int>(nodeInd.begin(),nodeInd.begin()+nodeInd.size()-1);
        ParseTree& fatherNode = runTree[fatherInd];
        bool climb=true;
        for(int i=0;i<fatherNode.getChildrenNum();++i){
            fatherInd.push_back(i);
            if(fatherNode.hasSubtree(i) && vecMap.find(fatherInd)==vecMap.end()){climb=false;break;}
            fatherInd.pop_back();
        }
        if(climb) {
            stack.push(fatherInd);
        }
    }
    float ans = 0;
    floatVec root = vecMap[{}];
    for(unsigned int i=0;i<root.size();++i){
        ans = ans + root[i]*lambda[i];
    }
    return ans;
}

bool MultiplicityTreeAcceptor::checkIsPositive() const{
    for(auto elem: lambda){
        if(elem<0){return false;}
    }
    for(auto c: alphabet){
        MultiLinearMap m = transitions.find(c)->second;
        intVec maxLengths(c.rank+1, dim);
        for(IndexArray ind(maxLengths);!ind.getOverflow();++ind){
            if(m.getParam(ind.getIntVector())<0){return false;}
        }
    }
    return true;
}

vector<float> MultiplicityTreeAcceptor::getParamSums(bool softmax) const{
    vector<float> sums;
    sums.push_back(0);
    for(auto elem: lambda){
        sums[0] = sums[0] + (softmax?exp(elem):elem);
    }
    for(int state=0;state<dim;++state){
        float sum=0;
        for(auto c: alphabet){
            MultiLinearMap m = transitions.find(c)->second;
            intVec maxLengths(c.rank, dim);
            intVec currVec(c.rank+1); currVec[0]=state;
            for(IndexArray ind(maxLengths);!ind.getOverflow();++ind){
                for(int i=0;i<c.rank;++i){
                    currVec[i+1]=ind.get(i);
                }
                sum = sum + (softmax?exp(m.getParam(currVec)):m.getParam(currVec));
            }
        }
        sums.push_back(sum);
    }
    return sums;
}

MultiplicityTreeAcceptor MultiplicityTreeAcceptor::getNormalizedAcceptor(bool softmax) const{
    MultiplicityTreeAcceptor ans(*this);
    if(!softmax && !checkIsPositive()){
        throw std::invalid_argument("Tree acceptor must be positive to normalize, unless using softmax");
    }
    vector<float> sums = getParamSums(softmax);
    for(auto& elem: ans.lambda){elem=(softmax?exp(elem):elem)/sums[0];}
    for(auto c: alphabet){
        MultiLinearMap& m = ans.transitions[c];
        intVec maxLengths(c.rank+1, dim);
        for(IndexArray ind(maxLengths);!ind.getOverflow();++ind){
            intVec currInd = ind.getIntVector();
            float elem = m.getParam(currInd);
            if(sums[currInd[0]+1]==0){
                m.setParam(0, currInd);
                continue;
            }
            m.setParam((softmax?exp(elem):elem)/sums[currInd[0]+1],currInd);
        }
    }
    return ans;
}

MultiplicityTreeAcceptor MultiplicityTreeAcceptor::getAcceptorWithoutState(int state) const{
    if(isStateReachable(state)){
        throw std::invalid_argument("Can't remove a reachable state");
    }
    MultiplicityTreeAcceptor ans(alphabet, dim-1);
    for(auto c: alphabet){
        const MultiLinearMap& m = (*transitions.find(c)).second;
        MultiLinearMap mNew = m.removeDimension(state);
        ans.addTransition(mNew, c);
    }
    vector<float> newLambda;
    for(int i=0;i<dim;++i){if(i!=state){newLambda.push_back(lambda[i]);}}
    ans.setLambda(newLambda);
    return ans;
}

bool MultiplicityTreeAcceptor::isStateReachable(int state) const{
    return false;
}

MultiLinearMap MultiplicityTreeAcceptor::getMap(rankedChar c) const{
    auto it = transitions.find(c);
    if(it==transitions.end()){
        throw invalid_argument("No transition for given character");
    }
    return it->second;
}


bool MultiplicityTreeAcceptor::testMap(const MultiLinearMap& map, const rankedChar& c){
    return map.getParamNum()==c.rank;
}

vector<rankedChar> MultiplicityTreeAcceptor::getAlphabet() const{
    vector<rankedChar> ans;
    for(auto rc: alphabet){
        ans.push_back(rc);
    }
    return ans;
}

vector<rankedChar> MultiplicityTreeAcceptor::getAlphabet(int rank) const{
    vector<rankedChar> ans;
    for(auto rc: alphabet){
        if(rc.rank==rank){
            ans.push_back(rc);
        }
    }
    return ans;
}

vector<int> MultiplicityTreeAcceptor::getRanks() const{
    set<int> ranks;
    vector<int> ans;
    for(auto rc: alphabet){
        ranks.insert(rc.rank);
    }
    for(auto r: ranks){
        ans.push_back(r);
    }
    return ans;
}


void MultiplicityTreeAcceptor::printDesc() const {
    cout << "acceptor" << endl;
    for(auto tPair: transitions){
        cout << tPair.first.c << " , " << tPair.first.rank << endl;
        tPair.second.printDesc();
    }
    cout << "lambda [";
    for(unsigned int i=0;i<lambda.size();++i){
        cout << lambda[i];
        if(i<lambda.size()-1){
            cout << ",";
        }
    }
    cout << "]" << endl;
}