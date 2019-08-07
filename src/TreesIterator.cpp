#include "TreesIterator.h"
#include "ParseTree.h"


using namespace std;

TreesIterator::TreesIterator(vector<ParseTree*> prevLevel, set<rankedChar> alphabet, int maxLevel):prevLevels(prevLevel.size()),
alphabet(),currChar(-1),remainingLevels(maxLevel), arr(){
    for(unsigned int i=0;i<prevLevel.size();++i){
        this->prevLevels[i] = treeLevelPair(new ParseTree(*prevLevel[i]), remainingLevels);
    }
    for(auto c: alphabet){
        this->alphabet.push_back(c);
    }
    incChar();
    createNewTree();
}

TreesIterator::TreesIterator(set<rankedChar> alphabet, int maxLevel):prevLevels(),alphabet(),
currChar(-1),remainingLevels(maxLevel), arr(){
    for(auto c: alphabet){
        this->alphabet.push_back(c);
    }
    incChar();
    createNewTree();
}

TreesIterator::TreesIterator(const TreesIterator& other):prevLevels(), currLevel(), alphabet(other.alphabet), currChar(other.currChar),
remainingLevels(other.remainingLevels),arr(other.arr){
    copy(other);
}

TreesIterator& TreesIterator::operator=(const TreesIterator& other){
    if(this==&other){
        return *this;
    }
    clear();
    copy(other);
    alphabet = other.alphabet; currChar = other.currChar;
    remainingLevels = other.remainingLevels; arr = other.arr;
}

TreesIterator::TreesIterator(TreesIterator&& other):currLevel(std::move(other.currLevel)),
prevLevels(std::move(other.prevLevels)), alphabet(other.alphabet), remainingLevels(other.remainingLevels),arr(other.arr){

}

TreesIterator& TreesIterator::operator=(TreesIterator&& other){
    if(this==&other){
        return *this;
    }
    clear();
    prevLevels = std::move(other.prevLevels); currLevel = std::move(other.currLevel);
    alphabet = other.alphabet; currChar = other.currChar;
    remainingLevels = other.remainingLevels; arr = other.arr;
}

TreesIterator::~TreesIterator(){
    clear();
}

bool TreesIterator::hasNext() const{
    return remainingLevels>0;
}

ParseTree TreesIterator::operator*() const{
    if(currLevel.empty()){
        throw std::invalid_argument("No more trees!");
    }
    //printf("operator*\n");
    return *currLevel[currLevel.size()-1];
}

TreesIterator& TreesIterator::operator++(){
    //printf("operator++\n");
    do{
        ++arr;
        if(arr.getOverflow()){
            incChar();
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
    //printf("c new tree\n");
    auto ans = new ParseTree(alphabet[currChar].c);
    for(unsigned int i=0;i<alphabet[currChar].rank;++i) {
        ans->setSubtree(*prevLevels[arr.get(i)].first, i);
    }
    currLevel.push_back(ans);
}

void TreesIterator::incChar(){
    //printf("inc char\n");
    currChar++;
    for(;currChar<alphabet.size();++currChar){
        rankedChar& c = alphabet[currChar];
        //printf("c:%d\n c.rank==0 : %d\n prevLevel.empty(): %d\n", c.c, c.rank==0, prevLevel.empty());
        if(c.rank==0 == prevLevels.empty()){
            //printf("found\nremainingLevels:%d\n",remainingLevels);
            break;
        }
        //printf("next\n");
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
}

void TreesIterator::incLevel(){
    //printf("incLevel\n");
    for(auto t: currLevel){
        prevLevels.emplace_back(treeLevelPair(t, remainingLevels));
    }
    currLevel.clear();
    currChar = -1;
    remainingLevels--;
    incChar();
    createNewTree();
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
}