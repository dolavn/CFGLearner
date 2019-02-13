#ifndef GENEALIGNER_TREECONSTRUCTOR_H
#define GENEALIGNER_TREECONSTRUCTOR_H

#include <map>
#include <vector>
#include "Sequence.h"

class ParseTree;

typedef std::map<std::vector<int>, int> scoreTable;

namespace Trees{
    struct dpTable{
    public:
        struct dpRow{
        public:
            dpRow(const dpTable& table, int ind1);

            int& operator[](int ind2);
        private:
            int* table;
            int start;
        };
        dpTable();
        dpTable(int, int);
        dpTable(const dpTable& other);
        dpTable& operator=(const dpTable& other);
        dpTable(dpTable&& other) noexcept;
        dpTable& operator=(dpTable&& other) noexcept;
        ~dpTable();
        void printTable();
        dpRow operator[](int ind);

        int getVal(int, int) const;
        int getTrace(int, int) const;
    private:
        int* table;
        int len1;
        int len2;
        void clear();
        void copy(const dpTable& other);
    };
}

class TreeConstructor{
public:
    explicit TreeConstructor(scoreTable);
    int createTree(const Sequence&);
    ParseTree* getTree();
private:
    scoreTable table;
    Trees::dpTable dpTable;
    Sequence seq;

    ParseTree* traceback(const Trees::dpTable&, const Sequence&);
};

#endif //GENEALIGNER_TREECONSTRUCTOR_H
