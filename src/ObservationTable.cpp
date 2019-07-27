
#include "ObservationTable.h"
#include "Teacher.h"
#include "ParseTree.h"
#include <sstream>

using namespace std;

BaseTable::BaseTable():s(),sNew(),r(),rNew(),c(){

}

void BaseTable::addTree(const ParseTree& tree){
    if(tree.getIsContext()){
        throw invalid_argument("Can't add a context to S");
    }
    if(hasTree(tree)){
        cout << tree << endl;
        throw invalid_argument("Tree already exists");
    }
    auto newTree = new ParseTree(tree);
    completeTree(newTree);
    bool isComplete = checkTableComplete(newTree);
    if(!isComplete){
        sNew.push_back((int)(s.size()));
        s.push_back(newTree);
    }else {
        rNew.push_back((int)(r.size()));
        r.push_back(newTree);
    }
}

void BaseTable::addContext(const ParseTree& context){
    if (!context.getIsContext()) {
        throw invalid_argument("Tree must be a context to be added to C");
    }
    auto newContext = new ParseTree(context);
    c.push_back(newContext);
    completeContext(newContext);
    checkTableCompleteContext(newContext);
}


bool BaseTable::hasTree(const ParseTree& tree){
    for(auto t: s){
        if(*t==tree){
            return true;
        }
    }
    for(auto t: r){
        if(*t==tree){
            return true;
        }
    }
    return false;
}

bool BaseTable::treeInS(const ParseTree& tree){
    for(auto t: s){
        if(*t==tree){
            return true;
        }
    }
    return false;
}

void vectorClear(vector<ParseTree*>& vec){
    for(auto& t:vec){
        if(t){
            delete(t);
            t=nullptr;
        }
    }
}

BaseTable::~BaseTable(){
    vectorClear(s);
    vectorClear(r);
    vectorClear(c);
}

ObservationTable::ObservationTable(const Teacher& teacher):BaseTable(),teacher(teacher),obs(){}


vector<bool> ObservationTable::getObs(const ParseTree& tree){
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

int ObservationTable::getSObsInd(const ParseTree& tree){
    for(auto it=s.begin();it!=s.end();it++){
        if(obs[*it]==getObs(tree)){
            return (int)(it-s.begin());
        }
    }
    return -1;
}

const ParseTree& ObservationTable::getTreeS(int ind) const{
    return *s[ind];
}

void ObservationTable::printTable() {
    cout << "Contexts:" << endl;
    for (auto it = c.begin(); it != c.end(); it++) {
        ParseTree *tree = *it;
        cout << (it - c.begin()) << " - " << *tree << endl;
    }
    cout << "Trees in S:" << endl;
    for (auto tree: s) {
        cout << *tree << endl;
        vector<bool> obs = getObs(*tree);
        for (bool o: obs) {
            if (o) { cout << "1"; } else { cout << "0"; }
        }
        cout << endl;
    }
    cout << "Trees in R:" << endl;
    for (auto tree: r) {
        cout << *tree << endl;
        vector<bool> obs = getObs(*tree);
        for (bool o: obs) {
            if (o) { cout << "1"; } else { cout << "0"; }
        }
        cout << endl;
    }
}

string ObservationTable::getTableLatex(){
    stringstream stream;
    stream << "\\begin{tabular}{ l |";
    for(unsigned int i=0;i<c.size();++i){
        stream << " l |";
    }
    stream << " }" << endl;
    for(unsigned int i=0;i<c.size();++i){
        //stream << "& $c_{" << i << "}$";
        stream <<  "& " << c[i]->getLatexTree();
    }
    stream << "\\\\ \\hline" << endl;
    for(unsigned int i=0;i<s.size();i++){
        auto tree = s[i];
        vector<bool> obs = getObs(*tree);
        //stream << "$s_{" << i << "}$";
        stream << "\\color{blue}" << s[i]->getLatexTree();
        for(bool b: obs){
            stream << "&" << (b?"1":"0");
        }
        stream << "\\\\ \\hline" << endl;
    }
    for(unsigned int i=0;i<r.size();i++){
        auto tree = r[i];
        vector<bool> obs = getObs(*tree);
        //stream << "$r_{" << i << "}$";
        stream << r[i]->getLatexTree();
        for(bool b: obs){
            stream << "& " << (b?"1":"0");
        }
        stream << "\\\\ \\hline" << endl;
    }
    stream << "\\end{tabular}" << endl;
    return stream.str();
}

const vector<ParseTree*> ObservationTable::getRNew(){
    vector<ParseTree*> ans;
    for(int ind: rNew){
        ans.push_back(r[ind]);
    }
    rNew.clear();
    return ans;
}

const vector<ParseTree*> ObservationTable::getSNew(){
    vector<ParseTree*> ans;
    for(int ind: sNew){
        ans.push_back(s[ind]);
    }
    sNew.clear();
    return ans;
}

void ObservationTable::completeTree(ParseTree* tree){
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

void ObservationTable::completeContext(ParseTree* context){
    for (auto tree: s) {
        ParseTree *merged = context->mergeContext(*tree);
        obs[tree].push_back(teacher.membership(*merged));
        delete (merged);
    }
}

bool ObservationTable::checkTableComplete(ParseTree* newTree){
    for(auto sTree: s){ //check if obs(tree) is already covered by S.
        if(obs[sTree]==obs[newTree]){
            return true;
        }
    }
    return false;
}

void ObservationTable::checkTableCompleteContext(ParseTree* newContext){
    auto it = r.begin();
    int newState = -1;
    while(it!=r.end()) {
        ParseTree *tree = *it;
        ParseTree *merged = newContext->mergeContext(*tree);
        obs[tree].push_back(teacher.membership(*merged));
        delete (merged);
        if(getSObsInd(*tree)==-1){//Find if the new context creates a new state.
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
            if(newState==-1){newState = (int)s.size();}
            s.push_back(tree);
            r.erase(it); //Remove tree from r list.
        }else{
            ++it;
        }
    }
    if(newState==-1){return;} //No updates to transitions needed, since no new states were created
    it = r.begin();
    while(it!=r.end()) {
        ParseTree *tree = *it;
        //Checks if this tree now leads to a different state. If so, transitions need to be updated.
        if(getSObsInd(*tree)>=newState){
            rNew.push_back((int)(it-r.begin()));
        }
        ++it;
    }
}

void BaseTable::clear(){
    vectorClear(s);
    vectorClear(r);
    vectorClear(c);
}