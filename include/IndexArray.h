//
// Created by dolavn on 7/5/19.
//

#ifndef CFGLEARNER_INDEXARRAY_H
#define CFGLEARNER_INDEXARRAY_H
#include <vector>
#include <iostream>
#include <iterator>


/*
A class to hold a vector of indices, which can be used
to iterate over a multi-dimensional table.
*/

class IndexArray;

int cmp(const IndexArray& first, const IndexArray& second);
bool sameDimensions(const IndexArray& first, const IndexArray& second);

class IndexArray {
public:
    IndexArray(); //Default constructor, constructs empty index array.
    IndexArray(std::vector<int> maxArr);
    IndexArray(const IndexArray& other);
    IndexArray& operator=(const IndexArray& other);
    ~IndexArray();

    int getMax(int dim) const;
    std::vector<int> getIntVector() const{return arr;}
    /*Operators overloading*/
    IndexArray& operator++(); //prefix ++
    const IndexArray operator++(int); //postfix ++
    IndexArray& operator+=(const IndexArray& other);
    IndexArray& operator-=(const IndexArray& other);

    inline int& operator[](int ind) {return arr[ind];}
    inline int get(int ind) const{ return arr[ind]; }
    inline int getDimensions() const { return dimensions; }
    inline bool getOverflow() const { return overflow; }
    inline bool getUnderflow() const { return underflow; }

    std::vector<IndexArray> getNextIndices(int inc) const;
    void resetIndex();

    inline friend bool operator==(const IndexArray& first, const IndexArray& second) { return cmp(first, second) == 0; }
    inline friend bool operator<(const IndexArray& first, const IndexArray& second) { return cmp(first, second) < 0; }
    inline friend bool operator>(const IndexArray& first, const IndexArray& second) { return cmp(first, second) > 0; }
    inline friend bool operator<=(const IndexArray& first, const IndexArray& second) { return cmp(first, second) <= 0; }
    inline friend bool operator>=(const IndexArray& first, const IndexArray& second) { return cmp(first, second) >= 0; }
    inline friend bool operator!=(const IndexArray& first, const IndexArray& second) { return cmp(first, second) != 0; }
    inline friend IndexArray operator+(const IndexArray& first, const IndexArray& second) {
        IndexArray ans = first;
        ans += second;
        return ans;
    }
    inline friend IndexArray operator-(const IndexArray& first, const IndexArray& second) {
        IndexArray ans = first;
        ans -= second;
        return ans;
    }

    friend std::ostream& operator<<(std::ostream& stream, const IndexArray& arr);
private:
    unsigned int dimensions;
    std::vector<int> arr;
    std::vector<int> maxArr;
    bool overflow;
    bool underflow;


    friend bool sameDimensions(const IndexArray& first, const IndexArray& second);
    friend int cmp(const IndexArray& first, const IndexArray& second);
};


#endif //CFGLEARNER_INDEXARRAY_H
