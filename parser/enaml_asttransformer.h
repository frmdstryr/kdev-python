/*
    SPDX-FileCopyrightText: 2022 Jairus Martin <jrm@codelv.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "python_asttransformer.h"
#include "enaml_grammar.h"
#include "enaml_ast.h"

namespace Enaml {

class AstTransformer: public Python::AstTransformer {
public:
    AstTransformer(): Python::AstTransformer() {}

    void run(PyObject *syntaxtree, QString moduleName) override;
    QList<Python::Ast*> visitPythonModuleNode(PyObject* node, Python::Ast* parent);
    QList<Python::Ast*> visitEnamlDefBody(PyObject* node, Python::Ast* parent);
    Python::Ast* visitEnamlDefNode(PyObject* node, Python::Ast* parent);
    QList<Python::Ast*> visitEnamlDefItemNode(PyObject* node, Python::Ast* parent);
    Python::Ast* visitTemplateNode(PyObject* node, Python::Ast* parent);
    Python::Ast* visitPythonExpressionNode(PyObject* node, Python::Ast* parent);
    Python::Ast* visitPythonExpressionBlockNode(PyObject* node, Python::Ast* parent);

private:
    Python::AttributeAst* buildAttrChain(PyObject* chain, Python::Ast* parent, Python::ExpressionAst::Context ctx, int startIndex=0);

    Grammar enaml;
};

} // end namespace Python
