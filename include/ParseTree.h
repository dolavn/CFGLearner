#ifndef GENEALIGNER_PARSETREE_H
#define GENEALIGNER_PARSETREE_H

#include <stack>
#include <string>

struct TreeNode{
private:
    TreeNode* rightSon;
    TreeNode* leftSon;
    long data;
    bool symbol;
public:
    TreeNode();
    TreeNode(long data);
    TreeNode(const TreeNode& other);
    TreeNode& operator=(const TreeNode& other);
    TreeNode(TreeNode&& other) noexcept;
    TreeNode& operator=(TreeNode&& other) noexcept;
    ~TreeNode();

    inline void setRightSon(TreeNode* son){rightSon = son;}
    inline void setLeftSon(TreeNode* son){leftSon = son;}
    inline void setData(long data){this->data = data;}

    inline TreeNode* getRightSon() const {return rightSon;}
    inline TreeNode* getLeftSon() const {return leftSon;}
    inline long getData() const{return data;}
    inline bool getIsSymbol() const{return symbol;}
};

class ParseTree{
public:
    class iterator{
    public:
        iterator(const ParseTree&);
        bool hasNext();
        std::pair<TreeNode&, std::string> operator*() const;
        TreeNode* operator->() const;
        iterator& operator++();
        iterator operator++(int){
            iterator ans(*this);
            ++(*this);
            return ans;
        }
    private:
        typedef std::pair<TreeNode*, std::string> stackPair;
        std::stack<stackPair> stack;
        TreeNode* currNode;
        std::string currStr;
        bool locIterator;
    };
    ParseTree();
    ParseTree(TreeNode* root);
    ParseTree(const ParseTree& other);
    ParseTree& operator=(const ParseTree& other);
    ParseTree(ParseTree&& other) noexcept;
    ParseTree& operator=(ParseTree&& other) noexcept;
    iterator getIterator();
    ~ParseTree();
private:
    TreeNode* root;
    int size;
    void copy(const ParseTree& other);
    void clear();
};

class Context: public ParseTree{
public:
    Context(ParseTree*, TreeNode*);
private:
    void removeSubtree(TreeNode*);
};

#endif //GENEALIGNER_PARSETREE_H
