/*
    SPDX-FileCopyrightText: 2022 Jairus Martin <jrm@codelv.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once
#include <QtCore/QObject>
#include "ast.h"

namespace KDevelop {
    class TopDUContext;
    class ReferencedTopDUContext;
}

namespace Python {
class AstBuilder;
class CodeAst;


}

namespace Enaml {

class PyEnamlTest : public QObject
{
Q_OBJECT
public:
    explicit PyEnamlTest(QObject* parent = nullptr);
    void initShell();
    Python::CodeAst::Ptr getAst(QString code, const QUrl& filename);

private:
    QSharedPointer<Python::AstBuilder> m_builder;

private Q_SLOTS:
    void testCode(QString code);
    void testStatements();
    void testStatements_data();
    void testRanges();
    void testRanges_data();
};

}

