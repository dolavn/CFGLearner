#include "ObservationTable.h"
#include "ParseTree.h"
#include "Logger.h"
#include "MultiplicityTeacher.h"
#include "MultiplicityTreeAcceptor.h"
#include "utility.h"
#include "soplex.h"
#include <armadillo>
#include <algorithm>


#define EPSILON (0.000001)

using namespace std;
using namespace arma;

HankelMatrix::HankelMatrix(const MultiplicityTeacher& teacher):teacher(teacher),
alphabet(teacher.getAlphabet()),base(),obs(){
}

void HankelMatrix::completeTree(ParseTree* tree){
    obs[tree] = vector<double>();
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
    /*
    if(s.empty()){
        return false;
    }
    if(c.empty()){
        return true;
    }
     */
    mat sMat = getSMatrix(true);
    fillMatLastRow(sMat, *tree);
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
    soplex::SoPlex mysoplex;
    //mat sMat2 = getSMatrix(false);
    auto it = r.begin();
    while(it!=r.end()){
        auto tree = *it;
        ParseTree *merged = context->mergeContext(*tree);
        obs[tree].push_back(teacher.membership(*merged));
        delete (merged);
        fillMatLastRow(sMat, *tree);
        //arma::vec x = getObsVec(*tree);
        //arma::vec p;
        if(arma::rank(sMat)==s.size()+1){ //The vector is now linearly independent from s.
            s.push_back(tree);
            sMat = getSMatrix(true);
            r.erase(it);
        }else{
            /*
            cout << "rank:" << arma::rank(sMat) << endl;
            cout << sMat << endl;
            cout << "s size:" << s.size() << endl;
            arma::mat mat2(4, 6);
            mat2(0, 0) = 0.9; mat2(0, 1)=0.5; mat2(0, 2)=0.9; mat2(0, 3)=0.9; mat2(0, 4)=0.5; mat2(0, 5)=0.9;
            mat2(1, 0) = 0.1; mat2(1, 1)=0.1; mat2(1, 2)=0.5; mat2(1, 3)=0.5; mat2(1, 4)=0.1; mat2(1, 5)=0.5;
            mat2(2, 0) = 0.5; mat2(2, 1)=0.5; mat2(2, 2)=0.5; mat2(2, 3)=0.9; mat2(2, 4)=0.1; mat2(2, 5)=0.9;
            mat2(3, 0) = 0.5; mat2(3, 1)=0.1; mat2(3, 2)=0.9; mat2(3, 3)=0.5; mat2(3, 4)=0.5; mat2(3, 5)=0.5;
            cout << mat2 << endl;
            cout << "mat2 rank:" << arma::rank(mat2) << endl;
            */
            it++;
        }
    }
}

mat HankelMatrix::getSMatrix(bool extraSpace) const{
    mat sMat(s.size()+(extraSpace?1:0), c.size());
    //cout << s.size() << " , " << c.size() << endl;
    for(auto it=s.begin();it!=s.end();it++){
        vector<double> observation = obs.find(*it)->second;
        for(unsigned int i=0;i<c.size();++i){
            sMat((const uword)(it-s.begin()),i) = observation[i];
        }
    }
    return sMat;
}

void HankelMatrix::fillMatLastRow(mat& sMat, const ParseTree& tree) const{
    vector<double> observation = getObs(tree);
    for(unsigned int i=0;i<c.size();++i){
        sMat(s.size(),i) = observation[i];
    }
}

MultiplicityTreeAcceptor HankelMatrix::getAcceptor() const{
    if(!checkClosed()){
        throw std::invalid_argument("Table must be closed");
    }
    return getAcceptorTemp();
}

MultiplicityTreeAcceptor HankelMatrix::getAcceptorTemp() const{
    vector<MultiLinearMap> maps;
    vector<rankedChar> alphabetVec = getAlphabetVec();
    for(auto c: alphabetVec){
        maps.emplace_back(MultiLinearMap((int)(s.size()), c.rank));
    }
    arma::mat sInv = getSInv();
    MultiplicityTreeAcceptor acc(alphabet, s.size());
    for(auto currTree: s){
        rankedChar c = {currTree->getData(), (int)(currTree->getSubtrees().size())};
        unsigned int charInd = (unsigned int)(find(alphabetVec.begin(), alphabetVec.end(), c)-alphabetVec.begin());
        if(charInd>=alphabetVec.size()){
            throw std::invalid_argument("Character not in alphabet");
        }
        updateTransition(maps[charInd], *currTree, alphabetVec, sInv);
    }
    for(auto currTree: r){
        rankedChar c = {currTree->getData(), (int)(currTree->getSubtrees().size())};
        unsigned int charInd = (unsigned int)(find(alphabetVec.begin(), alphabetVec.end(), c)-alphabetVec.begin());
        if(charInd>=alphabetVec.size()){
            throw std::invalid_argument("Character not in alphabet");
        }
        updateTransition(maps[charInd], *currTree, alphabetVec, sInv);
    }
    for(unsigned int i=0;i<alphabetVec.size();++i){
        acc.addTransition(maps[i], alphabetVec[i]);
    }
    vector<float> lambdaVec;
    if(!c.empty()){
        for(auto treeS: s){
            vector<double> obs = getObs(*treeS);
            lambdaVec.push_back((float)(obs[0]));
        }
    }
    acc.setLambda(lambdaVec);
    return acc;
}

void HankelMatrix::printTable() const{
    cout << "contexts" << endl;
    for(auto con: c){
        cout << *con << endl;
    }
    cout << "trees in s" << endl;
    for(auto tree: s){
        cout << *tree << endl;
        vector<double> obs = getObs(*tree);
        cout << "[";
        for(unsigned int i=0;i<obs.size();++i){
            cout << obs[i];
            if(i<obs.size()-1){cout << ",";}
        }
        cout << "]" << endl;
    }
    cout << "trees in r" << endl;
    for(auto tree: r){
        cout << *tree << endl;
        vector<double> obs = getObs(*tree);
        cout << "[";
        for(unsigned int i=0;i<obs.size();++i){
            cout << obs[i];
            if(i<obs.size()-1){cout << ",";}
        }
        cout << "]" << endl;
    }
}

arma::vec HankelMatrix::getCoefficients(const ParseTree& tree, const arma::mat& s) const{
    arma::vec v = getObsVec(tree);
    arma::mat sT = s.t();
    //cout << v << endl;
    //arma::rowvec params = v*sInv;
    /*
    cout << "<------------->" << endl;
    cout << v << endl;
    cout << sT << endl;

    for(auto tree: this->s){
        cout << getObsVec(*tree) << endl;
    }
     */
    arma::vec params = arma::solve(sT, v);
    for(unsigned int i=0;i<sT.n_cols;++i){
        if(params(i)<EPSILON && params(i)>-EPSILON){params(i)=0;}
    }
/*
    cout << params << endl;
    cout << t << endl;

    cout << "<------------->" << endl;
*/
    return params;
}

void HankelMatrix::updateTransition(MultiLinearMap& m, const ParseTree& t, const vector<rankedChar>& alphabetVec,
        const arma::mat& s) const{
    if(this->s.size()==0 || c.size()==0){return;}
    vector<int> sIndices;
    for(auto subtree: t.getSubtrees()){
        int subtreeInd = getIndInS(*subtree);
        if(subtreeInd<0){
            throw std::invalid_argument("Subtree not in S");
        }
        sIndices.push_back(subtreeInd);
    }
    rankedChar c = {t.getData(), (int)(sIndices.size())};
    unsigned int charInd = (unsigned int)(find(alphabetVec.begin(), alphabetVec.end(), c)-alphabetVec.begin());
    if(charInd>=alphabetVec.size()){
        throw std::invalid_argument("Character not in alphabet");
    }
    arma::vec params = getCoefficients(t, s);
    vector<int> mapParams;
    mapParams.push_back(-1);
    mapParams.insert(mapParams.end(), sIndices.begin(), sIndices.end());
    for(unsigned int i=0;i<params.n_rows;++i){
        mapParams[0] = i;
        m.setParam(params(i), mapParams);
    }
}

void HankelMatrix::closeTable(){
    while(true){
        Logger& logger = Logger::getLogger();
        logger.setLoggingLevel(Logger::LOG_DEBUG);
        logger << "closing " << "c:" << c.size() << " s:" << s.size() << " r:" << r.size() << logger.endline;
        //if(c.size()>20){return;} //todo: delete
        if(s.empty()){
            for(auto c:alphabet){
                if(c.rank==0){
                    addTree(ParseTree(c.c));
                }
            }
        }
        if(c.empty()){ /*Adds an empty context */
            ParseTree t(1);
            pair<ParseTree*,ParseTree*> contextTreePair = t.makeContext({});
            SAFE_DELETE(contextTreePair.second);
            addContext(*contextTreePair.first);
            SAFE_DELETE(contextTreePair.first);
        }
        auto it = getSuffixIterator();
        bool closed=true;
        while(it.hasNext()){
            auto currTree = *it;
            ++it;
            if(!hasTree(currTree)){
                addTree(currTree);
                closed=false;
                break;
            }
        }
        if(closed){
            return;
        }
    }
}

string HankelMatrix::getTableLatex(){
    stringstream stream;
    static int MAX_CONTEXTS_SHOW = 10;
    stream << "\\begin{tabular}{ l |";
    for(unsigned int i=0;i<c.size();++i){
        stream << " l |";
    }
    stream << " }" << endl;
    for(unsigned int i=0;i<MIN(c.size(),MAX_CONTEXTS_SHOW);++i){
        //stream << "& $c_{" << i << "}$";
        stream <<  "& " << c[i]->getLatexTree();
    }
    stream << "\\\\ \\hline" << endl;
    for(unsigned int i=0;i<s.size();i++){
        auto tree = s[i];
        vector<double> obs = getObs(*tree);
        //stream << "$s_{" << i << "}$";
        //cout << "obs:" << obs.size() << endl;
        //cout << "c:" << c.size() << endl;
        stream << "\\color{blue}" << s[i]->getLatexTree();
        for(unsigned int i=0;i<MIN(c.size(), MAX_CONTEXTS_SHOW);++i){
            stream << "&" << obs[i];
        }
        stream << "\\\\ \\hline" << endl;
    }
    for(unsigned int i=0;i<r.size();i++){
        auto tree = r[i];
        vector<double> obs = getObs(*tree);
        //stream << "$r_{" << i << "}$";
        stream << r[i]->getLatexTree();
        for(unsigned int i=0;i<MIN(c.size(), MAX_CONTEXTS_SHOW);++i){
            stream << "&" << obs[i];
        }
        stream << "\\\\ \\hline" << endl;
    }
    stream << "\\end{tabular}" << endl;
    return stream.str();
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

bool HankelMatrix::checkIsTreeZero(const ParseTree& tree) const{
    vector<double> obs = getObs(tree);
    for(auto& elem: obs){
        if(elem!=0){return false;}
    }
    return true;
}

bool HankelMatrix::hasContext(const ParseTree& context) const{
    if(BaseTable::hasContext(context)){return true;}
    return false;
}

void HankelMatrix::makeConsistent(){
    Logger& logger = Logger::getLogger();
    closeTable();
    //TODO: Check if consistency is needed, or is being closed enough.
    //return;
    while(true){
        closeTable();
        vector<ParseTree*> newContexts;
        MultiplicityTreeAcceptor acc = getAcceptorTemp();
        //acc.printDesc();
        for(auto tree: s){
            vector<double> vec = getObs(*tree);
            for(auto it=c.begin();it!=c.end();++it){
                auto context = *it;
                ParseTree* merged = context->mergeContext(*tree);
                if(ABS(acc.run(*merged)-vec[it-c.begin()])>EPSILON){
                    logger.setLoggingLevel(Logger::LOG_DETAILS);
                    logger << *merged << logger.endline;
                    logger << "acc[c*t]=" << acc.run(*merged) << logger.endline;
                    logger << "h[c][t]="  << vec[it-c.begin()] << logger.endline;
                    for(auto& suffix: merged->getAllContexts()){
                        if(!hasContext(*suffix)){ // && c.size() < 20){
                            newContexts.push_back(suffix);
                        }else{
                            delete(suffix); suffix=nullptr;
                        }
                    }
                }
                SAFE_DELETE(merged)
            }
        }
        if(newContexts.empty()){return;}
        for(auto& context: newContexts){
            if(!hasContext(*context)){
                addContext(*context);
            }
            delete(context);
            context = nullptr;
        }
    }
}

vector<double> HankelMatrix::test(){
    arma::mat t(4, 4);
    t(0, 0)=0.9; t(0, 1)=1; t(0, 2)=0.9; t(0, 3)=1;
    t(1, 0)=0.9; t(1, 1)=0.5; t(1, 2)=1; t(1, 3)=1;
    t(2, 0)=0.5; t(2, 1)=1; t(2, 2)=1; t(2, 3)=1;
    t(3, 0)=0.9; t(3, 1)=1; t(3, 2)=1; t(3, 3)=1;
    //t = t.t();
    arma::vec a(4);
    a(0)=0.5; a(1)=1;a(2)=1;a(3)=1;
    cout << t << endl;
    cout << a << endl;
    arma::vec x = arma::solve(t,a);
    cout << x << endl;
    return {0};
}

arma::mat HankelMatrix::getSInv() const{
    if(c.size()==0 || s.size()==0){return arma::mat(0, 0);}
    mat sMat = getSMatrix(false);
    return sMat;
    for(auto con: c){
        cout << *con << endl;
    }
    cout << sMat << endl;
    for(auto t: r){
        arma::rowvec v = getObsVec(*t);
        cout << v << endl;
    }
    return arma::inv(sMat);
}

arma::vec HankelMatrix::getObsVec(const ParseTree& tree) const{
    vector<double> vec = getObs(tree);
    arma::vec ans(vec.size());
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

TreesIterator HankelMatrix::getSuffixIterator() const{
    return TreesIterator(this->s, this->alphabet, 1);
}