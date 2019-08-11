//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INC_SHIP_HPP
#define INC_SHIP_HPP

#include "Platform.hpp"
#include "Geometry.hpp"
#include "Viewport.hpp"
#include "Emitter.hpp"
#include "Image.hpp"
#include "SoundEffect.hpp"

class Ship {
public:
   Ship(Viewport* v);

   void Reset();

   void Display() const;
   void DrawExhaust();
   void DrawExplosion();
   void Move();
   void ProcessEffects(bool paused, bool exploding);
   void ThrustOn();
   void ThrustOff();
   void Thrust(double speed);
   void Turn(double delta);
   void Bounce();
   void ApplyGravity(double gravity);
   void CentreInViewport();

   bool CheckCollision(LineSegment& l, double dx=0, double dy=0) const;
   bool HotSpotCollision(LineSegment& l, double dx=0, double dy=0) const;
   bool BoxCollision(int x, int y, int w, int h) const;

   double GetX() const { return xpos; }
   double GetY() const { return ypos; }
   double GetXSpeed() const { return speedX; }
   double GetYSpeed() const { return speedY; }
   double GetAngle() const { return angle; }

   static const int SHIP_START_Y = 100;

private:
   void RotatePoints(const Point* pPoints, Point* pDest, int nCount,
                     double angle, int adjustx=0, int adjusty=0);

   Image shipImage;

   double xpos, ypos;
   double speedX, speedY, angle;

   Viewport* viewport;
   Explosion explosion;
   BlueSmokeTrail exhaust;
   bool thrusting;

   SoundEffect boingSound;

   static const int NUM_HOTSPOTS = 8;
   Point points[NUM_HOTSPOTS];
   static const Point hotspots[];
};

#endif
