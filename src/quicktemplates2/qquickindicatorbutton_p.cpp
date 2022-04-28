/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Quick Templates 2 module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/
#include "qquickindicatorbutton_p.h"
#include "qquickdeferredexecute_p_p.h"
#include "qquickcontrol_p_p.h"

QT_BEGIN_NAMESPACE

class QQuickIndicatorButton;

static inline QString indicatorName() { return QStringLiteral("indicator"); }

void QQuickIndicatorButtonPrivate::cancelIndicator()
{
    Q_Q(QQuickIndicatorButton);
    quickCancelDeferred(q, indicatorName());
}

void QQuickIndicatorButtonPrivate::executeIndicator(bool complete)
{
    Q_Q(QQuickIndicatorButton);
    if (indicator.wasExecuted())
        return;

    if (!indicator || complete)
        quickBeginDeferred(q, indicatorName(), indicator);
    if (complete)
        quickCompleteDeferred(q, indicatorName(), indicator);
}

QQuickIndicatorButton::QQuickIndicatorButton(QObject *parent)
    : QObject(*(new QQuickIndicatorButtonPrivate), parent)
{
}

bool QQuickIndicatorButton::isPressed() const
{
    Q_D(const QQuickIndicatorButton);
    return d->pressed;
}

void QQuickIndicatorButton::setPressed(bool pressed)
{
    Q_D(QQuickIndicatorButton);
    if (d->pressed == pressed)
        return;

    d->pressed = pressed;
    emit pressedChanged();
}

QQuickItem *QQuickIndicatorButton::indicator() const
{
    QQuickIndicatorButtonPrivate *d = const_cast<QQuickIndicatorButtonPrivate *>(d_func());
    if (!d->indicator)
        d->executeIndicator();
    return d->indicator;
}

void QQuickIndicatorButton::setIndicator(QQuickItem *indicator)
{
    Q_D(QQuickIndicatorButton);
    if (d->indicator == indicator)
        return;

    if (!d->indicator.isExecuting())
        d->cancelIndicator();

    const qreal oldImplicitIndicatorWidth = implicitIndicatorWidth();
    const qreal oldImplicitIndicatorHeight = implicitIndicatorHeight();

    QQuickControl *par = static_cast<QQuickControl *>(parent());

    QQuickControlPrivate::get(par)->removeImplicitSizeListener(d->indicator);
    QQuickControlPrivate::hideOldItem(d->indicator);
    d->indicator = indicator;

    if (indicator) {
        if (!indicator->parentItem())
            indicator->setParentItem(par);
        QQuickControlPrivate::get(par)->addImplicitSizeListener(indicator);
    }

    if (!qFuzzyCompare(oldImplicitIndicatorWidth, implicitIndicatorWidth()))
        emit implicitIndicatorWidthChanged();
    if (!qFuzzyCompare(oldImplicitIndicatorHeight, implicitIndicatorHeight()))
        emit implicitIndicatorHeightChanged();
    if (!d->indicator.isExecuting())
        emit indicatorChanged();
}

bool QQuickIndicatorButton::isHovered() const
{
    Q_D(const QQuickIndicatorButton);
    return d->hovered;
}

void QQuickIndicatorButton::setHovered(bool hovered)
{
    Q_D(QQuickIndicatorButton);
    if (d->hovered == hovered)
        return;

    d->hovered = hovered;
    emit hoveredChanged();
}

qreal QQuickIndicatorButton::implicitIndicatorWidth() const
{
    Q_D(const QQuickIndicatorButton);
    if (!d->indicator)
        return 0;
    return d->indicator->implicitWidth();
}

qreal QQuickIndicatorButton::implicitIndicatorHeight() const
{
    Q_D(const QQuickIndicatorButton);
    if (!d->indicator)
        return 0;
    return d->indicator->implicitHeight();
}

QT_END_NAMESPACE

#include "moc_qquickindicatorbutton_p.cpp"
