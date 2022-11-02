#pragma once
#include "rangefixvisitor.h"
#include "enaml_ast.h"

namespace Enaml {


class KDEVPYTHONPARSER_NO_EXPORT RangeFixVisitor : public Python::RangeFixVisitor {
public:
    RangeFixVisitor(const QString& contents): Python::RangeFixVisitor(contents) {};
    void visitClassDefinition(Python::ClassDefinitionAst* node) override;
    void visitFunctionDefinition(Python::FunctionDefinitionAst* node) override;
    void visitAnnotationAssignment(Python::AnnotationAssignmentAst* node) override;
    void visitAssignment(Python::AssignmentAst* node) override;
    void visitEnamlDef(EnamlDefAst* node);
    void visitChildDef(ChildDefAst* node);
    void visitStorageExpr(StorageExprAst* node);
    void visitBinding(BindingAst* node);
    void visitTemplate(TemplateAst* node);
    void visitEnamlAlias(AliasAst* node);
protected:
    int firstNonSpace(Python::Ast* node, int offset=0);
    // Check if the line contains the pattern ignoring any comments
    // return index of first result or -1 if not found
    int indexOf(int lineno, const QString &pattern, int offset=0);
};

} // end namespace Enaml
