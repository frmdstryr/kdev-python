/*
    SPDX-FileCopyrightText: 2022 Jairus Martin <jrm@codelv.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "enaml_ast.h"

namespace Enaml {


bool EnamlDefAst::visit(Python::AstVisitor* visitor)
{
    visitor->visitNode(self);
    if (identifier)
        visitor->visitNode(identifier);
    return false;
}


QString EnamlDefAst::dump() const
{
    QString r;
    r.append(isRoot ? "EnamlDef(" : "ChildDef(");
    dumpNode(r, "name=", name);
    dumpNode(r, ", base=", baseClasses.size() ? baseClasses.at(0) : nullptr);
    dumpList(r, ", body=", body, ",\n  ");
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString StorageExprAst::dump() const
{
    QString r = "StorageExpr(";
    dumpNode(r, "target=", target);
    dumpNode(r, ", value=", value);
    dumpNode(r, ", annotation=", annotation);
    switch (kind) {
        case Kind::Attr:
            r.append(", kind=Attr");
            break;
        case Kind::Event:
            r.append(", kind=Event");
            break;
        default:
            r.append(", kind=Invalid");
            break;
    }
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString BindingAst::dump() const
{
    QString r = "Binding(";
    dumpNode(r, "target=", target);
    dumpNode(r, ", value=", value);
    switch (op) {
        case Op::Assignment:
            r.append(", op=Assignment");
            break;
        case Op::Delegation:
            r.append(", op=Delegation");
            break;
        case Op::Subscription:
            r.append(", op=Subscription");
            break;
        case Op::Update:
            r.append(", op=Update");
            break;
        case Op::Notification:
            r.append(", op=Notification");
            break;
        default:
            r.append(", op=Invalid");
            break;
    }
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString ExBindingAst::dump() const
{
    QString r = "ExBinding(";
    dumpNode(r, "target=", target);
    dumpNode(r, ", value=", value);
    switch (op) {
        case Op::Assignment:
            r.append(", op=Assignment");
            break;
        case Op::Delegation:
            r.append(", op=Delegation");
            break;
        case Op::Subscription:
            r.append(", op=Subscription");
            break;
        case Op::Update:
            r.append(", op=Update");
            break;
        case Op::Notification:
            r.append(", op=Notification");
            break;
        default:
            r.append(", op=Invalid");
            break;
    }
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString AliasAst::dump() const
{
    QString r = "Alias(";
    dumpNode(r, "target=", target);
    dumpNode(r, ", value=", value);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

bool TemplateAst::visit(Python::AstVisitor* visitor)
{
    if (identifier)
        visitor->visitNode(identifier);
    return false;
}

} // end namespace Enaml
