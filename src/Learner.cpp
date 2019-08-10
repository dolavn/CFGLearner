#include "Learner.h"
#include "ParseTree.h"
#include "Teacher.h"
#include "MultiplicityTeacher.h"
#include "ObservationTable.h"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

using namespace std;

typedef pair<ParseTree*,ParseTree*> contextTreePair;

ofstream* currStream;

rankedChar getChar(TreeAcceptor& acc, const ParseTree& tree){
    int value = tree.getData();
    int rank = 0;
    for(int i=0;i<tree.getChildrenNum();i++){
        if(tree.hasSubtree(i)){rank++;}
    }
    return rankedChar{value, rank};
}

void addTransition(ObservationTable& s, TreeAcceptor& acc, const ParseTree& tree){
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

TreeAcceptor synthesize(ObservationTable& s, const Teacher& teacher, TreeAcceptor* acc){
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

contextTreePair decompose(ObservationTable& s, ParseTree& t){
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

void extend(ObservationTable& s, ParseTree* t, const Teacher& teacher){
    if(s.treeInS(*t)){
        s.printTable();
        cout << *t << endl;
        if(!teacher.membership(*t)){cout << "not ";}
        cout << "in language" << endl;
        throw invalid_argument("Counter example can't be a member of S!");
    }
    contextTreePair pair = decompose(s,*t);
    ParseTree* context = pair.first;
    ParseTree* tree = pair.second;
    if(currStream) {
        ofstream &myfile = *currStream;
        myfile << "\\textbf{Decomposed}\\\\" << endl;
        myfile << "Context:\\\\" << endl;
        myfile << "\\begin{center}" << endl;
        myfile << context->getLatexTree() << endl;
        myfile << "\\end{center}" << endl;
        myfile << "Tree:\\\\" << endl;
        myfile << "\\begin{center}" << endl;
        myfile << tree->getLatexTree() << endl;
        myfile << "\\end{center}" << endl;
    }
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
    ObservationTable table(teacher);
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
        while(teacher.membership(*counterExample)!=ans.run(*counterExample)) {
            myfile << "Current table:\\\\" << endl;
            myfile << "\\begin{center}" << endl;
            myfile << table.getTableLatex() << endl;
            myfile << "\\end{center}" << endl;
            myfile << "Extending\\\\" << endl;
            extend(table, counterExample, teacher);
            ans = synthesize(table, teacher, &ans);
        }
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

MultiplicityTreeAcceptor learn(const MultiplicityTeacher& teacher){
    HankelMatrix h(teacher);
    while(true){
        h.makeConsistent();
        MultiplicityTreeAcceptor acc = h.getAcceptor();
        ParseTree* counterExample = teacher.equivalence(acc);
        if(counterExample==nullptr){
            return acc;
        }else{
            cout << *counterExample << endl;
            for(auto& context: counterExample->getAllContexts()){
                if(!h.hasContext(*context)){
                    h.addContext(*context);
                    delete(context);
                    context=nullptr;
                }
            }
            delete(counterExample); counterExample=nullptr;
        }
    }
}

ObservationTable* table=nullptr;
const Teacher* t = nullptr;

void initLearner(const Teacher& teacher){
    clearLearner();
    table = new ObservationTable(teacher);
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