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
#include "OpenGL.hpp"

const double Ship::EXHAUST_ALPHA_DELTA(0.01);
const double Ship::EXHAUST_WIDTH_DELTA(2.0);
const double Ship::EXHAUST_HEIGHT_DELTA(1.0);
const double Ship::EXHAUST_WIDTH_BASE(8.0);
const double Ship::EXHAUST_ALPHA_BASE(0.3);

/*
 * Defines a simplified polygon representing the ship.
 */
const Point Ship::hotspots[] = {
   {2, 31}, {2, 26}, {4, 14}, {16, 0}, 
   {29, 14}, {31, 26}, {31, 31}, {17, 31} };


Ship::Ship(Viewport *v)
   : shipImage("images/ship.png"), exhaustImage("images/exhaust.png"),
     xpos(0), ypos(0), speedX(0), speedY(0), angle(0), viewport(v),
     thrusting(false), exhaust_alpha(0.0), exhaust_width(EXHAUST_WIDTH_BASE),
     exhaust_height(0.0),
     boingSound(LocateResource("sounds/boing1.wav"))
{
   
}

void Ship::Display()
{
   double dx = xpos - viewport->GetXAdjust();
   double dy = ypos - viewport->GetYAdjust();

   double width = shipImage.GetWidth();
   double height = shipImage.GetHeight();
   double e_width = exhaust_width;
   double e_height = exhaust_height;
   
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_BLEND);
   glBindTexture(GL_TEXTURE_2D, shipImage.GetGLTexture());
   glLoadIdentity();
   glTranslated(dx + width/2, dy + height/2, 0.0);
   glRotated(angle, 0.0, 0.0, 1.0);
   glColor4d(1.0, 1.0, 1.0, 1.0);
   glBegin(GL_QUADS);
     glTexCoord2d(0.0, 0.0); glVertex2i(-(width/2), -(height/2));
     glTexCoord2d(0.0, 1.0); glVertex2i(-(width/2), height/2);
     glTexCoord2d(1.0, 1.0); glVertex2i(width/2, height/2);
     glTexCoord2d(1.0, 0.0); glVertex2i(width/2, -(height/2));
   glEnd();

   glColor4d(1.0, 1.0, 1.0, exhaust_alpha);
   glTranslated(0.0, height/2 + e_height/2, 0.0);
   glBindTexture(GL_TEXTURE_2D, exhaustImage.GetGLTexture());
   glBegin(GL_QUADS);
     glTexCoord2d(0.0, 0.0); glVertex2i(-(e_width/2), -(e_height/2));
     glTexCoord2d(0.0, 1.0); glVertex2i(-(e_width/2), e_height/2);
     glTexCoord2d(1.0, 1.0); glVertex2i(e_width/2, e_height/2);
     glTexCoord2d(1.0, 0.0); glVertex2i(e_width/2, -(e_height/2));
   glEnd();
}

void Ship::DrawExplosion(bool createNew)
{
   explosion.Draw((double)viewport->GetXAdjust(),
                  (double)viewport->GetYAdjust(), createNew);
}

void Ship::Move()
{
    RotatePoints(hotspots, points, NUM_HOTSPOTS, angle*PI/180, -16, 16);

    xpos += speedX;
    ypos += speedY;

    // Check bounds
    if (xpos <= 0.0) {
       xpos = 0.0;
       speedX *= -0.5;
        boingSound.Play();
    }
    else if (xpos + shipImage.GetWidth() > viewport->GetLevelWidth()) {
       xpos = (double)(viewport->GetLevelWidth() - shipImage.GetWidth());
       speedX *= -0.5;
       boingSound.Play();
    }
    if (ypos <= 0.0) {
       ypos = 0.0;
       speedY *= -0.5;
       boingSound.Play();
    }
    else if (ypos + shipImage.GetHeight() > viewport->GetLevelHeight()) {
       ypos = (double)(viewport->GetLevelHeight() - shipImage.GetHeight());
       speedY *= -0.5;
       boingSound.Play();
    }
    
    explosion.xpos = xpos + shipImage.GetWidth()/2;
    explosion.ypos = ypos + shipImage.GetHeight()/2;
}

void Ship::ThrustOn()
{
   thrusting = true;
   if (exhaust_alpha < 1.0f)
      exhaust_alpha += EXHAUST_ALPHA_DELTA;
   if (exhaust_width < exhaustImage.GetWidth())
      exhaust_width += EXHAUST_WIDTH_DELTA;
   if (exhaust_height < exhaustImage.GetHeight())
      exhaust_height += EXHAUST_HEIGHT_DELTA;
}

void Ship::ThrustOff()
{
   thrusting = false;
   if (exhaust_alpha > EXHAUST_ALPHA_BASE)
      exhaust_alpha -= EXHAUST_ALPHA_DELTA;
   if (exhaust_width > EXHAUST_WIDTH_BASE)
      exhaust_width -= EXHAUST_WIDTH_DELTA;
   if (exhaust_height > 0.0)
      exhaust_height -= EXHAUST_HEIGHT_DELTA;
}

void Ship::Thrust(double speed)
{
   speedX += speed * sinf(angle*(PI/180));
   speedY -= speed * cosf(angle*(PI/180));
}

void Ship::Turn(double delta)
{
   angle += delta;
}

void Ship::ApplyGravity(double gravity)
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

void Ship::CentreInViewport()
{
   int centrex = (int)xpos + (shipImage.GetWidth()/2);
   int centrey = (int)ypos + (shipImage.GetHeight()/2);
   OpenGL &opengl = OpenGL::GetInstance();
   viewport->SetXAdjust(centrex - (opengl.GetWidth()/2));
   viewport->SetYAdjust(centrey - (opengl.GetHeight()/2));
}

/*
 * Reset at the start of a new level.
 */
void Ship::Reset()
{
   explosion.Reset();

   xpos = (double)viewport->GetLevelWidth()/2;
   ypos = SHIP_START_Y - 40;

   angle = 0.0f;
   speedX = 0.0f;
   speedY = 0.0f;
}

void Ship::RotatePoints(const Point *pPoints, Point *pDest, int nCount,
                        double angle, int adjustx, int adjusty)
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
bool Ship::HotSpotCollision(LineSegment &l, double dx, double dy)
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
bool Ship::CheckCollision(LineSegment &l, double dx, double dy)
{
   double xpos = this->xpos + dx;
   double ypos = this->ypos + dy;

   if (!viewport->PointInScreen
       ((int)xpos, (int)ypos, shipImage.GetWidth(), shipImage.GetHeight()))
      return false;

   // Get position after next move
   double cX = xpos + speedX;
   double cY = ypos + speedY;

   // Get displacement
   double vecX = cX - xpos;
   double vecY = cY - ypos;

   // Get line position
   double wallX = (double)(l.p2.x - l.p1.x);
   double wallY = (double)(l.p2.y - l.p1.y);

   // Work out numerator and denominator (used parametric equations)
   double numT = wallX * (ypos - l.p1.y) - wallY * (xpos - l.p1.x);
   double numU = vecX * (ypos - l.p1.y) - vecY * (xpos - l.p1.x);

   // Work out denominator
   double denom = wallY * (cX - xpos) - wallX * (cY - ypos);

   // Work out u and t
   double u = numU / denom;
   double t = numT / denom;

   // Collision occured if (0 < t < 1) and (0 < u < 1)
   return (t > 0.0f) && (t < 1.0f) && (u > 0.0f) && (u < 1.0f);
}

