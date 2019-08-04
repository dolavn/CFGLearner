#include "ObservationTable.h"
#include "ParseTree.h"
#include "MultiplicityTeacher.h"
#include "MultiplicityTreeAcceptor.h"
#include <armadillo>

using namespace std;
using namespace arma;

HankelMatrix::HankelMatrix(const MultiplicityTeacher& teacher, const set<rankedChar>& alphabet):teacher(teacher), obs(),
alphabet(alphabet), sInv(){

}

void HankelMatrix::completeTree(ParseTree* tree){
    for(auto it=c.begin();it!=c.end();it++){
        ParseTree* context = *it;
        ParseTree* merged = context->mergeContext(*tree);
        vector<double>& observation = obs[tree];
        while(((int)observation.size())-1<(it-c.begin())){
            observation.push_back(teacher.getDefaultValue());
        }
        obs[tree][it-c.begin()]=teacher.membership(*merged);
        delete(merged);
    }
}

void HankelMatrix::completeContextS(ParseTree* context){
    for (auto tree: s) {
        ParseTree *merged = context->mergeContext(*tree);
        obs[tree].push_back(teacher.membership(*merged));
        delete (merged);
    }
}

bool HankelMatrix::checkTableComplete(ParseTree* tree){
    mat sMat = getSMatrix(true);
    fillMatLastRow(sMat, tree);
    /*
    cout << sMat << endl;
    //printf("s size:%d\n", s.size());
    for(auto tree: s){
        //cout << *tree << endl;
        vector<double>& obsi = obs[tree];
        printf("[");
        for(int i=0;i<obsi.size();++i){
            printf("%lf", obsi[i]);
            if(i<obsi.size()-1) {
                printf(",");
            }
        }
        printf("]\n");
    }
    */
    //printf("rank:%ld\t s.size()=%ld\n", arma::rank(sMat), s.size());
    return arma::rank(sMat)!=s.size()+1;
}

vector<double> HankelMatrix::getObs(const ParseTree& tree) const{
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
        vector<double> ans(c.size());
        for(auto it=c.begin();it!=c.end();it++){
            ParseTree* context = *it;
            ParseTree* merged = context->mergeContext(tree);
            ans[it-c.begin()]=teacher.membership(*merged);
            delete(merged);
        }
        return ans;
    }else{ //Return the pre-calculated value of the tree.
        auto key = obs.find(ptr);
        return key->second;
    }
}

void HankelMatrix::completeContextR(ParseTree* context){
    mat sMat = getSMatrix(true);
    auto it = r.begin();
    while(it!=r.end()){
        auto tree = *it;
        ParseTree *merged = context->mergeContext(*tree);
        obs[tree].push_back(teacher.membership(*merged));
        delete (merged);
        fillMatLastRow(sMat, tree);
        if(arma::rank(sMat)==s.size()+1){ //The vector is now linearly independent from s.
            s.push_back(tree);
            sMat = getSMatrix(true);
            updateSInv();
            r.erase(it);
        }else{
            it++;
        }
    }
}

mat HankelMatrix::getSMatrix(bool extraSpace){
    mat sMat(s.size()+(extraSpace?1:0), c.size());
    for(auto it=s.begin();it!=s.end();it++){
        vector<double>& observation = obs[*it];
        for(unsigned int i=0;i<c.size();++i){
            sMat((const uword)(it-s.begin()),i) = observation[i];
        }
    }
    return sMat;
}

void HankelMatrix::fillMatLastRow(mat& sMat, ParseTree* tree){
    vector<double>& observation = obs[tree];
    for(unsigned int i=0;i<c.size();++i){
        sMat(s.size(),i) = observation[i];
    }
}

MultiplicityTreeAcceptor HankelMatrix::getAcceptor() const{
    return MultiplicityTreeAcceptor(alphabet, base.size());
}

void HankelMatrix::updateSInv(){
    mat sMat = getSMatrix(false);
    sInv = arma::inv(sMat);
}

HankelMatrix::suffixIterator::suffixIterator(HankelMatrix& mat):mat(mat),alphabet(),
currChar(-1),arr(){
    for(auto c: mat.alphabet){
        alphabet.push_back(c);
    }
    incChar();
}

bool HankelMatrix::suffixIterator::hasNext(){
    return currChar<alphabet.size();
}

ParseTree HankelMatrix::suffixIterator::operator*() const{
    ParseTree ans(alphabet[currChar].c);
    for(unsigned int i=0;i<alphabet[currChar].rank;++i){
        ans.setSubtree(*mat.s[arr.get(i)], i);
    }
    return ans;
}

HankelMatrix::suffixIterator& HankelMatrix::suffixIterator::operator++(){
    ++arr;
    if(arr.getOverflow()){
        incChar();
    }
}

void HankelMatrix::suffixIterator::incChar(){
    currChar++;
    for(;currChar<alphabet.size();++currChar){
        rankedChar& c = alphabet[currChar];
        if(c.rank!=0){
            break;
        }
    }
    if(currChar<alphabet.size()){
        vector<int> dim;
        for(int i=0;i<alphabet[currChar].rank;++i){
            dim.push_back(int(mat.s.size()));
        }
        arr = IndexArray(dim);
    }
}

HankelMatrix::suffixIterator HankelMatrix::getSuffixIterator(){
    return suffixIterator(*this);
}