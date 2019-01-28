#include "Learner.h"
#include "ParseTree.h"
#include "Teacher.h"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <set>

using namespace std;

inline void vectorClear(vector<ParseTree*>& vec){
    for(auto& t:vec){
        if(t){
            delete(t);
            t=nullptr;
        }
    }
}

struct observationTable{
private:
    const Teacher& teacher;
    vector<ParseTree*> s;
    vector<ParseTree*> r;
    vector<ParseTree*> c;
    unordered_map<ParseTree*,vector<bool>> obs;
    void completeTree(ParseTree* tree){
        for(auto it=c.begin();it!=c.end();it++){
            ParseTree* context = *it;
            ParseTree* merged = context->mergeContext(*tree);
            vector<bool>& observation = obs[tree];
            while(((int)observation.size())-1<(it-c.begin())){
                observation.push_back(false);
            }
            obs[tree][it-c.begin()]=teacher.membership(*merged);
            delete(merged);
        }
    }

    void clear(){
        vectorClear(s);
        vectorClear(r);
        vectorClear(c);
    }
public:
    explicit observationTable(const Teacher& teacher):teacher(teacher),s(),r(),c(),obs(){}
    observationTable(const observationTable& other)=delete;
    observationTable& operator=(const observationTable& other)=delete;
    observationTable(observationTable&& other)=delete;
    observationTable& operator=(observationTable&& other)=delete;
    ~observationTable(){
        clear();
    }
    void addTree(const ParseTree& tree){
        if(tree.getIsContext()){
            throw invalid_argument("Can't add a context to S");
        }
        auto newTree = new ParseTree(tree);
        completeTree(newTree);
        bool isComplete = false;
        for(auto sTree: s){ //check if obs(tree) is already covered by S.
            if(obs[sTree]==obs[newTree]){
                isComplete = true;
                break;
            }
        }
        if(!isComplete){
            s.push_back(newTree);
        }else {
            r.push_back(newTree);
        }
    }
    void addContext(const ParseTree& context) {
        if (!context.getIsContext()) {
            throw invalid_argument("Tree must be a context to be added to C");
        }
        auto newContext = new ParseTree(context);
        c.push_back(newContext);
        for (auto tree: s) {
            ParseTree *merged = newContext->mergeContext(*tree);
            obs[tree][c.size() - 1] = teacher.membership(*merged);
            delete (merged);
        }
        for (auto tree: r) {
            ParseTree *merged = newContext->mergeContext(*tree);
            obs[tree][c.size() - 1] = teacher.membership(*merged);
            delete (merged);
        }
    }
    vector<bool> getObs(const ParseTree& tree){
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
            vector<bool> ans(c.size());
            for(auto it=c.begin();it!=c.end();it++){
                ParseTree* context = *it;
                ParseTree* merged = context->mergeContext(tree);
                ans[it-c.begin()]=teacher.membership(*merged);
                delete(merged);
            }
            return ans;
        }else{ //Return the pre-calculated value of the tree.
            return obs[ptr];
        }
    }
    int getSObsInd(const ParseTree& tree){
        for(auto it=s.begin();it!=s.end();it++){
            if(obs[*it]==getObs(tree)){
                return (int)(it-s.begin());
            }
        }
        return -1;
    }
    inline const vector<ParseTree*>& getR(){return r;}
    inline const vector<ParseTree*>& getS(){return s;}
    inline const vector<ParseTree*>& getC(){return c;}
};

typedef vector<vector<ParseTree>> setsVec;

set<rankedChar> getAlphabet(observationTable& s){
    set<rankedChar> alphabet;
    for(auto t:s.getS()){
        for(auto it=t->getIndexIterator();it.hasNext();++it){
            int value = (*t)[*it].getData();
            int sign = 0;
            sign = sign + (*t)[*it].hasLeftSubtree()?1:0;
            sign = sign + (*t)[*it].hasRightSubtree()?1:0;
            alphabet.insert({value,sign});
        }
    }
    for(auto t:s.getR()){
        for(auto it=t->getIndexIterator();it.hasNext();++it){
            int value = (*t)[*it].getData();
            int sign = 0;
            sign = sign + (*t)[*it].hasLeftSubtree()?1:0;
            sign = sign + (*t)[*it].hasRightSubtree()?1:0;
            alphabet.insert({value,sign});
        }
    }
    return alphabet;
}

void addTransition(observationTable& s, TreeAcceptor& acc, const ParseTree& tree){
    int value = tree.getData();
    vector<int> states;
    if(tree.hasLeftSubtree()){
        states.push_back(s.getSObsInd(tree.getNode("0")));
    }
    if(tree.hasRightSubtree()){
        states.push_back(s.getSObsInd(tree.getNode("1")));
    }
    int targetState = s.getSObsInd(tree);
    rankedChar c{value,(int)(states.size())};
    string vecStr = "[";
    for(auto state: states){
        vecStr = vecStr + (char)(state+'0') + " , ";
    }
    vecStr = vecStr + "]";
    std::cerr << vecStr << " - " << value << " - " << targetState << std::endl;
    acc.addTransition(states,c,targetState);
}

TreeAcceptor synthesize(observationTable& s, const Teacher& teacher){
    set<rankedChar> alphabet = getAlphabet(s);
    TreeAcceptor ans(alphabet,(int)(s.getS().size()));
    for(auto tree: s.getR()){
        for(auto it = tree->getIndexIterator();it.hasNext();++it){
            string str = *it;
            addTransition(s,ans,(*tree)[str]);
        }
    }
    for(auto tree: s.getS()){
        for(auto it = tree->getIndexIterator();it.hasNext();++it){
            string str = *it;
            addTransition(s,ans,(*tree)[str]);
        }
    }
    for(auto it=s.getS().begin();it!=s.getS().end();it++){
        if(teacher.membership(**it)){
            ans.setAccepting((int)(it-s.getS().begin()),true);
        }
    }
    return ans;
}

void extend(observationTable& s, ParseTree* t){
}

TreeAcceptor learn(const Teacher& teacher){
    observationTable table(teacher);
    TreeAcceptor ans(set<rankedChar>{{2,0}});
    for(;;){
        ans = synthesize(table,teacher);
        ParseTree* counterExample = teacher.equivalence(ans);
        if(!counterExample){
            break;
        }
        extend(table,counterExample);
    }
    return ans;
}

observationTable* table=nullptr;
const Teacher* t = nullptr;

void initLearner(const Teacher& teacher){
    clearLearner();
    table = new observationTable(teacher);
    t = &teacher;
}

void clearLearner(){
    if(table){
        delete(table);
        table=nullptr;
    }
}

void learnerAddTree(const ParseTree& tree){
    table->addTree(tree);
}

void learnerAddContext(const ParseTree& tree){
    table->addContext(tree);
}

vector<bool> learnerGetObs(const ParseTree& tree){
    return table->getObs(tree);
}

TreeAcceptor learnerSynthesize(){
    return synthesize(*table,*t);
}