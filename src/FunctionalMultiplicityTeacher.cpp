#include "MultiplicityTeacher.h"

using namespace std;

FunctionalMultiplicityTeacher::FunctionalMultiplicityTeacher(double epsilon, double defaultValue,
        function<double(const ParseTree&)>& func, set<rankedChar>& alphabet, long maxSize):epsilon(epsilon),
        defaultValue(defaultValue), func(func), alphabet(alphabet), maxSize(maxSize){

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
    return nullptr;
}