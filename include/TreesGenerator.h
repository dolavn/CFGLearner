//
// Created by dolavn on 2/25/20.
//

#ifndef CFGLEARNER_TREESGENERATOR_H
#define CFGLEARNER_TREESGENERATOR_H

#include <vector>

class ParseTree;

class TreesGenerator{
public:
    virtual bool hasNext() const=0;
    virtual ParseTree getNext()=0;
};

class DuplicationsGenerator{
public:
    DuplicationsGenerator(std::vector<ParseTree*>, int);
    DuplicationsGenerator(const DuplicationsGenerator&);
    DuplicationsGenerator& operator=(const DuplicationsGenerator&);
    DuplicationsGenerator(DuplicationsGenerator&&);
    DuplicationsGenerator& operator=(DuplicationsGenerator&&);

    virtual bool hasNext() const;
    virtual ParseTree getNext();
private:
    void clear();
    void copy(const DuplicationsGenerator&);

    std::vector<ParseTree*> trees;
    int depth;
};

#endif //CFGLEARNER_TREESGENERATOR_H
