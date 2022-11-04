/*
    SPDX-FileCopyrightText: 2022 Jairus Martin <jrm@codelv.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <language/duchain/classdeclaration.h>
#include <serialization/indexedstring.h>

#include "pythonduchainexport.h"

namespace Python {

class KDEVPYTHONDUCHAIN_EXPORT ClassDeclarationData : public KDevelop::ClassDeclarationData
{
public:
    ClassDeclarationData()
        : KDevelop::ClassDeclarationData()
        , m_isDynamicallyScoped(false)
        , m_isMetaClass(false){}

    ClassDeclarationData(const ClassDeclarationData& rhs)
        : KDevelop::ClassDeclarationData(rhs)
            , m_isDynamicallyScoped(rhs.m_isDynamicallyScoped)
            , m_isMetaClass(rhs.m_isMetaClass) {}

    ~ClassDeclarationData() {}

    bool m_isDynamicallyScoped: 1;
    bool m_isMetaClass: 1;
};

class KDEVPYTHONDUCHAIN_EXPORT ClassDeclaration : public KDevelop::ClassDeclaration
{
public:
    ClassDeclaration(const ClassDeclaration &rhs);
    explicit ClassDeclaration(ClassDeclarationData &data);
    ClassDeclaration(const KDevelop::RangeInRevision &range, KDevelop::DUContext *context);
    ClassDeclaration(ClassDeclarationData &data, const KDevelop::RangeInRevision &range, KDevelop::DUContext *context);
    ~ClassDeclaration() override;

    enum {
        Identity = 127
    };

    inline bool isDynamicallyScoped() const {
        return d_func()->m_isDynamicallyScoped;
    }

    inline void setDynamicallyScoped(bool isDynamicallyScoped) {
        d_func_dynamic()->m_isDynamicallyScoped = isDynamicallyScoped;
    }

    inline bool isMetaClass() const {
        return d_func()->m_isMetaClass;
    }

    inline void setMetaClass(bool isMetaClass) {
        d_func_dynamic()->m_isMetaClass = isMetaClass;
    }

    typedef KDevelop::DUChainPointer<ClassDeclaration> Ptr;
private:
    DUCHAIN_DECLARE_DATA(ClassDeclaration);
};


typedef KDevelop::DUChainPointer<ClassDeclaration> ClassDeclarationPointer;

} // end namespace Python
