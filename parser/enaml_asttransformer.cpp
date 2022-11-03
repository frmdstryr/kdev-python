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
        QString identifier = getattr<QString>(node, "identifier");
        if (identifier.size())
        {
            v->identifier = new Python::Identifier(identifier);
            v->identifier->startLine = v->startLine;
            v->identifier->endLine = v->endLine;
            v->identifier->startCol = v->baseClasses.at(0)->endCol + 1;
            v->identifier->endCol = v->identifier->startCol + identifier.size() - 1;
        }
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

    // Python::Identifier* identifier = nullptr;
    // Python::Identifier* class_name = nullptr;
    // bool root = false;
    // if (auto enamldef = dynamic_cast<EnamlDefAst*>(parent))
    // {
    //     identifier = enamldef->identifier;
    //     class_name = enamldef->name;
    //     root = true;
    // }
    // else if (auto childdef = dynamic_cast<ChildDefAst*>(parent))
    // {
    //     identifier = childdef->identifier;
    //     class_name = static_cast<Python::NameAst*>(childdef->baseClasses.at(0))->identifier;
    // }
    // {
    //     // self = EnamlDefCls()
    //     auto assignment = new Python::AssignmentAst(parent);
    //     auto target = new Python::NameAst(assignment);
    //     target->context = Python::ExpressionAst::Store;
    //     auto self = new Python::Identifier("self");
    //     target->identifier = self;
    //     self->startLine = parent->startLine;
    //     self->startCol = 0; // Zero sized at end of line
    //     self->endLine = parent->startLine;
    //     self->endCol = 0;
    //     target->startLine = parent->startLine;
    //     target->startCol = self->startCol;
    //     target->endLine = parent->endLine;
    //     target->endCol = self->endCol;
    //     assignment->targets.append(target);
    //
    //     auto call = new Python::CallAst(assignment);
    //     auto func = new Python::NameAst(call);
    //     func->context = Python::ExpressionAst::Load;
    //     auto cls = new Python::Identifier(class_name->value);
    //     func->identifier = cls;
    //     cls->startLine = parent->startLine;
    //     cls->startCol = 0; // Zero sized at end of line
    //     cls->endCol = parent->endCol;
    //     cls->endLine = 0;
    //
    //     func->startLine = cls->startLine;
    //     func->startCol = cls->startCol;
    //     func->endCol = cls->endCol;
    //     func->endLine = cls->endLine;
    //     call->function = func;
    //     call->startCol = func->startCol;
    //     call->endCol = func->endCol;
    //     call->startLine = func->startLine;
    //     call->endLine = func->endLine;
    //     assignment->value = call;
    //
    //     assignment->startLine = parent->startLine;
    //     assignment->endLine = parent->endLine;
    //
    //     nodelist.append(assignment);
    // }
    //
    // Q_ASSERT(class_name);
    //
    // if (identifier)
    // {
    //     // ident = self
    //     auto assignment = new Python::AssignmentAst(parent);
    //     auto target = new Python::NameAst(assignment);
    //     target->context = Python::ExpressionAst::Store;
    //     auto ident = new Python::Identifier(identifier->value);
    //     target->identifier = ident;
    //     ident->startLine = identifier->startLine;
    //     ident->startCol = identifier->startCol;
    //     ident->endLine = identifier->endLine;
    //     ident->endCol = identifier->endCol;
    //     target->startLine = ident->startLine;
    //     target->startCol = ident->startCol;
    //     target->endLine = ident->endLine;
    //     target->endCol = ident->endCol;
    //     assignment->targets.append(target);
    //
    //     auto value = new Python::NameAst(assignment);
    //     auto self = new Python::Identifier("self");
    //     self->startLine = ident->startLine;
    //     self->startCol = ident->startCol;
    //     self->endLine = ident->endLine;
    //     self->endCol = ident->endCol;
    //     value->identifier = self;
    //     value->context = Python::ExpressionAst::Load;
    //     value->startLine = ident->startLine;
    //     value->endLine = ident->endLine;
    //     assignment->value = value;
    //     assignment->startLine = parent->startLine;
    //     assignment->endLine = parent->endLine;
    //
    //     nodelist.append(assignment);
    // }

    // TODO: If not root

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
    const int lineno = tline(getattr<int>(node, "lineno"));
    const int end_lineno = tline(getattr<int>(node, "end_lineno"));
    const int col_offset = getattr<int>(node, "col_offset");
    const int end_col_offset = getattr<int>(node, "end_col_offset");

    if (PyObject_IsInstance(node, enaml.ast_ChildDef)) {
        // qDebug() << "childdef";
        auto v = new Enaml::ChildDefAst(parent);
        v->startLine = lineno;
        v->startCol = col_offset;
        v->endCol = end_col_offset;
        v->endLine = end_lineno;

        QString name = getattr<QString>(node, "typename");
        if ( name.size() ) {
            v->name = new Python::Identifier(name + "_" + QString::fromLatin1("%1").arg(lineno+1));
            v->name->startCol = col_offset; // Fixed by range fix visitor
            v->name->startLine = lineno;
            v->name->endCol = v->name->startCol + name.size() - 1;
            v->name->endLine = lineno;
        }
        else {
            v->name = nullptr;
        }

        {
            // Exactly one base is required
            Python::NameAst* base = new Python::NameAst(v);
            QString base_name = getattr<QString>(node, "typename");
            base->identifier = new Python::Identifier(base_name);
            base->context = Python::ExpressionAst::Context::Load;
            base->identifier->startCol = col_offset;
            base->identifier->endCol = base->identifier->startCol + base_name.size() - 1;
            base->identifier->startLine = lineno;
            base->identifier->endLine = lineno;
            base->startCol = base->identifier->startCol;
            base->endCol = base->identifier->endCol;
            base->startLine = base->identifier->startLine;
            base->endLine = base->identifier->endLine;
            v->baseClasses.append(base);
        }

        {
            QString identifier = getattr<QString>(node, "identifier");
            if (identifier.size())
            {
                v->identifier = new Python::Identifier(identifier);
                v->identifier->startLine = lineno;
                v->identifier->endLine = lineno;
                v->identifier->startCol = v->baseClasses.at(0)->endCol + 1;
                v->identifier->endCol = v->identifier->startCol + identifier.size() - 1;
            }
        }

        {
            PyObjectRef body = getattr<PyObjectRef>(node, "body");
            v->body = visitEnamlDefBody(body, v);
        }
        result.append(v);

        // Build a with ChildDef() as self to set the block context


    }
    else if (PyObject_IsInstance(node, enaml.ast_Binding)) {
        // eg: x := y
        // qDebug() << "binding";
        auto v = new  Enaml::BindingAst(parent);
        v->startLine = lineno;
        v->startCol = col_offset;
        v->endCol = end_col_offset;
        v->endLine = end_lineno;

        QString name = getattr<QString>(node, "name");
        {
            //auto target = newSelfAttr(lineno, v, name);
            auto target = new Python::NameAst(v);
            target->identifier = new Python::Identifier(name);
            // TODO: Depends on expr op
            target->context = Python::ExpressionAst::Context::Store;
            target->identifier->startLine = lineno;
            target->identifier->startCol = col_offset;
            target->identifier->endLine = lineno;
            target->identifier->endCol = target->identifier->startCol + name.size() - 1;
            target->startLine = target->identifier->startLine;
            target->startCol = target->identifier->startCol;
            target->endLine = target->identifier->endLine;
            target->endCol = target->identifier->endCol;
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
            PyObjectRef expr_value = getattr<PyObjectRef>(expr, "value");
            if (PyObject_IsInstance(expr_value, enaml.ast_PythonModule)) {
                // Enaml generates a function named "f" for expression blocks
                auto body = visitPythonModuleNode(expr_value, parent);
                Q_ASSERT(body.size() == 1);
                auto func = static_cast<Python::FunctionDefinitionAst*>(body.at(0));
                Q_ASSERT(func);

                func->startLine = lineno;
                func->name->value = name + "_expr_" + QString::fromLatin1("%1").arg(lineno+1);
                func->name->startLine = lineno;
                func->name->endLine = lineno;
                func->name->startCol = col_offset;
                func->name->endCol = func->name->startCol + name.size() - 1;
                result.append(func);

                // Generate call
                //auto target = newSelfAttr(lineno, parent, func->name->value);
                auto call = new Python::CallAst(v);
                auto target = new Python::NameAst(call);
                target->identifier = new Python::Identifier(func->name->value);
                target->identifier->startCol = func->name->startCol;
                target->identifier->endCol = func->name->endCol;
                target->identifier->startLine = func->name->startLine;
                target->identifier->endLine = func->name->endLine;
                target->startCol = target->identifier->startCol;
                target->endCol = target->identifier->endCol;
                target->startLine = target->identifier->startLine;
                target->endLine = target->identifier->endLine;

                call->function = target;
                call->startCol = func->name->endCol + 1;
                call->endCol = func->name->endCol + 2;
                call->startLine = lineno;
                call->endLine = lineno;
                v->value = call;
            }
            else {
                v->value = static_cast<Python::ExpressionAst*>(visitPythonExpressionNode(expr_value, v));
            }
        }
        result.append(v);
    }
    else if (PyObject_IsInstance(node, enaml.ast_ExBinding)) {
        // qDebug() << "exbinding";
        // eg: foo.x := y
        auto v = new  Enaml::ExBindingAst(parent);
        v->startLine = lineno;
        v->startCol = col_offset;
        v->endCol = end_col_offset;
        v->endLine = end_lineno;

        {
            PyObject* chain = getattr<PyObjectRef>(node, "chain");
            Q_ASSERT(PyTuple_Check(chain));
            Q_ASSERT(PyTuple_Size(chain) > 0);
            PyObject* first = PyTuple_GET_ITEM(chain, 0);
            Py_INCREF(first);
            QString name = PyUnicodeObjectToQString(first);
            auto target = new Python::NameAst(v);
            target->identifier = new Python::Identifier(name);
            target->context = Python::ExpressionAst::Context::Load;
            target->identifier->startLine = lineno;
            target->identifier->startCol = col_offset;
            target->identifier->endLine = lineno;
            target->identifier->endCol = target->identifier->startCol + name.size() - 1;
            target->startLine = target->identifier->startLine;
            target->startCol = target->identifier->startCol;
            target->endLine = target->identifier->endLine;
            target->endCol = target->identifier->endCol;
            v->target = buildAttrChain(chain, target, Python::ExpressionAst::Context::Store, 1);
            Py_DECREF(first);
            Py_DECREF(chain);

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

            auto attr = static_cast<Python::AttributeAst*>(v->target);
            QString name = attr->attribute->value;

            // qDebug() << "  op " << op;
            PyObjectRef expr_value = getattr<PyObjectRef>(expr, "value");
            if (PyObject_IsInstance(expr_value, enaml.ast_PythonModule)) {
                // Enaml generates a function named "f" for expression blocks
                auto body = visitPythonModuleNode(expr_value, parent);
                Q_ASSERT(body.size() == 1);
                auto func = static_cast<Python::FunctionDefinitionAst*>(body.at(0));
                Q_ASSERT(func);

                func->startLine = lineno;
                func->name->value = name + "_expr_" + QString::fromLatin1("%1").arg(lineno+1);
                func->name->startLine = lineno;
                func->name->endLine = lineno;
                func->name->startCol = col_offset;
                func->name->endCol = func->name->startCol + name.size() - 1;
                result.append(func);

                // Generate call
                //auto target = newSelfAttr(lineno, parent, func->name->value);
                auto call = new Python::CallAst(v);
                auto target = new Python::NameAst(call);
                target->identifier = new Python::Identifier(func->name->value);
                target->identifier->startCol = func->name->startCol;
                target->identifier->endCol = func->name->endCol;
                target->identifier->startLine = func->name->startLine;
                target->identifier->endLine = func->name->endLine;
                target->startCol = target->identifier->startCol;
                target->endCol = target->identifier->endCol;
                target->startLine = target->identifier->startLine;
                target->endLine = target->identifier->endLine;

                call->function = target;
                call->startCol = func->name->endCol + 1;
                call->endCol = func->name->endCol + 2;
                call->startLine = lineno;
                call->endLine = lineno;
                v->value = call;
            }
            else {
                v->value = static_cast<Python::ExpressionAst*>(visitPythonExpressionNode(expr_value, v));
            }
        }
        result.append(v);
    }
    else if (PyObject_IsInstance(node, enaml.ast_FuncDef) || PyObject_IsInstance(node, enaml.ast_AsyncFuncDef)) {
        // qDebug() << "funcdef";
        PyObjectRef funcdef = getattr<PyObjectRef>(node, "funcdef");
        result.append(visitStmtNode(funcdef, parent));
    }
    else if (PyObject_IsInstance(node, enaml.ast_AliasExpr)) {
        // qDebug() << "aliasexpr";
        auto* v = new  Enaml::AliasAst(parent);
        v->startLine = lineno;
        v->startCol = col_offset;
        v->endCol = end_col_offset;
        v->endLine = end_lineno;
        {
            QString name = getattr<QString>(node, "name");
            auto target = new Python::NameAst(v);
            target->identifier = new Python::Identifier(name);
            target->context = Python::ExpressionAst::Context::Store;
            target->identifier->startLine = lineno;
            target->identifier->startCol = col_offset;
            target->identifier->endLine = lineno;
            target->identifier->endCol = target->identifier->startCol + name.size() - 1;
            target->startLine = target->identifier->startLine;
            target->startCol = target->identifier->startCol;
            target->endLine = target->identifier->endLine;
            target->endCol = target->identifier->endCol;
            v->target = target;
        }

        {
            QString target = getattr<QString>(node, "target");
            auto value = new Python::NameAst(v);
            value->identifier = new Python::Identifier(target);
            value->context = Python::ExpressionAst::Context::Load;
            value->identifier->startLine = lineno;
            value->identifier->startCol = col_offset;
            value->identifier->endLine = value->identifier->startLine;
            value->identifier->endCol = value->identifier->startCol + target.size() - 1;
            value->startLine = value->identifier->startLine;
            value->startCol = value->identifier->startCol;
            value->endLine = value->identifier->endLine;
            value->endCol = value->identifier->endCol;

            PyObject* chain = getattr<PyObjectRef>(node, "chain");
            Q_ASSERT(PyTuple_Check(chain));
            if (PyTuple_Size(chain) > 0)
                v->value = buildAttrChain(chain, value, Python::ExpressionAst::Context::Load);
            else
                v->value = value;
            Py_DECREF(chain);
        }
        result.append(v);
    }
    else if (PyObject_IsInstance(node, enaml.ast_StorageExpr)) {
        // qDebug() << "storagexpr";
        auto v = new  Enaml::StorageExprAst(parent);
        v->startLine = lineno;
        v->startCol = col_offset;
        v->endCol = end_col_offset;
        v->endLine = end_lineno;
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
        target->identifier->startLine = lineno;
        target->identifier->startCol = col_offset;
        target->identifier->endLine = lineno;
        target->identifier->endCol = target->identifier->startCol + name.size() - 1;
        target->startLine = target->identifier->startLine;
        target->startCol = target->identifier->startCol;
        target->endLine = target->identifier->endLine;
        target->endCol = target->identifier->endCol;

        // Build value
        Python::ExpressionAst* value = nullptr;
        PyObjectRef expr = getattr<PyObjectRef>(node, "expr");

        if (expr != Py_None)
        {
            QString op = getattr<QString>(expr, "operator");
            if (op == "=" || op == "::")
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
        result.append(v);
    }
    else {
        qWarning() << "Unhandled enaml item" << Python::PyUnicodeObjectToQString(PyObject_Str(node));
    }

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
    auto self = new Python::NameAst(v);
    self->context = Python::ExpressionAst::Load;
    auto id = new Python::Identifier("self");
    self->identifier = id;
    id->startCol = 0;
    id->endCol = 0;
    id->startLine = lineno;
    id->endLine = lineno;
    self->startCol = id->startCol;
    self->endCol = id->endCol;
    self->startLine = lineno;
    self->endLine = lineno;

    v->value = self;
    auto attribute = new Python::Identifier(attr);
    attribute->startLine = lineno;
    attribute->endLine = lineno;
    attribute->startCol = 0;
    attribute->endCol = attr.size() - 1;
    v->attribute = attribute;
    v->startCol = 0;
    v->endCol = attribute->endCol;
    v->startLine = lineno;
    v->endLine = lineno;

    return v;
}

Python::AttributeAst* AstTransformer::buildAttrChain(PyObject* chain, Python::Ast* parent, Python::ExpressionAst::Context ctx, int startIndex)
{
    Q_ASSERT(PyTuple_Check(chain));
    Python::AttributeAst* attr = nullptr;
    Python::Ast* next_parent = parent;
    const int lineno = parent->startLine;
    int col_offset = parent->endCol + 2; // " attr."
    for (int i=startIndex; i < PyTuple_Size(chain); i++) {
        PyObject* item = PyTuple_GET_ITEM(chain, i); // borrowed
        Py_INCREF(item); // Next line does a decref
        QString key = PyUnicodeObjectToQString(item);
        attr = new Python::AttributeAst(next_parent);
        attr->context = Python::ExpressionAst::Context::Load;
        attr->value = static_cast<Python::ExpressionAst*>(next_parent);
        attr->attribute = new Python::Identifier(key);
        attr->attribute->startCol = col_offset;
        attr->attribute->endCol = attr->attribute->startCol + key.size() - 1;
        attr->attribute->startLine = lineno;
        attr->attribute->endLine = lineno;
        attr->startCol = attr->attribute->startCol;
        attr->endCol = attr->attribute->endCol;
        attr->startLine = lineno;
        attr->endLine = lineno;
        next_parent = attr;
        col_offset = attr->attribute->endCol + 2;
    }
    Q_ASSERT(attr);
    attr->context = ctx;
    return attr;
}

} // end namespace Enaml