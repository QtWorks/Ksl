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

#ifndef KSL_XYSCALE_P_H
#define KSL_XYSCALE_P_H

#include <Ksl/XYScale.h>
#include <Ksl/FigureScale_p.h>

namespace Ksl {

class XYScalePrivate
    : public FigureScalePrivate
{
public:

    XYScalePrivate(XYScale *publ)
        : FigureScalePrivate(publ)
        , xLowBound(20), xUpBound(20)
        , yLowBound(20), yUpBound(20)
    { }

    int xLowBound, xUpBound;
    int yLowBound, yUpBound;

    int figXmin, figXmax;
    int figYmin, figYmax;
    int figWidth, figHeight;

    double xMin, xMax;
    double yMin, yMax;
    double width, height;
};

} // namespace Ksl

#endif // KSL_XYSCALE_P_H
