#pragma once
#include "python_parser.h"
#include "cython_asttransformer.h"

namespace Cython {

struct Parser: public Python::Parser
{
    Parser(QMutex& lock): Python::Parser(lock, false, true) {}
    bool load() override
    {
        m_parser_mod = PyImport_ImportModule("Cython.Compiler.TreeFragment");
        if (!m_parser_mod)
            return false;
        m_parse_func = PyObject_GetAttrString(m_parser_mod, "parse_from_strings");
        if (!m_parse_func)
            return false;
        return true;
    }

    PyObject* parse(QString const &source, QString const &filename) const override
    {
        PyObject* args = PyTuple_New(2);
        PyTuple_SET_ITEM(args, 0, PyUnicode_FromString(filename.toUtf8().data()));
        PyTuple_SET_ITEM(args, 1, PyUnicode_FromString(source.toUtf8().data()));
        PyObject *result = PyObject_CallObject(m_parse_func, args);
        Py_DECREF(args);
        return result;
    }

    Python::CodeAst::Ptr transform(PyObject* syntaxtree, QString filename) const override
    {
        AstTransformer t;
        t.run(syntaxtree, filename);
        return Python::CodeAst::Ptr(t.ast);
    }

};

} // end namespace Python
