/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Labs Controls module of the Qt Toolkit.
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

#include <QtQuickControls/private/qquickstyleplugin_p.h>

#include "qquickuniversalfocusrectangle_p.h"
#include "qquickuniversalprogressring_p.h"
#include "qquickuniversalprogressstrip_p.h"
#include "qquickuniversalstyle_p.h"
#include "qquickuniversaltheme_p.h"

#include <QtQuickControls/private/qquickcolorimageprovider_p.h>
#include <QtQuickControls/private/qquickpluginutils_p.h>

static inline void initResources()
{
    Q_INIT_RESOURCE(qtquickuniversalstyleplugin);
#ifdef QT_STATIC
    Q_INIT_RESOURCE(qmake_Qt_labs_controls_universal);
#endif
}

QT_BEGIN_NAMESPACE

class QtQuickUniversalStylePlugin: public QQuickStylePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0")

public:
    QtQuickUniversalStylePlugin(QObject *parent = nullptr);

    void registerTypes(const char *uri) override;
    void initializeEngine(QQmlEngine *engine, const char *uri) override;

    QString name() const override;
    QQuickProxyTheme *createTheme() const override;
};

QtQuickUniversalStylePlugin::QtQuickUniversalStylePlugin(QObject *parent) : QQuickStylePlugin(parent)
{
    initResources();
}

void QtQuickUniversalStylePlugin::registerTypes(const char *uri)
{
    qmlRegisterUncreatableType<QQuickUniversalStyle>(uri, 1, 0, "Universal", tr("Universal is an attached property"));
}

void QtQuickUniversalStylePlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQuickStylePlugin::initializeEngine(engine, uri);

    engine->addImageProvider(name(), new QQuickColorImageProvider(QStringLiteral(":/qt-project.org/imports/Qt/labs/controls/universal/images")));

    QByteArray import = QByteArray(uri) + ".impl";
    qmlRegisterType<QQuickUniversalFocusRectangle>(import, 1, 0, "FocusRectangle");
    qmlRegisterType<QQuickUniversalProgressRing>(import, 1, 0, "ProgressRing");
    qmlRegisterType<QQuickUniversalProgressRingAnimator>(import, 1, 0, "ProgressRingAnimator");
    qmlRegisterType<QQuickUniversalProgressStrip>(import, 1, 0, "ProgressStrip");
    qmlRegisterType<QQuickUniversalProgressStripAnimator>(import, 1, 0, "ProgressStripAnimator");

    const QString pluginBasePath = QQuickPluginUtils::pluginBasePath(*this);
    qmlRegisterType(QUrl(pluginBasePath + QStringLiteral("/RadioIndicator.qml")), import, 1, 0, "RadioIndicator");
    qmlRegisterType(QUrl(pluginBasePath + QStringLiteral("/SwitchIndicator.qml")), import, 1, 0, "SwitchIndicator");
}

QString QtQuickUniversalStylePlugin::name() const
{
    return QStringLiteral("universal");
}

QQuickProxyTheme *QtQuickUniversalStylePlugin::createTheme() const
{
    return new QQuickUniversalTheme;
}

QT_END_NAMESPACE

#include "qtquickuniversalstyleplugin.moc"
