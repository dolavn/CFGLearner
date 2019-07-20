#include "MultiplicityTeacher.h"
#include "ParseTree.h"

SimpleMultiplicityTeacher::SimpleMultiplicityTeacher(double epsilon):epsilon(epsilon), trees(){

}

void SimpleMultiplicityTeacher::addExample(taggedTree t){
    trees.push_back(t);
}

double SimpleMultiplicityTeacher::membership(const ParseTree& tree){
    return 0;
}

example* SimpleMultiplicityTeacher::equivalence(const MultiplicityTreeAcceptor& acc){
    return nullptr;
}