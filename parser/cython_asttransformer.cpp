
#include "cython_asttransformer.h"
#include "astbuilder.h"

namespace Cython {

using Python::PyObjectRef;
using Python::PyUnicodeObjectToQString;
using Python::Ast;



void AstTransformer::run(PyObject* syntaxtree, QString moduleName)
{
    Q_ASSERT(PyObject_IsInstance(syntaxtree, cython.ast_ModuleNode));
    ast = new Python::CodeAst();
    ast->name = new Python::Identifier(moduleName);
    {
        // qDebug() << "Visit cython module: " << PyUnicodeObjectToQString(PyObject_Str(syntaxtree));
        PyObjectRef body = getattr<PyObjectRef>(syntaxtree, "body");
        ast->body.append(visitNode(body, ast));
    }
}

Python::Ast* AstTransformer::visitNode(PyObject* node, Python::Ast* parent)
{
    if (!node || node == Py_None) return nullptr;
    Python::Ast* result = nullptr;
    Q_ASSERT(PyObject_IsInstance(node, cython.ast_Node));

    auto pos = cythonNodePos(node);
    const int lineno = tline(pos.first);
    const int col_offset = tline(pos.second);

    if (PyObject_IsInstance(node, cython.ast_SingleAssignmentNode)) {
        auto v = new Python::AugmentedAssignmentAst(parent);
        PyObjectRef lhs = getattr<PyObjectRef>(node, "lhs");
        v->target = static_cast<Python::ExpressionAst*>(visitNode(lhs, v));
        PyObjectRef rhs = getattr<PyObjectRef>(node, "rhs");
        v->value = static_cast<Python::ExpressionAst*>(visitNode(rhs, v));
        result = v;
    }
    else if (PyObject_IsInstance(node, cython.ast_NameNode)) {
        auto v = new Python::NameAst(parent);
        QString name = getattr<QString>(node, "name");
        v->identifier = new Python::Identifier(name);
        v->identifier->startLine = lineno;
        v->identifier->endLine = lineno;
        v->identifier->startCol = col_offset;
        v->identifier->endCol = col_offset + name.size() - 1;
        v->startLine = v->identifier->startLine;
        v->endLine = v->identifier->endLine;
        v->startCol = v->identifier->startCol;
        v->endCol = v->identifier->endCol;
        result = v;
    }
    else if (PyObject_IsInstance(node, cython.ast_IntNode)) {
        auto v = new Python::NumberAst(parent);
        QString value = getattr<QString>(node, "value");
        v->value = value.toInt();
        v->isInt = true;
        v->startLine = lineno;
        v->endLine = lineno;
        v->startCol = col_offset;
        v->endCol = col_offset + value.size() - 1;
        result = v;
    }
    else if (PyObject_IsInstance(node, cython.ast_StringNode)) {
        auto v = new Python::StringAst(parent);
        QString value = getattr<QString>(node, "value");
        v->value = value;
        v->startLine = lineno;
        v->endLine = lineno;
        v->startCol = col_offset;
        v->endCol = col_offset + value.size() - 1;
        result = v;
    }
    else {
        qWarning() << "Unhandled cython node" << Python::PyUnicodeObjectToQString(PyObject_Str(node));
    }
    return result;
}

std::pair<int, int> AstTransformer::cythonNodePos(PyObject* node)
{
    PyObjectRef pos = getattr<PyObjectRef>(node, "pos");
    Q_ASSERT(PyTuple_Check(pos));
    Q_ASSERT(PyTuple_Size(pos) >= 3);
    // (source_desc, lineno, col_offset)
    const int lineno = PyLong_AsLong(PyTuple_GetItem(pos, 1));
    const int col_offset = PyLong_AsLong(PyTuple_GetItem(pos, 2));
    return std::make_pair(lineno, col_offset);
}

} // end namespace Cython
