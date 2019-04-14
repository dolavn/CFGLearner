#include "Learner.h"
#include "ParseTree.h"
#include "Teacher.h"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

using namespace std;

typedef pair<ParseTree*,ParseTree*> contextTreePair;

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
    vector<int> sNew;
    vector<ParseTree*> r;
    vector<int> rNew;
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
    explicit observationTable(const Teacher& teacher):teacher(teacher),s(),sNew(),r(),rNew(),c(),obs(){}
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
        if(hasTree(tree)){
            cout << tree << endl;
            throw invalid_argument("Tree already exists");
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
            sNew.push_back((int)(s.size()));
            s.push_back(newTree);
        }else {
            rNew.push_back((int)(r.size()));
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
            obs[tree].push_back(teacher.membership(*merged));
            delete (merged);
        }
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
    const ParseTree& getTreeS(int ind) const{
        return *s[ind];
    }
    bool hasTree(const ParseTree& tree){
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
    bool treeInS(const ParseTree& tree){
        for(auto t: s){
            if(*t==tree){
                return true;
            }
        }
        return false;
    }
    void printTable(){
        cout << "Contexts:" << endl;
        for(auto it = c.begin();it!=c.end();it++){
            ParseTree* tree = *it;
            cout << (it-c.begin()) << " - " << *tree << endl;
        }
        cout << "Trees in S:" << endl;
        for(auto tree: s) {
            cout << *tree << endl;
            vector<bool> obs = getObs(*tree);
            for(bool o: obs){
                if(o){cout << "1";}else{cout << "0";}
            }
            cout << endl;
        }
        cout << "Trees in R:" << endl;
        for(auto tree: r){
            cout << *tree << endl;
            vector<bool> obs = getObs(*tree);
            for(bool o: obs){
                if(o){cout << "1";}else{cout << "0";}
            }
            cout << endl;
        }
    }
    std::string getTableLatex(){
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
            stream << s[i]->getLatexTree();
            for(bool b: obs){
                stream << "& " << (b?"1":"0");
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
    inline const vector<ParseTree*>& getR(){return r;}
    inline const vector<ParseTree*>& getS(){return s;}
    inline const vector<ParseTree*> getRNew(){
        vector<ParseTree*> ans;
        for(int ind: rNew){
            ans.push_back(r[ind]);
        }
        rNew.clear();
        return ans;
    }
    inline const vector<ParseTree*> getSNew(){
        vector<ParseTree*> ans;
        for(int ind: sNew){
            ans.push_back(s[ind]);
        }
        sNew.clear();
        return ans;
    }
    inline const vector<ParseTree*>& getC(){return c;}
};

ofstream* currStream;

rankedChar getChar(TreeAcceptor& acc, const ParseTree& tree){
    int value = tree.getData();
    int rank = 0;
    for(int i=0;i<tree.getChildrenNum();i++){
        if(tree.hasSubtree(i)){rank++;}
    }
    return rankedChar{value, rank};
}

void addTransition(observationTable& s, TreeAcceptor& acc, const ParseTree& tree){
    int value = tree.getData();
    vector<int> states;
    for(int i=0;i<tree.getChildrenNum();i++){
        if(tree.hasSubtree(i)){
            states.push_back(s.getSObsInd(tree.getNode({i})));
        }
    }
    int targetState = s.getSObsInd(tree);
    rankedChar c{value,(int)(states.size())};
    /*cout << tree << endl;
    cout << "adding transition ["; for(auto state: states){cout << state << ",";}
    cout << "," << c.c << "] -> " << targetState << endl;*/
    acc.addTransition(states,c,targetState);
}

TreeAcceptor synthesize(observationTable& s, const Teacher& teacher, TreeAcceptor* acc){
    //set<rankedChar> alphabet = getAlphabet(s);
    //TreeAcceptor ans(alphabet,(int)(s.getS().size()));
    TreeAcceptor temp(set<rankedChar>(),0);
    if(!acc){acc=&temp;}
    for(auto tree: s.getSNew()){
        int state = acc->addState();
        /*cout << "adding state" << state << endl;
        cout << *tree << endl;*/
        acc->setAccepting(state, teacher.membership(*tree));
        for(auto it = tree->getIndexIterator();it.hasNext();++it){
            vector<int> ind = *it;
            acc->addChar(getChar(*acc,(*tree)[ind]));
            addTransition(s,*acc,(*tree)[ind]);
        }
    }
    for(auto tree: s.getRNew()){
        for(auto it = tree->getIndexIterator();it.hasNext();++it){
            vector<int> ind = *it;
            acc->addChar(getChar(*acc,(*tree)[ind]));
            addTransition(s,*acc,(*tree)[ind]);
        }
    }
    return *acc;
}

contextTreePair decompose(observationTable& s, ParseTree& t){
    vector<int> splitInd;
    for(auto it = t.getIndexIterator();it.hasNext();++it){
        splitInd = *it;
        const ParseTree& currTree = t.getNode(splitInd);
        if(s.treeInS(currTree)){
            continue;
        }
        bool contextClosed = true;
        for(auto subtree: currTree.getSubtrees()){
            if(subtree && !s.treeInS(*subtree)){
                contextClosed = false;
                break;
            }
        }
        if(contextClosed){
            break;
        }
    }
    if(s.treeInS(t.getNode(splitInd))){
        throw invalid_argument("Tree in split index shouldn't be in S!");
    }
    return t.makeContext(splitInd);
}

void extend(observationTable& s, ParseTree* t, const Teacher& teacher){
    if(s.treeInS(*t)){
        s.printTable();
        cout << *t << endl;
        if(!teacher.membership(*t)){cout << "not ";}
        cout << "in language" << endl;
        throw invalid_argument("Counter example can't be a member of S!");
    }
    ofstream& myfile = *currStream;
    contextTreePair pair = decompose(s,*t);
    ParseTree* context = pair.first;
    ParseTree* tree = pair.second;
    myfile << "\\textbf{Decomposed}\\\\" << endl;
    myfile << "Context:\\\\" << endl;
    myfile << "\\begin{center}" << endl;
    myfile << context->getLatexTree() << endl;
    myfile << "\\end{center}" << endl;
    myfile << "Tree:\\\\" << endl;
    myfile << "\\begin{center}" << endl;
    myfile << tree->getLatexTree() << endl;
    myfile << "\\end{center}" << endl;
    if(s.hasTree(*tree)){ //Tree in R
        int sInd = s.getSObsInd(*tree);
        const ParseTree& sTree = s.getTreeS(sInd);
        ParseTree* mergedTree = context->mergeContext(sTree);
        if(*t==*mergedTree){
            throw invalid_argument("Endless loop");
        }
        if(teacher.membership(*t)==teacher.membership(*mergedTree)){
            extend(s, mergedTree, teacher);
            delete(mergedTree);
        }else{
            delete(mergedTree);
            s.addContext(*context);
        }
    }else{
        s.addTree(*tree);
    }
    delete(tree);
    delete(context);
}

TreeAcceptor learn(const Teacher& teacher){
    observationTable table(teacher);
    ofstream myfile;
    myfile.open("example");
    currStream = &myfile;
    TreeAcceptor ans(set<rankedChar>{});
    for(;;){
        clock_t begin = clock();
        ans = synthesize(table,teacher,&ans);
        clock_t end = clock();
        double syntTime = 1000*double(end-begin)/CLOCKS_PER_SEC;
        begin = clock();
        ParseTree* counterExample = teacher.equivalence(ans);
        end = clock();
        double equivTime = 1000*double(end-begin)/CLOCKS_PER_SEC;
        if(!counterExample){
            break;
        }
        myfile << "Current table:\\\\" << endl;
        myfile << "\\begin{center}" << endl;
        myfile << table.getTableLatex() << endl;
        myfile << "\\end{center}" << endl;
        myfile << "Counter example given:\\\\" << endl;
        myfile << "\\begin{center}" << endl;
        myfile << counterExample->getLatexTree() << endl;
        myfile << "\\end{center}" << endl;
        /*
        cout << "counter example given:" << endl;
        cout << *counterExample << endl;
        if(!teacher.membership(*counterExample)){cout << "not ";}
        cout << "in the language" << endl;
        if(!ans.run(*counterExample)){cout << "not ";}
        cout << "accepted by our tree automata" << endl;
        ans.printDescription();
        */
        begin = clock();
        extend(table, counterExample, teacher);
        end = clock();
        double extendTime = 1000*double(end-begin)/CLOCKS_PER_SEC;
        /*cout << "syntTime:" << syntTime << endl;
        cout << "equivTime:" << equivTime << endl;
        cout << "extendTime:" << extendTime << endl;*/
        delete(counterExample);
    }
    myfile << "Final table:\\\\" << endl;
    myfile << "\\begin{center}" << endl;
    myfile << table.getTableLatex() << endl;
    myfile << "\\end{center}" << endl;
    myfile.close();
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
    return synthesize(*table,*t,nullptr);
}

contextTreePair learnerDecompose(ParseTree& tree){
    return decompose(*table,tree);
}

void learnerExtend(const ParseTree& tree){
    auto treeCopy = new ParseTree(tree);
    extend(*table,treeCopy,*t);
    delete(treeCopy);

}