#include "TreeComparator.h"
#include "ParseTree.h"
#include "utility.h"
#include <limits>

#define INDEL 2

using namespace std;

int& scoresMap::operator[](intPair p){
    return map[p];
}

TreeAligner::TreeAligner(scoresMap scores, int indelScore):indelScore(indelScore),
innerNode(-1),replaceScore(-1),scores(std::move(scores)){

}

TreeAligner::TreeAligner(int innerNode, int replaceScore, int indelScore):indelScore(indelScore),innerNode(innerNode),
replaceScore(replaceScore),scores(){

}

int TreeAligner::alignInnerNodes(const ParseTree& t1, const ParseTree& t2,
        treeToIndMap& m1, treeToIndMap& m2, alignmentTable& bigTable){
    //cout << "filling inner table" << endl;
    unsigned long lengths[] = {t1.getSubtrees().size()+1, t2.getSubtrees().size()+1};
    alignmentTable table(lengths[0], vector<int>(lengths[1]));
    table[0][0] = getScore(t1.getData(), t2.getData());
    for(unsigned long i=0;i<lengths[0];++i){
        for(unsigned long j=i>0?0:1;j<lengths[1];++j){
            int currMin = numeric_limits<int>::max();
            //cout << "currMin: " << currMin << endl;
            if(i>0){
                currMin = MIN(currMin, safeAdd(table[i-1][j],indelScore));
            }
            if(j>0){
                currMin = MIN(currMin, safeAdd(table[i][j-1],indelScore));
            }
            //cout << "currMin2: " << currMin << endl;
            if(i>0 && j>0){
                const ParseTree* subtree1 = t1.getSubtrees()[i-1];
                const ParseTree* subtree2 = t2.getSubtrees()[j-1];
                int replace = bigTable[m1[subtree1]][m2[subtree2]];
                currMin = MIN(currMin, safeAdd(table[i-1][j-1],replace));
            }
            table[i][j] = currMin;
            //cout << "[" << i << "," << j << "]=" << table[i][j] << endl;
        }
    }
    return table[lengths[0]-1][lengths[1]-1];
}

unordered_map<const ParseTree*, int> createMapping(vector<const ParseTree*> v){
    unordered_map<const ParseTree*, int> map;
    for(unsigned int i=0;i<v.size();++i){
        map[v[i]] = i;
    }
    return map;
}

int TreeAligner::compare(const ParseTree& t1, const ParseTree& t2){
    //cout << "comparing" << endl;
    //cout << "t1 " << t1 << endl;
    //cout << "t2 " << t2 << endl;
    //cout << "innerNode:" << innerNode << " indelScore:" << indelScore << " replace:" << replaceScore << endl;
    vector<const ParseTree*> v1 = t1.getInOrderPtrList();
    vector<const ParseTree*> v2 = t2.getInOrderPtrList();
    treeToIndMap v1PtrToIndMapping = createMapping(v1);
    treeToIndMap v2PtrToIndMapping = createMapping(v2);
    alignmentTable table(v1.size(), vector<int>(v2.size()));
    for(unsigned int i=0;i<v1.size();++i){
        for(unsigned int j=0;j<v2.size();++j){
            const ParseTree* subtree1 = v1[i];
            const ParseTree* subtree2 = v2[j];
            //cout << "[" << i << "," << j << "]" << endl;
            if(subtree1->isLeaf() && subtree2->isLeaf()){
                //cout << subtree1->getData() << " , " << subtree1->isLeaf() << endl;
                //cout << subtree2->getData() << " , " << subtree2->isLeaf() << endl;
                table[i][j] = getScore(subtree1->getData(), subtree2->getData());
            }else{
                table[i][j] = alignInnerNodes(*subtree1, *subtree2, v1PtrToIndMapping, v2PtrToIndMapping,
                        table);
            }
            //cout << "[" << i << "," << j << "]=" << table[i][j] << endl;
            //cout << "------------" << endl;
        }
    }
    /*int score = table[v1.size()-1][v2.size()-1];
    cout << "score:" << score << endl;*/
    return table[v1.size()-1][v2.size()-1];
}


int TreeAligner::getScore(int a, int b){
    if(innerNode==-1){
        return scores[{a,b}];
    }else{
        if(a==b){
            return 0;
        }
        if(a==innerNode || b==innerNode){
            return numeric_limits<int>::max();
        }
        return 1;
    }
}

SwapComparator::SwapComparator(int replaceScore, int swapScore):replaceScore(replaceScore),swapScore(swapScore){

}


int SwapComparator::compare(const ParseTree& t1, const ParseTree& t2){
    vector<const ParseTree*> v1 = t1.getInOrderPtrList();
    vector<const ParseTree*> v2 = t2.getInOrderPtrList();
    treeToIndMap v1PtrToIndMapping = createMapping(v1);
    treeToIndMap v2PtrToIndMapping = createMapping(v2);
    alignmentTable table(v1.size(), vector<int>(v2.size()));
    for(unsigned int i=0;i<v1.size();++i){
        for(unsigned int j=0;j<v2.size();++j){
            const ParseTree* subtree1 = v1[i];
            const ParseTree* subtree2 = v2[j];
            if(subtree1->isLeaf() && subtree2->isLeaf()){ //Both are leaves
                table[i][j] = subtree1->getData()==subtree2->getData()?0:replaceScore;
            }else{
                if(subtree1->isLeaf() || subtree2->isLeaf()){ //One is a leaf
                    table[i][j] = std::numeric_limits<int>::max();
                    continue;
                }
                if(subtree1->getChildrenNum()!=2 || subtree2->getChildrenNum()!=2){
                    throw std::invalid_argument("Parse Tree must be binary");
                }
                vector<const ParseTree*> subtrees1 = subtree1->getSubtrees();
                vector<const ParseTree*> subtrees2 = subtree2->getSubtrees();
                int normal = safeAdd(table[v1PtrToIndMapping[subtrees1[0]]][v2PtrToIndMapping[subtrees2[0]]],
                             table[v1PtrToIndMapping[subtrees1[1]]][v2PtrToIndMapping[subtrees2[1]]]);
                int swap =   safeAdd(table[v1PtrToIndMapping[subtrees1[0]]][v2PtrToIndMapping[subtrees2[1]]],
                             table[v1PtrToIndMapping[subtrees1[1]]][v2PtrToIndMapping[subtrees2[0]]])+swapScore;
                table[i][j] = min(normal, swap);
            }
        }
    }
    return table[v1.size()-1][v2.size()-1];
}

int safeAdd(int a, int b){
    if(b>0 && a>numeric_limits<int>::max()-b){
        return numeric_limits<int>::max();
    }
    if(b<0 && a<numeric_limits<int>::min()-b){
        return numeric_limits<int>::min();
    }
    return a+b;
}