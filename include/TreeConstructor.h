#ifndef GENEALIGNER_TREECONSTRUCTOR_H
#define GENEALIGNER_TREECONSTRUCTOR_H

#include <map>
#include <vector>
#include "Sequence.h"

class ParseTree;

typedef std::map<std::vector<int>, float> scoreTable;

namespace Trees{
    struct dpTable{
    public:
        /*
        struct dpRow{
        public:
            dpRow(const dpTable& table, int ind1);

            float& operator[](int ind2);
        private:
            float* table;
            int start;
        };*/
        dpTable();
        dpTable(int, int);
        /*
        dpTable(const dpTable& other);
        dpTable& operator=(const dpTable& other);
        dpTable(dpTable&& other) noexcept;
        dpTable& operator=(dpTable&& other) noexcept;
        ~dpTable();
         */
        void printTable();
        std::vector<float>& operator[](int ind);
        /*
        dpRow operator[](int ind);
*/
        float getVal(int, int) const;
        int getTrace(int, int) const;
    private:
        std::vector<std::vector<float>> table;
        int len1;
        int len2;
        /*
        void clear();
        void copy(const dpTable& other);*/
    };
}

class TreeConstructor{
public:
    explicit TreeConstructor(scoreTable);
    float createTree(const Sequence&);
    float createTree(const std::vector<int>&);
    ParseTree getTree();
    void setLambda(float);
    void setConcat(bool);

private:
    scoreTable table;
    Trees::dpTable calcTable;
    Sequence seq;
    Sequence originalSeq; //For duplications concat
    std::vector<std::pair<std::pair<int, int>, int>> duplications;
    float lambda;
    bool concatDuplications;

    std::vector<std::pair<int, int>> getDuplicationIndices() const;
    ParseTree* traceback(const Trees::dpTable&, const Sequence&);
    ParseTree getDuplicatedTree(int, int) const;
    ParseTree postProcessTree(const ParseTree&) const; //For duplications concat
    void concat(Trees::dpTable&);
};

#endif //GENEALIGNER_TREECONSTRUCTOR_H
