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
        // qDebug() << "Visit enaml module: " << PyUnicodeObjectToQString(PyObject_Str(syntaxtree));
        PyObject* body = getattr<PyObjectRef>(syntaxtree, "body");
        Q_ASSERT(PyList_Check(body));
        for ( int i=0; i < PyList_Size(body); i++ )
        {
            PyObject* currentNode = PyList_GET_ITEM(body, i);
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
        Py_DECREF(body);
        // TODO: Pragmas?
    }
}

QList<Ast*> AstTransformer::visitPythonModuleNode(PyObject* node, Ast* parent)
{
    // qDebug() << "Visit enaml python module node: " << PyUnicodeObjectToQString(PyObject_Str(node));
    QList<Ast*> nodes;
    Q_ASSERT(PyObject_IsInstance(node, enaml.ast_PythonModule));
    PyObjectRef mod = getattr<PyObjectRef>(node, "ast");
    Q_ASSERT(PyObject_IsInstance(mod, grammar.ast_Module));
    PyObjectRef mod_body = getattr<PyObjectRef>(mod, "body");
    nodes.append(visitNodeList<Ast>(mod_body, parent));
    return nodes;
}

Ast* AstTransformer::visitEnamlDefNode(PyObject* node, Ast* parent)
{
    // qDebug() << "Visit enaml node: " << PyUnicodeObjectToQString(PyObject_Str(node));
    Q_ASSERT(PyObject_IsInstance(node, enaml.ast_EnamlDef));

    auto* v = new Enaml::EnamlDefAst(parent);
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

QList<Ast*> AstTransformer::visitEnamlDefBody(PyObject* node, Ast* parent)
{
    // qDebug() << "visit enamldef body: " << PyUnicodeObjectToQString(PyObject_Str(node))<< "parent" << parent;
    QList<Ast*> nodelist;
    if (!node || node == Py_None) return nodelist;
    Q_ASSERT(PyList_Check(node));
    for ( int i=0; i < PyList_Size(node); i++ )
    {
        PyObject* currentNode = PyList_GetItem(node, i);
        QList<Ast*> result = visitEnamlDefItemNode(currentNode, parent);
        nodelist.append(result);
    }
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


QList<Ast*> AstTransformer::visitEnamlDefItemNode(PyObject* node, Ast* parent)
{
    QList<Ast*> result;
    if (!node || node == Py_None) return result;
    // qDebug() << "visit enamldef item: " << PyUnicodeObjectToQString(PyObject_Str(node)) << "parent" << parent;
    int lineno = tline(getattr<int>(node, "lineno"));
    if (PyObject_IsInstance(node, enaml.ast_ChildDef)) {
        // qDebug() << "childdef";
        auto v = new Enaml::ChildDefAst(parent);
        QString name = getattr<QString>(node, "typename");
        if ( name.size() ) {
            v->name = new Python::Identifier(name + "_" + QString::fromLatin1("%1").arg(lineno));
            v->name->startCol = 9; // enamldef must start at beginning
            v->name->startLine = lineno;
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
        result.append(v);

        // Build a with ChildDef() as self to set the block context


    }
    else if (PyObject_IsInstance(node, enaml.ast_Binding)) {
        // qDebug() << "binding";
        auto v = new  Enaml::BindingAst(parent);
        QString name = getattr<QString>(node, "name");
        {
            //auto target = newSelfAttr(lineno, v, name);
            auto target = new Python::NameAst(v);
            target->identifier = new Python::Identifier(name);
            // TODO: Depends on expr op
            target->context = Python::ExpressionAst::Context::Store;
            target->identifier->startLine = tline(getattr<int>(node, "lineno"));
            target->identifier->startCol = 0;
            target->identifier->endLine = target->identifier->startLine;
            target->identifier->endCol = target->identifier->startCol + name.size() - 1;
            target->startLine = target->identifier->startLine;
            target->startCol = target->identifier->startCol;
            target->endLine = target->identifier->endLine;
            target->endCol = target->identifier->endCol;
            v->startLine = target->startLine;
            v->startCol = target->startCol;
            v->endLine = target->endLine;
            v->endCol = target->endCol;
            // auto target = new Python::AttributeAst(v);
            // target->context = Python::ExpressionAst::Store;
            // target->startLine = tline(getattr<int>(node, "lineno"));
            // target->endLine = target->endLine;
            // auto self = new Python::NameAst(target);
            // self->identifier = new Python::Identifier("self");
            // self->context = Python::ExpressionAst::Load;
            // target->value = self;
            // target->attribute = new Python::Identifier(name);
            v->target = target;
        }
        {
            PyObjectRef expr = getattr<PyObjectRef>(node, "expr");
            Q_ASSERT(expr && expr != Py_None);
            QString op = getattr<QString>(expr, "operator");
            if (op == "=")
                v->op = BindingAst::Op::Assignment;
            else if (op == ":=")
                v->op = BindingAst::Op::Delegation;
            else if (op == "<<")
                v->op = BindingAst::Op::Subscription;
            else if (op == ">>")
                v->op = BindingAst::Op::Update;
            else if (op == "::")
                v->op = BindingAst::Op::Notification;

            // qDebug() << "  op " << op;
            if (op == "=" || op == "<<" || op == ":=")
            {
                PyObjectRef expr_value = getattr<PyObjectRef>(expr, "value");
                if (PyObject_IsInstance(expr_value, enaml.ast_PythonModule)) {
                    // Enaml generates a function named "f" for expression blocks
                    auto body = visitPythonModuleNode(expr_value, parent);
                    Q_ASSERT(body.size() == 1);
                    auto func = static_cast<Python::FunctionDefinitionAst*>(body.at(0));
                    Q_ASSERT(func);

                    func->name->value = "_" + name + "_expr_" + QString::fromLatin1("%1").arg(lineno);
                    func->startCol = 0;
                    func->endCol = func->startCol + name.size() - 1;
                    result.append(func);

                    // Generate call
                    auto target = newSelfAttr(lineno, parent, func->name->value);
                    auto call = new Python::CallAst(v);
                    call->function = target;
                    v->value = call;
                }
                else {
                    v->value = static_cast<Python::ExpressionAst*>(visitPythonExpressionNode(expr_value, v));
                }

                //qWarning() << "before " << Ast::dump(v->value);
                v->value->startCol = v->target->endCol + op.size();
                v->value->startLine = v->startLine;
                //qWarning() << "after " << Ast::dump(v->value);
            }
        }
        //updateRanges(v);
        result.append(v);
    }
    else if (PyObject_IsInstance(node, enaml.ast_ExBinding)) {
        // qDebug() << "exbinding";
    }
    else if (PyObject_IsInstance(node, enaml.ast_OperatorExpr)) {
        // qDebug() << "operatorexpr";
    }
    else if (PyObject_IsInstance(node, enaml.ast_FuncDef) || PyObject_IsInstance(node, enaml.ast_AsyncFuncDef)) {
        // qDebug() << "funcdef";
        PyObjectRef funcdef = getattr<PyObjectRef>(node, "funcdef");
        result.append(visitStmtNode(funcdef, parent));
    }
    else if (PyObject_IsInstance(node, enaml.ast_AliasExpr)) {
        // qDebug() << "aliasexpr";
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
        // qDebug() << "storagexpr";
        auto v = new  Enaml::StorageExprAst(parent);
        //auto v = new  Python::AnnotationAssignmentAst(parent);

        QString kind = getattr<QString>(node, "kind");
        if (kind == "attr")
            v->kind = StorageExprAst::Kind::Attr;
        else if (kind == "event")
            v->kind = StorageExprAst::Kind::Event;
        else
            v->kind = StorageExprAst::Kind::Invalid;

        QString name = getattr<QString>(node, "name");
        auto target = new Python::NameAst(v);
        target->identifier = new Python::Identifier(name);
        target->context = Python::ExpressionAst::Context::Store;
        target->identifier->startLine = tline(getattr<int>(node, "lineno"));
        target->identifier->startCol = 0;
        target->identifier->endLine = target->identifier->startLine;
        target->identifier->endCol = target->identifier->startCol + name.size() - 1;
        target->startLine = target->identifier->startLine;
        target->startCol = target->identifier->startCol;
        target->endLine = target->identifier->endLine;
        target->endCol = target->identifier->endCol;
        v->startLine = target->startLine;
        v->startCol = target->startCol;
        v->endLine = target->endLine;
        v->endCol = target->endCol;

        // Build value
        Python::ExpressionAst* value = nullptr;
        PyObjectRef expr = getattr<PyObjectRef>(node, "expr");


        if (expr != Py_None && kind == "attr")
        {
            QString op = getattr<QString>(expr, "operator");
            if (op == "=" || op == "<<" || op == ":=")
            {
                PyObjectRef expr_value = getattr<PyObjectRef>(expr, "value");
                value = static_cast<Python::ExpressionAst*>(visitPythonExpressionNode(expr_value, v));
            }
        }

        if (value == nullptr) {
            auto constant = new Python::NameConstantAst(v);
            constant->value = Python::NameConstantAst::None;
            value = constant;
        }
        //PyObjectRef tp = getattr<PyObjectRef>(node, "typename"); // THIS IS A DEATH TRAP
        //PyErr_Occurred
        // setattr(tp, "col_offset", 0); // TODO: Bug? Why is it missing?
        // if (tp == Py_None)
        //     v->annotation = nullptr;
        // else
        //     v->annotation = static_cast<Python::ExpressionAst*>(visitExprNode(tp, v));
        v->target = target;
        v->value = value;
        updateRanges(v);
        result.append(v);
    }
    else {
        qWarning() << "Unhandled enaml item" << Python::PyUnicodeObjectToQString(PyObject_Str(node));
    }

    // if ( ! ranges_copied ) {
    //     result->startCol = 0;
    //     result->endCol = result->startCol;
    //     result->startLine = tline(getattr<int>(node, "lineno"));
    //     result->endLine = result->startLine;
    //     result->hasUsefulRangeInformation = true;
    // }
    // else {
    //     result->hasUsefulRangeInformation = true;
    // }
    // updateRanges(result);

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

Python::AttributeAst* AstTransformer::newSelfAttr(int lineno, Python::Ast* parent, QString attr)
{
    auto v = new Python::AttributeAst(parent);
    v->context = Python::ExpressionAst::Store;
    v->startLine = lineno;
    v->endLine = lineno;
    auto self = new Python::NameAst(v);
    self->identifier = new Python::Identifier("self");
    self->context = Python::ExpressionAst::Load;
    self->startCol = 0;
    self->endCol = 3;
    self->startLine = lineno;
    self->endLine = lineno;
    v->value = self;
    v->attribute = new Python::Identifier(attr);
    return v;
}

} // end namespace Enaml
