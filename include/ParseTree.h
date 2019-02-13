#ifndef GENEALIGNER_PARSETREE_H
#define GENEALIGNER_PARSETREE_H

#include <stack>
#include <string>
#include <vector>
#include <iostream>

class ParseTree{
private:
    typedef std::pair<ParseTree*, std::vector<int>> stackPair;
    bool empty;
    int data;
    int size;
    void copy(const ParseTree& other);
    void clear();
    bool isContext;
    std::vector<int> contextLoc;
    static stackPair incStack(std::stack<stackPair>&, stackPair&);
    ParseTree* getSubtree(const std::vector<int>&);
    std::vector<ParseTree*> subtrees;
    struct stackElem{
        ParseTree* curr;
        const ParseTree* other;
        std::vector<int> loc;
        stackElem(ParseTree* curr, const ParseTree* other, std::vector<int> loc):curr(curr),other(other),
        loc(std::move(loc)){}
    };
public:
    class iterator{
    public:
        explicit iterator(ParseTree&);
        bool hasNext();
        int operator*() const;
        int* operator->() const;
        iterator& operator++();
        const iterator operator++(int){
            iterator ans(*this);
            ++(*this);
            return ans;
        }
    private:
        std::stack<stackPair> stack;
        ParseTree* currNode;
        std::vector<int> currLoc;
    };
    class indexIterator{
    public:
        explicit indexIterator(ParseTree&);
        indexIterator(ParseTree&,bool);
        bool hasNext();
        std::vector<int> operator*() const;
        indexIterator& operator++();
        const indexIterator operator++(int){
            indexIterator ans(*this);
            ++(*this);
            return ans;
        }
    private:
        std::stack<stackPair> stack;
        ParseTree* currNode;
        std::vector<int> currLoc;
        bool leaves;
    };
    ParseTree();
    ParseTree(bool,std::vector<int>);
    explicit ParseTree(int);
    ParseTree(int, std::vector<ParseTree>);
    ParseTree(int, bool,std::vector<int>);
    ParseTree(const ParseTree&);
    ParseTree& operator=(const ParseTree&);
    ParseTree(ParseTree&&) noexcept;
    ParseTree& operator=(ParseTree&&) noexcept;
    ParseTree& operator[](const std::vector<int>&);
    friend bool operator==(const ParseTree&, const ParseTree&);
    inline friend bool operator!=(const ParseTree& lhs, const ParseTree& rhs){return !(lhs==rhs);}
    friend std::ostream& operator<<(std::ostream&,const ParseTree&);
    const ParseTree& getNode(const std::vector<int>&) const;
    std::vector<ParseTree*> getSubtrees() const;
    iterator getIterator();
    indexIterator getIndexIterator();
    indexIterator getLeafIterator();
    void setPointer(ParseTree* ptr, unsigned int ind){while(subtrees.size()<=ind){subtrees.push_back(nullptr);}subtrees[ind]=ptr;}
    void setSubtree(const ParseTree&, unsigned int ind);
    bool hasSubtree(unsigned int ind) const{return subtrees.size()>=ind+1 && subtrees[ind]!=nullptr;}
    void setData(int data){this->data = data;this->empty=false;}
    bool isLeaf() const;
    inline bool getIsContext() const{return isContext;}
    inline int getChildrenNum() const{return (int)(subtrees.size());}
    inline bool isEmpty() const{return empty;}
    int getData() const{return this->data;}
    std::size_t getHash() const;
    std::vector<int> getContextLoc() const{return this->contextLoc;}
    std::pair<ParseTree*,ParseTree*> makeContext(std::vector<int>) const;
    ParseTree* mergeContext(const ParseTree&) const;
    ParseTree getSkeleton() const;
    static bool isPrefix(const std::vector<int>&,const std::vector<int>&);
    ~ParseTree();
};

#endif //GENEALIGNER_PARSETREE_H
