//
// Created by דולב נתאי on 05/10/2018.
//

#include "../include/Tree.h"

template<typename T>
Tree<T>::Tree():root(nullptr){

}

template<typename T>
Tree<T>::Tree(const Tree<T>& other):root(new treeNode<T>(*other.root)){

}

/*
 * Declaring a tree class to prevent linking problems.
 */
void tempFunction(){
    Tree<int> tree;
}