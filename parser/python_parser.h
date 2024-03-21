/*
    SPDX-FileCopyrightText: 2007 Andreas Pakulat <apaku@gmx.de>
    SPDX-FileCopyrightText: 2010-2011 Sven Brauch <svenbrauch@googlemail.com>
    SPDX-FileCopyrightText: 2022 Jairus Martin <jrm@codelv.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include <QMutex>
#include <parserdebug.h>
#include "python_asttransformer.h"
#include "python_header.h"
#include "rangefixvisitor.h"

#ifdef Q_OS_UNIX
#include <dlfcn.h>
#endif

namespace Python {

QString pythonLibraryName() {
    return QString::fromUtf8(PYTHON_LIBRARY).section(QLatin1Char('/'), -1, -1);
}


#if QT_VERSION >= 0x060000
struct Parser : private QMutexLocker<QMutex>
#else
struct Parser : private QMutexLocker
#endif
{
    PyObject* m_parser_mod = nullptr;
    PyObject* m_parse_func = nullptr;

    Parser(QMutex& lock, bool nosite = true, bool extsupport = false): QMutexLocker(&lock)
    {
        Py_NoSiteFlag = nosite ? 1 : 0;
#ifdef Q_OS_UNIX
        if (extsupport)
        {
            // We are usually (outside of unit tests) loaded as a plugin. KPluginLoader
            // loads plugins with RTLD_LOCAL (through QLibrary, but effectively it does).
            // This means libraries our plugin links against are also loaded into the main process
            // with RTLD_LOCAL. Thus, the symbols from libpython are *not* in the global symbol
            // table of the process we're running in. If Python itself loads a plugin -- which easily
            // happens for anything that "import"s a C module -- that will fail, since this plugin
            // will not be able to find the symbols from libpython.
            // Thus, we re-open the library here (usign RTLD_NOLOAD) to change the symbol
            // visibility to global scope.
            auto* lib = dlopen(pythonLibraryName().toUtf8().data(), RTLD_LAZY | RTLD_NOLOAD | RTLD_GLOBAL);
            if (!lib)
                qCWarning(KDEV_PYTHON_PARSER) << "Error loading libpython:" << dlerror();
        }
#endif

        Py_InitializeEx(0);
        Q_ASSERT(Py_IsInitialized());
    }

    virtual bool load()
    {
        m_parser_mod = PyImport_ImportModule("ast");
        if (!m_parser_mod)
            return false;
        m_parse_func = PyObject_GetAttrString(m_parser_mod, "parse");
        if (!m_parse_func)
            return false;

        return true;
    }

    // Call parser function and return the python ast.Module.
    // NOTE: The caller must DECREF the result
    virtual PyObject* parse(QString const &source, QString const &filename) const
    {
        PyObject* args = PyTuple_New(3);
        PyTuple_SET_ITEM(args, 0, PyUnicode_FromString(source.toUtf8().data()));
        PyTuple_SET_ITEM(args, 1, PyUnicode_FromString(filename.toUtf8().data()));
        PyTuple_SET_ITEM(args, 2, PyUnicode_FromString("exec"));
        PyObject *result = PyObject_CallObject(m_parse_func, args);
        Py_DECREF(args);
        return result;
    }

    virtual CodeAst::Ptr transform(PyObject* syntaxtree, QString filename) const
    {
        AstTransformer t;
        t.run(syntaxtree, filename);
        return CodeAst::Ptr(t.ast);
    }

    virtual void fixRanges(QString &contents, CodeAst::Ptr ast)
    {
        RangeFixVisitor fixVisitor(contents);
        fixVisitor.visitNode(ast.get());
    }

    virtual ~Parser()
    {
        if (Py_IsInitialized())
        {
            Py_XDECREF(m_parse_func);
            Py_XDECREF(m_parser_mod);
            Py_Finalize();
        }
    }
};

} // end namespace Python

