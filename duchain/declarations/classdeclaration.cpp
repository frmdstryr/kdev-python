/*
    SPDX-FileCopyrightText: 2011 Sven Brauch <svenbrauch@googlemail.com>
    SPDX-FileCopyrightText: 2022 Jairus Martin <jrm@codelv.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "classdeclaration.h"
#include <language/duchain/duchainregister.h>

namespace Python {

REGISTER_DUCHAIN_ITEM(ClassDeclaration);

ClassDeclaration::ClassDeclaration(const ClassDeclaration& rhs)
        : KDevelop::ClassDeclaration(*new ClassDeclarationData(*rhs.d_func()))
{
}

ClassDeclaration::ClassDeclaration(const KDevelop::RangeInRevision& range, KDevelop::DUContext* context)
        : KDevelop::ClassDeclaration(*new ClassDeclarationData, range, context)
{
    d_func_dynamic()->setClassId(this);
    if (context) {
        setContext(context);
    }
}

ClassDeclaration::ClassDeclaration(ClassDeclarationData& data)
        : KDevelop::ClassDeclaration(data)
{
}

ClassDeclaration::ClassDeclaration(ClassDeclarationData& data, const KDevelop::RangeInRevision& range, KDevelop::DUContext* context)
        : KDevelop::ClassDeclaration(data, range, context)
{
    if (context) {
        setContext(context);
    }
}

ClassDeclaration::~ClassDeclaration()
{
}

}
