//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#pragma once

#include "GameObjFwd.hpp"
#include "Emitter.hpp"
#include "SoundEffect.hpp"
#include "Image.hpp"

class Missile {
public:
   enum Side { SIDE_LEFT, SIDE_RIGHT };

   Missile(ObjectGrid* o, Viewport* v, Side s);

   void Draw() const;
   void Move(const Ship& ship);
   bool CheckCollison(const Ship& ship);
private:
   void MoveFixed(const Ship& ship);
   void MoveFlying();
   void MoveDestroyed();

   Viewport* viewport;
   int x, y, dx, dy;
   double angle, speed;

   enum State { FIXED, FLYING, DESTROYED };
   State state;
   OrangeSmokeTrail exhaust;
   static SoundEffect* fireSound;
   Image image;
   static const double ACCEL;
   static const double MAX_SPEED;
   static const int HORIZ_FIRE_RANGE, VERT_FIRE_RANGE;
};
