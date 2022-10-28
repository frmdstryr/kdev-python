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

#include "pyenamltest.h"
#include "../astbuilder.h"
#include "../parserdebug.h"

#include <ktexteditor_version.h>

#include <QDebug>
#include <QtTest>

using namespace Python;

QTEST_MAIN(PyEnamlTest)

PyEnamlTest::PyEnamlTest(QObject* parent): QObject(parent)
{
    initShell();
}

void PyEnamlTest::initShell()
{
    AutoTestShell::init();
    TestCore* core = new TestCore();
    core->initialize(KDevelop::Core::NoUi);
    DUChain::self()->disablePersistentStorage();
    KDevelop::CodeRepresentation::setDiskChangesForbidden(true);
}

CodeAst::Ptr PyEnamlTest::getAst(QString code, const QUrl& filename)
{
    QSharedPointer<AstBuilder> builder(new AstBuilder);
    m_builder = builder;
    CodeAst::Ptr result = builder->parse(filename, code);
    qDebug() << result->dump();
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


void PyEnamlTest::testCode(QString code)
{
    CodeAst::Ptr ast = getAst(code, QUrl("test.enaml"));
    VerifyVisitor v;
    v.visitCode(ast.data());
}


void PyEnamlTest::testStatements()
{
    QFETCH(QString, code);
    testCode(code);
}

void PyEnamlTest::testStatements_data()
{
    QTest::addColumn<QString>("code");
    QTest::newRow("assign_int") << "a = 3";
    QTest::newRow("enamldef") << "enamldef A(B):\n pass";
    QTest::newRow("enamldef_attr") << "enamldef A(B):\n attr x";
    QTest::newRow("enamldef_attr_default") << "enamldef A(B):\n attr x = 1";
    QTest::newRow("enamldef_attr_type_default") << "enamldef A(B):\n attr x: int = 1";
    QTest::newRow("enamldef_binding_eq") << "enamldef A(B):\n x = 1";
    QTest::newRow("enamldef_binding_left") << "enamldef A(B):\n x << y";
    QTest::newRow("enamldef_binding_both") << "enamldef A(B):\n x := y";
    QTest::newRow("enamldef_func") << "enamldef A(B):\n func x():\n  pass";
    QTest::newRow("enamldef_func2") << "enamldef A(B):\n func x():\n  return 1";
    QTest::newRow("enamldef_func3") << "enamldef A(B):\n func x(a):\n  pass";
    QTest::newRow("enamldef_func4") << "enamldef A(B):\n func x(a: int, *b):\n  pass";
    QTest::newRow("enamldef_func5") << "enamldef A(B):\n func x(a, *b, **kwargs):\n  pass";
    QTest::newRow("enamldef_async_func") << "enamldef A(B):\n async func x(a):\n  pass";
}

// void PyEnamlTest::testEnamlRanges() {
//     QFETCH(QString, code);
//     QFETCH(KTextEditor::Range, range);
//
//     CodeAst::Ptr ast = getAst(code, QUrl("test.enaml"));
//     QVERIFY(ast);
//     foreach ( Ast* node, ast->body ) {
//         if ( node->astType != Ast::FunctionDefinitionAstType ) {
//             continue;
//         }
//         FunctionDefinitionAst* func = static_cast<FunctionDefinitionAst*>(node);
//         QVERIFY(func->name);
//         qCDebug(KDEV_PYTHON_PARSER) << func->name->range() << range;
//         QCOMPARE(func->name->range(), range);
//     }
// }
//
// void PyEnamlTest::testEnamlRanges_data()
// {
//     QTest::addColumn<QString>("code");
//     QTest::addColumn<KTextEditor::Range>("range");
//
//     QTest::newRow("cdef") << "cdef foobar(arg): pass" << KTextEditor::Range(0, 5, 0, 10);
//     QTest::newRow("cdef_return") << "cdef float* foobar(arg): pass" << KTextEditor::Range(0, 12, 0, 17);
//     QTest::newRow("normal_def") << "def foobar(arg): pass" << KTextEditor::Range(0, 4, 0, 9);
// }
//


