#include "TreeComparator.h"
#include "ParseTree.h"
#include <limits>

#define INDEL 2
#define MAX(A,B)    A>=B?A:B
#define MIN(A,B)    A<=B?A:B

using namespace std;

int& scoresMap::operator[](intPair p){
    return map[p];
}

TreeComparator::TreeComparator(scoresMap scores, int indelScore):scores(std::move(scores)),indelScore(indelScore){

}

TreeComparator::TreeComparator(std::unordered_map<intPair, int, pair_hash> map, int indelScore):scores(map), indelScore(indelScore) {

}


int TreeComparator::alignInnerNodes(const ParseTree& t1, const ParseTree& t2,
        treeToIndMap& m1, treeToIndMap& m2, alignmentTable& bigTable){
    unsigned long lengths[] = {t1.getSubtrees().size()+1, t2.getSubtrees().size()+1};
    alignmentTable table(lengths[0], vector<int>(lengths[1]));
    table[0][0] = scores[{t1.getData(), t2.getData()}];
    for(int i=0;i<lengths[0];++i){
        for(int j=i>0?0:1;j<lengths[1];++j){
            int currMin = numeric_limits<int>::max();
            if(i>0){
                currMin = MIN(currMin, table[i-1][j]+indelScore);
            }
            if(j>0){
                currMin = MIN(currMin, table[i][j-1]+indelScore);
            }
            if(i>0 && j>0){
                const ParseTree* subtree1 = t1.getSubtrees()[i-1];
                const ParseTree* subtree2 = t2.getSubtrees()[j-1];
                int replace = bigTable[m1[subtree1]][m2[subtree2]];
                currMin = MIN(currMin, table[i-1][j-1]+replace);
            }
            table[i][j] = currMin;
        }
    }
    return table[lengths[0]-1][lengths[1]-1];
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
    treeToIndMap v1PtrToIndMapping = createMapping(v1);
    treeToIndMap v2PtrToIndMapping = createMapping(v2);
    alignmentTable table(v1.size(), vector<int>(v2.size()));
    for(int i=0;i<v1.size();++i){
        for(int j=0;j<v2.size();++j){
            const ParseTree* subtree1 = v1[i];
            const ParseTree* subtree2 = v2[j];
            //cout << "[" << i << "," << j << "]" << endl;
            if(subtree1->isLeaf() && subtree2->isLeaf()){
                //cout << subtree1->getData() << " , " << subtree1->isLeaf() << endl;
                //cout << subtree2->getData() << " , " << subtree2->isLeaf() << endl;
                table[i][j] = scores[{subtree1->getData(), subtree2->getData()}];
            }else{
                table[i][j] = alignInnerNodes(*subtree1, *subtree2, v1PtrToIndMapping, v2PtrToIndMapping,
                        table);
            }
            //cout << "------------" << endl;
        }
    }
    return table[v1.size()-1][v2.size()-1];
}
