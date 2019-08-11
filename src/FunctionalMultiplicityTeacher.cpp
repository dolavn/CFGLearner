#include "MultiplicityTeacher.h"
#include "MultiplicityTreeAcceptor.h"
#include "TreesIterator.h"
#include "ParseTree.h"
#include "utility.h"

using namespace std;

FunctionalMultiplicityTeacher::FunctionalMultiplicityTeacher(double epsilon, double defaultValue,
        function<double(const ParseTree&)> func, const TreesIterator& it):epsilon(epsilon),
        defaultValue(defaultValue), func(std::move(func)), it(it){

}

double FunctionalMultiplicityTeacher::membership(const ParseTree& tree) const{
    double ans;
    try{
        ans = func(tree);
    }catch(invalid_argument& e){
        ans = defaultValue;
    }
    return ans;
}

set<rankedChar> FunctionalMultiplicityTeacher::getAlphabet() const{
    return it.getAlphabet();
}

ParseTree* FunctionalMultiplicityTeacher::equivalence(const MultiplicityTreeAcceptor& acc) const{
    it.resetIterator();
    while(it.hasNext()){
        ParseTree t = *it;
        cout << t << endl;
        double diff = ABS(acc.run(t)-func(t));
        if(diff>epsilon){
            return new ParseTree(t);
        }
        ++it;
    }
    return nullptr;
}

string FunctionalMultiplicityTeacher::toString() const{
    return "FunctionalMultiplicityTeacher";
}