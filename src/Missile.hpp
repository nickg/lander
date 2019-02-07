//
//  Missile.hpp -- Missiles on asteroids, etc.
//  Copyright (C) 2008-2019  Nick Gasson
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#pragma once

#include "GameObjFwd.hpp"
#include "GraphicsFwd.hpp"
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
