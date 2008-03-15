/*  Ship.cpp -- The player's ship.
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

#include "Ship.hpp"
#include "DataFile.hpp"

extern DataFile *g_pData;

GLuint Ship::uShipTexture = 0;

Ship::Ship(Viewport *v)
   : xpos(0), ypos(0), speedX(0), speedY(0), angle(0), viewport(v)
{
   tq.width = SHIP_TEX_WIDTH;
   tq.height = SHIP_TEX_HEIGHT;
}

void Ship::Load()
{
   OpenGL &opengl = OpenGL::GetInstance();
   
   uShipTexture = opengl.LoadTextureAlpha(g_pData, "Ship.bmp");
}

void Ship::Display()
{
   tq.x = (int)xpos - viewport->GetXAdjust();
   tq.y = (int)ypos - viewport->GetYAdjust();
   tq.uTexture = uShipTexture;
   
   OpenGL::GetInstance().DrawRotate(&tq, angle);
}

/*
 * Check for collision between the ship and a polygon.
 */
bool Ship::HotSpotCollision(LineSegment &l, Point *points, int nPoints, float dx, float dy)
{
   for (int i = 0; i < nPoints; i++) {
      if (CheckCollision(l, dx + points[i].x, dy + points[i].y))
         return true;
   }

   return false;
}

/*
 * Checks for collision between the ship and a box.
 */
bool Ship::BoxCollision(int x, int y, int w, int h, Point *points, int nPoints)
{
   if (!viewport->PointInScreen(x, y, w, h))
      return false;
   
   LineSegment l1(x, y, x + w, y);
   LineSegment l2(x + w, y, x + w, y + h);
   LineSegment l3(x + w, y + h, x, y + h);
   LineSegment l4(x, y + h, x, y);

   return HotSpotCollision(l1, points, nPoints)
      || HotSpotCollision(l2, points, nPoints)
      || HotSpotCollision(l3, points, nPoints)
      || HotSpotCollision(l4, points, nPoints);
}

/*
 * Checks for collision between the ship and a line segment.
 */
bool Ship::CheckCollision(LineSegment &l, float dx, float dy)
{
   float xpos = this->xpos + dx;
   float ypos = this->ypos + dy;

   if (!viewport->PointInScreen(xpos, ypos, SHIP_TEX_WIDTH, SHIP_TEX_HEIGHT))
      return false;

   // Get position after next move
   float cX = xpos + speedX;
   float cY = ypos + speedY;

   // Get displacement
   float vecX = cX - xpos;
   float vecY = cY - ypos;

   // Get line position
   float wallX = (float)(l.p2.x - l.p1.x);
   float wallY = (float)(l.p2.y - l.p1.y);

   // Work out numerator and denominator (used parametric equations)
   float numT = wallX * (ypos - l.p1.y) - wallY * (xpos - l.p1.x);
   float numU = vecX * (ypos - l.p1.y) - vecY * (xpos - l.p1.x);

   // Work out denominator
   float denom = wallY * (cX - xpos) - wallX * (cY - ypos);

   // Work out u and t
   float u = numU / denom;
   float t = numT / denom;

   // Collision occured if (0 < t < 1) and (0 < u < 1)
   return (t > 0.0f) && (t < 1.0f) && (u > 0.0f) && (u < 1.0f);
}

