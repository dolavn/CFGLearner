#include "ObservationTable.h"
#include <armadillo>


HankelMatrix::HankelMatrix(const MultiplicityTeacher& teacher):teacher(teacher), obs(){

}

void HankelMatrix::completeTree(ParseTree* tree){

}

void HankelMatrix::completeContext(ParseTree* context){

}

bool HankelMatrix::checkTableComplete(ParseTree* tree){
    return false;
}

void HankelMatrix::checkTableCompleteContext(ParseTree* tree){

}