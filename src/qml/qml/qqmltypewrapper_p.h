/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtQml module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QV8TYPEWRAPPER_P_H
#define QV8TYPEWRAPPER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qglobal.h>

#include <private/qqmlguard_p.h>
#include <private/qv4value_p.h>
#include <private/qv4object_p.h>

QT_BEGIN_NAMESPACE

class QV8Engine;
class QQmlTypeNameCache;

namespace QV4 {

struct Q_QML_EXPORT QmlTypeWrapper : Object
{
    Q_MANAGED
private:
    QmlTypeWrapper(QV8Engine *engine);
    ~QmlTypeWrapper();

public:
    enum TypeNameMode { IncludeEnums, ExcludeEnums };

    QVariant toVariant() const;

    static QV4::Value create(QV8Engine *, QObject *, QQmlType *, TypeNameMode = IncludeEnums);
    static QV4::Value create(QV8Engine *, QObject *, QQmlTypeNameCache *, const void *, TypeNameMode = IncludeEnums);


    static Value get(Managed *m, ExecutionContext *ctx, String *name, bool *hasProperty);
    static void put(Managed *m, ExecutionContext *ctx, String *name, const Value &value);
    static void destroy(Managed *that);

private:
    QV8Engine *v8;
    TypeNameMode mode;
    QQmlGuard<QObject> object;

    QQmlType *type;
    QQmlTypeNameCache *typeNamespace;
    const void *importNamespace;
};

}

QT_END_NAMESPACE

#endif // QV8TYPEWRAPPER_P_H

