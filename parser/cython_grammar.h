#pragma once
#include "python_header.h"
namespace Cython {

#define Py_GRAMMAR_GET(mod, name) \
    ast_##name = PyObject_GetAttrString(mod, #name); \
    Q_ASSERT(ast_##name)

class Grammar {
public:
    PyObject* ast_ModuleNode;
    PyObject* ast_Node;
    PyObject* ast_SingleAssignmentNode;
    PyObject* ast_StatListNode;

    PyObject* ast_NameNode;
    PyObject* ast_IntNode;
    PyObject* ast_StringNode;


    PyObject* Nodes;
    PyObject* ExprNodes;

    Grammar() {
        ast_ModuleNode = PyImport_ImportModule("Cython.Compiler.ModuleNode");
        Nodes = PyImport_ImportModule("Cython.Compiler.Nodes");
        if (Nodes) {
            Py_GRAMMAR_GET(Nodes, Node);
            Py_GRAMMAR_GET(Nodes, SingleAssignmentNode);
            Py_GRAMMAR_GET(Nodes, StatListNode);
        }
        ExprNodes = PyImport_ImportModule("Cython.Compiler.ExprNodes");
        if (ExprNodes) {
            Py_GRAMMAR_GET(ExprNodes, NameNode);
            Py_GRAMMAR_GET(ExprNodes, IntNode);
            Py_GRAMMAR_GET(ExprNodes, StringNode);
        }
    }

    ~Grammar() {
        Py_XDECREF(ast_ModuleNode);

        Py_XDECREF(ast_Node);
        Py_XDECREF(ast_SingleAssignmentNode);
        Py_XDECREF(ast_StatListNode);

        Py_XDECREF(ast_NameNode);
        Py_XDECREF(ast_IntNode);
        Py_XDECREF(ast_StringNode);

        Py_XDECREF(Nodes);
        Py_XDECREF(ExprNodes);
    }

};

#undef Py_GRAMMAR_GET

} // end namespace Cython
