
#include "cython_asttransformer.h"
namespace Cython {


Python::Ast* AstTransformer::visitNode(PyObject* node, Python::Ast* parent)
{
    //if (PyObject_IsInstance(node, cython_grammar.ast_CompilerNode))
    //    return visitCompilerNode(node, parent);
    return Python::AstTransformer::visitNode(node, parent);
}

} // end namespace Cython
