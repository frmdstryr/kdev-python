#pragma once

#include "python_asttransformer.h"
#include "cython_grammar.h"

namespace Cython {

class AstTransformer: public Python::AstTransformer {
public:
    AstTransformer(): Python::AstTransformer() {}

    void run(PyObject *syntaxtree, QString moduleName) override;
    Python::Ast* visitNode(PyObject* node, Python::Ast* parent) override;

    std::pair<int, int> cythonNodePos(PyObject* node);
private:
    Grammar cython;
};

} // end namespace Python
