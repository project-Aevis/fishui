/*
 * Copyright (C) 2018 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * The box blur implementation is based on AlphaBoxBlur from Firefox.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

// own
#include "boxshadowrenderer.h"

// Qt
#include <QPainter>
#include <QtMath>

static inline int calculateBlurRadius(qreal stdDev)
{
    // True Gaussian blur radius: 3*sigma captures 99.7% of the Gaussian falloff
    return qMax(2, qCeil(stdDev * 3.0));
}

static inline qreal calculateBlurStdDev(int radius)
{
    // See https://www.w3.org/TR/css-backgrounds-3/#shadow-blur
    return radius * 0.5;
}

static inline QSize calculateBlurExtent(int radius)
{
    const int blurRadius = calculateBlurRadius(calculateBlurStdDev(radius));
    return QSize(blurRadius, blurRadius);
}

/**
 * Convolve the alpha channel of an ARGB32 image with a true separable 2D Gaussian kernel.
 *
 * @param image The input/output image.
 * @param stdDev The standard deviation of the Gaussian filter.
 **/
static void gaussianBlurAlpha(QImage &image, qreal stdDev)
{
    if (stdDev < 0.2) {
        return;
    }

    const int width = image.width();
    const int height = image.height();
    if (width <= 0 || height <= 0) {
        return;
    }

    const int kRadius = qMax(1, qCeil(stdDev * 3.0));
    const int kernelSize = 2 * kRadius + 1;

    QVector<float> kernel(kernelSize);
    const float twoSigmaSq = 2.0f * static_cast<float>(stdDev * stdDev);
    float kernelSum = 0.0f;
    for (int i = -kRadius; i <= kRadius; ++i) {
        float val = std::exp(-(i * i) / twoSigmaSq);
        kernel[i + kRadius] = val;
        kernelSum += val;
    }
    for (int i = 0; i < kernelSize; ++i) {
        kernel[i] /= kernelSum;
    }

    const int alphaOffset = QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 3;
    const int pixelStride = image.depth() >> 3;

    QVector<float> buffer(width * height);

    // Pass 1: Horizontal Gaussian blur (image -> buffer)
    for (int y = 0; y < height; ++y) {
        const uint8_t *srcRow = image.constScanLine(y) + alphaOffset;
        float *dstRow = buffer.data() + y * width;
        for (int x = 0; x < width; ++x) {
            float sum = srcRow[x * pixelStride] * kernel[kRadius];
            for (int k = 1; k <= kRadius; ++k) {
                const float w = kernel[kRadius + k];
                const int leftX = x - k;
                const int rightX = x + k;
                float sampleSum = 0.0f;
                if (leftX >= 0) {
                    sampleSum += srcRow[leftX * pixelStride];
                }
                if (rightX < width) {
                    sampleSum += srcRow[rightX * pixelStride];
                }
                sum += sampleSum * w;
            }
            dstRow[x] = sum;
        }
    }

    // Pass 2: Vertical Gaussian blur (buffer -> image)
    for (int y = 0; y < height; ++y) {
        uint8_t *dstRow = image.scanLine(y) + alphaOffset;
        for (int x = 0; x < width; ++x) {
            float sum = buffer[y * width + x] * kernel[kRadius];
            for (int k = 1; k <= kRadius; ++k) {
                const float w = kernel[kRadius + k];
                const int topY = y - k;
                const int bottomY = y + k;
                float sampleSum = 0.0f;
                if (topY >= 0) {
                    sampleSum += buffer[topY * width + x];
                }
                if (bottomY < height) {
                    sampleSum += buffer[bottomY * width + x];
                }
                sum += sampleSum * w;
            }
            dstRow[x * pixelStride] = static_cast<uint8_t>(qBound(0.0f, sum + 0.5f, 255.0f));
        }
    }
}

static inline void mirrorTopLeftQuadrant(QImage &image)
{
    const int width = image.width();
    const int height = image.height();

    const int centerX = qCeil(width * 0.5);
    const int centerY = qCeil(height * 0.5);

    const int alphaOffset = QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 3;
    const int stride = image.depth() >> 3;

    for (int y = 0; y < centerY; ++y) {
        uint8_t *in = image.scanLine(y) + alphaOffset;
        uint8_t *out = in + (width - 1) * stride;

        for (int x = 0; x < centerX; ++x, in += stride, out -= stride) {
            *out = *in;
        }
    }

    for (int y = 0; y < centerY; ++y) {
        const uint8_t *in = image.scanLine(y) + alphaOffset;
        uint8_t *out = image.scanLine(height - y - 1) + alphaOffset;

        for (int x = 0; x < width; ++x, in += stride, out += stride) {
            *out = *in;
        }
    }
}

static void renderShadow(QPainter *painter, const QRect &rect, qreal borderRadius, const QPoint &offset, int radius, const QColor &color)
{
    const QSize inflation = calculateBlurExtent(radius);
    const QSize size = rect.size() + 2 * inflation;

    const qreal dpr = painter->device()->devicePixelRatioF();

    QImage shadow(size * dpr, QImage::Format_ARGB32_Premultiplied);
    shadow.setDevicePixelRatio(dpr);
    shadow.fill(Qt::transparent);

    QRect boxRect(QPoint(0, 0), rect.size());
    boxRect.moveCenter(QRect(QPoint(0, 0), size).center());

    const qreal xRadius = 2.0 * borderRadius / boxRect.width();
    const qreal yRadius = 2.0 * borderRadius / boxRect.height();

    QPainter shadowPainter;
    shadowPainter.begin(&shadow);
    shadowPainter.setRenderHint(QPainter::Antialiasing);
    shadowPainter.setPen(Qt::NoPen);
    shadowPainter.setBrush(Qt::black);
    shadowPainter.drawRoundedRect(boxRect, xRadius, yRadius);
    shadowPainter.end();

    const qreal scaledStdDev = calculateBlurStdDev(radius) * dpr;
    gaussianBlurAlpha(shadow, scaledStdDev);
    mirrorTopLeftQuadrant(shadow);

    // Give the shadow a tint of the desired color.
    shadowPainter.begin(&shadow);
    shadowPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    shadowPainter.fillRect(shadow.rect(), color);
    shadowPainter.end();

    // Actually, present the shadow.
    QRect shadowRect = shadow.rect();
    shadowRect.setSize(shadowRect.size() / dpr);
    shadowRect.moveCenter(rect.center() + offset);
    painter->drawImage(shadowRect, shadow);
}

void BoxShadowRenderer::setBoxSize(const QSize &size)
{
    m_boxSize = size;
}

void BoxShadowRenderer::setBorderRadius(qreal radius)
{
    m_borderRadius = radius;
}

void BoxShadowRenderer::setDevicePixelRatio(qreal dpr)
{
    m_dpr = dpr;
}

void BoxShadowRenderer::addShadow(const QPoint &offset, int radius, const QColor &color)
{
    Shadow shadow = {};
    shadow.offset = offset;
    shadow.radius = radius;
    shadow.color = color;
    m_shadows.append(shadow);
}

QImage BoxShadowRenderer::render() const
{
    if (m_shadows.isEmpty()) {
        return {};
    }

    QSize canvasSize;
    for (const Shadow &shadow : std::as_const(m_shadows)) {
        canvasSize = canvasSize.expandedTo(
            calculateMinimumShadowTextureSize(m_boxSize, shadow.radius, shadow.offset));
    }

    QImage canvas(canvasSize * m_dpr, QImage::Format_ARGB32_Premultiplied);
    canvas.setDevicePixelRatio(m_dpr);
    canvas.fill(Qt::transparent);

    QRect boxRect(QPoint(0, 0), m_boxSize);
    boxRect.moveCenter(QRect(QPoint(0, 0), canvasSize).center());

    QPainter painter(&canvas);
    for (const Shadow &shadow : std::as_const(m_shadows)) {
        renderShadow(&painter, boxRect, m_borderRadius, shadow.offset, shadow.radius, shadow.color);
    }
    painter.end();

    return canvas;
}

QSize BoxShadowRenderer::calculateMinimumBoxSize(int radius)
{
    const QSize blurExtent = calculateBlurExtent(radius);
    return 2 * blurExtent + QSize(1, 1);
}

QSize BoxShadowRenderer::calculateMinimumShadowTextureSize(const QSize &boxSize, int radius, const QPoint &offset)
{
    return boxSize + 2 * calculateBlurExtent(radius) + QSize(qAbs(offset.x()), qAbs(offset.y()));
}