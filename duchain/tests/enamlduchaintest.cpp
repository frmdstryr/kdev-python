/*
    SPDX-FileCopyrightText: 2010 Miquel Canes Gonzalez <miquelcanes@gmail.com>
    SPDX-FileCopyrightText: 2012 Sven Brauch <svenbrauch@googlemail.com>

    SPDX-License-Identifier: MIT
*/

#include <QDebug>
#include "duchaindebug.h"

#include "enamlduchaintest.h"

#include <stdlib.h>

#include <language/duchain/topducontext.h>
#include <language/codegen/coderepresentation.h>
#include <tests/autotestshell.h>
#include <tests/testcore.h>
#include <language/duchain/duchain.h>
#include <QtTest>
#include <QApplication>
#include <language/duchain/types/functiontype.h>
#include <language/duchain/types/containertypes.h>
#include <language/duchain/aliasdeclaration.h>
#include <language/backgroundparser/backgroundparser.h>
#include <language/interfaces/iastcontainer.h>
#include <interfaces/ilanguagecontroller.h>
#include <tests/testfile.h>

#include <KTextEditor/Range>

#include "parsesession.h"
#include "pythoneditorintegrator.h"
#include "declarationbuilder.h"
#include "usebuilder.h"
#include "astdefaultvisitor.h"
#include "expressionvisitor.h"
#include "contextbuilder.h"
#include "astbuilder.h"

#include "duchain/helpers.h"

#include "kdevpythonversion.h"
#include <dumpchain.h>

QTEST_MAIN(EnamlDUChainTest)

using namespace KDevelop;
using namespace Enaml;


EnamlDUChainTest::EnamlDUChainTest(QObject* parent): QObject(parent)
{
    assetsDir = QDir(DUCHAIN_PY_DATA_DIR);
    if (!assetsDir.cd("data")) {
        qFatal("Failed find data directory for test files. Aborting");
    }

    testDir = QDir(testDirOwner.path());

    //qputenv("PYTHONPATH", assetsDir.absolutePath().toUtf8());

    initShell();
}

QList<QString> EnamlDUChainTest::FindPyFiles(QDir& rootDir)
{
    QList<QString> foundfiles;
    rootDir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
    rootDir.setNameFilters(QStringList()<< "*.py" << "*.enaml"); // We only want .enaml files

    QDirIterator it(rootDir, QDirIterator::Subdirectories);
    while(it.hasNext()) {
        foundfiles.append(it.next());
    }
    return foundfiles;
}

void EnamlDUChainTest::init()
{
    QString currentTest = QString(QTest::currentTestFunction());
    if (lastTest == currentTest) {
        qCDebug(KDEV_PYTHON_DUCHAIN) << "Already prepared assets for " << currentTest << ", skipping";
        return;
    } else {
        lastTest = currentTest;
    }
    qCDebug(KDEV_PYTHON_DUCHAIN) << "Preparing assets for test " << currentTest;

    QDir assetModuleDir = QDir(assetsDir.absolutePath());

    if (!assetModuleDir.cd(currentTest)) {
        qCDebug(KDEV_PYTHON_DUCHAIN) << "Asset directory " << currentTest
                 <<  " does not exist under " << assetModuleDir.absolutePath() << ". Skipping it.";
        return;
    }

    qCDebug(KDEV_PYTHON_DUCHAIN) << "Searching for enaml files in " << assetModuleDir.absolutePath();

    QList<QString> foundfiles = FindPyFiles(assetModuleDir);

    QString correctionFileDir = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "kdevpythonsupport/correction_files", QStandardPaths::LocateDirectory);
    auto correctionFileUrl = QUrl(QDir::cleanPath(correctionFileDir + "/testCorrectionFiles/example.py"));
    foundfiles.prepend(correctionFileUrl.path());

    for ( int i = 0; i < 2; i++ ) {
        // Parse each file twice, to ensure no parsing-order related bugs appear.
        // Such bugs will need separate unit tests and should not influence these.
        foreach(const QString filename, foundfiles) {
            qCDebug(KDEV_PYTHON_DUCHAIN) << "Parsing asset: " << filename;
            DUChain::self()->updateContextForUrl(IndexedString(filename), KDevelop::TopDUContext::AllDeclarationsContextsAndUses);
            ICore::self()->languageController()->backgroundParser()->parseDocuments();
        }
        foreach(const QString filename, foundfiles) {
            DUChain::self()->waitForUpdate(IndexedString(filename), KDevelop::TopDUContext::AllDeclarationsContextsAndUses);
        }
        while ( ICore::self()->languageController()->backgroundParser()->queuedCount() > 0 ) {
            // make sure to wait for all parsejobs to finish
            QTest::qWait(10);
        }
    }
}


void EnamlDUChainTest::initShell()
{
    AutoTestShell::init();
    TestCore* core = new TestCore();
    core->initialize(KDevelop::Core::NoUi);

    auto doc_url = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                          "kdevpythonsupport/documentation_files/builtindocumentation.py");

    qCDebug(KDEV_PYTHON_DUCHAIN) << doc_url;


    DUChain::self()->updateContextForUrl(IndexedString(doc_url), KDevelop::TopDUContext::AllDeclarationsContextsAndUses);
    ICore::self()->languageController()->backgroundParser()->parseDocuments();
    DUChain::self()->waitForUpdate(IndexedString(doc_url), KDevelop::TopDUContext::AllDeclarationsContextsAndUses);

    DUChain::self()->disablePersistentStorage();
    KDevelop::CodeRepresentation::setDiskChangesForbidden(true);
}

ReferencedTopDUContext EnamlDUChainTest::parse(const QString& code)
{
    TestFile* testfile = new TestFile("#!/usr/bin/python\n" + code + "\n", "enaml", nullptr, testDir.absolutePath().append("/"));
    createdFiles << testfile;

    // qDebug() << "url: " << testfile->url();
    // qDebug() << "mime:" << QMimeDatabase().mimeTypeForFile(testfile->url().toUrl().toLocalFile());

    testfile->parse(TopDUContext::ForceUpdate | TopDUContext::AST);
    testfile->waitForParsed(2000);

    if ( testfile->isReady() ) {
        Q_ASSERT(testfile->topContext());
        m_ast = static_cast<Python::ParseSession*>(testfile->topContext()->ast().data())->ast;
        return testfile->topContext();
    }
    else Q_ASSERT(false && "Timed out waiting for parser results, aborting all tests");
    return nullptr;
}

EnamlDUChainTest::~EnamlDUChainTest()
{
    foreach ( TestFile* f, createdFiles ) {
        delete f;
    }
    testDir.rmdir(testDir.absolutePath());
}

class TypeTestVisitor : public Python::AstDefaultVisitor {
public:
    QString searchingForType;
    QString rootNode;
    TopDUContextPointer ctx;
    bool found;
    void visitAttribute(Python::AttributeAst* node) override {
        checkType(node, node->attribute);
        AstDefaultVisitor::visitAttribute(node);
    }

    void visitName(Python::NameAst* node) override {
        checkType(node, node->identifier);
        AstDefaultVisitor::visitName(node);
    };

    void checkType(Python::Ast *node, Python::Identifier *identifier) {
        if ( identifier->value != "checkme" ) return;
        auto roots = ctx->findDeclarations(QualifiedIdentifier(rootNode));
        QVERIFY(roots.size() == 1);
        DUContext* classContext = roots.at(0)->internalContext();
        QVERIFY(classContext);
        QVERIFY(classContext->type() == DUContext::Class);
        Declaration* d = Python::Helper::declarationForName(identifier->value, CursorInRevision::invalid(), DUChainPointer<const DUContext>(classContext));
        if ( ! d ) {
            qCDebug(KDEV_PYTHON_DUCHAIN) << "No declaration found for" << identifier->value << "in" << node->dump();
            return;
        }
        QVERIFY(d->abstractType());
        qCDebug(KDEV_PYTHON_DUCHAIN) << "found: " << identifier->value << "is" << d->abstractType()->toString() << "should be" << searchingForType;
        if ( d->abstractType()->toString().replace("__kdevpythondocumentation_builtin_", "").startsWith(searchingForType) ) {
            found = true;
            return;
        }
    }

};

void EnamlDUChainTest::testTypes()
{

    QFETCH(QString, code);
    QFETCH(QString, expectedType);
    QFETCH(QString, rootNode);

    ReferencedTopDUContext ctx = parse(code.toUtf8());
    QVERIFY(ctx);
    QVERIFY(m_ast);

    DUChainReadLocker lock(DUChain::lock());
    TypeTestVisitor* visitor = new TypeTestVisitor();
    visitor->ctx = TopDUContextPointer(ctx.data());
    visitor->searchingForType = expectedType;
    visitor->rootNode = rootNode;
    visitor->visitCode(m_ast.data());
    if (!visitor->found)
    {
        qDebug() << m_ast->dump();
        Python::DumpChain().dump(ctx.data());
    }
    QCOMPARE(visitor->found, true);
}

void EnamlDUChainTest::testTypes_data()
{
    QTest::addColumn<QString>("code");
    QTest::addColumn<QString>("expectedType");
    QTest::addColumn<QString>("rootNode");

    QTest::newRow("enamldef") << "enamldef Main(Window): checkme:\n pass" << "Main" << "Main";
    QTest::newRow("self") << "enamldef Main(Window):\n attr checkme = self" << "Main" << "Main";
    QTest::newRow("attr") << "enamldef Main(Window):\n attr checkme = 'abc'" << "str" << "Main";
    QTest::newRow("attr_type") << "enamldef Main(Window):\n attr checkme: int" << "int" << "Main";
    QTest::newRow("attr_opt_type") <<
        "class Menu:\n"
        " pass\n"
        "enamldef Main(Window):\n"
        " attr checkme: Menu = None" << "unsure (None, Menu)" << "Main";
    QTest::newRow("binding") <<
        "class Str:\n"
        " pass\n"
        "class Base:\n"
        " checkme = Str()\n"
        "enamldef Main(Base):\n"
        " checkme = foo.bar" << "Str" << "Main";
   QTest::newRow("binding_attr") <<
        "enamldef Base(Window):\n"
        " attr checkme = 1\n"
        "enamldef Main(Base):\n"
        " checkme = foo.bar\n" << "int" << "Main";
    QTest::newRow("value_in_expr") <<
        "enamldef Main(Base):\n"
        " attr foo = 1\n"
        " attr checkme = [foo]\n" << "list of int" << "Main";
    // QTest::newRow("value_in_func") <<
    //     "enamldef Main(Base):\n"
    //     " attr foo = 1\n"
    //     " func bar():\n"
    //     "    checkme = foo\n" << "int" << "Main";

    // QTest::newRow("value_out_of_order") <<
    //     "enamldef Main(Base):\n"
    //     " attr checkme = [foo]\n"
    //     " attr foo = 1\n" << "list of int" << "Main";
}

void EnamlDUChainTest::testProblemCount()
{
    QFETCH(QString, code);
    QFETCH(int, problemsCount);

    ReferencedTopDUContext ctx = parse(code);
    QVERIFY(ctx);

    DUChainReadLocker lock;
    const int n = ctx->problems().size();
    if (n != problemsCount)
    {
        qDebug() << m_ast->dump();
        Python::DumpChain().dump(ctx.data());
    }
    QCOMPARE(n, problemsCount);
}

void EnamlDUChainTest::testProblemCount_data()
{
    QTest::addColumn<QString>("code");
    QTest::addColumn<int>("problemsCount");

    QTest::newRow("correct_binding") <<
        "class Base:\n"
        " foo = 0\n"
        "enamldef Main(Base):\n"
        " foo = 1\n" << 0;
    QTest::newRow("missing_binding") <<
        "class Base:\n"
        " pass\n"
        "enamldef Main(Base):\n"
        " foo = 1\n" << 1;

}

typedef QPair<Declaration*, int> pair;

