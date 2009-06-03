//  Geometry.hpp -- Geometric types.
//  Copyright (C) 2008  Nick Gasson
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
