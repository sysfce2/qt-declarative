// Copyright (C) 2016 Jolla Ltd, author: <gunnar.sletta@jollamobile.com>
// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qgfxsourceproxy_p.h"

#include <private/qquickshadereffectsource_p.h>
#include <private/qquickitem_p.h>
#include <private/qquickimage_p.h>

QT_BEGIN_NAMESPACE

QGfxSourceProxyME::QGfxSourceProxyME(QQuickItem *parentItem)
    : QQuickItem(parentItem)
{
}

QGfxSourceProxyME::~QGfxSourceProxyME()
{
    delete m_proxy;
}

void QGfxSourceProxyME::setInput(QQuickItem *input)
{
    if (m_input == input)
        return;

    if (m_input)
        disconnect(m_input, nullptr, this, nullptr);
    m_input = input;
    polish();
    if (m_input) {
        if (QQuickImage *image = qobject_cast<QQuickImage *>(m_input)) {
            connect(image, &QQuickImage::sourceSizeChanged, this, &QGfxSourceProxyME::repolish);
            connect(image, &QQuickImage::fillModeChanged, this, &QGfxSourceProxyME::repolish);
        }
        connect(m_input, &QQuickItem::childrenChanged, this, &QGfxSourceProxyME::repolish);
    }
    Q_EMIT inputChanged();
}

void QGfxSourceProxyME::setOutput(QQuickItem *output)
{
    if (m_output == output)
        return;
    m_output = output;
    Q_EMIT activeChanged();
    Q_EMIT outputChanged();
}

void QGfxSourceProxyME::setSourceRect(const QRectF &sourceRect)
{
    if (m_sourceRect == sourceRect)
        return;
    m_sourceRect = sourceRect;
    polish();
    Q_EMIT sourceRectChanged();
}

void QGfxSourceProxyME::setInterpolation(Interpolation i)
{
    if (m_interpolation == i)
        return;
    m_interpolation = i;
    polish();
    Q_EMIT interpolationChanged();
}

void QGfxSourceProxyME::useProxy()
{
    if (!m_proxy)
        m_proxy = new QQuickShaderEffectSource(this);
    m_proxy->setSourceRect(m_sourceRect);
    m_proxy->setSourceItem(m_input);
    m_proxy->setSmooth(m_interpolation != Interpolation::Nearest);
    setOutput(m_proxy);
}

void QGfxSourceProxyME::repolish()
{
    polish();
}

QObject *QGfxSourceProxyME::findLayer(QQuickItem *item)
{
    if (!item)
        return nullptr;
    QQuickItemPrivate *d = QQuickItemPrivate::get(item);
    if (d->extra.isAllocated() && d->extra->layer) {
        QObject *layer = qvariant_cast<QObject *>(item->property("layer"));
        if (layer && layer->property("enabled").toBool())
            return layer;
    }
    return nullptr;
}

void QGfxSourceProxyME::updatePolish()
{
    if (!m_input) {
        setOutput(nullptr);
        return;
    }

    QQuickImage *image = qobject_cast<QQuickImage *>(m_input);
    QQuickShaderEffectSource *shaderSource = qobject_cast<QQuickShaderEffectSource *>(m_input);
    bool childless = m_input->childItems().size() == 0;
    bool interpOk = m_interpolation == Interpolation::Any
                    || (m_interpolation == Interpolation::Linear && m_input->smooth() == true)
                    || (m_interpolation == Interpolation::Nearest && m_input->smooth() == false);

    // Layers can be used in two different ways. Option 1 is when the item is
    // used as input to a separate ShaderEffect component. In this case,
    // m_input will be the item itself.
    QObject *layer = findLayer(m_input);
    if (!layer && shaderSource) {
        // Alternatively, the effect is applied via layer.effect, and the
        // input to the effect will be the layer's internal ShaderEffectSource
        // item. In this case, we need to backtrack and find the item that has
        // the layer and configure it accordingly.
        layer = findLayer(shaderSource->sourceItem());
    }

    // A bit crude test, but we're only using source rect for
    // blurring+transparent edge, so this is good enough.
    bool padded = m_sourceRect.x() < 0 || m_sourceRect.y() < 0;

    bool direct = false;

    if (layer) {
        // Auto-configure the layer so interpolation and padding works as
        // expected without allocating additional FBOs. In edgecases, where
        // this feature is undesiered, the user can simply use
        // ShaderEffectSource rather than layer.
        layer->setProperty("sourceRect", m_sourceRect);
        layer->setProperty("smooth", m_interpolation != Interpolation::Nearest);
        direct = true;

    } else if (childless && interpOk) {
        if (shaderSource) {
            if (shaderSource->sourceRect() == m_sourceRect || m_sourceRect.isEmpty())
                direct = true;

        } else if (!padded && ((image && image->fillMode() == QQuickImage::Stretch && !image->sourceSize().isNull())
                                || (!image && m_input->isTextureProvider())
                              )
                  ) {
            direct = true;
        }
    }

    if (direct)
        setOutput(m_input);
    else
        useProxy();

    // Remove the proxy if it is not in use..
    if (m_proxy && m_output == m_input) {
        delete m_proxy;
        m_proxy = nullptr;
    }
}

QT_END_NAMESPACE

#include "moc_qgfxsourceproxy_p.cpp"
