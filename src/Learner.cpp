#include "Learner.h"
#include "ParseTree.h"
#include "Teacher.h"
#include <vector>
#include <unordered_map>

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
    unordered_map<ParseTree*,unordered_map<ParseTree*,bool>> obs;
    void clear(){
        vectorClear(s);
        vectorClear(r);
        vectorClear(c);
    }
public:
    observationTable(const Teacher& teacher):teacher(teacher),s(),r(),c(),obs(){}
    observationTable(const observationTable& other)=delete;
    observationTable& operator=(const observationTable& other)=delete;
    observationTable(observationTable&& other)=delete;
    observationTable& operator=(observationTable&& other)=delete;
    ~observationTable(){
        clear();
    }
};

typedef vector<vector<ParseTree>> setsVec;

TreeAcceptor synthesize(observationTable& s){
    return TreeAcceptor(set<rankedChar>{{2,0}});
}

void extend(observationTable& s, ParseTree* t){

}

TreeAcceptor learn(const Teacher& teacher){
    observationTable table(teacher);
    TreeAcceptor ans(set<rankedChar>{{2,0}});
    for(;;){
        ans = synthesize(table);
        ParseTree* counterExample = teacher.equivalence(ans);
        if(!counterExample){
            break;
        }
        extend(table,counterExample);
    }
    return ans;
}