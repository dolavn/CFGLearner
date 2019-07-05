/****************************************/
/* IndexArray.cpp                       */
/* Copyright (c) Dolav Nitay            */
/****************************************/
#include "IndexArray.h"

using namespace std;

IndexArray::IndexArray() :arr(), maxArr(), dimensions(0), overflow(false), underflow(false) {

}
IndexArray::IndexArray(vector<int> maxArr) :arr(maxArr.size()),maxArr(maxArr),dimensions(maxArr.size()),overflow(false),underflow(false){
}

IndexArray::IndexArray(const IndexArray& other) : dimensions(other.dimensions),arr(other.arr),maxArr(other.maxArr),overflow(other.overflow),underflow(other.underflow){
}

IndexArray& IndexArray::operator=(const IndexArray& other) {
    if (this == &other) {
        return *this;
    }else {
        dimensions = other.dimensions;
        overflow = other.overflow;
        underflow = other.underflow;
        arr = other.arr;
        maxArr = other.maxArr;
        return *this;
    }
}

IndexArray::~IndexArray() {
}

int IndexArray::getMax(int dim) const{
    return maxArr[dim];
}

IndexArray& IndexArray::operator++() {
    int i = dimensions - 1;
    while (i >= 0 && arr[i] == maxArr[i] - 1) {
        arr[i--] = 0;
    }
    if (i >= 0) {
        arr[i]++;
    }else{
        overflow = true;
    }
    return *this;
}

const IndexArray IndexArray::operator++(int) {
    IndexArray ans(*this);
    ++(*this);
    return ans;
}

IndexArray& IndexArray::operator+=(const IndexArray& other) {
    if (!sameDimensions(*this, other)) {
        throw invalid_argument("Both operands must be of the same dimensions.");
    }
    int remainder = 0;
    int i = dimensions - 1;
    while (i >= 0) {
        int toAdd=other.get(i)+remainder;
        if (arr[i] + toAdd >= maxArr[i]) {
            remainder = 1;
            if (i == 0) {
                overflow = true;
            }
        }
        else {
            remainder = 0;
        }
        arr[i] = (arr[i] + toAdd) % (maxArr[i]);
        i--;
    }
    return *this;
}

IndexArray& IndexArray::operator-=(const IndexArray& other) {
    if (!sameDimensions(*this, other)) {
        throw invalid_argument("Both operands must be of the same dimensions.");
    }
    int remainder = 0;
    int i = dimensions - 1;
    while (i >= 0) {
        int toSub = other.get(i) + remainder;
        if (arr[i] - toSub <0) {
            remainder = 1;
            if (i == 0) {
                underflow = true;
            }
        }
        else {
            remainder = 0;
        }
        arr[i] = (maxArr[i] + arr[i] - toSub) % (maxArr[i]);
        i--;
    }
    return *this;
}

void IndexArray::resetIndex() {
    for (unsigned int i = 0; i < dimensions; ++i) {
        (*this)[i] = 0;
    }
    overflow = false;
    underflow = false;
}

vector<IndexArray> IndexArray::getNextIndices(int inc) const{
    vector<int> dim(dimensions, 2);
    vector<IndexArray> ans = vector<IndexArray>();
    IndexArray ind(dim);
    ind++;
    for (; !ind.getOverflow(); ++ind) { //O(2^k) - Not good for non-small inputs!
        bool isValid = true;
        IndexArray currInd(*this);
        for (unsigned int i = 0; i < dimensions && isValid; ++i) {
            int afterInc = currInd[i] + ind[i] * inc;
            if (afterInc >= maxArr[i] || afterInc < 0) {
                isValid = false;
            }
            else {
                currInd[i] = afterInc;
            }
        }
        if (isValid) { ans.push_back(currInd); }
    }
    return ans;
}

bool sameDimensions(const IndexArray& first, const IndexArray& second) {
    if (first.dimensions != second.dimensions) { return false; }
    for (unsigned int i = 0; i < first.dimensions; ++i) {
        if (first.getMax(i) != second.getMax(i)) { return false; }
    }
    return true;
}

ostream& operator<<(ostream& stream, const IndexArray& arr) {
    stream << "[";
    for (unsigned int i = 0; i < arr.dimensions; i++) {
        stream << arr.arr[i];
        if (i < arr.dimensions - 1) {
            stream << ",";
        }
    }
    stream << "]";
    return stream;
}