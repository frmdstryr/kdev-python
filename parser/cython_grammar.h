#pragma once
#include "python_header.h"
namespace Cython {

#define Py_GRAMMAR_GET(mod, name) \
    ast_##name = PyObject_GetAttrString(mod, #name); \
    Q_ASSERT(ast_##name)

class Grammar {
public:
    Grammar() {
        PyObject* ModuleNode = PyImport_ImportModule("Cython.Compiler.ModuleNode");
        PyObject* Nodes = PyImport_ImportModule("Cython.Compiler.Nodes");


        Py_XDECREF(ModuleNode);
        Py_XDECREF(Nodes);
    }

    ~Grammar() {

    }

};

} // end namespace Cython
