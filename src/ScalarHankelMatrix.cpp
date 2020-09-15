#include "ObservationTable.h"
#include "ColinearHankelMatrix.h"
#include "ConicCombinationFinder.h"
#include "MultiplicityTeacher.h"
#include "MultiplicityTreeAcceptor.h"
#include "Logger.h"
#include "utility.h"
using namespace std;
using namespace arma;

#define EPSILON (0.000001)

//#define FULL_CONSISTENCY


ScalarHankelMatrix::ScalarHankelMatrix(const MultiplicityTeacher& teacher):HankelMatrix(teacher){
    if(teacher.getDefaultValue()<0){
        throw std::invalid_argument("Default value must be positive for positive Hankel Matrix");
    }
    Logger& logger = Logger::getLogger();
    logger.setLoggingLevel(Logger::LOG_DEBUG);
    logger << "Matrix Created";
    complete();
}

void ScalarHankelMatrix::completeTree(ParseTree* tree){
    Logger& logger = Logger::getLogger();
    logger.setLoggingLevel(Logger::LOG_DEBUG);
    obs[tree] = vector<double>();
    for(auto it=c.begin();it!=c.end();it++){
        ParseTree* context = *it;
        ParseTree* merged = context->mergeContext(*tree);
        vector<double>& observation = obs[tree];
        while(((int)observation.size())-1<(it-c.begin())){
            observation.push_back(teacher.getDefaultValue());
        }
        double val;
        if((val=teacher.membership(*merged))<0){
            delete(merged);
            throw std::invalid_argument("Must return positive value for positive Hankel Matrix");
        }
        obs[tree][it-c.begin()]=val;
        delete(merged);
    }
    logger << "finish complete" << logger.endline;
}

void ScalarHankelMatrix::completeContextR(ParseTree* context){
    auto it = r.begin();
    while(it!=r.end()){
        auto tree = *it;
        ParseTree *merged = context->mergeContext(*tree);
        double val;
        if((val=teacher.membership(*merged))<0){
            throw std::invalid_argument("Must return positive value for positive Hankel Matrix");
        }
        obs[tree].push_back(val);
        /*
        if(!checkTableComplete(tree)){
            auto itNew = rNew.begin();
            while(itNew!=rNew.end()){ //Remove tree if from rNew list if it's there.
                int& ind = *itNew;
                if(ind==(int)(it-r.begin())){
                    rNew.erase(itNew);
                    break;
                }
                itNew++;
            }
            sNew.push_back((int)(s.size()));
            s.push_back(tree);
            r.erase(it); //Remove tree from r list.
        }else{
            it++;
        }*/
        it++;
        delete (merged);
    }
}

vector<ParseTree> ScalarHankelMatrix::getSExtensions() const{
    vector<ParseTree> ans;
    for(auto c:alphabet){
        if(c.rank==0){
            ans.push_back(ParseTree(c.c));
        }
    }
    for(auto it = getSuffixIterator();it.hasNext();++it){
        ans.push_back(*it);
    }
    return ans;
}

void ScalarHankelMatrix::closeTable(){
    //printTable();
    while(true){
        Logger& logger = Logger::getLogger();
        logger.setLoggingLevel(Logger::LOG_DEBUG);
        logger << "closing " << "c:" << c.size() << " s:" << s.size() << " r:" << r.size() << logger.endline;
        if(c.empty()){ /*Adds an empty context */
            ParseTree t(1);
            pair<ParseTree*,ParseTree*> contextTreePair = t.makeContext({});
            SAFE_DELETE(contextTreePair.second);
            addContext(*contextTreePair.first);
            SAFE_DELETE(contextTreePair.first);
        }
        if(s.empty()){
            for(auto c:alphabet){
                if(c.rank==0){
                    addTree(ParseTree(c.c));
                }
            }
        }
        bool closed=true;
        for(auto currTree: getSExtensions()){
            if(!hasTree(currTree)){
                addTree(currTree);
                closed=false;
                break;
            }else{
                if(!checkTableComplete(currTree)){
                    cout << "elsing" << endl;
                    closed = false;
                    auto itR = r.begin();
                    while(itR!=r.end()){if(**itR==currTree){break;}itR++;}
                    auto tree = *itR;
                    if(itR==r.end()){throw std::exception();}
                    auto itNew = rNew.begin();
                    while(itNew!=rNew.end()){ //Remove tree if from rNew list if it's there.
                        int& ind = *itNew;
                        if(ind==(int)(itR-r.begin())){
                            rNew.erase(itNew);
                            break;
                        }
                        itNew++;
                    }
                    sNew.push_back((int)(s.size()));
                    s.push_back(tree);
                    r.erase(itR); //Remove tree from r list.
                }
            }
        }
        if(closed){
            return;
        }
    }
}

void ScalarHankelMatrix::completeContextS(ParseTree* context){
    for (auto tree: s) {
        ParseTree *merged = context->mergeContext(*tree);
        double val;
        if((val=teacher.membership(*merged))<0){
            throw std::invalid_argument("Must return positive value for positive Hankel Matrix");
        }
        obs[tree].push_back(val);
        delete (merged);
    }
}

int ScalarHankelMatrix::getZeroVecInd() const{
    for(int i=0;i<s.size();++i){
        auto t = s[i];
        vector<double> row = getRow(*t);
        bool allZero=true;
        for(auto elem: row){
            if(elem!=0){allZero=false; break;}
        }
        if(allZero){return i;}
    }
    return -1;
}

bool ScalarHankelMatrix::checkTableComplete(const ParseTree& tree){
    mat sMat = getSMatrix(false);
    mat currMat(2, c.size());
    bool zeroTree = true;
    vector<double> observation = getObs(tree);
    /*cout << endl << "-------------------" << endl;
    cout << "tree:" << *tree << endl;
    cout << "["; for(auto elem: observation){cout << elem << ",";} cout << "]" << endl;*/
    for(int i=0;i<c.size();++i){
        if(observation[i]!=0){zeroTree=false;}
        currMat(0,i) = observation[i];
    }
    for(int i=0; i<sMat.n_rows;++i){
        bool currElemInSZero = true;
        for(int j=0;j<c.size();++j){
            if(sMat(i,j)!=0){ currElemInSZero=false; }
            currMat(1,j) = sMat(i,j);
        }
        if(zeroTree && currElemInSZero){/*cout << "dependent" << endl;*/ return true;}
        if(!zeroTree && !currElemInSZero && arma::rank(currMat)==1){
            /*cout << currMat << endl;
            cout << arma::rank(currMat) << endl;
            cout << "dependent" << endl;*/
            return true;
        }
    }
    /*cout << "independent" << endl;*/
    return false;
}

void ScalarHankelMatrix::giveCounterExample(const ParseTree& counterExample){
    //throw std::invalid_argument("Not yet implemented");
    for(auto& prefix: counterExample.getAllPrefixes()){
        if(!hasTree(prefix)){
            addTree(prefix);
        }
    }
    for(auto& context: counterExample.getAllContexts()){
        if(!hasContext(*context)){
            addContext(*context);
        }
        SAFE_DELETE(context)
    }
    complete();
}

double ScalarHankelMatrix::getCoeff(const ParseTree& t1, const ParseTree& t2) const{
    vector<double> row1 = getRow(t1);
    vector<double> row2 = getRow(t2);
    for(int i=0;i<row1.size();++i){
        if(row2[i]!=0){
            return row1[i]/row2[i];
        }
    }
    return 1;
}

vector<double> ScalarHankelMatrix::getRow(const ParseTree& tree) const{
    for(auto treePtr: s){
        if(*treePtr==tree){
            auto it = obs.find(treePtr);
            if(it==obs.end()){
                throw std::invalid_argument("tree not in map");
            }
            return it->second;
        }
    }
    for(auto treePtr: r){
        if(*treePtr==tree){
            auto it = obs.find(treePtr);
            if(it==obs.end()){
                throw std::invalid_argument("tree not in map");
            }
            return it->second;
        }
    }
    throw std::invalid_argument("Tree not in map");
}

bool ScalarHankelMatrix::checkExtension(extension e1, extension e2, const ParseTree& context, double alpha){
    bool toAdd = true;
    ParseTree* mergedContext1 = context.mergeContext(e1.first);
    ParseTree* mergedContext2 = context.mergeContext(e2.first);
    //printTable();
    if(teacher.membership(*mergedContext2)==0){
        /*cout << "zero" << endl;
        cout << teacher.membership(*mergedContext2) << " , " << teacher.membership(*mergedContext1) << ", " << alpha <<
        endl;*/
        if(teacher.membership(*mergedContext1)==0){
            toAdd=false;
        }
    }else{
        cout << "else" << endl;
        if(ABS((teacher.membership(*mergedContext1)/teacher.membership(*mergedContext2))-alpha)<EPSILON){
            toAdd=false;
        }
    }
    if(toAdd){
        auto contextTreePair = e1.first.makeContext({e1.second});
        /*cout << endl << "---------------" << endl;
        cout << context << endl;
        cout << *contextTreePair.first << endl;*/
        auto newContext = context.mergeContext(*contextTreePair.first);
        //cout << "adding" << *newContext << endl;
        //printTable();
        addContext(*newContext);
        SAFE_DELETE(contextTreePair.first); SAFE_DELETE(contextTreePair.second);
        SAFE_DELETE(newContext);
    }
    SAFE_DELETE(mergedContext1); SAFE_DELETE(mergedContext2);
    return !toAdd;
}

void ScalarHankelMatrix::complete(){
    bool changed=false;
    do{
        changed = false;
        unsigned long currS = s.size();
        unsigned long currR = r.size();
        unsigned long currC = c.size();
        closeTable();
        makeConsistent();
        if(s.size()>currS || r.size()>currR || c.size()>currC){changed=true;}
    }while(changed);
}


void ScalarHankelMatrix::makeConsistent(){
    Logger& logger = Logger::getLogger();
    //closeTable();
    logger.setLoggingLevel(Logger::LOG_DETAILS);
    logger << "consistent" << logger.endline;
    //printTable();
    cout << "con" << endl;
    for(int i=0;i<s.size();++i){
        vector<const ParseTree*> treesInClass = getTreesInClass(i);
        vector<pair<const ParseTree*, const ParseTree*>> pairs = getPairsVec(treesInClass);
        cout << "total pairs:" << pairs.size() << endl;
        if(pairs.size()>0){
            cout << "extension size:" << getExtensions(*pairs[0].first).size() << endl;
        }
        for(int pair_ind=0;pair_ind<pairs.size();pair_ind++){
            auto treePair = pairs[pair_ind];
            double alpha = getCoeff(*treePair.first, *treePair.second);
            vector<extension> extensions1 = getExtensions(*treePair.first);
            vector<extension> extensions2 = getExtensions(*treePair.second);
            for(int j=0;j<extensions1.size();++j){
                for(auto context: c){
                    //TODO: DELETE
                    /*
                    cout << "****************" << endl;
                    cout << *context << endl;
                    cout << "****************" << endl;
                    */
                    if(!context->getIsContext()){
                        //cout << *context << endl;
                        throw std::invalid_argument("problem");
                    }
                    try{
                        /*cout << "[" << pair_ind << "/" << pairs.size() << "]" << endl;
                        cout << "[" << i << "/" << s.size() << "]" << endl;
                        cout << "[" << j << "/" << extensions1.size() << "]" << endl;*/
                        if(!checkExtension(extensions1[j], extensions2[j], *context, alpha)){
                            cout << "fincon" << endl;
                            return;
                        }
                        //cout << "finChecking" << endl;
                    }catch(invalid_argument& e){
                        cout << "alpha:" << alpha << endl;
                        cout << "first tree:" << *treePair.first << endl;
                        cout << "second tree:" << *treePair.second << endl;
                        printTable();
                        throw e;
                    }
                    //printTable();
                }
            }
        }
    }
    cout << "fincon noChange" << endl;
}

arma::vec ScalarHankelMatrix::getCoefficients(const ParseTree& tree, const arma::mat& s) const{ //CHANGE
    mat sMat = getSMatrix(false);
    mat currMat(2, c.size());
    vector<double> observation = getObs(tree);
    arma::vec ans = arma::zeros(this->s.size());
    for(int i=0;i<c.size();++i){
        currMat(0,i) = observation[i];
    }
    for(int i=0; i<sMat.n_rows;++i){
        for(int j=0;j<c.size();++j){
            currMat(1,j) = sMat(i,j);
        }
        if(arma::rank(currMat)==1){
            for(int j=0;j<c.size();++j){
                if(observation[j]!=0){
                    double factor = observation[j]/currMat(1,j);
                    ans[i] = factor;
                    break;
                }
            }
        }
    }
    return ans;
}

int ScalarHankelMatrix::getSObsInd(const ParseTree& tree) const{
    mat sMat = getSMatrix(false);
    mat currMat(2, c.size());
    vector<double> observation = getObs(tree);
    bool zeroVec = isZeroVec(observation);
    for(int i=0;i<c.size();++i){
        currMat(0,i) = observation[i];
    }
    for(int i=0; i<sMat.n_rows;++i){
        bool currRowIsZero=true;
        for(int j=0;j<c.size();++j){
            if(sMat(i,j)!=0){currRowIsZero=false;}
            currMat(1,j) = sMat(i,j);
        }
        if(zeroVec&&currRowIsZero){return i;}
        if(!zeroVec && !currRowIsZero && arma::rank(currMat)==1){
            return i;
        }
    }
    printTable();
    throw std::invalid_argument("Shouldn't happen");
}

vector<const ParseTree*> ScalarHankelMatrix::getTreesInClass(int ind) const{
    vector<const ParseTree*> ans;
    for(auto t: s){
        if(getSObsInd(*t)==ind){ans.emplace_back(t);}
    }
    for(auto t: r){
        if(getSObsInd(*t)==ind){ans.emplace_back(t);}
    }
    return ans;
}

vector<pair<const ParseTree*, const ParseTree*>> ScalarHankelMatrix::getPairsVec(vector<const ParseTree*> vec) const{
    vector<pair<const ParseTree*, const ParseTree*>> ans;

    for(int i=0;i<vec.size();++i){
        for(int j=i+1;j<vec.size();++j){
            ans.emplace_back(vec[i], vec[j]);
        }
    }

    return ans;
}

vector<pair<ParseTree, int>> ScalarHankelMatrix::getExtensions(const ParseTree& tree) const{
    vector<ParseTree> trees;
    for(auto tree: s){
        trees.emplace_back(*tree);
    }
#ifdef FULL_CONSISTENCY
    for(auto tree: r){
        trees.emplace_back(*tree);
    }
#endif
    return extendSet(tree, trees, alphabet);
}

bool ScalarHankelMatrix::checkIsSExtension(const ParseTree& t) const{
    for(auto subtree: t.getSubtrees()) {
        int subtreeInd = getIndInS(*subtree);
        if (subtreeInd < 0) {
            return false;
        }
    }
    return true;
}


MultiplicityTreeAcceptor ScalarHankelMatrix::getAcceptor() const{
    vector<MultiLinearMap> maps;
    vector<rankedChar> alphabetVec = getAlphabetVec();
    for(auto c: alphabetVec){
        maps.emplace_back(MultiLinearMap((int)(s.size()), c.rank));
    }
    MultiplicityTreeAcceptor ans(alphabet, s.size());
    for(auto currTree: s){
        rankedChar c = {currTree->getData(), (int)(currTree->getSubtrees().size())};
        unsigned int charInd = (unsigned int)(find(alphabetVec.begin(), alphabetVec.end(), c)-alphabetVec.begin());
        if(charInd>=alphabetVec.size()){
            throw std::invalid_argument("Character not in alphabet");
        }
        updateTransition(maps[charInd], *currTree, alphabetVec);
    }
    for(auto currTree: r){
        if(!checkIsSExtension(*currTree)){continue;}
        rankedChar c = {currTree->getData(), (int)(currTree->getSubtrees().size())};
        unsigned int charInd = (unsigned int)(find(alphabetVec.begin(), alphabetVec.end(), c)-alphabetVec.begin());
        if(charInd>=alphabetVec.size()){
            throw std::invalid_argument("Character not in alphabet");
        }
        updateTransition(maps[charInd], *currTree, alphabetVec);
    }
    for(unsigned int i=0;i<alphabetVec.size();++i){
        ans.addTransition(maps[i], alphabetVec[i]);
    }
    vector<float> lambdaVec;
    if(!c.empty()){
        for(auto treeS: s){
            vector<double> obs = getObs(*treeS);
            lambdaVec.push_back((float)(obs[0]));
        }
    }
    ans.setLambda(lambdaVec);
    return ans;
}

void ScalarHankelMatrix::updateTransition(MultiLinearMap& m, const ParseTree& t,
                                          const vector<rankedChar>& alphabetVec) const{
    if(this->s.size()==0 || c.size()==0){return;}
    vector<int> sIndices;
    for(auto subtree: t.getSubtrees()){
        int subtreeInd = getIndInS(*subtree);
        if(subtreeInd<0){
            printTable();
            cout << t << endl;
            throw std::invalid_argument("Subtree not in S");
        }
        sIndices.push_back(subtreeInd);
    }
    rankedChar c = {t.getData(), (int)(sIndices.size())};
    unsigned int charInd = (unsigned int)(find(alphabetVec.begin(), alphabetVec.end(), c)-alphabetVec.begin());
    if(charInd>=alphabetVec.size()){
        throw std::invalid_argument("Character not in alphabet");
    }
    int sInd = getSObsInd(t);
    float alpha = getCoeff(t, *s[sInd]);
    vector<int> mapParams;
    mapParams.push_back(-1);
    mapParams.insert(mapParams.end(), sIndices.begin(), sIndices.end());
    for(unsigned int i=0;i<s.size();++i){
        mapParams[0] = i;
        m.setParam(i==sInd?alpha:0, mapParams);
    }
}