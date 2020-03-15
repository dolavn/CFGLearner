//
// Created by dolavn on 2/25/20.
//

#ifndef CFGLEARNER_TREESGENERATOR_H
#define CFGLEARNER_TREESGENERATOR_H

#include <vector>
#include "IndexArray.h"
#include "utility.h"

class ParseTree;

class TreesGenerator{
public:
    virtual bool hasNext() const=0;
    virtual ParseTree operator*()=0;
    virtual TreesGenerator& operator++()=0;
    virtual void reset()=0;
    virtual TreesGenerator* clone() const=0;

    virtual ~TreesGenerator()=default;
};


class PartitionsIterator{
public:
    PartitionsIterator();
    PartitionsIterator(int, int);
    PartitionsIterator(int, int, int);
    PartitionsIterator(int, int, int, int);

    PartitionsIterator& operator++();
    PartitionsIterator operator++(int){
        PartitionsIterator ans(*this);
        ++(*this);
        return ans;
    }

    typedef std::vector<int> partition;

    partition operator*();

    bool hasNext() const;
private:
    int elements;
    int sets;
    int maxInSet;
    int freeIndex;
    void checkState();
    bool testIndexVec(const IndexArray&);
    IndexArray indices;
};

class DuplicationsGenerator: public TreesGenerator{
public:
    DuplicationsGenerator(std::vector<ParseTree>, int);
    DuplicationsGenerator(std::vector<ParseTree*>, int);
    DuplicationsGenerator(const DuplicationsGenerator&);
    DuplicationsGenerator& operator=(const DuplicationsGenerator&);
    DuplicationsGenerator(DuplicationsGenerator&&);
    DuplicationsGenerator& operator=(DuplicationsGenerator&&);

    TreesGenerator* clone() const;
    virtual void reset();

    virtual ~DuplicationsGenerator();
    DuplicationsGenerator& operator++();
    DuplicationsGenerator operator++(int){
        DuplicationsGenerator ans(*this);
        ++(*this);
        return ans;
    }

    virtual bool hasNext() const;
    virtual ParseTree operator*();

private:
    enum direction{
        LEFT,
        RIGHT
    };

    void clear();
    void copy(const DuplicationsGenerator&);
    void createIterator();
    bool isDirectional();
    ParseTree createDuplication(const ParseTree&, PartitionsIterator::partition);

    std::vector<ParseTree*> trees;
    int currTree;
    int depth;
    PartitionsIterator currIt;
    direction directionsIterator;
};

class TreeConstructor;

class ConstructorGenerator: public TreesGenerator{
public:
    ConstructorGenerator(TreeConstructor&, int, int, std::vector<int>);
    ConstructorGenerator(const ConstructorGenerator&);
    ConstructorGenerator(ConstructorGenerator&&);
    ConstructorGenerator& operator=(const ConstructorGenerator&)=delete;
    ConstructorGenerator& operator=(ConstructorGenerator&&)=delete;
    virtual ~ConstructorGenerator();


    TreesGenerator* clone() const;
    virtual void reset();
    virtual bool hasNext() const;
    virtual ParseTree operator*();

    ConstructorGenerator& operator++();
    ConstructorGenerator operator++(int){
        ConstructorGenerator ans(*this);
        ++(*this);
        return ans;
    }
private:
    void copy(const ConstructorGenerator&);
    void clear();
    unsigned int generateSeqLen() const;
    unsigned int convertSampleToLen(unsigned int) const;
    std::vector<int> generateSeq() const;
    void generateTree();

    TreeConstructor& constructor;
    int maxLen;
    int numTrees;
    std::vector<int> alphabet;
    ParseTree* currTree;
    int treesGenerated;
};

#endif //CFGLEARNER_TREESGENERATOR_H
