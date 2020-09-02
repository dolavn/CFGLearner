#include "TreesIterator.h"
#include "ParseTree.h"


using namespace std;

TreesIterator::TreesIterator(vector<ParseTree*> prevLevel, set<rankedChar> alphabet, int maxLevel):
currLevel(), prevLevels(prevLevel.size()), alphabet(),currChar(-1),maxLevel(maxLevel),remainingLevels(maxLevel),
arr(){
    for(unsigned int i=0;i<prevLevel.size();++i){
        this->prevLevels[i] = treeLevelPair(new ParseTree(*prevLevel[i]), remainingLevels+1);
    }
    for(auto c: alphabet){
        this->alphabet.push_back(c);
    }
    incChar();
    createNewTree();
}

TreesIterator::TreesIterator(set<rankedChar> alphabet, int maxLevel):currLevel(), prevLevels(),alphabet(),
currChar(-1),maxLevel(maxLevel),remainingLevels(maxLevel), arr(){
    for(auto c: alphabet){
        this->alphabet.push_back(c);
    }
    //verbose = true; //todo: delete
    incChar();
    createNewTree();
}

TreesIterator::TreesIterator(const TreesIterator& other):currLevel(), prevLevels(), alphabet(other.alphabet),
currChar(other.currChar),maxLevel(other.maxLevel),remainingLevels(other.remainingLevels),arr(other.arr){
    copy(other);
    verbose = other.verbose; //todo:delete
}

TreesIterator& TreesIterator::operator=(const TreesIterator& other){
    if(this==&other){
        return *this;
    }
    clear();
    copy(other);
    alphabet = other.alphabet; currChar = other.currChar;
    maxLevel = other.maxLevel; remainingLevels = other.remainingLevels;
    arr = other.arr;
    verbose = other.verbose; //todo:delete;
}

TreesIterator::TreesIterator(TreesIterator&& other):currLevel(std::move(other.currLevel)),
prevLevels(std::move(other.prevLevels)), alphabet(other.alphabet), maxLevel(other.maxLevel),
remainingLevels(other.remainingLevels),arr(other.arr){
    verbose = other.verbose; //todo:delete;
}

TreesIterator& TreesIterator::operator=(TreesIterator&& other){
    if(this==&other){
        return *this;
    }
    clear();
    prevLevels = std::move(other.prevLevels); currLevel = std::move(other.currLevel);
    alphabet = other.alphabet; currChar = other.currChar;
    maxLevel = other.maxLevel; remainingLevels = other.remainingLevels;
    arr = other.arr;
    verbose = other.verbose; //todo:delete;
}

TreesIterator::~TreesIterator(){
    clear();
}

void TreesIterator::resetIterator() {
    if(verbose){
        cout << "reset" << endl;
    }
    clearVec(currLevel);
    currChar = -1;
    remainingLevels = maxLevel;
    for(auto it = prevLevels.begin();it!=prevLevels.end();){
        treeLevelPair& curr = *it;
        if(curr.second==remainingLevels+1){
            ++it;
        }else{
            if(curr.first){
                delete(curr.first);
                curr.first = nullptr;
            }
            prevLevels.erase(it);
        }
    }
    incChar();
    createNewTree();
}

bool TreesIterator::hasNext() const{
    return remainingLevels>0;
}

set<rankedChar> TreesIterator::getAlphabet() const{
    set<rankedChar> ans = set<rankedChar>(alphabet.begin(), alphabet.end());
    return ans;
}

ParseTree TreesIterator::operator*() const{
    if(currLevel.empty()){
        throw std::invalid_argument("No more trees!");
    }
    if(verbose){printf("operator*\n");}
    return *currLevel[currLevel.size()-1];
}

TreesIterator& TreesIterator::operator++(){
    if(verbose){printf("operator++\n");}
    do{
        if(verbose){cout << arr << endl;}
        /*if(verbose){
            for(int i=0;i<prevLevels.size();++i){
                cout << i << " " << *prevLevels[i].first << endl;
            }
        }*/
        ++arr;
        if(arr.getOverflow()){
            //cout << "overflow" << alphabet[currChar].c << endl;
            incChar();
            //todo: check it
            //if(alphabet[currChar].rank==0){break;}
        }
    }while(!checkIndex(arr));
    createNewTree();
    return *this;
}

bool TreesIterator::checkIndex(const IndexArray& arr){
    for(int i=0;i<arr.getDimensions();++i){
        int ind = arr.get(i);
        treeLevelPair p = prevLevels[ind];
        if(p.second==remainingLevels+1){return true;}
    }
    return false;
}

void TreesIterator::createNewTree(){
    if(!hasNext()){return;}
    if(verbose){printf("c new tree\n");}
    auto ans = new ParseTree(alphabet[currChar].c);
    for(unsigned int i=0;i<alphabet[currChar].rank;++i) {
        ans->setSubtree(*prevLevels[arr.get(i)].first, i);
    }
    if(verbose){
        cout << "t " << *ans << endl;
    }
    currLevel.push_back(ans);
}

void TreesIterator::incChar(){
    if(verbose){printf("inc char\n");}
    currChar++;
    for(;currChar<alphabet.size();++currChar){
        rankedChar& c = alphabet[currChar];
        if(verbose){printf("c:%d\n c.rank==0 : %d\n prevLevel.empty(): %d\n", c.c, c.rank==0, prevLevels.empty());}
        if(c.rank==0 == prevLevels.empty()){
            /*Either it's the first level and we reached a character with arity 0, or it's an
             * advanced level and we reached a character with arity>0
             */
            if(verbose){cout << "{" << c.c << " , " << c.rank << "}" << endl;}
            //if(verbose){printf("found\nremainingLevels:%d\n",remainingLevels);}
            break;
        }
        //if(verbose){printf("next\n");}
    }
    if(currChar<alphabet.size()){
        vector<int> dim;
        for(int i=0;i<alphabet[currChar].rank;++i){
            dim.push_back(int(prevLevels.size()));
        }
        arr = IndexArray(dim);
    }else{
        incLevel();
    }
    if(verbose){printf("inc char fin\n");}
}

void TreesIterator::incLevel(){
    if(verbose){printf("incLevel\n");}
    for(auto t: currLevel){
        prevLevels.emplace_back(treeLevelPair(t, remainingLevels));
    }
    currLevel.clear();
    currChar = -1;
    remainingLevels--;
    incChar();
    if(verbose){printf("inc level fin\n");}
}

void TreesIterator::copyVec(const vector<ParseTree*>& orig, vector<ParseTree*>& dest){
    for(auto t: orig){
        dest.push_back(new ParseTree(*t));
    }
}

void TreesIterator::clearVec(vector<ParseTree*>& vec){
    for(auto& t: vec){
        if(t){
            delete(t);
            t = nullptr;
        }
    }
    vec.clear();
}


void TreesIterator::copy(const TreesIterator& other){
    copyVec(other.currLevel, currLevel);
    for(auto p: other.prevLevels){
        prevLevels.emplace_back(new ParseTree(*p.first), p.second);
    }
}

void TreesIterator::clear(){
    clearVec(currLevel);
    for(auto& p: prevLevels){
        if(p.first){
            delete(p.first);
            p.first = nullptr;
        }
    }
    prevLevels.clear();
}

IteratorWrapper::IteratorWrapper(const vector<ParseTree>& trees, set<rankedChar> alphabet, int maxLevel):copies(), it(){
    for(auto tree: trees){
        copies.push_back(new ParseTree(tree));
    }
    it = new TreesIterator(copies, alphabet, maxLevel);
}

IteratorWrapper::IteratorWrapper(const IteratorWrapper& other):copies(),it(){
    copy(other);
}

IteratorWrapper& IteratorWrapper::operator=(const IteratorWrapper& other){
    if(this==&other){return *this;}
    clear();
    copy(other);
    return *this;
}

IteratorWrapper::IteratorWrapper(IteratorWrapper&& other):copies(move(other.copies)),it(other.it){
    other.it=nullptr;
}

IteratorWrapper& IteratorWrapper::operator=(IteratorWrapper&& other){
    if(this==&other){return *this;}
    clear();
    copies = move(other.copies);
    it = other.it;
    other.it=nullptr;
    return *this;
}

IteratorWrapper::~IteratorWrapper(){
    clear();
}

void IteratorWrapper::copy(const IteratorWrapper& other){
    for(auto tree: other.copies){
        copies.push_back(new ParseTree(*tree));
    }
    it = new TreesIterator(*other.it);
}

void IteratorWrapper::clear(){
    for(auto& tree: copies){
        delete(tree); tree=nullptr;
    }
    delete(it); it=nullptr;
}

TreesIterator& IteratorWrapper::operator*(){
    return *it;
}