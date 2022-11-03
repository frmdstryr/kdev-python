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

using namespace Enaml;
using Python::CodeAst;
using Python::Ast;
using Python::NameAst;
using Python::AstDefaultVisitor;

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
    QTest::newRow("attr") << "enamldef A(B):\n attr x";
    QTest::newRow("alias") << "enamldef A(B):\n alias x: y";
    QTest::newRow("alias_chain") << "enamldef A(B):\n alias x: y.z";
    QTest::newRow("alias_chain_triple") << "enamldef A(B):\n alias x: a.b.c";
    QTest::newRow("two_attr") << "enamldef A(B):\n attr x\n attr y";
    QTest::newRow("attr_default") << "enamldef A(B):\n attr x = 1";
    QTest::newRow("attr_type_default") << "enamldef A(B):\n attr x: int = 1";
    QTest::newRow("binding_eq") << "enamldef A(B):\n x = 1";
    QTest::newRow("binding_read") << "enamldef A(B):\n x << y";
    QTest::newRow("binding_both") << "enamldef A(B):\n x := y";
    QTest::newRow("two_bindings") << "enamldef A(B):\n x := y\n a = 2";
    QTest::newRow("binding_change_block") << "enamldef A(B):\n x ::\n  x+=1\n  print(x)";
    QTest::newRow("binding_read_block") << "enamldef A(B):\n x <<\n  return 1";
    QTest::newRow("ex_binding") << "enamldef A(B):\n x.y = z";
    QTest::newRow("func") << "enamldef A(B):\n func x():\n  pass";
    QTest::newRow("two_funcs") << "enamldef A(B):\n func x():\n  pass\n func y():\n  pass";
    QTest::newRow("func2") << "enamldef A(B):\n func x():\n  return 1";
    QTest::newRow("func_override") << "enamldef A(B):\n func x():\n  return 1\nenamldef C(A):\n x=>():\n  return 2";
    QTest::newRow("async_func") << "enamldef A(B):\n async func x(a):\n  pass";
    QTest::newRow("childdef") << "enamldef A(B):\n C:\n  pass";
    QTest::newRow("enamldef_then_py_mod") << "enamldef A(B):\n pass\ndef foo():\n pass";
    QTest::newRow("py_mod_enamldef_py_mod") << "from a import B\nenamldef A(B):\n pass\nclass C:\n pass";
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


void PyEnamlTest::testRanges() {
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

void PyEnamlTest::testRanges_data()
{
    QTest::addColumn<QString>("code");
    QTest::addColumn<QString>("identifier");
    QTest::addColumn<KTextEditor::Range>("range");

    // NOTE: endCol is inclusive (aka len - 1)
    QTest::newRow("enamldef") << "enamldef Main(Window):\n pass" << "Main" << KTextEditor::Range(0, 9, 0, 12);
    QTest::newRow("enamldef_base") << "enamldef Main(Window):\n pass" << "Window" << KTextEditor::Range(0, 14, 0, 19);
    QTest::newRow("enamldef_with_ident") << "enamldef Main(Window): main:\n pass" << "Main" << KTextEditor::Range(0, 9, 0, 12);

    QTest::newRow("func") << "enamldef Main(Window):\n func submit():\n  pass" << "submit" << KTextEditor::Range(1, 6, 1, 11);
    QTest::newRow("async_func") << "enamldef Main(Window):\n async func submit():\n  pass" << "submit" << KTextEditor::Range(1, 12, 1, 17);
    QTest::newRow("func_override") << "enamldef A(B):\n foo=>():\n  pass" << "foo" << KTextEditor::Range(1, 1, 1, 3);
    QTest::newRow("async_func_override") << "enamldef A(B):\n  async foo => ():\n   pass" << "foo" << KTextEditor::Range(1, 8, 1, 10);

    QTest::newRow("attr") << "enamldef Main(Window):\n attr x: int = 1" << "x" << KTextEditor::Range(1, 6, 1, 6);
    QTest::newRow("alias") << "enamldef Main(Window):\n alias x" << "x" << KTextEditor::Range(1, 7, 1, 7);
    QTest::newRow("alias_with_target") << "enamldef Main(Window):\n alias x: y" << "x" << KTextEditor::Range(1, 7, 1, 7);
    QTest::newRow("alias_target") << "enamldef Main(Window):\n alias x: y" << "y" << KTextEditor::Range(1, 10, 1, 10);
    QTest::newRow("alias_target2") << "enamldef Main(Window):\n alias x:  y" << "y" << KTextEditor::Range(1, 11, 1, 11);

    QTest::newRow("binding") << "enamldef Main(Window):\n x = 1" << "x" << KTextEditor::Range(1, 1, 1, 1);
    QTest::newRow("binding_value") << "enamldef Main(Window):\n x = y" << "y" << KTextEditor::Range(1, 5, 1, 5);
    QTest::newRow("ex_binding_value") << "enamldef Main(Window):\n x.y = z" << "z" << KTextEditor::Range(1, 7, 1, 7);

    QTest::newRow("binding_implicit_block_func") << "enamldef Main(Window):\n x ::\n  print(change)" << "x_expr_2" << KTextEditor::Range(1, 1, 1, 1);
    QTest::newRow("childdef") << "enamldef Main(Window):\n Label:\n  pass" << "Label_2" << KTextEditor::Range(1, 1, 1, 5);

    QTest::newRow("childdef_func") << "enamldef Main(Window):\n Label:\n  func foo():\n    pass" << "foo" << KTextEditor::Range(2, 7, 2, 9);
    QTest::newRow("childdef_indented") << "enamldef Main(Window):\n    Label:\n        pass" << "Label_2" << KTextEditor::Range(1, 4, 1, 8);
    QTest::newRow("nested_childdef") << "enamldef Main(Window):\n Container:\n  Label:\n   pass" << "Label_3" << KTextEditor::Range(2, 2, 2, 6);

    QTest::newRow("enamldef_then_mod") << "enamldef Main(Window):\n pass\ndef foo():\n pass" << "foo" << KTextEditor::Range(2, 4, 2, 6);
    QTest::newRow("mod_enamldef_mod") << "from x import Window\nenamldef Main(Window):\n pass\ndef foo():\n pass" << "foo" << KTextEditor::Range(3, 4, 3, 6);

    // TODO: How should idents be defined??
    // QTest::newRow("childdef_ident") << "enamldef Main(Window):\n Label: lbl:\n  pass" << "lbl" << KTextEditor::Range(1, 8, 1, 10);
    // QTest::newRow("enamldef_ident") << "enamldef Main(Window): main:\n pass" << "main" << KTextEditor::Range(0, 23, 0, 26);

    // TODO: Update test to support Attributes
    // QTest::newRow("alias_chain") << "enamldef Main(Window):\n alias x: y.z" << "z" << KTextEditor::Range(1, 12, 1, 12);
    // QTest::newRow("ex_binding") << "enamldef Main(Window):\n x.y = z" << "y" << KTextEditor::Range(1, 3, 1, 3);
}



