#include "MultiplicityTeacher.h"
#include "MultiplicityTreeAcceptor.h"
#include "TreesIterator.h"
#include "ParseTree.h"

using namespace std;

FunctionalMultiplicityTeacher::FunctionalMultiplicityTeacher(double epsilon, double defaultValue,
        function<double(const ParseTree&)>& func, TreesIterator& it):epsilon(epsilon),
        defaultValue(defaultValue), func(func), it(it){

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

ParseTree* FunctionalMultiplicityTeacher::equivalence(const MultiplicityTreeAcceptor& acc) const{
    it.resetIterator();
    while(it.hasNext()){
        ParseTree t = *it;
        double diff = acc.run(t)-func(t);
        if(diff>epsilon){
            return new ParseTree(t);
        }
        it++;
    }
    return nullptr;
}