/*
    SPDX-FileCopyrightText: 2022 Jairus Martin <jrm@codelv.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include "ast.h"
#include "astvisitor.h"

namespace Enaml {

class KDEVPYTHONPARSER_EXPORT EnamlDefAst: public Python::ClassDefinitionAst {
public:
    EnamlDefAst(Python::Ast* parent): Python::ClassDefinitionAst(parent) {};
    Python::Identifier* self = nullptr;
    Python::NameAst* identifier = nullptr;
    bool visit(Python::AstVisitor* visitor) override;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT ChildDefAst: public Python::ClassDefinitionAst {
public:
    ChildDefAst(Python::Ast* parent): Python::ClassDefinitionAst(parent) {};
    bool visit(Python::AstVisitor* visitor) override;
    Python::Identifier* self = nullptr;
    Python::NameAst* identifier = nullptr;
};

class KDEVPYTHONPARSER_EXPORT TemplateAst: public Python::ClassDefinitionAst {
public:
    TemplateAst(Python::Ast* parent): Python::ClassDefinitionAst(parent) {};
    bool visit(Python::AstVisitor* visitor) override;
    Python::Identifier* self = nullptr;
    Python::NameAst* identifier = nullptr;
};

class KDEVPYTHONPARSER_EXPORT StorageExprAst: public Python::AnnotationAssignmentAst {
public:
    StorageExprAst(Python::Ast* parent): Python::AnnotationAssignmentAst(parent) {};

    enum Kind {
        Attr = 1,
        Event = 2,
        Invalid = -1,
    };

    Kind kind = Invalid;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT BindingAst: public Python::AnnotationAssignmentAst {
public:
    BindingAst(Python::Ast* parent): Python::AnnotationAssignmentAst(parent) {};
    enum Op {
        Assignment = 1,
        Delegation = 2,
        Subscription = 3,
        Update = 4,
        Notification = 5,
        Invalid = -1,
    };
    Op op = Invalid;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT ExBindingAst: public BindingAst {
public:
    ExBindingAst(Python::Ast* parent): BindingAst(parent) {};
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT AliasAst: public Python::AnnotationAssignmentAst {
public:
    AliasAst(Python::Ast* parent): Python::AnnotationAssignmentAst(parent) {};
    QString dump() const override;
};



} // end namespace Enaml
