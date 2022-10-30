#include "enaml_rangefixvisitor.h"
#include "enaml_ast.h"

namespace Enaml {

void RangeFixVisitor::visitClassDefinition(Python::ClassDefinitionAst* node) {
    if (auto n = dynamic_cast<EnamlDefAst*>(node))
        visitEnamlDef(n);
    else if (auto n = dynamic_cast<ChildDefAst*>(node))
        visitChildDef(n);
    else if (auto n = dynamic_cast<TemplateAst*>(node))
        visitTemplate(n);
    else
        Python::RangeFixVisitor::visitClassDefinition(node);
};

void RangeFixVisitor::visitFunctionDefinition(Python::FunctionDefinitionAst* node) {
    if (dynamic_cast<EnamlDefAst*>(node->parent) || dynamic_cast<ChildDefAst*>(node->parent))
    {
        const int asyncOffset = node->async ? indexOf(node->startLine, "async") + 5: 0;
        const int offset = indexOf(node->startLine, "=>") > 0 ? 0 : 5;
        const int i = firstNonSpace(node->name, asyncOffset);
        const int previousLength = node->name->endCol - node->name->startCol;
        node->name->startCol = i + offset;
        node->name->endCol = node->name->startCol + previousLength;
        AstDefaultVisitor::visitFunctionDefinition(node);
        return;
    }
    Python::RangeFixVisitor::visitFunctionDefinition(node);
};


void RangeFixVisitor::visitEnamlDef(EnamlDefAst* node) {
    AstDefaultVisitor::visitClassDefinition(node);
}

void RangeFixVisitor::visitChildDef(ChildDefAst* node) {
    const int previousLength = node->endCol - node->startCol;
    const int i = firstNonSpace(node);
    node->name->startCol = i;
    node->name->endCol = i + previousLength;
    node->startCol = node->name->startCol;
    node->endCol = node->name->endCol;
    AstDefaultVisitor::visitClassDefinition(node);
}

void RangeFixVisitor::visitTemplate(TemplateAst* node) {
    AstDefaultVisitor::visitClassDefinition(node);
}

void RangeFixVisitor::visitAnnotationAssignment(Python::AnnotationAssignmentAst* node)
{
    if (auto n = dynamic_cast<BindingAst*>(node))
        visitBinding(n);
    else if (auto n = dynamic_cast<StorageExprAst*>(node))
        visitStorageExpr(n);
    else
        Python::RangeFixVisitor::visitAnnotationAssignment(node);

}


void RangeFixVisitor::visitStorageExpr(StorageExprAst* node)
{
    int n = 0;
    switch (node->kind) {
        case StorageExprAst::Kind::Attr:
            n = 4 + 1; // attr + space
            break;
        case StorageExprAst::Kind::Event:
            n = 5 + 1; // event + space
            break;
        default:
            break;
    }
    const int previousLength = node->endCol - node->startCol;
    const int i = firstNonSpace(node);
    node->target->startCol = i + n;
    node->target->endCol = i + n + previousLength;
    if (auto name = dynamic_cast<Python::NameAst*>(node->target))
    {
        name->identifier->startCol = i + n;
        name->identifier->endCol = i + n + previousLength;
    }
    node->startCol = node->target->startCol;
    node->endCol = node->target->endCol;


    AstDefaultVisitor::visitAnnotationAssignment(node);
}

void RangeFixVisitor::visitBinding(BindingAst* node)
{
    int n = 0;
    const int previousLength = node->endCol - node->startCol;
    const int i = firstNonSpace(node);
    node->target->startCol = i + n;
    node->target->endCol = i + n + previousLength;
    if (auto name = dynamic_cast<Python::NameAst*>(node->target))
    {
        name->identifier->startCol = i + n;
        name->identifier->endCol = i + n + previousLength;
    }
    node->startCol = node->target->startCol;
    node->endCol = node->target->endCol;
    // if (node->value) {
    //     const int valueLength = node->value->endCol - node->value->startCol;
    //     switch (node->op) {
    //         case BindingAst::Op::Assignment:
    //             node->value->startCol = indexOf(node->startLine, "=") + 1;
    //             break;
    //         case BindingAst::Op::Delegation:
    //             node->value->startCol = indexOf(node->startLine, ":=") + 2;
    //             break;
    //         case BindingAst::Op::Subscription:
    //             node->value->startCol = indexOf(node->startLine, "<<") + 2;
    //             break;
    //         case BindingAst::Op::Update:
    //             node->value->startCol = indexOf(node->startLine, ">>") + 2;
    //             break;
    //         case BindingAst::Op::Notification:
    //             node->value->startCol = indexOf(node->startLine, "::") + 2;
    //             break;
    //         default:
    //             node->value->startCol = node->endCol + 1;
    //     }
    //     node->value->endCol = node->value->startCol + valueLength;
    // }
    AstDefaultVisitor::visitAnnotationAssignment(node);
}

int RangeFixVisitor::firstNonSpace(Python::Ast* node, int offset)
{
    const QString &line = lines.at(node->startLine);
    int i = offset;
    if (i >= 0) {
        for ( i = offset; i < line.size(); i++ ) {
            if (line.at(i).isSpace())
                continue;
            break;
        }
    }
    return i;
}

int RangeFixVisitor::indexOf(int lineno, const QString&pattern, int offset)
{
    if (lineno < 0 || lineno > lines.size())
        return -1;
    const QString &line = lines.at(lineno);
    auto parts = line.split("#");
    return parts.at(0).indexOf(pattern, offset);
}

} // end namespace Enaml
