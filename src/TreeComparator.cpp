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

unordered_map<const ParseTree*, int> createMapping(vector<const ParseTree*> v){
    unordered_map<const ParseTree*, int> map;
    for(int i=0;i<v.size();++i){
        map[v[i]] = i;
    }
    return map;
}

int TreeComparator::compare(const ParseTree& t1, const ParseTree& t2){
    vector<const ParseTree*> v1 = t1.getInOrderPtrList();
    vector<const ParseTree*> v2 = t2.getInOrderPtrList();
    unordered_map<const ParseTree*, int> v1PtrToIndMapping = createMapping(v1);
    unordered_map<const ParseTree*, int> v2PtrToIndMapping = createMapping(v2);
    vector<vector<int>> table(v1.size(), vector<int>(v2.size()));
    for(int i=0;i<v1.size();++i){
        for(int j=0;j<v2.size();++j){
            const ParseTree* subtree1 = v1[i];
            const ParseTree* subtree2 = v2[j];
            cout << "[" << i << "," << j << "]" << endl;
            if(subtree1->isLeaf() && subtree2->isLeaf()){
                cout << subtree1->getData() << " , " << subtree1->isLeaf() << endl;
                cout << subtree2->getData() << " , " << subtree2->isLeaf() << endl;
                table[i][j] = scores[{subtree1->getData(), subtree2->getData()}];
            }else{
                for(auto son1: subtree1->getSubtrees()){
                    for(auto son2: subtree2->getSubtrees()){
                        cout << v1PtrToIndMapping[son1] << endl;
                        cout << v2PtrToIndMapping[son2] << endl;
                        cout << "---" << endl;
                    }
                }
            }
            cout << "------------" << endl;
        }
    }
    return 0;
}
