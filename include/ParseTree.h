#ifndef GENEALIGNER_PARSETREE_H
#define GENEALIGNER_PARSETREE_H

#include <stack>
#include <string>
#include <vector>

class ParseTree{
private:
    typedef std::pair<ParseTree*, std::string> stackPair;
    long data;
    int size;
    void copy(const ParseTree& other);
    void clear();
    bool isContext;
    bool empty;
    static stackPair incStack(std::stack<stackPair>&, stackPair&);
    ParseTree* getSubtree(std::string);
    ParseTree* leftSubtree;
    ParseTree* rightSubtree;
    struct stackElem{
        ParseTree* curr;
        ParseTree* other;
        std::string str;
        stackElem(ParseTree* curr, ParseTree* other, std::string str):curr(curr),other(other),str(str){}
    };
public:
    class iterator{
    public:
        iterator(ParseTree&);
        bool hasNext();
        long operator*() const;
        long* operator->() const;
        iterator& operator++();
        iterator operator++(int){
            iterator ans(*this);
            ++(*this);
            return ans;
        }
    private:
        std::stack<stackPair> stack;
        ParseTree* currNode;
        std::string currStr;
    };
    class indexIterator{
    public:
        indexIterator(ParseTree&);
        bool hasNext();
        std::string operator*() const;
        indexIterator& operator++();
    private:
        std::stack<stackPair> stack;
        ParseTree* currNode;
        std::string currStr;
    };
    ParseTree();
    ParseTree(bool context,bool context2);
    ParseTree(long data);
    ParseTree(long data, bool context);
    ParseTree(const ParseTree& other);
    ParseTree& operator=(const ParseTree& other);
    ParseTree(ParseTree&& other) noexcept;
    ParseTree& operator=(ParseTree&& other) noexcept;
    const ParseTree& operator[](std::string);
    iterator getIterator();
    indexIterator getIndexIterator();
    void setRightPointer(ParseTree* right){rightSubtree = right;}
    void setLeftPointer(ParseTree* left){leftSubtree = left;}
    void removeLeftSubtree();
    void removeRightSubtree();
    bool hasRightSubtree(){return rightSubtree!=nullptr;}
    bool hasLeftSubtree(){return leftSubtree!=nullptr;}
    void setRightSubtree(const ParseTree&);
    void setLeftSubtree(const ParseTree&);
    void setData(long data){this->data = data;this->empty=false;}
    long getData() const{return this->data;}
    std::pair<ParseTree*,ParseTree*> makeContext(std::string);
    ~ParseTree();
};


#endif //GENEALIGNER_PARSETREE_H
