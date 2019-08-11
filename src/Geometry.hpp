//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INC_GEOMETRY_H
#define INC_GEOMETRY_H

struct Point
{
    int x, y;
};

struct LineSegment
{
    LineSegment(int x1, int y1, int x2, int y2)
    { p1.x=x1; p1.y=y1; p2.x=x2; p2.y=y2; }
    LineSegment()
    { p1.x = 0; p1.y = 0; p2.x = 0; p2.y = 0; }

    Point p1, p2;
};

#endif
