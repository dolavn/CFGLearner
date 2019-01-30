#include <Python.h>
#include <iostream>
#include "Teacher.h"
#include "Learner.h"
#include "TreeAcceptor.h"
#include <vector>
#include <stack>
// This is the definition of a method

#define BRACKET_OPEN -1
#define BRACKET_CLOSE -2

using namespace std;

static ParseTree* parseTree(PyObject* tuple){
    ParseTree* ans = nullptr;
    if(!PyList_Check(tuple)){
        return ans;
    }
    Py_ssize_t size = PyList_Size(tuple);
    std::vector<int> seq(size);
    for (int i = 0; i < size; ++i) {
        PyObject *curr = PyList_GetItem(tuple, i);
        if(!PyLong_Check(curr)){
            return ans;
        }
        seq[i] = (int) (PyLong_AsLong(curr));
    }
    stack<ParseTree*> s;
    ans = new ParseTree(seq[0]);
    s.push(ans);
    for(unsigned int i=1;i<size;++i){
        if(s.empty()){ //error
            goto error;
        }
        ParseTree* curr = s.top();
        if(seq[i]==BRACKET_OPEN){
            if(curr->hasLeftSubtree() && curr->hasRightSubtree()){ //already has two sons
                goto error;
            }
            if(!curr->hasLeftSubtree()){
                ParseTree* left = new ParseTree(0);
                curr->setLeftPointer(left);
                s.push(left);
            }else{
                ParseTree* right = new ParseTree(0);
                curr->setRightPointer(right);
                s.push(right);
            }
            continue;
        }
        if(seq[i]==BRACKET_CLOSE){
            s.pop();
            continue;
        }
        curr->setData(seq[i]);
    }
    return ans;
    error:
    if(ans) {
        delete (ans);
        ans=nullptr;
    }
    return nullptr;
}

static PyObject* addTree(PyObject *self, PyObject *args) {
    PyObject* tree;
    PyObject* positive;
    if (PyArg_UnpackTuple(args, "ref", 2, 2, &tree, &positive)) {
        int truth = PyObject_IsTrue(positive);
        if(!truth){
        }
        ParseTree* pTree = parseTree(tree);
        if(!pTree){
            PyErr_SetString(PyExc_ValueError, "Invalid tree.");
            return NULL;
        }
        for(auto it = pTree->getIndexIterator();it.hasNext();++it){
            cout << (*pTree)[*it].getData() << endl;
        }
        delete(pTree);
    }else{
        PyErr_SetString(PyExc_ValueError, "Invalid number of parameters.");
        return NULL;
    }
    Py_RETURN_NONE;
}


static PyObject* learn(PyObject *self, PyObject *args) {
    return PyLong_FromLong(1);
}


// Exported methods are collected in a table
PyMethodDef method_table[] = {
        {"addTree", (PyCFunction) addTree, METH_VARARGS, "Method docstring"},
        {"learn", (PyCFunction) learn, METH_VARARGS, "Method docstring"},
        {NULL, NULL, 0, NULL} // Sentinel value ending the table
};

// A struct contains the definition of a module
PyModuleDef CFGLearner_module = {
        PyModuleDef_HEAD_INIT,
        "CFGLearner", // Module name
        "This is the module docstring",
        -1,   // Optional size of the module state memory
        method_table,
        NULL, // Optional slot definitions
        NULL, // Optional traversal function
        NULL, // Optional clear function
        NULL  // Optional module deallocation function
};

// The module init function
PyMODINIT_FUNC PyInit_CFGLearner(void) {
    return PyModule_Create(&CFGLearner_module);
}
