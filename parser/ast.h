/*
    SPDX-FileCopyrightText: 2007 Andreas Pakulat <apaku@gmx.de>
    SPDX-FileCopyrightText: 2012 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// The Python 3.4 Language Reference was used as basis for this AST

#ifndef PYTHON_AST_H
#define PYTHON_AST_H

#include <QList>
#include <QString>
#include <QSharedPointer>
#include <KTextEditor/Range>
#include "parserexport.h"
#include "kdevpythonversion.h"

namespace KDevelop
{
    class DUContext;
}

namespace Python {
    class StatementAst;
    class FunctionDefinitionAst;
    class AssignmentAst;
    class PassAst;
    class NonlocalAst;
    class ExpressionAst;
    class NameAst;
    class CallAst;
    class AttributeAst;
    class ArgumentsAst;
    class KeywordAst;
    
    class ExpressionAst;
    class StatementAst;
    class Ast;
    class AstVisitor;
    class ExceptionHandlerAst;
    class AliasAst;
    class ComprehensionAst;
    class SliceAst;
}

namespace Python
{


// Base class for all other Abstract Syntax Tree classes
class KDEVPYTHONPARSER_EXPORT Ast
{
public:
    enum AstType
    {
        StatementAstType,
        FunctionDefinitionAstType,
        AssignmentAstType,
        PassAstType,
        NonlocalAstType,
        ArgumentsAstType,
        ArgAstType,
        KeywordAstType,
        ClassDefinitionAstType,
        ReturnAstType,
        DeleteAstType,
        ForAstType,
        WhileAstType,
        IfAstType,
        WithAstType,
        WithItemAstType,
        RaiseAstType,
        TryAstType,
        TryStarAstType,
        ImportAstType,
        ImportFromAstType,
        GlobalAstType,
        BreakAstType,
        ContinueAstType,
        AssertionAstType,
        AugmentedAssignmentAstType,
        AnnotationAssignmentAstType,
        MatchAstType,
        LastStatementType,

        ExpressionAstType, // everything below is an expression
        AwaitAstType,
        ConstantAstType,
        NameAstType,
        NameConstantAstType,
        CallAstType,
        AttributeAstType,
        DictionaryComprehensionAstType,
        BooleanOperationAstType,
        BinaryOperationAstType,
        UnaryOperationAstType,
        LambdaAstType,
        IfExpressionAstType, // the short one, if a then b else c
        DictAstType,
        SetAstType,
        ListComprehensionAstType,
        SetComprehensionAstType,
        GeneratorExpressionAstType,
        YieldAstType,
        CompareAstType,
        NumberAstType,
        StringAstType,
        JoinedStringAstType,
        FormattedValueAstType,
        BytesAstType,
        SubscriptAstType,
        StarredAstType,
        ListAstType,
        TupleAstType,
        YieldFromAstType,
        ComprehensionAstType,
        SliceAstType,
        EllipsisAstType,
        AssignmentExpressionAstType,
        LastExpressionType, // keep this at the end of the expr ast list

        // everything below is a pattern
        MatchCaseAstType,
        PatternAstType,
        MatchValueAstType,
        MatchSingletonAstType,
        MatchSequenceAstType,
        MatchMappingAstType,
        MatchClassAstType,
        MatchStarAstType,
        MatchAsAstType,
        MatchOrAstType,
        LastPatternType,

        CodeAstType,
        ExceptionHandlerAstType,
        AliasAstType, // for imports
        IdentifierAstType,
        LastAstType // the largest one, not valid!
    };
    
    enum BooleanOperationTypes {
        BooleanAnd = 1,
        BooleanOr,
        BooleanInvalidOperation
    };
    
    enum OperatorTypes {
        OperatorAdd = 1,
        OperatorSub,
        OperatorMult,
#if PYTHON_VERSION >= QT_VERSION_CHECK(3, 5, 0)
        OperatorMatMult,
#endif
        OperatorDiv,
        OperatorMod,
        OperatorPow,
        OperatorLeftShift,
        OperatorRightShift,
        OperatorBitwiseOr,
        OperatorBitwiseXor,
        OperatorBitwiseAnd,
        OperatorFloorDivision,
        OperatorInvalid
    };
    
    enum UnaryOperatorTypes {
        UnaryOperatorInvert = 1,
        UnaryOperatorNot,
        UnaryOperatorAdd,
        UnaryOperatorSub,
        UnaryOperatorInvalid
    };
    
    enum ComparisonOperatorTypes {
        ComparisonOperatorEquals = 1,
        ComparisonOperatorNotEquals,
        ComparisonOperatorLessThan,
        ComparisonOperatorLessThanEqual,
        ComparisonOperatorGreaterThan,
        ComparisonOperatorGreaterThanEqual,
        ComparisonOperatorIs,
        ComparisonOperatorIsNot,
        ComparisonOperatorIn,
        ComparisonOperatorNotIn,
        ComparisonOperatorInvalid
    };

    Ast(Ast* parent, AstType type);
    Ast();
    virtual ~Ast() {};
    Ast* parent = nullptr;
    AstType astType;

    bool isExpression() const {
        return astType >= ExpressionAstType && astType <= LastExpressionType;
    }

    void copyRange(const Ast* other) {
        startCol = other->startCol;
        endCol = other->endCol;
        startLine = other->startLine;
        endLine = other->endLine;
    }
    
    bool appearsBefore(const Ast* other) {
        return startLine < other->startLine || ( startLine == other->startLine && startCol < other->startCol );
    }
    
    const KTextEditor::Range range() const {
        return KTextEditor::Range(startLine, startCol, endLine, endCol);
    };
    
    const KTextEditor::Cursor start() const {
        return {startLine, startCol};
    }
    
    const KTextEditor::Cursor end() const {
        return {endLine, endCol};
    }

    bool isChildOf(Ast* other) const {
        const Ast* parent = this;
        while ( parent ) {
            if ( parent == other ) {
                return true;
            }
            parent = parent->parent;
        }
        return false;
    }

    static QString dump(const Ast *node)
    {
        if (!node) return QStringLiteral("nullptr");
        QString r = QStringLiteral("Ast(astType=");
        r.append(QString::fromLatin1("%1").arg(node->astType));
        Ast::dumpRange(r, QStringLiteral(", range="), node);
        r.append(QStringLiteral(")"));
        return r;
    }

    virtual QString dump() const { return Ast::dump(this); }
    static void dumpRange(QString &r,const QString &prefix, const Ast* node);
    static void dumpNode(QString &r, const QString &prefix, const Ast* node);
    template<class T>
    static void dumpList(QString &r, const QString &prefix, const T list, QString sep=QStringLiteral(", "));

    // Override to hook custom Ast nodes into the default visitor.
    // return true to prevent default handling for the ast type
    virtual bool visit(AstVisitor* visitor) {
        Q_UNUSED(visitor);
        return false;
    };

    int startCol = 0;
    int startLine = 0;
    int endCol = 0;
    int endLine = 0;
    
    bool hasUsefulRangeInformation = false;
    
    KDevelop::DUContext* context = nullptr;
};

class KDEVPYTHONPARSER_EXPORT Identifier : public Ast {
public:
    Identifier(QString value);
    bool operator==(const Identifier& rhs) const {
        return value == rhs.value;
    };
    bool operator==(const QString& rhs) const {
        return value == rhs;
    };
    void setEndColumn() {
        endCol = startCol + value.length() - 1;
    }
    operator QString() const {
        return value;
    };
    QString value;

    QString dump() const override;
};

// this replaces ModuleAst
class KDEVPYTHONPARSER_EXPORT CodeAst : public Ast {
public:
    CodeAst();
    ~CodeAst();
    typedef QSharedPointer<CodeAst> Ptr;
    QList<Ast*> body;
    Identifier* name = nullptr; // module name
    QString dump() const override;

};

/** Statement classes **/
class KDEVPYTHONPARSER_EXPORT StatementAst : public Ast {
public:
    StatementAst(Ast* parent, AstType type);
};

class KDEVPYTHONPARSER_EXPORT FunctionDefinitionAst : public StatementAst {
public:
    FunctionDefinitionAst(Ast* parent);
    Identifier* name = nullptr;
    ArgumentsAst* arguments = nullptr;
    QList<ExpressionAst*> decorators;
    QList<Ast*> body;
    ExpressionAst* returns;
    bool async = false;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT ClassDefinitionAst : public StatementAst {
public:
    ClassDefinitionAst(Ast* parent);
    Identifier* name = nullptr;
    QList<ExpressionAst*> baseClasses;
    QList<Ast*> body;
    QList<ExpressionAst*> decorators;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT ReturnAst : public StatementAst {
public:
    ReturnAst(Ast* parent);
    ExpressionAst* value = nullptr;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT DeleteAst : public StatementAst {
public:
    DeleteAst(Ast* parent);
    QList<ExpressionAst*> targets;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT AssignmentAst : public StatementAst {
public:
    AssignmentAst(Ast* parent);
    QList<ExpressionAst*> targets;
    ExpressionAst* value = nullptr;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT AugmentedAssignmentAst : public StatementAst {
public:
    AugmentedAssignmentAst(Ast* parent);
    ExpressionAst* target = nullptr;
    Ast::OperatorTypes op = Ast::OperatorInvalid;
    ExpressionAst* value = nullptr;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT AnnotationAssignmentAst : public StatementAst {
public:
    AnnotationAssignmentAst(Ast* parent);
    ExpressionAst* target = nullptr;
    ExpressionAst* value = nullptr;
    ExpressionAst* annotation = nullptr;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT ForAst : public StatementAst {
public:
    ForAst(Ast* parent);
    ExpressionAst* target = nullptr;
    ExpressionAst* iterator = nullptr;
    QList<Ast*> body;
    QList<Ast*> orelse;
    bool async = false;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT WhileAst : public StatementAst {
public:
    WhileAst(Ast* parent);
    ExpressionAst* condition = nullptr;
    QList<Ast*> body;
    QList<Ast*> orelse;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT IfAst : public StatementAst {
public:
    IfAst(Ast* parent);
    ExpressionAst* condition = nullptr;
    QList<Ast*> body;
    QList<Ast*> orelse;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT WithItemAst : public Ast {
public:
    WithItemAst(Ast* parent);
    ExpressionAst* contextExpression = nullptr;
    ExpressionAst* optionalVars = nullptr;
};

class KDEVPYTHONPARSER_EXPORT WithAst : public StatementAst {
public:
    WithAst(Ast* parent);
    QList<Ast*> body;
    QList<WithItemAst*> items;
    bool async = false;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT RaiseAst : public StatementAst {
public:
    RaiseAst(Ast* parent);
    ExpressionAst* type = nullptr;
    // TODO check what the other things in the grammar actually are and add them
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT TryAst : public StatementAst {
public:
    TryAst(Ast* parent);
    QList<Ast*> body;
    QList<ExceptionHandlerAst*> handlers;
    QList<Ast*> orelse;
    QList<Ast*> finally;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT AssertionAst : public StatementAst {
public:
    AssertionAst(Ast* parent);
    ExpressionAst* condition = nullptr;
    ExpressionAst* message = nullptr;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT TryStarAst : public StatementAst {
public:
    TryStarAst(Ast* parent);
    QList<Ast*> body;
    QList<ExceptionHandlerAst*> handlers;
    QList<Ast*> orelse;
    QList<Ast*> finally;
};

class KDEVPYTHONPARSER_EXPORT ImportAst : public StatementAst {
public:
    ImportAst(Ast* parent);
    QList<AliasAst*> names;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT ImportFromAst : public StatementAst {
public:
    ImportFromAst(Ast* parent);
    Identifier* module = nullptr;
    QList<AliasAst*> names;
    int level = 0;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT GlobalAst : public StatementAst {
public:
    GlobalAst(Ast* parent);
    QList<Identifier*> names;
    QString dump() const override;
};

// TODO what's stmt::Expr(expr value) in the grammar and what do we need it for?

class KDEVPYTHONPARSER_EXPORT BreakAst : public StatementAst {
public:
    BreakAst(Ast* parent);
    QString dump() const override { return QStringLiteral("Break()"); }
};

class KDEVPYTHONPARSER_EXPORT ContinueAst : public StatementAst {
public:
    ContinueAst(Ast* parent);
    QString dump() const override { return QStringLiteral("Continue()"); }
};

class KDEVPYTHONPARSER_EXPORT PassAst : public StatementAst {
public:
    PassAst(Ast* parent);
    QString dump() const override { return QStringLiteral("Pass()"); }
};

class KDEVPYTHONPARSER_EXPORT NonlocalAst : public StatementAst {
public:
    NonlocalAst(Ast* parent);
    QString dump() const override { return QStringLiteral("Nonlocal()"); }
};


/** Expression classes **/
class KDEVPYTHONPARSER_EXPORT ExpressionAst : public Ast {
public:
    ExpressionAst(Ast* parent, AstType type = Ast::ExpressionAstType);
    enum Context {
        Load = 1, // the object is read
        Store = 2, // the object is written
        Delete = 3, // the object is deleted
        Invalid = -1
    };
    ExpressionAst* value = nullptr; // WARNING this is not set in most cases!
};

class KDEVPYTHONPARSER_EXPORT ConstantAst : public ExpressionAst {
public:
    ConstantAst(Ast* parent, AstType type = Ast::ConstantAstType): ExpressionAst(parent, type) {}
    // TODO: Python 3.8 + removed classes ast.Num, ast.Str, ast.Bytes, ast.NameConstant and ast.Ellipsis
};

class KDEVPYTHONPARSER_EXPORT AssignmentExpressionAst : public ExpressionAst {
public:
    AssignmentExpressionAst(Ast* parent);
    ExpressionAst* target = nullptr;
    ExpressionAst* value = nullptr;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT AwaitAst : public ExpressionAst {
public:
    AwaitAst(Ast* parent);
    ExpressionAst* value = nullptr;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT YieldFromAst : public ExpressionAst {
public:
    YieldFromAst(Ast* parent);
    ExpressionAst* value = nullptr;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT BooleanOperationAst : public ExpressionAst {
public:
    BooleanOperationAst(Ast* parent);
    Ast::BooleanOperationTypes type = Ast::BooleanInvalidOperation;
    QList<ExpressionAst*> values;
};

class KDEVPYTHONPARSER_EXPORT BinaryOperationAst : public ExpressionAst {
public:
    BinaryOperationAst(Ast* parent);
    Ast::OperatorTypes type  = Ast::OperatorInvalid;
    ExpressionAst* lhs = nullptr;
    ExpressionAst* rhs = nullptr;
    inline QString methodName() const {
        switch ( type ) {
            case Python::Ast::OperatorAdd: return QLatin1String("__add__");
            case Python::Ast::OperatorBitwiseAnd: return QLatin1String("__and__");
            case Python::Ast::OperatorBitwiseOr: return QLatin1String("__or__");
            case Python::Ast::OperatorBitwiseXor: return QLatin1String("__xor__");
            case Python::Ast::OperatorDiv: return QLatin1String("__div__");
            case Python::Ast::OperatorFloorDivision: return QLatin1String("__floordiv__");
            case Python::Ast::OperatorLeftShift: return QLatin1String("__lshift__");
            case Python::Ast::OperatorMod: return QLatin1String("__mod__");
            case Python::Ast::OperatorMult: return QLatin1String("__mul__");
#if PYTHON_VERSION >= QT_VERSION_CHECK(3, 5, 0)
            case Python::Ast::OperatorMatMult: return QLatin1String("__matmul__");
#endif
            case Python::Ast::OperatorPow: return QLatin1String("__pow__");
            case Python::Ast::OperatorRightShift: return QLatin1String("__rshift__");
            case Python::Ast::OperatorSub: return QLatin1String("__sub__");
            case Python::Ast::OperatorInvalid: // fallthrough
            default: return QString();
        }
    };
    // incremental methods, for e.g. a += 3
    inline QString incMethodName() const {
        QString name = methodName();
        if ( name.size() < 3 ) {
            return name;
        }
        name.insert(2, QLatin1Char('i'));
        return name;
    }
};

class KDEVPYTHONPARSER_EXPORT UnaryOperationAst : public ExpressionAst {
public:
    UnaryOperationAst(Ast* parent);
    Ast::UnaryOperatorTypes type = Ast::UnaryOperatorInvalid;
    ExpressionAst* operand = nullptr;

    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT LambdaAst : public ExpressionAst {
public:
    LambdaAst(Ast* parent);
    ArgumentsAst* arguments = nullptr;
    ExpressionAst* body = nullptr;
};

class KDEVPYTHONPARSER_EXPORT IfExpressionAst : public ExpressionAst {
public:
    IfExpressionAst(Ast* parent);
    ExpressionAst* condition = nullptr;
    ExpressionAst* body = nullptr;
    ExpressionAst* orelse = nullptr;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT DictAst : public ExpressionAst {
public:
    DictAst(Ast* parent);
    QList<ExpressionAst*> keys; // WARNING: Can contain null elements: `{**other}`
    QList<ExpressionAst*> values;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT SetAst : public ExpressionAst {
public:
    SetAst(Ast* parent);
    QList<ExpressionAst*> elements;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT ListComprehensionAst : public ExpressionAst {
public:
    ListComprehensionAst(Ast* parent);
    ExpressionAst* element = nullptr;
    QList<ComprehensionAst*> generators;
};

class KDEVPYTHONPARSER_EXPORT SetComprehensionAst : public ExpressionAst {
public:
    SetComprehensionAst(Ast* parent);
    ExpressionAst* element = nullptr;
    QList<ComprehensionAst*> generators;
};

class KDEVPYTHONPARSER_EXPORT DictionaryComprehensionAst : public ExpressionAst {
public:
    DictionaryComprehensionAst(Ast* parent);
    ExpressionAst* key = nullptr;
    ExpressionAst* value = nullptr;
    QList<ComprehensionAst*> generators;
};

class KDEVPYTHONPARSER_EXPORT GeneratorExpressionAst : public ExpressionAst {
public:
    GeneratorExpressionAst(Ast* parent);
    ExpressionAst* element = nullptr;
    QList<ComprehensionAst*> generators;
};

class KDEVPYTHONPARSER_EXPORT CompareAst : public ExpressionAst {
public:
    CompareAst(Ast* parent);
    ExpressionAst* leftmostElement = nullptr;
    QList<ComparisonOperatorTypes> operators;
    QList<ExpressionAst*> comparands;
};

// TODO whats this exactly?
class KDEVPYTHONPARSER_EXPORT ReprAst : public ExpressionAst {
public:
    ReprAst(Ast* parent);
    ExpressionAst* value = nullptr;
};

class KDEVPYTHONPARSER_EXPORT NumberAst : public ConstantAst {
public:
    NumberAst(Ast* parent) : ConstantAst(parent, Ast::NumberAstType) {}
    long value = 0; // only used for ints
    bool isInt = false; // otherwise it's a float
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT StringAst : public ConstantAst {
public:
    StringAst(Ast* parent) : ConstantAst(parent, Ast::StringAstType) {}
    QString value = QString();
    bool usedAsComment = false;
    QString dump() const override { return QStringLiteral("Str('") + value + QStringLiteral("')"); }
};

class KDEVPYTHONPARSER_EXPORT JoinedStringAst : public ExpressionAst {
public:
    JoinedStringAst(Ast* parent);
    QList<ExpressionAst*> values;
};

class KDEVPYTHONPARSER_EXPORT FormattedValueAst : public ExpressionAst {
public:
    FormattedValueAst(Ast* parent);
    ExpressionAst* value = nullptr;
    int conversion = 0;
    ExpressionAst* formatSpec = nullptr;
};

class KDEVPYTHONPARSER_EXPORT BytesAst : public ConstantAst {
public:
    BytesAst(Ast* parent) : ConstantAst(parent, Ast::BytesAstType) {};
    QString value = QString();
};

class KDEVPYTHONPARSER_EXPORT YieldAst : public ExpressionAst {
public:
    YieldAst(Ast* parent);
    ExpressionAst* value = nullptr;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT NameAst : public ExpressionAst {
public:
    NameAst(Ast* parent);
    Identifier* identifier = nullptr;
    ExpressionAst::Context context = ExpressionAst::Context::Invalid;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT NameConstantAst : public ConstantAst {
public:
    NameConstantAst(Ast* parent) : ConstantAst(parent, Ast::NameConstantAstType) {}
    enum NameConstantTypes {
        False,
        True,
        None,
        Invalid // should not happen
    };
    NameConstantTypes value = Invalid;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT CallAst : public ExpressionAst {
public:
    CallAst(Ast* parent);
    ExpressionAst* function = nullptr;
    QList<ExpressionAst*> arguments;
    QList<KeywordAst*> keywords;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT AttributeAst : public ExpressionAst {
public:
    AttributeAst(Ast* parent);
    ExpressionAst* value = nullptr;
    Identifier* attribute = nullptr;
    ExpressionAst::Context context = ExpressionAst::Context::Invalid;
    int depth = 0;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT SubscriptAst : public ExpressionAst {
public:
    SubscriptAst(Ast* parent);
    ExpressionAst* value = nullptr;
    ExpressionAst* slice = nullptr;
    ExpressionAst::Context context = ExpressionAst::Context::Invalid;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT StarredAst : public ExpressionAst {
public:
    StarredAst(Ast* parent);
    ExpressionAst* value = nullptr;
    ExpressionAst::Context context = ExpressionAst::Context::Invalid;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT ListAst : public ExpressionAst {
public:
    ListAst(Ast* parent);
    QList<ExpressionAst*> elements;
    ExpressionAst::Context context = ExpressionAst::Context::Invalid;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT TupleAst : public ExpressionAst {
public:
    TupleAst(Ast* parent);
    QList<ExpressionAst*> elements;
    ExpressionAst::Context context = ExpressionAst::Context::Invalid;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT EllipsisAst : public ConstantAst {
public:
    EllipsisAst(Ast* parent) : ConstantAst(parent, Ast::EllipsisAstType) {};
    QString dump() const override { return QStringLiteral("Ellipsis()"); }
};

class KDEVPYTHONPARSER_EXPORT SliceAst : public ExpressionAst {
public:
    SliceAst(Ast* parent);
    ExpressionAst* lower = nullptr;
    ExpressionAst* upper = nullptr;
    ExpressionAst* step = nullptr;
    QString dump() const override;
};


/** Independent classes **/

class KDEVPYTHONPARSER_EXPORT ArgAst : public Ast {
public:
    ArgAst(Ast* parent);
    Identifier* argumentName = nullptr;
    ExpressionAst* annotation = nullptr;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT ArgumentsAst : public Ast {
public:
    ArgumentsAst(Ast* parent);
    QList<ArgAst*> arguments;
    QList<ArgAst*> kwonlyargs;
    QList<ArgAst*> posonlyargs;
    QList<ExpressionAst*> defaultValues;
    QList<ExpressionAst*> defaultKwValues;
    ArgAst* vararg = nullptr;
    ArgAst* kwarg = nullptr;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT KeywordAst : public Ast {
public:
    KeywordAst(Ast* parent);
    Identifier* argumentName = nullptr;
    ExpressionAst* value = nullptr;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT ComprehensionAst : public Ast {
public:
    ComprehensionAst(Ast* parent);
    ExpressionAst* target = nullptr;
    ExpressionAst* iterator = nullptr;
    QList<ExpressionAst*> conditions;
};

class KDEVPYTHONPARSER_EXPORT ExceptionHandlerAst : public Ast {
public:
    ExceptionHandlerAst(Ast* parent) : Ast(parent, Ast::ExceptionHandlerAstType) {};
    ExpressionAst* type = nullptr;
    Identifier* name = nullptr;
    QList<Ast*> body;
};

class KDEVPYTHONPARSER_EXPORT AliasAst : public Ast {
public:
    AliasAst(Ast* parent) : Ast(parent, Ast::AliasAstType) {};
    Identifier* name = nullptr;
    Identifier* asName = nullptr;
    QString dump() const override;
};


/** Match classes **/

class KDEVPYTHONPARSER_EXPORT PatternAst : public Ast {
public:
    PatternAst(Ast* parent, AstType type = Ast::PatternAstType): Ast(parent, type) {};
};

class KDEVPYTHONPARSER_EXPORT MatchCaseAst : public Ast {
public:
    MatchCaseAst(Ast* parent) : Ast(parent, Ast::MatchCaseAstType) {};
    PatternAst* pattern = nullptr;
    ExpressionAst* guard = nullptr;
    QList<Ast*> body;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT MatchAst : public Ast {
public:
    MatchAst(Ast* parent) : Ast(parent, Ast::MatchAstType) {};
    ExpressionAst* subject = nullptr;
    QList<MatchCaseAst*> cases;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT MatchValueAst : public PatternAst {
public:
    MatchValueAst(Ast* parent): PatternAst(parent, Ast::MatchValueAstType) {};
    ExpressionAst* value = nullptr;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT MatchSingletonAst : public PatternAst {
public:
    MatchSingletonAst(Ast* parent): PatternAst(parent, Ast::MatchSingletonAstType) {};
    NameConstantAst::NameConstantTypes value = NameConstantAst::NameConstantTypes::Invalid;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT MatchSequenceAst : public PatternAst {
public:
    MatchSequenceAst(Ast* parent) : PatternAst(parent, Ast::MatchSequenceAstType) {};
    QList<PatternAst*> patterns;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT MatchMappingAst : public PatternAst {
public:
    MatchMappingAst(Ast* parent) : PatternAst(parent, Ast::MatchMappingAstType) {};
    QList<ExpressionAst*> keys;
    Identifier *rest = nullptr;
    QList<PatternAst*> patterns;
    QString dump() const override;
};


class KDEVPYTHONPARSER_EXPORT MatchClassAst : public PatternAst {
public:
    MatchClassAst(Ast* parent): PatternAst(parent, Ast::MatchClassAstType) {};
    ExpressionAst* cls = nullptr;
    QList<PatternAst*> patterns;
    Identifier *kwdAttrs = nullptr;
    QList<PatternAst*> kwdPatterns;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT MatchStarAst : public PatternAst {
public:
    MatchStarAst(Ast* parent): PatternAst(parent, Ast::MatchStarAstType) {};
    Identifier* name = nullptr;
    QString dump() const override;
};

class KDEVPYTHONPARSER_EXPORT MatchAsAst : public PatternAst {
public:
    MatchAsAst(Ast* parent): PatternAst(parent, Ast::MatchAsAstType) {};
    PatternAst* pattern = nullptr;
    Identifier* name = nullptr;
    QString dump() const override;
};


class KDEVPYTHONPARSER_EXPORT MatchOrAst : public PatternAst {
public:
    MatchOrAst(Ast* parent): PatternAst(parent, Ast::MatchOrAstType) {};
    QList<PatternAst*> patterns;
    QString dump() const override;
};

} // end namespace Python

#endif
