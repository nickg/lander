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

/*
 * Defines a simplified polygon representing the ship.
 */
const Point Ship::hotspots[] = {
   {1, 31}, {1, 26}, {3, 14}, {15, 0}, 
   {28, 14}, {30, 26}, {30, 31}, {16, 31} };


Ship::Ship(Viewport *v)
   : xpos(0), ypos(0), speedX(0), speedY(0), angle(0), viewport(v),
     thrusting(false)
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

void Ship::DrawExhaust(bool paused)
{
   static float xlast, ylast;
   
   if (thrusting) {
      if (sqrt(speedX*speedX + speedY*speedY) > 2.0f) {
         exhaust.NewCluster
            ((int)(exhaust.xpos + (exhaust.xpos - xlast)/2), 
             (int)(exhaust.ypos + (exhaust.ypos - ylast)/2));
      }
      exhaust.Draw((float)viewport->GetXAdjust(),
                   (float)viewport->GetYAdjust(), true);
   }
   else if (paused)
      exhaust.Draw((float)viewport->GetXAdjust(),
                   (float)viewport->GetYAdjust(), false, false);
   else
      exhaust.Draw((float)viewport->GetXAdjust(),
                   (float)viewport->GetYAdjust(), false);

   xlast = exhaust.xpos;
   ylast = exhaust.ypos;
}

void Ship::DrawExplosion(bool createNew)
{
   explosion.Draw((float)viewport->GetXAdjust(),
                  (float)viewport->GetYAdjust(), createNew);
}

void Ship::Move()
{
    RotatePoints(hotspots, points, NUM_HOTSPOTS, angle*PI/180, -16, 16);

    xpos += speedX;
    ypos += speedY;

    // Check bounds
    if (xpos <= 0.0f) {
       xpos = 0.0f;
       speedX *= -0.5f;
    }
    else if (xpos + tq.width > viewport->GetLevelWidth()) {
      xpos = (float)(viewport->GetLevelWidth() - tq.width);
      speedX *= -0.5f;
    }
    if (ypos <= 0.0f) {
       ypos = 0.0f;
       speedY *= -0.5f;
    }
    else if (ypos + tq.height > viewport->GetLevelHeight()) {
       ypos = (float)(viewport->GetLevelHeight() - tq.height);
       speedY *= -0.5f;
    }
    
    exhaust.xpos = xpos + tq.width/2
       - (tq.width/2)*(float)sin(angle*(PI/180));
    exhaust.ypos = ypos + tq.height/2
       + (tq.height/2)*(float)cos(angle*(PI/180));
    exhaust.yg = speedY; //+ (flGravity * 10);
    exhaust.xg = speedX;
    explosion.xpos = xpos + tq.width/2;
    explosion.ypos = ypos + tq.height/2;
}

void Ship::ThrustOn()
{
   thrusting = true;
}

void Ship::ThrustOff()
{
   thrusting = false;
}

void Ship::Thrust(float speed)
{
   speedX += speed * sinf(angle*(PI/180));
   speedY -= speed * cosf(angle*(PI/180));
}

void Ship::ApplyGravity(float gravity)
{
   speedY += gravity;
}

void Ship::Bounce()
{
   speedX *= -1;
   speedY *= -1;
   speedX /= 2;
   speedY /= 2;
}

/*
 * Reset at the start of a new level.
 */
void Ship::Reset()
{
   exhaust.Reset();
   explosion.Reset();

   xpos = (float)viewport->GetLevelWidth()/2;
   ypos = SHIP_START_Y - 40;
}

void Ship::RotatePoints(const Point *pPoints, Point *pDest, int nCount,
                        float angle, int adjustx, int adjusty)
{
   for (int i = 0; i < nCount; i++) {
      int x = pPoints[i].x + adjustx;
      int y = pPoints[i].y*-1 + adjusty;
      pDest[i].x = (int)(x*cos(angle)) + (int)(y*sin(angle));
      pDest[i].y = (int)(y*cos(angle)) - (int)(x*sin(angle));
      pDest[i].y -= adjusty;
      pDest[i].x -= adjustx;
      pDest[i].y *= -1;
   }
}

/*
 * Check for collision between the ship and a polygon.
 */
bool Ship::HotSpotCollision(LineSegment &l, float dx, float dy)
{
   for (int i = 0; i < NUM_HOTSPOTS; i++) {
      if (CheckCollision(l, dx + points[i].x, dy + points[i].y))
         return true;
   }

   return false;
}

/*
 * Checks for collision between the ship and a box.
 */
bool Ship::BoxCollision(int x, int y, int w, int h)
{
   if (!viewport->PointInScreen(x, y, w, h))
      return false;
   
   LineSegment l1(x, y, x + w, y);
   LineSegment l2(x + w, y, x + w, y + h);
   LineSegment l3(x + w, y + h, x, y + h);
   LineSegment l4(x, y + h, x, y);

   return HotSpotCollision(l1) || HotSpotCollision(l2)
      || HotSpotCollision(l3) || HotSpotCollision(l4);
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
