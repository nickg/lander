/*  Mechanics.hpp -- Frame-rate independent movemoment.
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

#ifndef INC_MECHANICS_HPP
#define INC_MECHANICS_HPP

class Velocity {
public:
   Velocity(float xspeed, float yspeed);
   Velocity() { Velocity(0.0f, 0.0f); }

   static Velocity Project(float speed, float angle);

   float GetXSpeed() const { return xspeed; }
   float GetYSpeed() const { return yspeed; }
   
private:
   float xspeed, yspeed;
};
   
class Position {
public:
   Position(float x, float y);
   Position() { Position(0, 0); }

   float GetX() const { return x; }
   float GetY() const { return y; }

   Position operator+(Velocity &v) const;
   Position operator-(Velocity &v) const;
   void operator+=(Velocity &v);
   void operator-=(Velocity &v);
   
private:
   float x, y;
};

#endif
