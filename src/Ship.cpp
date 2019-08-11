//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "Ship.hpp"
#include "OpenGL.hpp"

#include <string>
#include <cmath>

//
// Defines a simplified polygon representing the ship.
//
const Point Ship::hotspots[] = {
   {2, 31}, {2, 26}, {4, 14}, {16, 0},
   {29, 14}, {31, 26}, {31, 31}, {17, 31} };


Ship::Ship(Viewport* v)
   : shipImage("images/ship.png"),
     xpos(0), ypos(0), speedX(0), speedY(0), angle(0), viewport(v),
     thrusting(false),
     boingSound(LocateResource("sounds/boing1.wav"))
{

}

void Ship::Display() const
{
   int dx = (int)xpos - viewport->GetXAdjust();
   int dy = (int)ypos - viewport->GetYAdjust();

   shipImage.Draw(dx, dy, angle);
}

void Ship::DrawExhaust()
{
   exhaust.Draw((float)viewport->GetXAdjust(),
                (float)viewport->GetYAdjust());
}

void Ship::DrawExplosion()
{
   explosion.Draw((float)viewport->GetXAdjust(),
                  (float)viewport->GetYAdjust());
}

void Ship::Move()
{
   RotatePoints(hotspots, points, NUM_HOTSPOTS, angle*M_PI/180, -16, 16);

   const OpenGL::TimeScale timeScale = OpenGL::GetInstance().GetTimeScale();

   xpos += speedX * timeScale;
   ypos += speedY * timeScale;

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

   exhaust.xpos = xpos + shipImage.GetWidth()/2
      - (shipImage.GetWidth()/2)*sin(angle*(M_PI/180));
   exhaust.ypos = ypos + shipImage.GetHeight()/2
      + (shipImage.GetHeight()/2)*cos(angle*(M_PI/180));

   const float SCALE = 1.0f;
   exhaust.yi_bias = SCALE * cosf(angle*M_PI/180) + speedY;
   exhaust.xi_bias = SCALE * -sinf(angle*M_PI/180) + speedX;

   explosion.xpos = xpos + shipImage.GetWidth()/2;
   explosion.ypos = ypos + shipImage.GetHeight()/2;
}

void Ship::ProcessEffects(bool paused, bool exploding)
{
   exhaust.Process(thrusting, !paused);
   explosion.Process(exploding);
}

void Ship::ThrustOn()
{
   thrusting = true;
}

void Ship::ThrustOff()
{
   thrusting = false;
}

void Ship::Thrust(double speed)
{
   speedX += speed * sin(angle*(M_PI/180));
   speedY -= speed * cos(angle*(M_PI/180));
}

void Ship::Turn(double delta)
{
   angle += delta * OpenGL::GetInstance().GetTimeScale();
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
   OpenGL& opengl = OpenGL::GetInstance();
   viewport->SetXAdjust(centrex - (opengl.GetWidth()/2));
   viewport->SetYAdjust(centrey - (opengl.GetHeight()/2));
}

//
// Reset at the start of a new level.
//
void Ship::Reset()
{
   exhaust.Reset();
   explosion.Reset();

   xpos = (double)viewport->GetLevelWidth()/2;
   ypos = SHIP_START_Y - 40;

   angle = 0.0f;
   speedX = 0.0f;
   speedY = 0.0f;
}

void Ship::RotatePoints(const Point* pPoints, Point* pDest, int nCount,
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

//
// Check for collision between the ship and a polygon.
//
bool Ship::HotSpotCollision(LineSegment& l, double dx, double dy) const
{
   for (int i = 0; i < NUM_HOTSPOTS; i++) {
      if (CheckCollision(l, dx + points[i].x, dy + points[i].y))
         return true;
   }

   return false;
}

//
// Checks for collision between the ship and a box.
//
bool Ship::BoxCollision(int x, int y, int w, int h) const
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

//
// Checks for collision between the ship and a line segment.
//
bool Ship::CheckCollision(LineSegment& l, double dx, double dy) const
{
   double xpos = this->xpos + dx;
   double ypos = this->ypos + dy;

   if (!viewport->PointInScreen
       ((int)xpos, (int)ypos, shipImage.GetWidth(), shipImage.GetHeight()))
      return false;

   const OpenGL::TimeScale timeScale = OpenGL::GetInstance().GetTimeScale();

   // Get position after next move
   double cX = xpos + speedX * timeScale;
   double cY = ypos + speedY * timeScale;

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
   return (t > 0.0) && (t < 1.0) && (u > 0.0) && (u < 1.0);
}
