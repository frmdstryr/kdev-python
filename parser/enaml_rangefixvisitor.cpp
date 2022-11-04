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
        const bool isOverride = indexOf(node->startLine, "=>") > 0;
        const bool isNotification = indexOf(node->startLine, "::") > 0;
        const bool isUpdate = indexOf(node->startLine, "<<") > 0;
        const int offset = (isOverride || isNotification || isUpdate) ? 0 : 5;
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
    const int previousLength = node->name->endCol - node->name->startCol;
    const int i = firstNonSpace(node, 9);
    node->name->startCol = i;
    node->name->endCol = i + previousLength;
    node->startCol = node->name->startCol;
    node->endCol = node->name->endCol;

    // Fix base
    {
        auto base = static_cast<Python::NameAst*>(node->baseClasses.at(0));
        const int baseLength = base->identifier->endCol - base->identifier->startCol;
        const int startCol = firstNonSpace(node, indexOf(node->startLine, "(") + 1);
        base->identifier->startCol = startCol;
        base->identifier->endCol = startCol + baseLength;
        base->startCol = base->identifier->startCol;
        base->endCol = base->identifier->endCol;
    }

    // Fix ident
    if (node->identifier)
    {
        const int identLength = node->identifier->endCol - node->identifier->startCol;
        const int startCol = firstNonSpace(node, indexOf(node->startLine, ":") + 1);
        node->identifier->startCol = startCol;
        node->identifier->endCol = startCol + identLength;
    }

    AstDefaultVisitor::visitClassDefinition(node);
}

void RangeFixVisitor::visitChildDef(ChildDefAst* node) {
    const int previousLength = node->name->endCol - node->name->startCol;
    const int startCol = firstNonSpace(node);
    node->name->startCol = startCol;
    node->name->endCol = startCol + previousLength;

    // Fix base
    auto base = static_cast<Python::NameAst*>(node->baseClasses.at(0));
    base->identifier->startCol = node->name->startCol;
    base->identifier->endCol = node->name->endCol;

    // Fix ident
    if (node->identifier)
    {
        const int identLength = node->identifier->endCol - node->identifier->startCol;
        const int startCol = firstNonSpace(node, indexOf(node->startLine, ":") + 1);
        node->identifier->startCol = startCol;
        node->identifier->endCol = startCol + identLength;
    }

    AstDefaultVisitor::visitClassDefinition(node);
}

void RangeFixVisitor::visitTemplate(TemplateAst* node) {
    AstDefaultVisitor::visitClassDefinition(node);
}

void RangeFixVisitor::visitAssignment(Python::AssignmentAst* node)
{
    // Eg the name part of `enamldef Type(Base): name:` or `ChildDef: name:`
    if (dynamic_cast<EnamlDefAst*>(node->parent) || dynamic_cast<ChildDefAst*>(node->parent))
    {
        if (auto target = dynamic_cast<Python::NameAst*>(node->targets.at(0))) {
            const int offset = indexOf(node->startLine, ":");
            bool isSelf = target->identifier->value == "self";
            const int previousLength = isSelf ? 0 : target->identifier->endCol - target->identifier->startCol;
            // Put self definition at the semicolon :
            const int startCol = isSelf ? offset: firstNonSpace(target->identifier, offset+1);
            target->identifier->startCol = startCol;
            target->identifier->endCol = startCol + previousLength;
            target->startCol = target->identifier->startCol;
            target->endCol = target->identifier->endCol;
        }

        // If a name referring to self
        if (auto value = dynamic_cast<Python::NameAst*>(node->value)) {
            if (value->identifier->value == "self") {
                const int startCol = indexOf(node->parent->startLine, ":");
                value->identifier->startCol = startCol;
                value->identifier->endCol = startCol;
                value->startCol = value->identifier->startCol;
                value->endCol = value->identifier->endCol;
            }
        }

    }
    AstDefaultVisitor::visitAssignment(node);
}

void RangeFixVisitor::visitAnnotationAssignment(Python::AnnotationAssignmentAst* node)
{
    if (auto n = dynamic_cast<BindingAst*>(node))
        visitBinding(n);
    else if (auto n = dynamic_cast<StorageExprAst*>(node))
        visitStorageExpr(n);
    else if (auto n = dynamic_cast<AliasAst*>(node))
        visitEnamlAlias(n);
    else
        Python::RangeFixVisitor::visitAnnotationAssignment(node);

}


void RangeFixVisitor::visitStorageExpr(StorageExprAst* node)
{
    int n = 0;
    switch (node->kind) {
        case StorageExprAst::Kind::Attr:
            n = 4; // attr + space
            break;
        case StorageExprAst::Kind::Event:
            n = 5; // event + space
            break;
        default:
            break;
    }
    const int startCol = firstNonSpace(node, firstNonSpace(node) + n);
    if (startCol >= 0)
    {
        const int previousLength = node->target->endCol - node->target->startCol;
        node->target->startCol = startCol;
        node->target->endCol = startCol + previousLength;
        if (auto name = dynamic_cast<Python::NameAst*>(node->target))
        {
            name->identifier->startCol = startCol;
            name->identifier->endCol = startCol + previousLength;
        }
        node->startCol = node->target->startCol;
        node->endCol = node->target->endCol;
    }

    AstDefaultVisitor::visitAnnotationAssignment(node);
}

void RangeFixVisitor::visitEnamlAlias(AliasAst* node) {
    const int startCol = firstNonSpace(node, indexOf(node->startLine, "alias") + 5);
    if (startCol > 0)
    {
        const int targetLength = node->target->endCol - node->target->startCol;
        node->target->startCol = startCol;
        node->target->endCol = startCol + targetLength;
        if (auto name = dynamic_cast<Python::NameAst*>(node->target))
        {
            name->identifier->startCol = startCol;
            name->identifier->endCol = startCol + targetLength;
        }

        const int colonPos = indexOf(node->startCol, ":");
        const int valueStartCol = (colonPos > 0) ? firstNonSpace(node, colonPos + 1): startCol;
        if (auto value = dynamic_cast<Python::NameAst*>(node->value))
        {
            const int valueLength = node->value->endCol - node->value->startCol;
            value->startCol = valueStartCol;
            value->endCol = valueStartCol + valueLength;
            value->identifier->startCol = valueStartCol;
            value->identifier->endCol = valueStartCol + valueLength;
        } else if (auto attr = dynamic_cast<Python::AttributeAst*>(node->value)) {
            // TODO: This
        }
    }
    AstDefaultVisitor::visitAnnotationAssignment(node);
}

void RangeFixVisitor::visitBinding(BindingAst* node)
{
    const int startCol = firstNonSpace(node);
    if (startCol >= 0)
    {
        const int previousLength = node->target->endCol - node->target->startCol;
        node->target->startCol = startCol;
        node->target->endCol = startCol + previousLength;
        if (auto name = dynamic_cast<Python::NameAst*>(node->target))
        {
            name->identifier->startCol = startCol;
            name->identifier->endCol = startCol + previousLength;
        }
        node->startCol = node->target->startCol;
        node->endCol = node->target->endCol;
    }
    AstDefaultVisitor::visitAnnotationAssignment(node);
}

int RangeFixVisitor::firstNonSpace(Python::Ast* node, int offset)
{
    const int lineno = node->startLine;
    if (lineno < 0 || lineno > lines.size())
    {
        qWarning() << "Node start line is invalid: " << node->dump();
        return -1;
    }
    const QString &line = lines.at(lineno);
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
