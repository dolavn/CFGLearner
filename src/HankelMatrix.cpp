#include "ObservationTable.h"
#include "ParseTree.h"
#include "MultiplicityTeacher.h"
#include "MultiplicityTreeAcceptor.h"
#include <armadillo>
#include <algorithm>

using namespace std;
using namespace arma;

HankelMatrix::HankelMatrix(const MultiplicityTeacher& teacher, const set<rankedChar>& alphabet):teacher(teacher), obs(),
alphabet(alphabet){

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
            r.erase(it);
        }else{
            it++;
        }
    }
}

mat HankelMatrix::getSMatrix(bool extraSpace) const{
    mat sMat(s.size()+(extraSpace?1:0), c.size());
    for(auto it=s.begin();it!=s.end();it++){
        vector<double> observation = obs.find(*it)->second;
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
    if(!checkClosed()){
        throw std::invalid_argument("Table must be closed");
    }
    vector<MultiLinearMap> maps;
    vector<rankedChar> alphabetVec = getAlphabetVec();
    for(auto c: alphabetVec){
        maps.emplace_back(MultiLinearMap((int)(s.size()), c.rank));
    }
    arma::mat sInv = getSInv();
    MultiplicityTreeAcceptor acc(alphabet, base.size());
    for(auto currTree: s){
        rankedChar c = {currTree->getData(), (int)(currTree->getSubtrees().size())};
        int charInd = (int)(find(alphabetVec.begin(), alphabetVec.end(), c)-alphabetVec.begin());
        if(charInd>=alphabetVec.size()){
            throw std::invalid_argument("Character not in alphabet");
        }
        updateTransition(maps[charInd], *currTree, alphabetVec, sInv);
    }
    for(auto currTree: r){
        rankedChar c = {currTree->getData(), (int)(currTree->getSubtrees().size())};
        int charInd = (int)(find(alphabetVec.begin(), alphabetVec.end(), c)-alphabetVec.begin());
        if(charInd>=alphabetVec.size()){
            throw std::invalid_argument("Character not in alphabet");
        }
        updateTransition(maps[charInd], *currTree, alphabetVec, sInv);
    }
    return acc;
}

void HankelMatrix::updateTransition(MultiLinearMap& m, const ParseTree& t, const vector<rankedChar>& alphabetVec,
        const arma::mat& sInv) const{
    vector<int> sIndices;
    for(auto subtree: t.getSubtrees()){
        int subtreeInd = getIndInS(*subtree);
        if(subtreeInd<0){
            throw std::invalid_argument("Subtree not in S");
        }
        sIndices.push_back(subtreeInd);
    }
    rankedChar c = {t.getData(), (int)(sIndices.size())};
    int charInd = (int)(find(alphabetVec.begin(), alphabetVec.end(), c)-alphabetVec.begin());
    if(charInd>=alphabetVec.size()){
        throw std::invalid_argument("Character not in alphabet");
    }
    arma::rowvec v = getObsVec(t);
    arma::rowvec params = v*sInv;
    vector<int> mapParams;
    mapParams.push_back(-1);
    mapParams.insert(mapParams.end(), sIndices.begin(), sIndices.end());
    for(int i=0;i<v.size();++i){
        mapParams[0] = i;
        m.setParam(params(i), mapParams);
    }
}

void HankelMatrix::closeTable(){
    auto it = getSuffixIterator();
    while(it.hasNext()){
        auto currTree = *it++;
        if(!hasTree(currTree)){
            addTree(currTree);
        }
    }
}

bool HankelMatrix::checkClosed() const{
    auto it = getSuffixIterator();
    while(it.hasNext()){
        auto currTree = *it++;
        if(!hasTree(currTree)){
            return false;
        }
    }
    return true;
}

arma::mat HankelMatrix::getSInv() const{
    mat sMat = getSMatrix(false);
    return arma::inv(sMat);
}

arma::rowvec HankelMatrix::getObsVec(const ParseTree& tree) const{
    vector<double> vec = getObs(tree);
    arma::rowvec ans(vec.size());
    for(unsigned int i=0;i<vec.size();++i){
        ans(i) = vec[i];
    }
    return ans;
}

vector<rankedChar> HankelMatrix::getAlphabetVec() const{
    vector<rankedChar> alphabetVec;
    for(auto c: alphabet){
        alphabetVec.push_back(c);
    }
    return alphabetVec;
}

HankelMatrix::suffixIterator::suffixIterator(const HankelMatrix& mat):mat(mat),alphabet(),
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

HankelMatrix::suffixIterator HankelMatrix::getSuffixIterator() const{
    return suffixIterator(*this);
}