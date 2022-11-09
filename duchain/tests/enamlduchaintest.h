/*
    SPDX-FileCopyrightText: 2010 Miquel Canes Gonzalez <miquelcanes@gmail.com>
    SPDX-FileCopyrightText: 2011-2012 Sven Brauch <svenbrauch@googlemail.com>
    SPDX-FileCopyrightText: 2022 Jairus Martin <jrm@codelv.com>

    SPDX-License-Identifier: MIT
*/
#pragma once

#include <QObject>
#include <QTemporaryDir>
#include "ast.h"
#include "enaml_ast.h"
#include <serialization/indexedstring.h>
#include <language/duchain/topducontext.h>
#include <tests/testfile.h>

namespace KDevelop {
class TopDUContext;
class ReferencedTopDUContext;
}

using namespace KDevelop;

class EnamlDUChainTest : public QObject
{
    Q_OBJECT
    public:
        explicit EnamlDUChainTest(QObject* parent = nullptr);
        void initShell();
        ~EnamlDUChainTest() override;

        KDevelop::ReferencedTopDUContext parse(const QString& code);

        Python::CodeAst::Ptr m_ast;

    private slots:
        void init();
        void testTypes();
        void testTypes_data();
        void testProblemCount();
        void testProblemCount_data();
    private:
        QList<KDevelop::TestFile*> createdFiles;
        QTemporaryDir testDirOwner;
        QDir testDir;
        QDir assetsDir;
        QString lastTest;

        QList<QString> FindPyFiles(QDir& root);
};
