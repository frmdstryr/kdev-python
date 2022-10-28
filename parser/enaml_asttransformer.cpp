#include "enaml_asttransformer.h"
#include "astbuilder.h"

namespace Enaml{

using Python::PyObjectRef;
using Python::PyUnicodeObjectToQString;
using Python::Ast;

void setattr(PyObject* node, const char* attr, int value)
{
    if (node)
        PyObject_SetAttrString(node, attr, PyLong_FromLong(value));
}

void AstTransformer::run(PyObject* syntaxtree, QString moduleName)
{
    Q_ASSERT(PyObject_IsInstance(syntaxtree, enaml.ast_Module));
    ast = new Python::CodeAst();
    ast->name = new Python::Identifier(moduleName);
    {
        // qDebug() << "Visit enaml module: " << PyUnicodeObjectToQString(PyObject_Str(node));
        PyObjectRef body = getattr<PyObjectRef>(syntaxtree, "body");
        Q_ASSERT(PyList_Check(body));
        for ( int i=0; i < PyList_Size(body); i++ )
        {
            PyObject* currentNode = PyList_GetItem(body, i);
            if (PyObject_IsInstance(currentNode, enaml.ast_PythonModule))
                ast->body.append(visitPythonModuleNode(currentNode, ast));
            else if (PyObject_IsInstance(currentNode, enaml.ast_EnamlDef))
                ast->body.append(visitEnamlDefNode(currentNode, ast));
            //else if (PyObject_IsInstance(currentNode, enaml.ast_Template))
            //    ast->body.append(visitTemplateNode(currentNode, ast));
            else
                qWarning() << "Unhandled enaml node" << Python::PyUnicodeObjectToQString(PyObject_Str(currentNode));
            //
        }
        // TODO: Pragmas?
    }
}

QList<Ast*> AstTransformer::visitPythonModuleNode(PyObject* node, Ast* parent)
{
    QList<Ast*> nodes;
    Q_ASSERT(PyObject_IsInstance(node, enaml.ast_PythonModule));
    PyObjectRef mod = getattr<PyObjectRef>(node, "ast");
    Q_ASSERT(PyObject_IsInstance(mod, grammar.ast_Module));
    PyObjectRef mod_body = getattr<PyObjectRef>(mod, "body");
    nodes.append(visitNodeList<Ast>(mod_body, ast));
    return nodes;
}

QList<Ast*> AstTransformer::visitEnamlDefBody(PyObject* node, Ast* parent)
{
    // qDebug() << "visit enamldef body: " << PyUnicodeObjectToQString(PyObject_Str(node));
    QList<Ast*> nodelist;
    Q_ASSERT(PyList_Check(node));
    for ( int i=0; i < PyList_Size(node); i++ )
    {
        PyObject* currentNode = PyList_GetItem(node, i);
        Ast* result = visitEnamlDefItemNode(currentNode, parent);
        if (result)
            nodelist.append(result);
    }
    // qDebug() << "  size: " << nodelist.size();
    if (!nodelist.size())
    {
        // The enaml parser ignores the final pass
        auto pass = new Python::PassAst(parent);
        pass->startLine = parent->startLine + 1;
        pass->endLine = pass->startLine;
        nodelist.append(pass);
    }
    return nodelist;
}



Ast* AstTransformer::visitEnamlDefNode(PyObject* node, Ast* parent)
{
    // qDebug() << "Visit enaml node: " << PyUnicodeObjectToQString(PyObject_Str(node));
    Q_ASSERT(PyObject_IsInstance(node, enaml.ast_EnamlDef));

    Python::ClassDefinitionAst* v = new Python::ClassDefinitionAst(parent);
    QString name = getattr<QString>(node, "typename");
    v->name = new Python::Identifier(name);
    v->name->startCol = 9; // enamldef must start at beginning
    v->name->startLine = tline(getattr<int>(node, "lineno"));
    v->name->endCol = v->name->startCol + name.size() - 1;
    v->name->endLine = v->name->startLine;

    v->startCol = v->name->startCol;
    v->startLine = v->name->startLine;
    v->endCol = v->name->endCol;
    v->endLine = v->name->endLine;

    qWarning() << v->startLine;

    {
        // Exactly one base is required
        Python::NameAst* base = new Python::NameAst(v);
        QString base_name = getattr<QString>(node, "base");
        base->identifier = new Python::Identifier(base_name);
        base->context = Python::ExpressionAst::Context::Load;
        base->identifier->startCol = v->endCol + 2;
        base->identifier->endCol = base->identifier->startCol + base_name.size() - 1;
        base->identifier->startLine = v->startLine;
        base->identifier->endLine = v->endLine;
        base->startCol = base->identifier->startCol;
        base->endCol = base->identifier->endCol;
        base->startLine = base->identifier->startLine;
        base->endLine = base->identifier->endLine;
        v->baseClasses.append(base);
    }
    {
        PyObjectRef body = getattr<PyObjectRef>(node, "body");
        v->body = visitEnamlDefBody(body, v);
    }
    return v;
}

Ast* AstTransformer::visitEnamlDefItemNode(PyObject* node, Ast* parent)
{
    bool ranges_copied = false;
    Ast* result = nullptr;
    if (PyObject_IsInstance(node, enaml.ast_ChildDef)) {
        Python::ClassDefinitionAst* v = new Python::ClassDefinitionAst(parent);
        QString name = getattr<QString>(node, "typename");
        if ( name.size() ) {
            v->name = new Python::Identifier(name);
            v->name->startCol = 9; // enamldef must start at beginning
            v->name->startLine = tline(getattr<int>(node, "lineno"));
            v->name->endCol = v->name->startCol + name.size() - 1;
            v->name->endLine = v->name->startLine;

            v->startCol = v->name->startCol;
            v->startLine = v->name->startLine;
            v->endCol = v->name->endCol;
            v->endLine = v->name->endLine;
        }
        else {
            v->name = nullptr;
        }
        {
            PyObjectRef body = getattr<PyObjectRef>(node, "body");
            v->body = visitEnamlDefBody(body, v);
        }
        result = v;
        ranges_copied = true;
    }
    else if (PyObject_IsInstance(node, enaml.ast_Binding)) {
        auto v = new  Python::AssignmentAst(parent);
        {
            QString name = getattr<QString>(node, "name");
            auto target = new Python::AttributeAst(v);
            target->context = Python::ExpressionAst::Store;
            target->startLine = tline(getattr<int>(node, "lineno"));
            target->endLine = target->endLine;
            auto self = new Python::NameAst(target);
            self->identifier = new Python::Identifier("self");
            self->context = Python::ExpressionAst::Load;
            target->value = self;
            target->attribute = new Python::Identifier(name);
            v->targets.append(target);

            v->startLine = target->startLine;
            v->endLine = v->startLine;
        }
        {
            PyObjectRef expr = getattr<PyObjectRef>(node, "expr");
            Q_ASSERT(expr && expr != Py_None);
            QString op = getattr<QString>(expr, "operator");
            if (op == "=" || op == "<<" || op == ":=")
            {
                PyObjectRef expr_value = getattr<PyObjectRef>(expr, "value");
                v->value = static_cast<Python::ExpressionAst*>(visitPythonExpressionNode(expr_value, result));
            }
        }

        result = v;
    }
    else if (PyObject_IsInstance(node, enaml.ast_ExBinding)) {

    }
    else if (PyObject_IsInstance(node, enaml.ast_OperatorExpr)) {

    }
    else if (PyObject_IsInstance(node, enaml.ast_FuncDef) || PyObject_IsInstance(node, enaml.ast_AsyncFuncDef)) {
        PyObjectRef funcdef = getattr<PyObjectRef>(node, "funcdef");
        result = visitStmtNode(funcdef, parent);
    }
    else if (PyObject_IsInstance(node, enaml.ast_AliasExpr)) {
        // Python::AssignmentAst* v = new  Python::AssignmentAst(parent);
        // {
        //     QString name = getattr<QString>(node, "name");
        //     auto target = new Python::NameAst(v);
        //     v->targets.append(target);
        //     target->identifier = new Python::Identifier(name);
        //     target->context = Python::ExpressionAst::Context::Load;
        //     target->identifier->startLine = tline(getattr<int>(node, "lineno"));
        //     target->identifier->endLine = target->identifier->startLine;
        //     v->startLine = target->identifier->startLine;
        //     v->endLine = v->startLine;
        // }
        // {
        //     QString target = getattr<QString>(node, "target");
        //     v->annotation = static_cast<Python::ExpressionAst*>(visitPythonExpressionNode(expr, v));
        // }
        // result = v;
    }
    else if (PyObject_IsInstance(node, enaml.ast_StorageExpr)) {
        PyObjectRef tp = getattr<PyObjectRef>(node, "typename");
        if (tp == Py_None)
            result = new  Python::AssignmentAst(parent);
        else
            result = new  Python::AnnotationAssignmentAst(parent);

        QString name = getattr<QString>(node, "name");
        auto target = new Python::NameAst(result);

        target->identifier = new Python::Identifier(name);
        target->context = Python::ExpressionAst::Context::Load;
        target->identifier->startLine = tline(getattr<int>(node, "lineno"));
        target->identifier->endLine = target->identifier->startLine;
        result->startLine = target->identifier->startLine;
        result->endLine = result->startLine;

        // Build value
        Python::ExpressionAst* value = nullptr;
        PyObjectRef expr = getattr<PyObjectRef>(node, "expr");
        QString kind = getattr<QString>(node, "kind");
        if (expr != Py_None || kind == "event")
        {
            QString op = getattr<QString>(expr, "operator");
            if (op == "=" || op == "<<" || op == ":=")
            {
                PyObjectRef expr_value = getattr<PyObjectRef>(expr, "value");
                value = static_cast<Python::ExpressionAst*>(visitPythonExpressionNode(expr_value, result));
            }
        }

        if (value == nullptr)
        {
            auto constant = new Python::NameConstantAst(result);
            constant->value = Python::NameConstantAst::None;
            value = constant;
        }

        if (tp == Py_None)
        {
            auto v = static_cast<Python::AssignmentAst*>(result);
            v->targets.append(target);
            v->value = value;
        }
        else {
            auto v = static_cast<Python::AnnotationAssignmentAst*>(result);
            setattr(tp, "col_offset", 0); // TODO: Bug? Why is it missing?
            v->annotation = static_cast<Python::ExpressionAst*>(visitExprNode(tp, v));
            v->target = target;
            v->value = value;
        }
    }
    else {
        qWarning() << "Unhandled enaml item" << Python::PyUnicodeObjectToQString(PyObject_Str(node));
    }

    if ( ! result ) return nullptr;
    if ( ! ranges_copied ) {
        result->startCol = 0;
        result->endCol = result->startCol;
        result->startLine = tline(getattr<int>(node, "lineno"));
        result->endLine = result->startLine;
        result->hasUsefulRangeInformation = true;
    }
    else {
        result->hasUsefulRangeInformation = true;
    }
    updateRanges(result);

    return result;
}

Ast* AstTransformer::visitTemplateNode(PyObject* node, Ast* parent)
{
    return nullptr;
}

Ast* AstTransformer::visitPythonExpressionNode(PyObject* node, Ast* parent)
{
    if (!node || node == Py_None) return nullptr;
    // qDebug() << "Visit enaml python expr node: " << PyUnicodeObjectToQString(PyObject_Str(node));
    Q_ASSERT(PyObject_IsInstance(node, enaml.ast_PythonExpression));
    PyObjectRef ast = getattr<PyObjectRef>(node, "ast");
    Q_ASSERT(PyObject_IsInstance(ast, grammar.ast_Expression));
    PyObjectRef body = getattr<PyObjectRef>(ast, "body");
    // qDebug() << " body: " << PyUnicodeObjectToQString(PyObject_Str(body));
    return visitExprNode(body, parent);
}


} // end namespace Enaml
