#include "TreeComparator.h"
#include "ParseTree.h"

using namespace std;

int& scoresMap::operator[](vector<int> v){
    if(v.size()!=2){
        throw std::invalid_argument("key must be vector of size 2");
    }
    return map[v];
}

TreeComparator::TreeComparator(scoresMap scores):scores(std::move(scores)){

}

int TreeComparator::compare(const ParseTree& t1, const ParseTree& t2){
    vector<const ParseTree*> v1 = t1.getInOrderPtrList();
    vector<const ParseTree*> v2 = t2.getInOrderPtrList();
    vector<vector<int>> table(v1.size(), vector<int>(v2.size()));
    for(int i=0;i<v1.size();++i){
        for(int j=0;j<v2.size();++j){
            const ParseTree* subtree1 = v1[i];
            const ParseTree* subtree2 = v2[j];
            cout << subtree1->getData() << " , " << subtree1->isLeaf() << endl;
            cout << subtree2->getData() << " , " << subtree2->isLeaf() << endl;
            cout << "------------" << endl;
            table[i][j] = 5;
        }
    }
    return 0;
}