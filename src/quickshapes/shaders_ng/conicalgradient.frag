// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#version 440

layout(location = 0) in vec2 coord;
layout(location = 0) out vec4 fragColor;

layout(binding = 1) uniform sampler2D gradTabTexture;

layout(std140, binding = 0) uniform buf {
#if QSHADER_VIEW_COUNT >= 2
    mat4 matrix[QSHADER_VIEW_COUNT];
#else
    mat4 matrix;
#endif
    mat4 gradientMatrix;
    vec2 translationPoint;
    float angle;
    float opacity;
} ubuf;

#define INVERSE_2PI 0.1591549430918953358

void main()
{
    float t;
    if (abs(coord.y) == abs(coord.x))
        t = (atan(-coord.y + 0.002, coord.x) + ubuf.angle) * INVERSE_2PI;
    else
        t = (atan(-coord.y, coord.x) + ubuf.angle) * INVERSE_2PI;
    fragColor = texture(gradTabTexture, vec2(t - floor(t), 0.5)) * ubuf.opacity;
}
