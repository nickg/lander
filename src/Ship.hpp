/*  Ship.hpp -- The player's ship.
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

#ifndef INC_SHIP_HPP
#define INC_SHIP_HPP

#include "Platform.hpp"
#include "OpenGL.hpp"
#include "Geometry.hpp"
#include "Viewport.hpp"

class Ship {
public:
   Ship(Viewport *v);

   static void Load();

   void Display();
   
   bool CheckCollision(LineSegment &l, float dx=0, float dy=0);
   bool HotSpotCollision(LineSegment &l, Point *points, int nPoints, float dx=0, float dy=0);
   bool BoxCollision(int x, int y, int w, int h, Point *points, int nPoints);
   
   float xpos, ypos;
   float speedX, speedY, angle;
   TextureQuad tq;

private:   
   static const int SHIP_TEX_WIDTH = 32;
   static const int SHIP_TEX_HEIGHT = 32;
   
   static GLuint uShipTexture;

   Viewport *viewport;
};

#endif
