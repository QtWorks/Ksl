/*
 * Copyright (C) 2016  Elvis Teixeira
 *
 * This source code is free software: you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General
 * Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any
 * later version.
 *
 * This source code is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <Ksl/XYAxis_p.h>
#include <Ksl/FigureScale.h>

namespace Ksl {

XYAxis::XYAxis(Qt::Orientation orientation,
               const QString &name, QObject *parent)
    : FigureItem(new XYAxisPrivate(this), name, parent)
{
    KSL_PUBLIC(XYAxis);
    m->rescalable = false;
    m->orientation = orientation;
    m->sampler = new XYAxisSampler();
}

XYAxisPrivate::~XYAxisPrivate() {
    delete sampler;
}

void XYAxis::setPosition(double min, double max,
                         double anchor)
{
    KSL_PUBLIC(XYAxis);
    m->min = min;
    m->max = max;
    m->anchor = anchor;
}

XYAxisSampler* XYAxis::sampler() const {
    KSL_PUBLIC(const XYAxis);
    return m->sampler;
}

XYAxis::Component XYAxis::visibleComponents() const {
    KSL_PUBLIC(const XYAxis);
    return m->components;
}

void XYAxis::showComponents(Component components) {
    KSL_PUBLIC(XYAxis);
    if (m->components != components) {
        m->components = components;
        emit appearenceChanged(this);
    }
}

QRect XYAxis::figureRect() const {
    KSL_PUBLIC(const XYAxis);
    if (m->scale) {
        if (m->orientation == Qt::Horizontal) {
            QPoint p1 = m->scale->map(QPointF(m->min, m->anchor));
            QPoint p2 = m->scale->map(QPointF(m->max, m->anchor));
            return QRect(p1.x(), p1.y()-4, p2.x() - p1.x(), 8);
        }
        else { // m->orientation == Qt::Vertical
            QPoint p1 = m->scale->map(QPointF(m->anchor, m->max));
            QPoint p2 = m->scale->map(QPointF(m->anchor, m->min));
            return QRect(p1.x()-4, p1.y(), 8, p2.y() - p1.y());
        }
    }
    return QRect();
}

QRectF XYAxis::dataRect() const {
    KSL_PUBLIC(const XYAxis);
    if (m->orientation == Qt::Horizontal) {
        return QRectF(m->min, m->anchor, m->max - m->min, 0.0);
    }
    // else m->orientation == Qt::Vertical
    return QRectF(m->anchor, m->min, 0.0, m->max - m->min);
}

void XYAxis::paint(QPainter *painter) {
    KSL_PUBLIC(XYAxis);

    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setPen(m->pen);

    // for gui apps it is good to provide hability to
    // visually mark the axis as selected
    if (m->selected)
        painter->fillRect(figureRect(), m->brush);

    m->setUpPaint();
    if (m->orientation == Qt::Horizontal)
        m->paintHorizontal(painter);
    else // m->orientation == Qt::Vertical
        m->paintVertical(painter);
}

void XYAxisPrivate::paintHorizontal(QPainter *painter) {
    QPoint p1 = scale->map(QPointF(min, anchor));
    QPoint p2 = scale->map(QPointF(max, anchor));

    QFontMetrics fontMetrics = painter->fontMetrics();
    int txtHei = fontMetrics.height();
    int txtWid = 0;

    // Draw line
    if (components & XYAxis::Line)
        painter->drawLine(p1, p2);

    // Draw downward ticks
    if (components & XYAxis::TicksDown) {
        for (auto &sample : sampler->sampleList()) {
            QPoint p = scale->map(QPointF(sample.coord, anchor));
            txtWid = fontMetrics.width(sample.label);
            painter->drawLine(p.x(), p.y(), p.x(), p.y()-3);
            painter->drawText(p.x()-txtWid/2, p.y()+txtHei, sample.label);
        }
    }
    // Draw upward ticks
    if (components & XYAxis::TicksUp) {
        for (auto &sample : sampler->sampleList()) {
            QPoint p = scale->map(QPointF(sample.coord, anchor));
            txtWid = fontMetrics.width(sample.label);
            painter->drawLine(p.x(), p.y(), p.x(), p.y()+3);
            painter->drawText(p.x()-txtWid/2, p.y()-txtHei/3, sample.label);
        }
    }
    // Draw middle title
    if (components & XYAxis::MiddleTitle) {
        txtWid = fontMetrics.width(name);
        int x = (p2.x() + p1.x() - txtWid) / 2;
        int y = p1.y();
        if (components & XYAxis::TicksUp)
            painter->drawText(x, y-5*txtHei/3, name);
        else
            painter->drawText(x, y+2*txtHei, name);
    }
    // Draw end title
    if (components & XYAxis::EndTitle) {
        txtWid = fontMetrics.width(name);
        int x = p2.x() + txtHei/3;
        int y = p2.y();
        painter->drawText(x, y, name);
    }
}

void XYAxisPrivate::paintVertical(QPainter *painter) {
    QPoint p1 = scale->map(QPointF(anchor, min));
    QPoint p2 = scale->map(QPointF(anchor, max));

    QFontMetrics fontMetrics = painter->fontMetrics();
    int txtHei = fontMetrics.height();
    int txtWid, maxTxtWid = 0;

    // Draw line
    if (components & XYAxis::Line)
        painter->drawLine(p1, p2);

    // Draw leftward ticks
    if (components & XYAxis::TicksDown) {
        for (auto &sample : sampler->sampleList()) {
            QPoint p = scale->map(QPointF(anchor, sample.coord));
            txtWid = fontMetrics.width(sample.label);
            if (txtWid > maxTxtWid) maxTxtWid = txtWid;
            painter->drawLine(p.x(), p.y(), p.x()+3, p.y());
            painter->drawText(p.x()-txtWid-txtHei/3,
                p.y()+txtHei/3, sample.label);
        }
    }
    // Draw rightward ticks
    if (components & XYAxis::TicksUp) {
        for (auto &sample : sampler->sampleList()) {
            QPoint p = scale->map(QPointF(anchor, sample.coord));
            txtWid = fontMetrics.width(sample.label);
            if (txtWid > maxTxtWid) maxTxtWid = txtWid;
            painter->drawLine(p.x(), p.y(), p.x()-3, p.y());
            painter->drawText(p.x()+txtHei/3,
                p.y()+txtHei/3, sample.label);
        }
    }
    // Draw middle title
    if (components & XYAxis::MiddleTitle) {
        txtWid = fontMetrics.width(name);
        int y = p1.x();
        int x = -(p1.y() + p2.y() + txtWid) / 2;
        painter->save();
        painter->rotate(-90.0);
        if (components & XYAxis::TicksUp)
            painter->drawText(x, y+4*txtHei/3+maxTxtWid, name);
        else
            painter->drawText(x, y-2*txtHei/3-maxTxtWid, name);
        painter->restore();
    }
    // Draw end title
    if (components & XYAxis::EndTitle) {
        txtWid = fontMetrics.width(name);
        int x = p2.x();
        int y = p2.y() - txtHei/3;
        painter->drawText(x, y, name);
    }
}

void XYAxisPrivate::setUpPaint() {
    if (orientation == Qt::Horizontal) {
        if (sampler->mode() == XYAxisSampler::AutoDecimal) {
            QPoint p1 = scale->map(QPointF(min, anchor));
            QPoint p2 = scale->map(QPointF(max, anchor));
            sampler->autoSampleDecimal(
                min, max, double(p2.x()-p1.x())/80.0);
        }
    }
    else { // orientation == Qt::Vertical
        if (sampler->mode() == XYAxisSampler::AutoDecimal) {
            QPoint p1 = scale->map(QPointF(anchor, min));
            QPoint p2 = scale->map(QPointF(anchor, max));
            sampler->autoSampleDecimal(
                min, max, double(p1.y()-p2.y())/60.0);
        }
    }
    if (hideZero) {
        for (auto &sample: sampler->sampleList())
            if (sample.coord == 0.0)
                sample.label = "";
    }
}

void XYAxis::hideZero(bool hide) {
    KSL_PUBLIC(XYAxis);
    if (m->hideZero != hide) {
        m->hideZero = hide;
        emit appearenceChanged(this);
    }
}

} // namespace Ksl
