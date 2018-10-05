//
// Created by Dolav Nitay
//

#ifndef TREE_H
#define TREE_H

#include <vector>

template <typename T>
struct treeNode{
    T data;
    std::vector<treeNode*> next;

    treeNode(T data):data(data),next(){}

    treeNode(const treeNode<T>& other):data(other.data),next(){
        copy(other);
    }

    treeNode& operator=(const treeNode<T>& other){
        if(this==&other){
            return *this;
        }
        clear();
        copy(other);
        return *this;
    }

    ~treeNode(){
        clear();
    }
private:
    void copy(const treeNode<T>& other){
        for(int i=0;i<other.next.size();++i){
            next.push_back(new treeNode(other.next[i]));
        }
    }

    void clear(){
        for(int i=0;i<next.size();++i){
            delete(next[i]);
        }
    }
};

template <typename T>
class Tree{
public:
    Tree();
    Tree(const Tree<T>& other);
private:
    treeNode<T>* root;
};

#endif //TREE_H
