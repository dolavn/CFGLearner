#include "ParseTree.h"
#include "Logger.h"
#include "MultiplicityTeacher.h"
#include "ColinearHankelMatrix.h"
#include "utility.h"
#include <functional>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>


using namespace std;


MultiplicityTreeAcceptor learnColin(const MultiplicityTeacher& teacher, ColinearHankelMatrix& h){
#ifdef DEBUG_MODE
    vector<ParseTree*> counterExamples;
#endif
    h.complete();
    while(true){
        MultiplicityTreeAcceptor acc = h.getAcceptor();
        cout << "equiv" << endl;
        ParseTree* counterExample = teacher.equivalence(acc);
        cout << "received" << endl;
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
            cout << "counter example given " << *counterExample << endl;
            cout << acc.run(*counterExample) << endl;
            cout << teacher.membership(*counterExample) << endl;
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
