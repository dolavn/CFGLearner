//#include "Learner.h"
#include "ParseTree.h"
#include "Logger.h"
//#include "Teacher.h"
#include "MultiplicityTeacher.h"
#include "MultiplicityTreeAcceptor.h"
//#include "ObservationTable.h"
#include "ColinearHankelMatrix.h"
#include "utility.h"
#include <functional>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>


#define DEBUG_MODE

using namespace std;
/*
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
    Logger& logger = Logger::getLogger();
    logger.setLoggingLevel(Logger::LOG_DEBUG);
    logger << tree << logger.endline;
    logger << "adding transition ["; for(auto state: states){logger << state << ",";}
    logger << "," << c.c << "] -> " << targetState << logger.endline;
    acc.addTransition(states,c,targetState);
}

TreeAcceptor synthesize(ObservationTable& s, const Teacher& teacher, TreeAcceptor* acc){
    //set<rankedChar> alphabet = getAlphabet(s);
    //TreeAcceptor ans(alphabet,(int)(s.getS().size()));
    Logger& logger = Logger::getLogger();
    TreeAcceptor temp(set<rankedChar>(),0);
    if(!acc){acc=&temp;}
    for(auto tree: s.getSNew()){
        int state = acc->addState();
        logger.setLoggingLevel(Logger::LOG_DEBUG);
        logger << "adding state" << state << logger.endline;
        logger << *tree << logger.endline;
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
    Logger& logger = Logger::getLogger();
    if(s.treeInS(*t)){
        logger.setLoggingLevel(Logger::LOG_ERRORS);
        s.printTable();
        logger << *t << logger.endline;
        if(!teacher.membership(*t)){logger << "not ";}
        logger << "in language" << logger.endline;
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
    SAFE_DELETE(tree)
    SAFE_DELETE(context)
}
*/
/*
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
        myfile << "\\end{center}" << endl;*/
        /*
        cout << "counter example given:" << endl;
        cout << *counterExample << endl;
        if(!teacher.membership(*counterExample)){cout << "not ";}
        cout << "in the language" << endl;
        if(!ans.run(*counterExample)){cout << "not ";}
        cout << "accepted by our tree automata" << endl;
        ans.printDescription();
        *//*
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
        double extendTime = 1000*double(end-begin)/CLOCKS_PER_SEC;*/
        /*cout << "syntTime:" << syntTime << endl;
        cout << "equivTime:" << equivTime << endl;
        cout << "extendTime:" << extendTime << endl;*//*
        SAFE_DELETE(counterExample)
    }
    myfile << "Final table:\\\\" << endl;
    myfile << "\\begin{center}" << endl;
    myfile << table.getTableLatex() << endl;
    myfile << "\\end{center}" << endl;
    myfile.close();
    return ans;
}*/
/*
MultiplicityTreeAcceptor learn(const MultiplicityTeacher& teacher, HankelMatrix& h){
    ofstream myfile;
    Logger& logger = Logger::getLogger();
    MultiplicityTreeAcceptor acc({}, 0);
    myfile.open("multLearn10");
    while(true){
        myfile << "Current table:\\\\" << endl;
        myfile << "\\begin{center}" << endl;
        myfile << h.getTableLatex() << endl;
        myfile << "\\end{center}" << endl;
        acc.printDesc();
        clock_t begin = clock();
        cout << "equiv" << endl;
        ParseTree* counterExample = teacher.equivalence(acc);
        clock_t end = clock();
        double equivTime = 1000*double(end-begin)/CLOCKS_PER_SEC;
        cout << "end equiv " << equivTime << endl;
        if(counterExample){
            logger << counterExample->getProb() << logger.endline;
            logger << acc.run(*counterExample) << logger.endline;
            logger << *counterExample << logger.endline;
        }*/
        /*
        if(0 && h.getC().size()>35){
            SAFE_DELETE(counterExample);
            myfile << "stopping learning" << endl;
        }
         *//*
        if(counterExample==nullptr){
            myfile << "Final table:\\\\" << endl;
            myfile << "\\begin{center}" << endl;
            myfile << h.getTableLatex() << endl;
            myfile << "\\end{center}" << endl;
            myfile.close();
            cout << h.getS().size() << endl;
            cout << h.getR().size() << endl;
            return acc;
        }else{
            h.giveCounterExample(*counterExample);
            SAFE_DELETE(counterExample)
        }
        begin = clock();
        h.makeConsistent();
        end = clock();
        double consistentTime = 1000*double(end-begin)/CLOCKS_PER_SEC;
        begin = clock();
        acc = h.getAcceptor();
        end = clock();
        double acceptorTime = 1000*double(end-begin)/CLOCKS_PER_SEC;
        logger.setLoggingLevel(Logger::LOG_DETAILS);
        logger << "c size:" << h.getC().size() << logger.endline;
        logger << "s size:" << h.getS().size() << logger.endline;
        logger.setLoggingLevel(Logger::LOG_DEBUG);
        logger << "Error:" << teacher.getError() << logger.endline;
        logger << "consistentTime:" << consistentTime << logger.endline;
        logger << "acceptorTime" << acceptorTime << logger.endline;
        logger << "equivTime:" << equivTime << logger.endline;
    }
}
*/
MultiplicityTreeAcceptor learnColin(const MultiplicityTeacher& teacher, ColinearHankelMatrix& h){
#ifdef DEBUG_MODE
    vector<ParseTree*> counterExamples;
#endif
    h.complete();
    while(true){
        MultiplicityTreeAcceptor acc = h.getAcceptor();
        cout << "equivalence query" << endl;
        ParseTree* counterExample = teacher.equivalence(acc);
        //cout << "received" << endl;
        if(!counterExample){
#ifdef DEBUG_MODE
            for(auto elem: counterExamples){
                delete(elem);
            }
#endif
            return acc;
        }else{
#ifndef DEBUG_MODE
            SAFE_DELETE(counterExample)
#else
            /*cout << "counter example given " << *counterExample << endl;
            cout << acc.run(*counterExample) << endl;
            cout << teacher.membership(*counterExample) << endl;*/
            for(auto elem: counterExamples){
                if(*elem==*counterExample){
                    h.printTable();
                    acc.printDesc();
                    for(auto currTree: h.getTrees()){
                        //cout << currTree << "table:" << teacher.membership(currTree);
                        //cout << "acceptor:" << acc.run(currTree) << endl;
                    }
                    throw std::invalid_argument("Same counter example given twice");
                }
            }
            counterExamples.push_back(counterExample);
#endif
            h.giveCounterExample(*counterExample);
        }
    }
}
/*
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

}*/