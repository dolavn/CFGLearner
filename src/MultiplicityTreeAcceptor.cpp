#include "MultiplicityTreeAcceptor.h"

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
    for(int i=0;i<root.size();++i){
        ans = ans + root[i]*lambda[i];
    }
    return ans;
}


bool MultiplicityTreeAcceptor::testMap(const MultiLinearMap& map, const rankedChar& c){
    return map.getParamNum()==c.rank;
}