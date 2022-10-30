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
    // Eg build a self.attr
    Python::AttributeAst* newSelfAttr(int lineno, Python::Ast* parent, QString attr);

    Grammar enaml;
};

} // end namespace Python
