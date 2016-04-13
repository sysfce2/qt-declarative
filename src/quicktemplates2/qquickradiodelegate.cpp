/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Labs Templates module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qquickradiodelegate_p.h"
#include "qquickabstractbutton_p_p.h"

#include <QtGui/qpa/qplatformtheme.h>

QT_BEGIN_NAMESPACE

/*!
    \qmltype RadioDelegate
    \inherits ItemDelegate
    \instantiates QQuickRadioDelegate
    \inqmlmodule Qt.labs.controls
    \ingroup qtlabscontrols-delegates
    \brief An item delegate that can be checked or unchecked.

    \image qtquickcontrols2-radiodelegate.gif

    RadioDelegate presents an item delegate that can be toggled on (checked) or
    off (unchecked). Radio delegates are typically used to select one option
    from a set of options.

    The state of the radio delegate can be set with the
    \l {AbstractButton::}{checked} property.

    \code
    ButtonGroup {
        id: buttonGroup
    }

    ListView {
        model: ["Option 1", "Option 2", "Option 3"]
        delegate: RadioDelegate {
            text: modelData
            checked: index == 0
            ButtonGroup.group: buttonGroup
        }
    }
    \endcode

    \labs

    \sa {Customizing RadioDelegate}, {Delegate Controls}
*/

QQuickRadioDelegate::QQuickRadioDelegate(QQuickItem *parent) :
    QQuickItemDelegate(parent)
{
    setCheckable(true);
    setAutoExclusive(true);
}

#ifndef QT_NO_ACCESSIBILITY
QAccessible::Role QQuickRadioDelegate::accessibleRole() const
{
    return QAccessible::RadioButton;
}
#endif

QT_END_NAMESPACE
