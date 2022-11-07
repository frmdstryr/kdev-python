/*
    SPDX-FileCopyrightText: 2014 Gregor Vollmer <gregor@celement.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <language/duchain/topducontext.h>
#include <language/codegen/coderepresentation.h>
#include <tests/autotestshell.h>
#include <tests/testcore.h>
#include <language/duchain/duchain.h>
#include <language/duchain/types/functiontype.h>

#include "pythoneditorintegrator.h"
#include "astdefaultvisitor.h"
#include "astbuilder.h"

#include "pycythontest.h"
#include "../astbuilder.h"
#include "../parserdebug.h"

#include <ktexteditor_version.h>

#include <QDebug>
#include <QtTest>

using namespace Cython;
using Python::CodeAst;
using Python::Ast;
using Python::NameAst;
using Python::AstDefaultVisitor;

QTEST_MAIN(PyCythonTest)

PyCythonTest::PyCythonTest(QObject* parent): QObject(parent)
{
    initShell();
}

void PyCythonTest::initShell()
{
    AutoTestShell::init();
    TestCore* core = new TestCore();
    core->initialize(KDevelop::Core::NoUi);
    DUChain::self()->disablePersistentStorage();
    KDevelop::CodeRepresentation::setDiskChangesForbidden(true);
}

CodeAst::Ptr PyCythonTest::getAst(QString code, const QUrl& filename)
{
    QSharedPointer<Python::AstBuilder> builder(new Python::AstBuilder);
    m_builder = builder;
    // qDebug() << "parsing: " << code;
    CodeAst::Ptr result = builder->parse(filename, code);
    // qDebug() << "ast: " << result->dump();
    return result;
}

class VerifyVisitor : public AstDefaultVisitor {
public:
    VerifyVisitor() : AstDefaultVisitor(), m_nodecount(0) { };
    void visitNode(Ast* node) override {
        m_nodecount += 1;
        QVERIFY(! node || node->astType < Ast::LastAstType);
        AstDefaultVisitor::visitNode(node);
    };
    void visitName(NameAst* node) override {
        QVERIFY(! node->identifier->value.isNull());
        AstDefaultVisitor::visitName(node);
    };
    void visitCode(CodeAst* node) override {
        AstDefaultVisitor::visitCode(node);
        qDebug() << "done, nodes visited:" << m_nodecount;
    };
    int m_nodecount;
};


void PyCythonTest::testCode(QString code)
{
    CodeAst::Ptr ast = getAst(code, QUrl("test.pyx"));
    VerifyVisitor v;
    v.visitCode(ast.data());
}


void PyCythonTest::testStatements()
{
    QFETCH(QString, code);
    testCode(code);
}

void PyCythonTest::testStatements_data()
{
    QTest::addColumn<QString>("code");
    QTest::newRow("assign_int") << "a = 3";
    QTest::newRow("assign_str") << "x = 'abc'";
}


class RangeVerifyVisitor : public AstDefaultVisitor {
public:
    RangeVerifyVisitor() : AstDefaultVisitor() { };
    void visitName(NameAst* node) override {
        QVERIFY(! node->identifier->value.isNull());
        if (node->identifier->value == searchingForIdentifier) {
            // HACK: Ignore implicit self = Class() calls
            bool ignore = false;
            if (auto call = dynamic_cast<Python::CallAst*>(node->parent))
            {
                if (auto assignment = dynamic_cast<Python::AssignmentAst*>(call->parent))
                {
                    if (auto target = dynamic_cast<Python::NameAst*>(assignment->targets.at(0)))
                        ignore = target->identifier->value == "self";
                }
            }
            if (!ignore) {
                found = true;
                QCOMPARE( node->identifier->range(),  searchingForRange);
            }
        }
        AstDefaultVisitor::visitName(node);
    };

    void visitClassDefinition(Python::ClassDefinitionAst* node) override {
        QVERIFY(! node->name->value.isNull());
        if (node->name->value == searchingForIdentifier) {
            QVERIFY(!found);
            found = true;
            QCOMPARE( node->name->range(),  searchingForRange);
        }
        AstDefaultVisitor::visitClassDefinition(node);
    };

    void visitFunctionDefinition(Python::FunctionDefinitionAst* node) override {
        QVERIFY(! node->name->value.isNull());
        if (node->name->value == searchingForIdentifier) {
            QVERIFY(!found);
            found = true;
            QCOMPARE( node->name->range(),  searchingForRange);
        }
        AstDefaultVisitor::visitFunctionDefinition(node);
    };

    bool found = false;
    QString searchingForIdentifier;
    KTextEditor::Range searchingForRange;
};


void PyCythonTest::testRanges() {
    QFETCH(QString, code);
    QFETCH(QString, identifier);
    QFETCH(KTextEditor::Range, range);

    CodeAst::Ptr ast = getAst(code, QUrl("test.enaml"));
    QVERIFY(ast);

    RangeVerifyVisitor visitor;
    visitor.searchingForRange = range;
    visitor.searchingForIdentifier = identifier;
    visitor.visitCode(ast.data());
    if (!visitor.found) {
        qDebug() << "code: " << code;
        qDebug() << "ast: " << ast->dump();
    }
    QVERIFY(visitor.found);
}

void PyCythonTest::testRanges_data()
{
    QTest::addColumn<QString>("code");
    QTest::addColumn<QString>("identifier");
    QTest::addColumn<KTextEditor::Range>("range");

    // NOTE: endCol is inclusive (aka len - 1)
    QTest::newRow("assign_int") << "bar = 1" << "bar" << KTextEditor::Range(0, 0, 0, 2);
}



