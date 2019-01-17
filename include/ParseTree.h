#ifndef GENEALIGNER_PARSETREE_H
#define GENEALIGNER_PARSETREE_H

#include <stack>
#include <string>
#include <vector>

class ParseTree{
private:
    typedef std::pair<ParseTree*, std::string> stackPair;
    int data;
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
        const ParseTree* other;
        std::string str;
    };
public:
    class iterator{
    public:
        iterator(ParseTree&);
        bool hasNext();
        int operator*() const;
        int* operator->() const;
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
        indexIterator(ParseTree&,bool);
        bool hasNext();
        std::string operator*() const;
        indexIterator& operator++();
        indexIterator operator++(int){
            indexIterator ans(*this);
            ++(*this);
            return ans;
        }
    private:
        std::stack<stackPair> stack;
        ParseTree* currNode;
        std::string currStr;
        bool leaves;
    };
    ParseTree();
    ParseTree(bool context,bool context2);
    ParseTree(int data);
    ParseTree(int data, std::vector<ParseTree>);
    ParseTree(int data, bool context);
    ParseTree(const ParseTree& other);
    ParseTree& operator=(const ParseTree& other);
    ParseTree(ParseTree&& other) noexcept;
    ParseTree& operator=(ParseTree&& other) noexcept;
    ParseTree& operator[](std::string);
    const ParseTree& getNode(std::string) const;
    iterator getIterator();
    indexIterator getIndexIterator();
    indexIterator getLeafIterator();
    void setRightPointer(ParseTree* right){rightSubtree = right;}
    void setLeftPointer(ParseTree* left){leftSubtree = left;}
    void setRightSubtree(const ParseTree&);
    void setLeftSubtree(const ParseTree&);
    bool hasRightSubtree() const{return rightSubtree!=nullptr;}
    bool hasLeftSubtree() const{return leftSubtree!=nullptr;}
    void setData(long data){this->data = data;this->empty=false;}
    bool isLeaf() const;
    inline bool isEmpty() const{return empty;}
    int getData() const{return this->data;}
    std::pair<ParseTree*,ParseTree*> makeContext(std::string) const;
    ParseTree getSkeleton() const;
    ~ParseTree();
};


#endif //GENEALIGNER_PARSETREE_H
