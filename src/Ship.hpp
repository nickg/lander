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
#include "Emitter.hpp"

class Ship {
public:
   Ship(Viewport *v);

   static void Load();

   void Reset();

   void Display();
   void DrawExhaust(bool thrusting, bool paused);
   void DrawExplosion(bool createNew);
   void Move();
   void Thrust(float speed);
   void Bounce();
   void ApplyGravity(float gravity);
   
   bool CheckCollision(LineSegment &l, float dx=0, float dy=0);
   bool HotSpotCollision(LineSegment &l, float dx=0, float dy=0);
   bool BoxCollision(int x, int y, int w, int h);
   
   float xpos, ypos;
   float speedX, speedY, angle;
   TextureQuad tq;
   
   static const int SHIP_START_Y = 100;

private:
   void RotatePoints(const Point *pPoints, Point *pDest, int nCount,
                     float angle, int adjustx=0, int adjusty=0);
   
   static const int SHIP_TEX_WIDTH = 32;
   static const int SHIP_TEX_HEIGHT = 32;
   
   static GLuint uShipTexture;

   Viewport *viewport;
   Explosion explosion;
   SmokeTrail exhaust;
   
   static const int NUM_HOTSPOTS = 8;
   Point points[NUM_HOTSPOTS];
   static const Point hotspots[];
};

#endif
