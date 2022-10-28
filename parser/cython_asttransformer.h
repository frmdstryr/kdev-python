#pragma once

#include "python_asttransformer.h"

namespace Cython {

class AstTransformer: public Python::AstTransformer {
public:
    AstTransformer(): Python::AstTransformer() {}

    Python::Ast* visitNode(PyObject* node, Python::Ast* parent) override;
};

} // end namespace Python
