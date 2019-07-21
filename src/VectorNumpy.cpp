#include "VectorNumpy.h"

using namespace std;

VectorNumpy::VectorNumpy(ssize_t size):size(size),data(size){

}

VectorNumpy::VectorNumpy(vector<double> data):size(data.size()),data(data){

}

