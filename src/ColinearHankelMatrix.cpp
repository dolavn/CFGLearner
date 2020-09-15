#include "ColinearHankelMatrix.h"
#include "MultiplicityTeacher.h"
#include "MultiplicityTreeAcceptor.h"
#include "Logger.h"
#include "utility.h"
#include <algorithm>
using namespace std;

#define EPSILON (0.000001)

//#define FULL_CONSISTENCY


ColinearHankelMatrix::ColinearHankelMatrix(const MultiplicityTeacher& teacher):teacher(teacher),trees(),contexts(),
obs(),classesRepresentatives(),alphabet(teacher.getAlphabet()),numClasses(){
    if(teacher.getDefaultValue()<0){
        throw std::invalid_argument("Default value must be positive for positive Hankel Matrix");
    }
    Logger& logger = Logger::getLogger();
    logger.setLoggingLevel(Logger::LOG_DEBUG);
    logger << "Matrix Created";
    initTable();
}

void ColinearHankelMatrix::completeTree(const ParseTree& tree){
    if(hasTreeInTable(tree)){return;}
    Logger& logger = Logger::getLogger();
    logger.setLoggingLevel(Logger::LOG_DEBUG);
    obs[tree] = vector<double>();
    for(auto it=contexts.begin();it!=contexts.end();it++){
        ParseTree context = *it;
        ParseTree* merged = context.mergeContext(tree);
        vector<double>& observation = obs[tree];
        while(((int)observation.size())-1<(it-contexts.begin())){
            observation.push_back(teacher.getDefaultValue());
        }
        double val;
        if((val=teacher.membership(*merged))<0){
            delete(merged);
            throw std::invalid_argument("Must return positive value for positive Hankel Matrix");
        }
        obs[tree][it-contexts.begin()]=val;
        SAFE_DELETE(merged);
    }
    logger << "finish complete" << logger.endline;
}

bool ColinearHankelMatrix::hasTreeInTable(const ParseTree& tree) const{
    auto it = obs.find(tree);
    return !(it==obs.end());
}

void ColinearHankelMatrix::completeContext(const ParseTree& context){
    auto it = obs.begin();
    while(it!=obs.end()){
        auto& tree = it->first;
        ParseTree *merged = context.mergeContext(tree);
        double val;
        if((val=teacher.membership(*merged))<0){
            throw std::invalid_argument("Must return positive value for positive Hankel Matrix");
        }
        obs[tree].push_back(val);
        /*if(hasTree(tree) && getClassInd(tree)==-1){
            cout << tree << endl;
            throw std::invalid_argument("check");
        }*/
        it++;
        SAFE_DELETE(merged);
    }
}

vector<ParseTree> ColinearHankelMatrix::getExtensions() const{
    vector<ParseTree> ans;
    if(!trees.empty()){
        for(auto c:alphabet){
            if(c.rank==0){
                ans.emplace_back(ParseTree(c.c));
            }
        }
    }
    auto itWrapper = getSuffixIterator(); auto it = *itWrapper;
    for(;it.hasNext();++it){
        ans.push_back(*it);
    }
    return ans;
}

void ColinearHankelMatrix::initTable(){
    if(contexts.empty()){ /*Adds an empty context */
        ParseTree t(1);
        pair<ParseTree*,ParseTree*> contextTreePair = t.makeContext({});
        SAFE_DELETE(contextTreePair.second);
        addContext(*contextTreePair.first);
        SAFE_DELETE(contextTreePair.first);
    }
    if(trees.empty()){
        for(auto c:alphabet){
            if(c.rank==0){
                addTree(ParseTree(c.c));
            }
        }
    }
}

void ColinearHankelMatrix::closeTable(){
    //printTable();
    while(true){
        Logger& logger = Logger::getLogger();
        logger.setLoggingLevel(Logger::LOG_DEBUG);
        logger << "closing " << "contexts:" << contexts.size() << " trees:" << trees.size() << logger.endline;
        bool closed=true;
        for(int i=0;i<trees.size();++i){
            const ParseTree& tree = trees[i];
            if(getClassInd(tree)==-1){
                numClasses++;
                classesRepresentatives[numClasses-1]=i;
                closed = false;
            }
        }
        for(const ParseTree& tree: getExtensions()){
            if(getClassInd(tree)==-1){
                addTree(tree);
                closed=false;
            }
        }
        if(closed){
            return;
        }
    }
}

int ColinearHankelMatrix::getZeroVecInd() const{
    for(int i=0;i<numClasses;++i){
        int representativeInd = classesRepresentatives.find(i)->second;
        auto t = trees[representativeInd];
        vector<double> row = getRow(t);
        bool allZero=true;
        for(auto elem: row){
            if(elem!=0){allZero=false; break;}
        }
        if(allZero){/*cout << "zeroVec:" << i << endl;*/return i;}
    }
    return -1;
}

void ColinearHankelMatrix::giveCounterExample(const ParseTree& counterExample){
    for(auto& prefix: counterExample.getAllPrefixes()){
        if(!hasTree(prefix)){
            addTree(prefix);
        }
    }
    /*
    for(auto& context: counterExample.getAllContexts()){
        if(!hasContext(*context)){
            addContext(*context);
        }
        SAFE_DELETE(context)
    }*/
    complete();
}

double ColinearHankelMatrix::getCoeff(const ParseTree& t1, const ParseTree& t2) const{
    vector<double> row1 = getObsVec(t1);
    vector<double> row2 = getObsVec(t2);
    double cos = getCosVectors(row1, row2);
    if(abs(cos-1)>EPSILON){return -1;}
    return getCoeffLazy(t1, t2);
    /*
    bool allZero=true;
    double alpha=-1;
    for(int i=0;i<row1.size();++i){
        if(row2[i]!=0){
            allZero=false;
            if(alpha==-1){
               alpha=row1[i]/row2[i];
            }else{
                if(abs(alpha-(row1[i]/row2[i]))>EPSILON){
                    if(abs(alpha-(row1[i]/row2[i]))<EPSILON){
                        throw std::invalid_argument("exception");
                    }
                    alpha=-1;
                    break;
                }
            }
        }else{
            if(row1[i]!=0){alpha=-1; allZero=false; break;}
        }

    }
    if(allZero){alpha=1;}
    return alpha;
     */
}

double ColinearHankelMatrix::getCoeffLazy(const ParseTree& t1, const ParseTree& t2) const{
    vector<double> row1 = getRow(t1);
    vector<double> row2 = getRow(t2);
    for(int i=0;i<row1.size();++i){
        if(row2[i]!=0){
            return row1[i]/row2[i];
        }
    }
    return 1;
}

void ColinearHankelMatrix::addTree(const ParseTree& tree){
    if(tree.getIsContext()){
        throw invalid_argument("Can't add a context to S");
    }
    Logger& logger = Logger::getLogger();
    logger.setLoggingLevel(Logger::LOG_DEBUG);
    logger << "adding tree";
    logger << tree;
    if(hasTree(tree)){
        cout << tree << endl;
        printTable();
        throw invalid_argument("Tree already exists");
    }
    completeTree(tree);
    int newClass=-1;
    if(getClassInd(tree)==-1){
        newClass=numClasses;
        numClasses++;
    }
    trees.push_back(tree);
    classesRepresentatives[newClass]=trees.size()-1;
    /*optimize this*/
    //todo: optimize
    for(auto& extension: getExtensions(tree)){
        completeTree(extension.first);
    }
}

bool ColinearHankelMatrix::hasContext(const ParseTree& context) const{
    for(auto c: contexts){
        if(c==context){return true;}
    }
    return false;
}

bool ColinearHankelMatrix::hasTree(const ParseTree& tree) const{
    for(auto t: trees){
        if(t==tree){return true;}
    }
    return false;
}

void ColinearHankelMatrix::addContext(const ParseTree& context){
    if (!context.getIsContext()) {
        cout << context << endl;
        cout << context.getIsContext() << endl;
        throw invalid_argument("Tree must be a context to be added to C");
    }
    if(hasContext(context)){
        cout << context << endl;
        cout << contexts.size() << endl;
        throw invalid_argument("Already has this context");
    }
    Logger& logger = Logger::getLogger();
    logger.setLoggingLevel(Logger::LOG_DEBUG);
    logger << "add context" << logger.endline;
    contexts.push_back(context);
    completeContext(context);
}

vector<double> ColinearHankelMatrix::getRow(const ParseTree& tree) const{
    auto it = obs.find(tree);
    if(it==obs.end()){return {};}
    return it->second;
}

bool ColinearHankelMatrix::checkIsExtension(const ParseTree& tree) const{
    for(auto child: tree.getSubtrees()){
        if(!hasTree(*child)){return false;}
    }
    return true;
}

bool ColinearHankelMatrix::checkExtension(extension e1, extension e2, const ParseTree& context, double alpha){
    bool toAdd = true;
    ParseTree* mergedContext1 = context.mergeContext(e1.first);
    ParseTree* mergedContext2 = context.mergeContext(e2.first);
    if(teacher.membership(*mergedContext2)==0){
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
        auto newContext = context.mergeContext(*contextTreePair.first);
        addContext(*newContext);
        SAFE_DELETE(contextTreePair.first); SAFE_DELETE(contextTreePair.second);
        SAFE_DELETE(newContext);
    }
    SAFE_DELETE(mergedContext1); SAFE_DELETE(mergedContext2);
    return !toAdd;
}

void ColinearHankelMatrix::complete(){
    bool changed;
    do{
        changed = false;
        unsigned long currS = trees.size();
        unsigned long currC = contexts.size();
        closeTable();
        makeConsistent();
        if(trees.size()>currS ||  contexts.size()>currC){changed=true;}
    }while(changed);
}


bool ColinearHankelMatrix::makeZeroConsistent(){
    int zeroInd = getZeroVecInd();
    vector<int> treesInClass = getTreesInClass(zeroInd);
    for(int currInd: treesInClass){
        ParseTree& tree = trees[currInd];
        vector<extension> extensions = getExtensions(tree);
        for(auto& extension: extensions){
            for(auto& context: contexts){
                ParseTree* mergedContext = context.mergeContext(extension.first);
                bool toAdd=false;
                if(teacher.membership(*mergedContext)!=0){
                    toAdd=true;
                    auto contextTreePair = extension.first.makeContext({extension.second});
                    auto newContext = context.mergeContext(*contextTreePair.first);
                    addContext(*newContext);
                    SAFE_DELETE(contextTreePair.first); SAFE_DELETE(contextTreePair.second);
                    SAFE_DELETE(newContext);
                }
                SAFE_DELETE(mergedContext);
                if(toAdd){return false;}
            }
        }
    }
    return true;
}


void ColinearHankelMatrix::makeConsistent(){
    Logger& logger = Logger::getLogger();
    logger.setLoggingLevel(Logger::LOG_DETAILS);
    logger << "consistent" << logger.endline;
    //printTable();
    //cout << "con" << endl;
    if(!makeZeroConsistent()){
        //cout << "not zero consistent" << endl;
        return;
    }
    for(int i=0;i<trees.size();++i){ //TODO: modify, should only go over number of classes and not all trees
        vector<int> treesInClass = getTreesInClass(i);
        vector<pair<int, int>> pairs = getPairsVec(treesInClass);
        for(int pair_ind=0;pair_ind<pairs.size();pair_ind++){
            auto treePair = pairs[pair_ind];
            ParseTree& firstTree = trees[treePair.first]; ParseTree& secondTree = trees[treePair.second];
            double alpha = getCoeff(firstTree, secondTree);
            vector<extension> extensions1 = getExtensions(firstTree);
            vector<extension> extensions2 = getExtensions(secondTree);
            for(int j=0;j<extensions1.size();++j){
                for(auto& context: contexts){
                    if(!context.getIsContext()){
                        //cout << *context << endl;
                        throw std::invalid_argument("problem");
                    }
                    try{
                        if(!checkExtension(extensions1[j], extensions2[j], context, alpha)){
                            //cout << "fincon" << endl;
                            //printTable();
                            return;
                        }
                    }catch(invalid_argument& e){
                        printTable();
                        cout << "origContext:" << context << endl;
                        cout << "alpha:" << alpha << endl;
                        cout << "first tree:" << firstTree << endl;
                        cout << "second tree:" << secondTree << endl;
                        throw e;
                    }
                }
            }
        }
    }
    //cout << "fincon noChange" << endl;
}

int ColinearHankelMatrix::getClassInd(const ParseTree& tree) const{
    for(int i=0;i<numClasses;++i){
        auto it = classesRepresentatives.find(i);
        if(it==classesRepresentatives.end()){
            throw std::invalid_argument("Class doesn't have a representative");
        }
        int repInd = it->second;
        const ParseTree& currTree = trees[repInd];
        if(getCoeff(tree, currTree)>0){
            return i;
        }
    }
    return -1;
}

vector<int> ColinearHankelMatrix::getTreesInClass(int ind) const{
    vector<int> ans;
    for(int i=0;i<trees.size();++i){
        auto t = trees[i];
        if(getClassInd(t)==ind){ans.emplace_back(i);}
    }
    return ans;
}

vector<pair<int, int>> ColinearHankelMatrix::getPairsVec(vector<int> vec) const{
    vector<pair<int, int>> ans;

    for(int i=0;i<vec.size();++i){
        for(int j=i+1;j<vec.size();++j){
            ans.emplace_back(vec[i], vec[j]);
        }
    }

    return ans;
}

void ColinearHankelMatrix::printTable() const{
    cout << "contexts" << endl;
    for(auto& c: contexts){
        cout << c << endl;
    }
    cout << "trees:" << endl;
    for(auto& t: trees){
        cout << t << " : ";
        vector<double> currObs = obs.find(t)->second;
        cout << "["; for(auto elem: currObs){cout << elem << ",";} cout << "]";
        cout << " " << getClassInd(t);
        int classTreeInd = classesRepresentatives.find(getClassInd(t))->second;
        cout << " coeff:" << getCoeff(t, trees[classTreeInd]);
        cout << endl;
    }
    cout << "extensions" << endl;
    for(auto& t: getExtensions()){
        cout << t << " : ";
        vector<double> currObs = obs.find(t)->second;
        cout << "["; for(auto elem: currObs){cout << elem << ",";} cout << "]";
        cout << " " << getClassInd(t);
        int classTreeInd = classesRepresentatives.find(getClassInd(t))->second;
        cout << " coeff:" << getCoeff(t, trees[classTreeInd]);
        cout << endl;
    }
    cout << "classes:" << numClasses << endl;
    for(auto elem: classesRepresentatives){
        cout << elem.first << " ->" << elem.second << endl;
    }
}

vector<pair<ParseTree, int>> ColinearHankelMatrix::getExtensions(const ParseTree& tree) const{
    return extendSet(tree, trees, alphabet);
}

vector<double> ColinearHankelMatrix::getObsVec(const ParseTree& tree) const{
    vector<double> row = getRow(tree); if(!row.empty()){return row;}
    vector<double> ans;
    for(const ParseTree& context: contexts){
        ParseTree *merged = context.mergeContext(tree);
        double val;
        if((val=teacher.membership(*merged))<0){
            throw std::invalid_argument("Must return positive value for positive Hankel Matrix");
        }
        ans.push_back(val);
    }
    return ans;
}

void ColinearHankelMatrix::updateTransition(MultiLinearMap& m, const ParseTree& t,
                                          const vector<rankedChar>& alphabetVec) const{
    if(trees.size()==0 || contexts.size()==0){return;}
    vector<int> sIndices;
    vector<float> alphas;
    for(auto subtree: t.getSubtrees()){
        int subtreeInd = getClassInd(*subtree);
        if(subtreeInd<0){
            printTable();
            cout << t << endl;
            throw std::invalid_argument("Subtree not in trees");
        }
        sIndices.push_back(subtreeInd);
        int representativeInd = classesRepresentatives.find(subtreeInd)->second;
        alphas.push_back(getCoeff(*subtree, trees[representativeInd]));
    }
    float deriv = 1; for(auto a: alphas){deriv=deriv*a;}
    rankedChar c = {t.getData(), (int)(sIndices.size())};
    unsigned int charInd = (unsigned int)(std::find(alphabetVec.begin(), alphabetVec.end(), c)-alphabetVec.begin());
    if(charInd>=alphabetVec.size()){
        throw std::invalid_argument("Character not in alphabet");
    }
    int classInd = getClassInd(t);
    int representativeInd = classesRepresentatives.find(classInd)->second;
    float alpha = getCoeff(t, trees[representativeInd])/deriv;
    vector<int> mapParams;
    mapParams.push_back(-1);
    mapParams.insert(mapParams.end(), sIndices.begin(), sIndices.end());
    /*if(alpha!=0 && classInd!=getZeroVecInd()){
        cout << "[" << classInd << ","; for(int i=1;i<=t.getChildrenNum();++i){cout << mapParams[i] << ",";}
        cout << "]" << alpha << "=" << getCoeff(t,trees[representativeInd]) << "/" << deriv << endl;
    }*/
    for(unsigned int i=0;i<numClasses;++i){
        mapParams[0] = i;
        m.setParam(i==classInd?alpha:0, mapParams);
    }
}


MultiplicityTreeAcceptor ColinearHankelMatrix::getAcceptor() const{
    vector<MultiLinearMap> maps;
    vector<rankedChar> alphabetVec = getAlphabetVec();
    for(auto c: alphabetVec){
        maps.emplace_back(MultiLinearMap(numClasses, c.rank));
    }
    MultiplicityTreeAcceptor acc(alphabet, numClasses);
    for(auto& currTree: trees){
        rankedChar c = {currTree.getData(), (int)(currTree.getSubtrees().size())};
        unsigned int charInd = (unsigned int)(std::find(alphabetVec.begin(), alphabetVec.end(), c)-alphabetVec.begin());
        if(charInd>=alphabetVec.size()){
            throw std::invalid_argument("Character not in alphabet");
        }
        updateTransition(maps[charInd], currTree, alphabetVec);
    }
    for(auto& currTree: getExtensions()){
        rankedChar c = {currTree.getData(), (int)(currTree.getSubtrees().size())};
        unsigned int charInd = (unsigned int)(std::find(alphabetVec.begin(), alphabetVec.end(), c)-alphabetVec.begin());
        if(charInd>=alphabetVec.size()){
            throw std::invalid_argument("Character not in alphabet");
        }
        updateTransition(maps[charInd], currTree, alphabetVec);
    }
    for(unsigned int i=0;i<alphabetVec.size();++i){
        acc.addTransition(maps[i], alphabetVec[i]);
    }
    vector<float> lambdaVec;
    if(!contexts.empty()){
        for(int i=0;i<numClasses;++i){
            int reprInd = classesRepresentatives.find(i)->second;
            const ParseTree& repr = trees[reprInd];
            vector<double> obs = getObsVec(repr);
            lambdaVec.push_back((float)(obs[0]));
        }
    }
    acc.setLambda(lambdaVec);
    //cout << "aut dim:" << acc.getDim() << endl;
    return acc;

}

IteratorWrapper ColinearHankelMatrix::getSuffixIterator() const{
    return IteratorWrapper(trees, this->alphabet, 1);
}

vector<rankedChar> ColinearHankelMatrix::getAlphabetVec() const{
    vector<rankedChar> alphabetVec;
    for(auto c: alphabet){
        alphabetVec.push_back(c);
    }
    return alphabetVec;
}

bool isZeroVec(vector<double>& vec){
    for(auto elem: vec){if(elem!=0){return false;}}
    return true;
}

double getNorm(vector<double>& vec){
    double sum = 0;
    for(auto elem: vec){sum=sum+(elem*elem);}
    return sqrt(sum);
}

double getCosVectors(vector<double>& v1, vector<double>& v2){
    if(v1.size()!=v2.size()){throw std::invalid_argument("Vectors must be of the same size");}
    if(v1.size()==0){return 1;}
    if(isZeroVec(v1) && isZeroVec(v2)){return 1;}
    if(isZeroVec(v1) || isZeroVec(v2)){return 0;}
    double sum = 0;
    for(int i=0;i<v1.size();++i){
        sum = sum + v1[i]*v2[i];
    }
    return sum/(getNorm(v1)*getNorm(v2));
}

vector<pair<ParseTree, int>> extendSet(const ParseTree& tree, vector<ParseTree> treeSet, set<rankedChar> alphabet){
    vector<pair<ParseTree,int>> ans;
    for(auto c: alphabet){
        if(c.rank==0){continue;}
        IndexArray arr(vector<int>(c.rank-1, treeSet.size()));
        while(!arr.getOverflow()){
            for(int k=0;k<c.rank;++k){
                vector<ParseTree> children;
                ParseTree currTree(c.c);
                currTree.setSubtree(tree, k);
                for(int currInd=0;currInd<c.rank-1;++currInd){
                    int treeIndex = arr[currInd];
                    ParseTree& child = treeSet[treeIndex];
                    currTree.setSubtree(child, currInd+(currInd>=k?1:0));
                }
                ans.emplace_back(currTree, k);
            }
            ++arr;
        }
    }
    return ans;
}