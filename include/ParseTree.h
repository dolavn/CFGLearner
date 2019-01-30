#ifndef GENEALIGNER_PARSETREE_H
#define GENEALIGNER_PARSETREE_H

#include <stack>
#include <string>
#include <vector>

class ParseTree{
private:
    typedef std::pair<ParseTree*, std::string> stackPair;
    bool empty;
    int data;
    int size;
    void copy(const ParseTree& other);
    void clear();
    bool isContext;
    std::string contextLoc;
    static stackPair incStack(std::stack<stackPair>&, stackPair&);
    ParseTree* getSubtree(std::string);
    ParseTree* leftSubtree;
    ParseTree* rightSubtree;
    struct stackElem{
        ParseTree* curr;
        const ParseTree* other;
        std::string str;
        stackElem(ParseTree* curr, const ParseTree* other, std::string str):curr(curr),other(other),str(std::move(str)){}
    };
public:
    class iterator{
    public:
        explicit iterator(ParseTree&);
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
        explicit indexIterator(ParseTree&);
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
    ParseTree(bool,std::string);
    explicit ParseTree(int);
    ParseTree(int, std::vector<ParseTree>);
    ParseTree(int, bool,std::string);
    ParseTree(const ParseTree&);
    ParseTree& operator=(const ParseTree&);
    ParseTree(ParseTree&&) noexcept;
    ParseTree& operator=(ParseTree&&) noexcept;
    ParseTree& operator[](std::string);
    friend bool operator==(const ParseTree&, const ParseTree&);
    inline friend bool operator!=(const ParseTree& lhs, const ParseTree& rhs){return !(lhs==rhs);}
    const ParseTree& getNode(std::string) const;
    std::vector<ParseTree*> getSubtrees() const;
    iterator getIterator();
    indexIterator getIndexIterator();
    indexIterator getLeafIterator();
    void setRightPointer(ParseTree* right){rightSubtree = right;}
    void setLeftPointer(ParseTree* left){leftSubtree = left;}
    void setRightSubtree(const ParseTree&);
    void setLeftSubtree(const ParseTree&);
    bool hasRightSubtree() const{return rightSubtree!=nullptr;}
    bool hasLeftSubtree() const{return leftSubtree!=nullptr;}
    void setData(int data){this->data = data;this->empty=false;}
    bool isLeaf() const;
    inline bool getIsContext() const{return isContext;}
    inline bool isEmpty() const{return empty;}
    int getData() const{return this->data;}
    std::string getContextLoc() const{return this->contextLoc;}
    std::pair<ParseTree*,ParseTree*> makeContext(std::string) const;
    ParseTree* mergeContext(const ParseTree&) const;
    ParseTree getSkeleton() const;
    ~ParseTree();
};

#endif //GENEALIGNER_PARSETREE_H
