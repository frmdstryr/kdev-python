/*
    SPDX-FileCopyrightText: 2022 Jairus Martin <jrm@codelv.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include "python_header.h"
namespace Enaml {

#define Py_GRAMMAR_GET(mod, name) \
    ast_##name = PyObject_GetAttrString(mod, #name); \
    Q_ASSERT(ast_##name)


class Grammar {
public:
    PyObject* ast_ASTNode;
    PyObject* ast_PragmaArg;
    PyObject* ast_Pragma;
    PyObject* ast_Module;
    PyObject* ast_PythonExpression;
    PyObject* ast_PythonModule;
    PyObject* ast_EnamlDef;
    PyObject* ast_ChildDef;
    PyObject* ast_ConstExpr;
    PyObject* ast_AliasExpr;
    PyObject* ast_FuncDef;
    PyObject* ast_AsyncFuncDef;
    PyObject* ast_OperatorExpr;
    PyObject* ast_Binding;
    PyObject* ast_ExBinding;
    PyObject* ast_StorageExpr;
    PyObject* ast_PositionalParameter;
    PyObject* ast_KeywordParameter;
    PyObject* ast_TemplateParameters;
    PyObject* ast_Template;
    PyObject* ast_TemplateArguments;
    PyObject* ast_TemplateIdentifiers;
    PyObject* ast_TemplateInst;
    PyObject* ast_TemplateInstBinding;

    Grammar() {
        PyObject* mod = PyImport_ImportModule("enaml.core.enaml_ast");
        if (mod) {
            Py_GRAMMAR_GET(mod, ASTNode);
            Py_GRAMMAR_GET(mod, PragmaArg);
            Py_GRAMMAR_GET(mod, Pragma);
            Py_GRAMMAR_GET(mod, Module);
            Py_GRAMMAR_GET(mod, PythonExpression);
            Py_GRAMMAR_GET(mod, PythonModule);
            Py_GRAMMAR_GET(mod, EnamlDef);
            Py_GRAMMAR_GET(mod, ChildDef);
            Py_GRAMMAR_GET(mod, ConstExpr);
            Py_GRAMMAR_GET(mod, AliasExpr);
            Py_GRAMMAR_GET(mod, FuncDef);
            Py_GRAMMAR_GET(mod, AsyncFuncDef);
            Py_GRAMMAR_GET(mod, OperatorExpr);
            Py_GRAMMAR_GET(mod, Binding);
            Py_GRAMMAR_GET(mod, ExBinding);
            Py_GRAMMAR_GET(mod, StorageExpr);
            Py_GRAMMAR_GET(mod, PositionalParameter);
            Py_GRAMMAR_GET(mod, KeywordParameter);
            Py_GRAMMAR_GET(mod, TemplateParameters);
            Py_GRAMMAR_GET(mod, Template);
            Py_GRAMMAR_GET(mod, TemplateArguments);
            Py_GRAMMAR_GET(mod, TemplateIdentifiers);
            Py_GRAMMAR_GET(mod, TemplateInst);
            Py_GRAMMAR_GET(mod, TemplateInstBinding);
            Py_DECREF(mod);
        }
    }

    ~Grammar() {
        Py_XDECREF(ast_ASTNode);
        Py_XDECREF(ast_PragmaArg);
        Py_XDECREF(ast_Pragma);
        Py_XDECREF(ast_Module);
        Py_XDECREF(ast_PythonExpression);
        Py_XDECREF(ast_PythonModule);
        Py_XDECREF(ast_EnamlDef);
        Py_XDECREF(ast_ChildDef);
        Py_XDECREF(ast_ConstExpr);
        Py_XDECREF(ast_AliasExpr);
        Py_XDECREF(ast_FuncDef);
        Py_XDECREF(ast_AsyncFuncDef);
        Py_XDECREF(ast_OperatorExpr);
        Py_XDECREF(ast_Binding);
        Py_XDECREF(ast_ExBinding);
        Py_XDECREF(ast_StorageExpr);
        Py_XDECREF(ast_PositionalParameter);
        Py_XDECREF(ast_KeywordParameter);
        Py_XDECREF(ast_TemplateParameters);
        Py_XDECREF(ast_Template);
        Py_XDECREF(ast_TemplateArguments);
        Py_XDECREF(ast_TemplateIdentifiers);
        Py_XDECREF(ast_TemplateInst);
        Py_XDECREF(ast_TemplateInstBinding);
    }
};

}
