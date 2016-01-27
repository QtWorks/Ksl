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

#include <QSL/FigurePrivate.h>
#include <QSL/FigureScale.h>
#include <QSL/FigureItem.h>

QSL_BEGIN_NAMESPACE

Figure::Figure(const QString &name, QObject *parent)
    : QObject(parent)
    , QSL::Object(new FigurePrivate(this, name))
{ }


QString Figure::name() const {
    QSL_PUBLIC(const Figure);
    return m->name;
}


QFont Figure::font() const {
    QSL_PUBLIC(const Figure);
    return m->font;
}


Figure::ScaleList& Figure::scaleList() {
    QSL_PUBLIC(Figure);
    return m->scaleList;
}

const Figure::ScaleList& Figure::scaleList() const {
    QSL_PUBLIC(const Figure);
    return m->scaleList;
}


FigureScale* Figure::scale(const QString &name) const {
    QSL_PUBLIC(const Figure);

    for (auto &scale : m->scaleList) {
        if (scale->name() == name) {
            return scale;
        }
    }
    // not found
    return nullptr;
}


FigureItem* Figure::item(const QString &name) const {
    QSL_PUBLIC(const Figure);

    for (auto &scale : m->scaleList) {
        auto item = scale->item(name);
        if (item != nullptr)
            return item;
    }
    // not found
    return nullptr;
}


void Figure::add(FigureScale *scale) {
    QSL_PUBLIC(Figure);

    if (m->scaleList.contains(scale) == false) {
        m->scaleList.append(scale);
        scale->setFigure(this);
        emit change(this);
    }
}


void Figure::setFont(const QFont &font) {
    QSL_PUBLIC(Figure);

    if (m->font != font) {
        m->font = font;
        emit change(this);
    }
}


void Figure::setTitlePen(const QPen &pen) {
    QSL_PUBLIC(Figure);

    if (m->titlePen != pen) {
        m->titlePen = pen;
        emit change(this);
    }
}


void Figure::setShowTitle(bool showTitle) {
    QSL_PUBLIC(Figure);

    if (m->showTitle != showTitle) {
        m->showTitle = showTitle;
        emit change(this);
    }
}


void Figure::paint(const QRect &targetRect, QPainter *painter) {
    QSL_PUBLIC(Figure);

    painter->save();
    painter->setClipRect(targetRect);
    if (m->fillBack) {
        painter->fillRect(targetRect, m->backBrush);
    }
    for (auto &scale : m->scaleList) {
        if (scale->visible() == true) {
            scale->paint(targetRect, painter);
        }
    }
    if (m->showTitle == true) {
        painter->setFont(m->font);
        auto fontMetrics = painter->fontMetrics();
        int x = targetRect.center().x() - fontMetrics.width(m->name)/2;
        int y = fontMetrics.height();
        painter->setPen(m->titlePen);
        painter->drawText(x, y, m->name);
    }
    painter->restore();
}


void Figure::save(const QString &fileName, const QSize &size,
                  const char *fileFormat)
{
    QImage image(size, QImage::Format_ARGB32);
    QPainter painter(&image);
    paint(QRect(QPoint(0,0), size), &painter);
    image.save(fileName, fileFormat);
}


void Figure::handleAppearenceChange(FigureItem *item) {
    Q_UNUSED(item)
    emit change(this);
}


void Figure::handleDataChange(FigureItem *item) {
    item->scale()->rescale();
    emit change(this);
}

QSL_END_NAMESPACE
