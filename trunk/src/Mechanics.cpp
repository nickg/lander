/*  Mechanics.cpp -- Frame-rate independent movemoment.
 *  Copyright (C) 2008  Nick Gasson
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <math.h>
#include "Mechanics.hpp"

Position::Position(float x, float y)
   : x(x), y(y)
{

}

Position Position::operator+(Velocity &v) const
{
   return Position(x + v.GetXSpeed(), y + v.GetYSpeed());
}

Position Position::operator-(Velocity &v) const
{
   return Position(x - v.GetXSpeed(), y - v.GetYSpeed());
}

void Position::operator+=(Velocity &v)
{
   x += v.GetXSpeed();
   y += v.GetYSpeed();
}


void Position::operator-=(Velocity &v)
{
   x -= v.GetXSpeed();
   y -= v.GetYSpeed();   
}

Velocity::Velocity(float xspeed, float yspeed)
   : xspeed(xspeed), yspeed(yspeed)
{

}

Velocity Velocity::Project(float speed, float angle)
{
   return Velocity(speed * cosf(angle), speed * sinf(angle));
}

