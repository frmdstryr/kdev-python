/*
    SPDX-FileCopyrightText: 2007 Andreas Pakulat <apaku@gmx.de>
    SPDX-FileCopyrightText: 2010-2012 Sven Brauch <svenbrauch@googlemail.com>
    SPDX-FileCopyrightText: 2012 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ast.h"
#include "astbuilder.h"
#include <language/duchain/problem.h>

namespace Python
{

void Ast::dumpNode(QString &r, const QString &prefix, const Ast* node)
{
    r.append(prefix);
    r.append(node ? node->dump(): "nullptr");
}

void Ast::dumpRange(QString &r, const QString &prefix, const Ast* node)
{
    r.append(prefix);
    r.append("(");
    r.append(QString::fromLatin1("%1").arg(node->startLine));
    r.append(", ");
    r.append(QString::fromLatin1("%1").arg(node->startCol));
    r.append(", ");
    r.append(QString::fromLatin1("%1").arg(node->endLine));
    r.append(", ");
    r.append(QString::fromLatin1("%1").arg(node->endCol));
    r.append(")");
}

template<class T>
void Ast::dumpList(QString &r, const QString &prefix, const T list, QString sep)
{
    int i = 0;
    r.append(prefix);
    r.append("[");
    foreach(const Ast* node, list) {
        i += 1;
        dumpNode(r, "", node);
        if (i < list.size())
            r.append(sep);
    }
    r.append("]");
}

void dumpContext(QString &r, const QString &prefix, ExpressionAst::Context context)
{
    r.append(prefix);
    switch(context) {
        case ExpressionAst::Context::Load:
            r.append("Load()");
            break;
        case ExpressionAst::Context::Store:
            r.append("Store()");
            break;
        case ExpressionAst::Context::Delete:
            r.append("Delete()");
            break;
        default:
            r.append("Invalid");
    }
}

static void dumpOperator(QString &r, const QString &prefix, Ast::OperatorTypes op)
{
    r.append(prefix);
    switch(op) {
        case Ast::OperatorAdd:
            r.append("Add()");
            break;
        case Ast::OperatorSub:
            r.append("Sub()");
            break;
        case Ast::OperatorMult:
            r.append("Mult()");
            break;
        case Ast::OperatorMatMult:
            r.append("MatMult()");
            break;
        case Ast::OperatorDiv:
            r.append("Div()");
            break;
        case Ast::OperatorPow:
            r.append("Pow()");
            break;
        case Ast::OperatorLeftShift:
            r.append("LShift()");
            break;
        case Ast::OperatorRightShift:
            r.append("RShift()");
            break;
        case Ast::OperatorBitwiseOr:
            r.append("BitwiseOr");
            break;
        case Ast::OperatorBitwiseXor:
            r.append("BitwiseXor()");
            break;
        case Ast::OperatorFloorDivision:
            r.append("FloorDivision()");
            break;
        default:
            r.append("Invalid");
    }
}


// We never need actual constructors for AST nodes, but it seems to be required, at least for some platforms
// so we provide pseudo implementations
// there's nothing happening here, don't bother reading the code
    
Ast::Ast( Ast* parent, Ast::AstType type ) : parent(parent), astType( type ), startCol(0), startLine(-99999), endCol(0), endLine(0), hasUsefulRangeInformation(false), context(nullptr) { }
Ast::Ast() :  parent(nullptr), startCol(0), startLine(-5), endCol(0), endLine(0), hasUsefulRangeInformation(false), context(nullptr) { }

ArgumentsAst::ArgumentsAst(Ast* parent): Ast(parent, Ast::ArgumentsAstType)
{
    
}

QString ArgumentsAst::dump() const
{
    QString r = "arguments(";
    dumpList(r, "posonlyargs=", posonlyargs);
    dumpList(r, ", args=", arguments);
    dumpList(r, ", kwonlyargs=", kwonlyargs);
    dumpList(r, ", defaults=", defaultValues);
    dumpList(r, ", kw_defaults=", defaultKwValues);
    if (vararg)
        dumpNode(r, ", vararg=", vararg);
    if (kwarg)
        dumpNode(r, ", kwarg=", kwarg);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

ArgAst::ArgAst(Ast* parent): Ast(parent, Ast::ArgAstType), argumentName(nullptr), annotation(nullptr)
{

}

QString ArgAst::dump() const
{
    QString r = "arg(";
    dumpNode(r, "name=", argumentName);
    dumpNode(r, ", annotation=", annotation);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

AssertionAst::AssertionAst(Ast* parent): StatementAst(parent, Ast::AssertionAstType) 
{
    
}

QString AssertionAst::dump() const
{
    QString r = "Assertion(";
    dumpNode(r, "condition=", condition);
    dumpNode(r, ", message=", message);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

AssignmentAst::AssignmentAst(Ast* parent): StatementAst(parent, Ast::AssignmentAstType), value(nullptr)
{
    
}

QString AssignmentAst::dump() const
{
    QString r = "Assign(";
    dumpList(r, "targets=", targets);
    dumpNode(r, ", value=", value);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

AttributeAst::AttributeAst(Ast* parent): ExpressionAst(parent, Ast::AttributeAstType), value(nullptr), depth(0)
{
    
}

QString AttributeAst::dump() const
{
    QString r = "Attribute(";
    dumpNode(r, "value=", value);
    dumpNode(r, ", attr=", attribute);
    dumpContext(r, ", ctx=", context);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

AugmentedAssignmentAst::AugmentedAssignmentAst(Ast* parent): StatementAst(parent, Ast::AugmentedAssignmentAstType), value(nullptr)
{
    
}

QString AugmentedAssignmentAst::dump() const
{
    QString r = "AugmentedAssignment(";
    dumpNode(r, "target=", target);
    dumpNode(r, ", value=", value);
    dumpOperator(r, ", op=", op);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

AnnotationAssignmentAst::AnnotationAssignmentAst(Ast* parent): StatementAst(parent, Ast::AnnotationAssignmentAstType), target(nullptr), value(nullptr), annotation(nullptr)
{

}

QString AnnotationAssignmentAst::dump() const
{
    QString r = "AnnotationAssignment(";
    dumpNode(r, "target=", target);
    dumpNode(r, ", value=", value);
    dumpNode(r, ", annotation=", annotation);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}


BinaryOperationAst::BinaryOperationAst(Ast* parent): ExpressionAst(parent, Ast::BinaryOperationAstType), lhs(nullptr), rhs(nullptr)
{

}


QString BinaryOperationAst::dump() const
{
    QString r = "BinaryOperation(";
    dumpNode(r, "lhs=", lhs);
    dumpOperator(r, ", op=", type);
    dumpNode(r, ", rhs=", rhs);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}


BooleanOperationAst::BooleanOperationAst(Ast* parent): ExpressionAst(parent, Ast::BooleanOperationAstType)
{
    
}

QString BooleanOperationAst::dump() const
{
    QString r = "BooleanOperation(type=";
    switch (type) {
        case Ast::BooleanAnd:
            r.append("And");
            break;
        case Ast::BooleanOr:
            r.append("Or");
            break;
        default:
            r.append("Invalid");
    }
    dumpList(r, ", values=", values);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

BreakAst::BreakAst(Ast* parent): StatementAst(parent, Ast::BreakAstType)
{
    
}

CallAst::CallAst(Ast* parent): ExpressionAst(parent, Ast::CallAstType), function(nullptr)
{
    
}


QString CallAst::dump() const
{
    QString r;
    r.append("Call(");
    dumpNode(r, "func=", function);
    dumpList(r, ", args=", arguments);
    dumpList(r, ", keywords=", keywords);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

ClassDefinitionAst::ClassDefinitionAst(Ast* parent): StatementAst(parent, Ast::ClassDefinitionAstType), name(nullptr)
{
    
}


QString ClassDefinitionAst::dump() const
{
    QString r;
    r.append("ClassDef(");
    dumpNode(r, "name=", name);
    dumpList(r, ", bases=", baseClasses);
    dumpList(r, ", body=", body, ",\n  ");
    // TODO: Keywords?
    dumpList(r, ", decorator_list=", decorators);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

CodeAst::CodeAst() : Ast(nullptr, Ast::CodeAstType), name(nullptr)
{
}

CodeAst::~CodeAst()
{
    free_ast_recursive(this);
}

QString CodeAst::dump() const
{
    QString r;
    r.append("Module(");
    dumpNode(r, "name=", name);
    dumpList(r, ", body=", body, ",\n  ");
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

CompareAst::CompareAst(Ast* parent): ExpressionAst(parent, Ast::CompareAstType), leftmostElement(nullptr)
{
    
}

ComprehensionAst::ComprehensionAst(Ast* parent): Ast(parent, Ast::ComprehensionAstType), target(nullptr), iterator(nullptr)
{
    
}


QString ComprehensionAst::dump() const
{
    QString r = "Comprehension(";
    dumpNode(r, "target=", target);
    dumpNode(r, ", iterator=", iterator);
    dumpList(r, ", conditions=", conditions);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

ContinueAst::ContinueAst(Ast* parent): StatementAst(parent, Ast::ContinueAstType)
{
    
}



DeleteAst::DeleteAst(Ast* parent): StatementAst(parent, Ast::DeleteAstType)
{
    
}

QString DeleteAst::dump() const
{
    QString r = "Delete(";
    dumpList(r, "targets=", targets);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}


DictAst::DictAst(Ast* parent): ExpressionAst(parent, Ast::DictAstType)
{

}

QString DictAst::dump() const
{
    QString r = "Dict(";
    dumpList(r, "keys=", keys);
    dumpList(r, ", values=", values);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString ExceptionHandlerAst::dump() const
{
    QString r = "ExceptionHandler(";
    dumpNode(r, "type=", type);
    dumpNode(r, ", name=", name);
    dumpList(r, ", body=", body);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}


SliceAst::SliceAst(Ast* parent): ExpressionAst(parent, Ast::SliceAstType), lower(nullptr), upper(nullptr), step(nullptr)
{

}

QString SliceAst::dump() const
{
    QString r;
    r.append("Slice(");
    dumpNode(r, "lower=", lower);
    dumpNode(r, ", upper=", upper);
    dumpNode(r, ", step=", step);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

DictionaryComprehensionAst::DictionaryComprehensionAst(Ast* parent): ExpressionAst(parent, Ast::DictionaryComprehensionAstType), key(nullptr), value(nullptr)
{
    
}

QString DictionaryComprehensionAst::dump() const
{
    QString r = "DictionaryComprehension(";
    dumpNode(r, "key=", key);
    dumpNode(r, ", value=", value);
    dumpList(r, ", generators=", generators);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

ListComprehensionAst::ListComprehensionAst(Ast* parent): ExpressionAst(parent, Ast::ListComprehensionAstType), element(nullptr)
{

}

QString ListComprehensionAst::dump() const
{
    QString r = "ListComprehension(";
    dumpNode(r, "element=", element);
    dumpList(r, ", generators=", generators);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

ExpressionAst::ExpressionAst(Ast* parent, AstType type): Ast(parent, type), value(nullptr)
{
    
}

AssignmentExpressionAst::AssignmentExpressionAst(Ast* parent): ExpressionAst(parent, Ast::AssignmentExpressionAstType), value(nullptr)
{

}

QString AssignmentExpressionAst::dump() const
{
    QString r = "AssignmentExpression(";
    dumpNode(r, "target=", target);
    dumpNode(r, ", value=", value);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

YieldFromAst::YieldFromAst(Ast* parent) : ExpressionAst(parent, Ast::YieldFromAstType)
{

}

QString YieldFromAst::dump() const
{
    QString r = "YieldFrom(";
    dumpNode(r, "value=", value);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

ForAst::ForAst(Ast* parent): StatementAst(parent, Ast::ForAstType), target(nullptr), iterator(nullptr)
{
    
}

QString ForAst::dump() const
{
    QString r = async ? "AsyncFor(": "For(";
    dumpNode(r, "target=", target);
    dumpNode(r, ", iterator=", iterator);
    dumpList(r, ", body=", body, ",\n    ");
    if (orelse.size())
        dumpList(r, ", orelse=", orelse, ",\n    ");
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

FunctionDefinitionAst::FunctionDefinitionAst(Ast* parent): StatementAst(parent, Ast::FunctionDefinitionAstType), name(nullptr), arguments(nullptr), async(false)
{
    
}

QString FunctionDefinitionAst::dump() const
{
    QString r = async ? "AsyncFuncDef(": "FuncDef(";
    dumpNode(r, "name=", name);
    dumpNode(r, ", args=", arguments);
    dumpList(r, ", body=", body, ",\n    ");
    if (decorators.size())
        dumpList(r, ", decorator_list=", decorators);
    if (returns)
        dumpNode(r, ", returns=", returns);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

GeneratorExpressionAst::GeneratorExpressionAst(Ast* parent): ExpressionAst(parent, Ast::GeneratorExpressionAstType), element(nullptr)
{
    
}

GlobalAst::GlobalAst(Ast* parent): StatementAst(parent, Ast::GlobalAstType)
{
    
}


QString GlobalAst::dump() const
{
    QString r = "Global(";
    dumpList(r, "names=", names);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

Identifier::Identifier(QString value) : Ast(nullptr, Ast::IdentifierAstType), value(value)
{
    
}

QString Identifier::dump() const
{
    QString r = "Identifier(";
    r.append("value='" + value + "'");
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

IfAst::IfAst(Ast* parent): StatementAst(parent, Ast::IfAstType), condition(nullptr)
{
    
}

QString IfAst::dump() const
{
    QString r = "If(";
    dumpNode(r, "condition=", condition);
    dumpList(r, ", body=", body, ",\n    ");
    if (orelse.size())
        dumpList(r, ", orelse=", orelse, ",\n    ");
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

IfExpressionAst::IfExpressionAst(Ast* parent): ExpressionAst(parent, Ast::IfExpressionAstType), condition(nullptr)
{
    
}

QString IfExpressionAst::dump() const
{
    QString r = "IfExpr(";
    dumpNode(r, "condition=", condition);
    dumpNode(r, ", body=", body);
    if (orelse)
        dumpNode(r, ", orelse=", orelse);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

ImportAst::ImportAst(Ast* parent): StatementAst(parent, Ast::ImportAstType)
{
    
}

QString ImportAst::dump() const
{
    QString r = "Import(";
    dumpList(r, "names=", names);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

ImportFromAst::ImportFromAst(Ast* parent): StatementAst(parent, Ast::ImportFromAstType), module(nullptr), level(0)
{
    
}

QString ImportFromAst::dump() const
{
    QString r = "ImportFrom(";
    dumpNode(r, "module=", module);
    dumpList(r, ", names=", names);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

KeywordAst::KeywordAst(Ast* parent): Ast(parent, Ast::KeywordAstType), argumentName(nullptr), value(nullptr)
{
    
}

QString KeywordAst::dump() const
{
    QString r;
    r.append("Keyword(");
    dumpNode(r, "arg=", argumentName);
    dumpNode(r, ", value=", value);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

LambdaAst::LambdaAst(Ast* parent): ExpressionAst(parent, Ast::LambdaAstType), arguments(nullptr)
{
    
}

QString LambdaAst::dump() const
{
    QString r = "Lambda(";
    dumpNode(r, "arguments=", arguments);
    dumpNode(r, ", body=", body);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

ListAst::ListAst(Ast* parent): ExpressionAst(parent, Ast::ListAstType)
{
    
}

QString ListAst::dump() const
{
    QString r = "List(";
    dumpList(r, "elts=", elements);
    dumpContext(r, ", ctx=", context);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

NameAst::NameAst(Ast* parent): ExpressionAst(parent, Ast::NameAstType), identifier(nullptr)
{
    
}

QString NameAst::dump() const
{
    QString r = "Name(";
    dumpNode(r, "id=", identifier);
    dumpContext(r, ", ctx=", context);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

AwaitAst::AwaitAst(Ast* parent): ExpressionAst(parent, Ast::AwaitAstType), value(nullptr)
{

}

QString AwaitAst::dump() const
{
    QString r = "AwaitAst(";
    dumpNode(r, "value=", value);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString NameConstantAst::dump() const
{
    switch (value) {
        case NameConstantAst::NameConstantTypes::False:
            return "False";
        case NameConstantAst::NameConstantTypes::True:
            return "True";
        case NameConstantAst::NameConstantTypes::None:
            return "None";
        default:
            return "Invalid";
    }
}

QString NumberAst::dump() const
{
    if (isInt)
        return QString::fromLatin1("Number(%1)").arg(value);
    else
        return "Float()";
}

PassAst::PassAst(Ast* parent): StatementAst(parent, Ast::PassAstType)
{
    
}

NonlocalAst::NonlocalAst(Ast* parent): StatementAst(parent, Ast::NonlocalAstType)
{
    
}

RaiseAst::RaiseAst(Ast* parent): StatementAst(parent, Ast::RaiseAstType), type(nullptr)
{
    
}


QString RaiseAst::dump() const
{
    QString r;
    r.append("Raise(");
    dumpNode(r, "type=", type);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

ReturnAst::ReturnAst(Ast* parent): StatementAst(parent, Ast::ReturnAstType), value(nullptr)
{
    
}

QString ReturnAst::dump() const
{
    QString r;
    r.append("Return(");
    dumpNode(r, "value=", value);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

SetAst::SetAst(Ast* parent): ExpressionAst(parent, Ast::SetAstType)
{
    
}

QString SetAst::dump() const
{
    QString r = "Set(";
    dumpList(r, "elts=", elements);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

SetComprehensionAst::SetComprehensionAst(Ast* parent): ExpressionAst(parent, Ast::SetComprehensionAstType), element(nullptr)
{
    
}

QString SetComprehensionAst::dump() const
{
    QString r = "SetComprehension(";
    dumpNode(r, "element=", element);
    dumpList(r, ", generators=", generators);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}


StatementAst::StatementAst(Ast* parent, AstType type): Ast(parent, type)
{
    
}

JoinedStringAst::JoinedStringAst(Ast* parent): ExpressionAst(parent, Ast::JoinedStringAstType), values()
{

}

FormattedValueAst::FormattedValueAst(Ast* parent): ExpressionAst(parent, Ast::FormattedValueAstType), value(nullptr), conversion(0), formatSpec(nullptr)
{

}

SubscriptAst::SubscriptAst(Ast* parent): ExpressionAst(parent, Ast::SubscriptAstType), value(nullptr), slice(nullptr)
{
    
}

QString SubscriptAst::dump() const
{
    QString r;
    r.append("Subscript(");
    dumpNode(r, "value=", value);
    dumpNode(r, ", slice=", slice);
    dumpContext(r, ", context=", context);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

StarredAst::StarredAst(Ast* parent): ExpressionAst(parent, Ast::StarredAstType)
{
    
}

QString StarredAst::dump() const
{
    QString r;
    r.append("Starred(");
    dumpNode(r, "value=", value);
    dumpContext(r, ", context=", context);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

TupleAst::TupleAst(Ast* parent): ExpressionAst(parent, Ast::TupleAstType)
{
    
}

QString TupleAst::dump() const
{
    QString r = "Tuple(";
    dumpList(r, "elts=", elements);
    dumpContext(r, ", context=", context);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}


UnaryOperationAst::UnaryOperationAst(Ast* parent): ExpressionAst(parent, Ast::UnaryOperationAstType), operand(nullptr)
{
    
}

QString UnaryOperationAst::dump() const {
    QString r;
    r.append("Unary(");
    dumpNode(r, "value=", operand);
    r.append(", op=");
    switch(type) {
       case Ast::UnaryOperatorInvert:
            r.append("Invert()");
            break;
        case Ast::UnaryOperatorNot:
            r.append("Not()");
            break;
        case Ast::UnaryOperatorAdd:
            r.append("UAdd()");
            break;
        case Ast::UnaryOperatorSub:
            r.append("USub()");
            break;
        default:
            r.append("Invalid");
    }
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

TryStarAst::TryStarAst(Ast* parent): StatementAst(parent, Ast::TryStarAstType)
{

}

TryAst::TryAst(Ast* parent): StatementAst(parent, Ast::TryAstType)
{

}

QString TryAst::dump() const
{
    QString r = "Try(";
    dumpList(r, "body=", body, ",\n    ");
    dumpList(r, ", handlers=", handlers);
    if (orelse.size())
        dumpList(r, ", orelse=", orelse, ",\n    ");
    if (finally.size())
        dumpList(r, ", finally=", finally, ",\n    ");
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}


WhileAst::WhileAst(Ast* parent): StatementAst(parent, Ast::WhileAstType), condition(nullptr)
{
    
}

QString WhileAst::dump() const
{
    QString r = "While(";
    dumpNode(r, "condition=", condition);
    dumpList(r, ", body=", body, ",\n    ");
    if (orelse.size())
        dumpList(r, ", orelse=", orelse, ",\n    ");
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}


WithAst::WithAst(Ast* parent): StatementAst(parent, Ast::WithAstType)
{
    
}

QString WithAst::dump() const
{
    QString r = async ? "AsyncWith(": "With(";
    dumpList(r, ", items=", items);
    dumpList(r, ", body=", body, ",\n    ");
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

WithItemAst::WithItemAst(Ast* parent): Ast(parent, Ast::WithItemAstType)
{

}

QString WithItemAst::dump() const
{
    QString r = "WithItem(";
    dumpNode(r, ", contextExpression=", optionalVars);
    dumpNode(r, ", optionalVars=", contextExpression);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

YieldAst::YieldAst(Ast* parent): ExpressionAst(parent, Ast::YieldAstType), value(nullptr)
{
    
}

QString YieldAst::dump() const
{
    QString r = "Yield(";
    dumpNode(r, "value=", value);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString AliasAst::dump() const
{
    QString r = "Alias(";
    dumpNode(r, "name=", name);
    dumpNode(r, ", as=", asName);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString MatchAst::dump() const
{
    QString r = "Match(";
    dumpNode(r, "subject=", subject);
    dumpList(r, ", cases=", cases);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString MatchCaseAst::dump() const
{
    QString r = "MatchCase(";
    dumpNode(r, "pattern=", pattern);
    if (guard)
        dumpNode(r, ", guard=", guard);
    dumpList(r, ", body=", body);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString MatchValueAst::dump() const
{
    QString r = "MatchValue(";
    dumpNode(r, "value=", value);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString MatchSingletonAst::dump() const
{
    QString r = "MatchSingleton(";
    switch (value) {
        case NameConstantAst::NameConstantTypes::True:
            r.append("value=True");
            break;
        case NameConstantAst::NameConstantTypes::False:
            r.append("value=False");
            break;
        case NameConstantAst::NameConstantTypes::None:
            r.append("value=None");
            break;
        default:
            r.append("value=Invalid");
    }
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString MatchSequenceAst::dump() const
{
    QString r = "MatchSequence(";
    dumpList(r, "patterns=", patterns);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString MatchMappingAst::dump() const
{
    QString r = "MatchMapping(";
    dumpList(r, "keys=", keys);
    dumpList(r, ", patterns=", patterns);
    dumpNode(r, ", rest=", rest);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString MatchClassAst::dump() const
{
    QString r = "MatchClass(";
    dumpNode(r, "cls=", cls);
    dumpList(r, ", patterns=", patterns);
    dumpNode(r, ", kwd_attrs=", kwdAttrs);
    dumpList(r, ", kwd_patterns=", kwdPatterns);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString MatchStarAst::dump() const
{
    QString r = "MatchStar(";
    dumpNode(r, "name=", name);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString MatchAsAst::dump() const
{
    QString r = "MatchAs(";
    dumpNode(r, "name=", name);
    if (pattern)
        dumpNode(r, ", pattern=", pattern);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}

QString MatchOrAst::dump() const
{
    QString r = "MatchOr(";
    dumpList(r, "patterns=", patterns);
    dumpRange(r, ", range=", this);
    r.append(")");
    return r;
}


}
