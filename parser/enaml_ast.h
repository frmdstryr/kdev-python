#pragma once
#include "ast.h"

namespace Enaml {

class KDEVPYTHONPARSER_EXPORT EnamlDefAst: public Python::ClassDefinitionAst {
public:
    EnamlDefAst(Python::Ast* parent): Python::ClassDefinitionAst(parent) {};
    Python::Identifier* identifier;
};

class KDEVPYTHONPARSER_EXPORT ChildDefAst: public Python::ClassDefinitionAst {
public:
    ChildDefAst(Python::Ast* parent): Python::ClassDefinitionAst(parent) {};
    Python::Identifier* identifier;
};

class KDEVPYTHONPARSER_EXPORT TemplateAst: public Python::ClassDefinitionAst {
public:
    TemplateAst(Python::Ast* parent): Python::ClassDefinitionAst(parent) {};
    Python::Identifier* identifier;
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
};



} // end namespace Enaml
