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
    r.append(isRoot ? QStringLiteral("EnamlDef(") : QStringLiteral("ChildDef("));
    dumpNode(r, QStringLiteral("name="), name);
    dumpNode(r, QStringLiteral(", base="), baseClasses.size() ? baseClasses.at(0) : nullptr);
    dumpList(r, QStringLiteral(", body="), body, QStringLiteral(",\n  "));
    dumpRange(r, QStringLiteral(", range="), this);
    r.append(QStringLiteral(")"));
    return r;
}

QString StorageExprAst::dump() const
{
    QString r = QStringLiteral("StorageExpr(");
    dumpNode(r, QStringLiteral("target="), target);
    dumpNode(r, QStringLiteral(", value="), value);
    dumpNode(r, QStringLiteral(", annotation="), annotation);
    switch (kind) {
        case Kind::Attr:
            r.append(QStringLiteral(", kind=Attr"));
            break;
        case Kind::Event:
            r.append(QStringLiteral(", kind=Event"));
            break;
        default:
            r.append(QStringLiteral(", kind=Invalid"));
            break;
    }
    dumpRange(r, QStringLiteral(", range="), this);
    r.append(QStringLiteral(")"));
    return r;
}

QString BindingAst::dump() const
{
    QString r = QStringLiteral("Binding(");
    dumpNode(r, QStringLiteral("target="), target);
    dumpNode(r, QStringLiteral(", value="), value);
    switch (op) {
        case Op::Assignment:
            r.append(QStringLiteral(", op=Assignment"));
            break;
        case Op::Delegation:
            r.append(QStringLiteral(", op=Delegation"));
            break;
        case Op::Subscription:
            r.append(QStringLiteral(", op=Subscription"));
            break;
        case Op::Update:
            r.append(QStringLiteral(", op=Update"));
            break;
        case Op::Notification:
            r.append(QStringLiteral(", op=Notification"));
            break;
        default:
            r.append(QStringLiteral(", op=Invalid"));
            break;
    }
    dumpRange(r, QStringLiteral(", range="), this);
    r.append(QStringLiteral(")"));
    return r;
}

QString ExBindingAst::dump() const
{
    QString r = QStringLiteral("ExBinding(");
    dumpNode(r, QStringLiteral("target="), target);
    dumpNode(r, QStringLiteral(", value="), value);
    switch (op) {
        case Op::Assignment:
            r.append(QStringLiteral(", op=Assignment"));
            break;
        case Op::Delegation:
            r.append(QStringLiteral(", op=Delegation"));
            break;
        case Op::Subscription:
            r.append(QStringLiteral(", op=Subscription"));
            break;
        case Op::Update:
            r.append(QStringLiteral(", op=Update"));
            break;
        case Op::Notification:
            r.append(QStringLiteral(", op=Notification"));
            break;
        default:
            r.append(QStringLiteral(", op=Invalid"));
            break;
    }
    dumpRange(r, QStringLiteral(", range="), this);
    r.append(QStringLiteral(")"));
    return r;
}

QString AliasAst::dump() const
{
    QString r = QStringLiteral("Alias(");
    dumpNode(r, QStringLiteral("target="), target);
    dumpNode(r, QStringLiteral(", value="), value);
    dumpRange(r, QStringLiteral(", range="), this);
    r.append(QStringLiteral(")"));
    return r;
}

bool TemplateAst::visit(Python::AstVisitor* visitor)
{
    if (identifier)
        visitor->visitNode(identifier);
    return false;
}

} // end namespace Enaml
