#include <Python.h>
#include <iostream>
#include "Teacher.h"
#include "Learner.h"
#include "TreeAcceptor.h"
// This is the definition of a method

using namespace std;

static PyObject* addTree(PyObject *self, PyObject *args) {
    PyObject* tree;
    PyObject* positive;
    if (PyArg_UnpackTuple(args, "ref", 1, 2, &tree, &positive)) {
        int truth = PyObject_IsTrue(positive);
    }
    Py_RETURN_NONE;
}


static PyObject* learn(PyObject *self, PyObject *args) {
    ParseTree* a = PyMem_New(ParseTree,1);
    *a = ParseTree(2);
    std::cout <<  a->getData() << std::endl;
    PyMem_Free(a);
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
