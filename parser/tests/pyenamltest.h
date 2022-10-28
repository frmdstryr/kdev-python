/*
    SPDX-FileCopyrightText: 2022 Jairus Martin <frmdstryr@protonmail.com>
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

class PyEnamlTest : public QObject
{
Q_OBJECT
public:
    explicit PyEnamlTest(QObject* parent = nullptr);
    void initShell();
    CodeAst::Ptr getAst(QString code, const QUrl& filename);

private:
    QSharedPointer<AstBuilder> m_builder;

private slots:
    void testCode(QString code);
    void testStatements();
    void testStatements_data();
    //void testEnamlRanges();
    //void testEnamlRanges_data();
};

}

