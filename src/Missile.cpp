//
//  Missile.cpp -- Missiles on asteroids, etc. 
//  Copyright (C) 2008  Nick Gasson
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

#include "Platform.hpp"
#include "Missile.hpp"
#include "LoadOnce.hpp"
#include "Image.hpp"
#include "ObjectGrid.hpp"
#include "Ship.hpp"

Image* Missile::image = NULL;

const double Missile::ACCEL(0.1);
const double Missile::MAX_SPEED(5.0);
const int Missile::HORIZ_FIRE_RANGE(600);
const int Missile::VERT_FIRE_RANGE(50); 

Missile::Missile(ObjectGrid* o, Viewport* v, Side s)
   : viewport(v), objgrid(o), speed(0.0), state(FIXED)
{
   // This constructor builds a missile attached to the side of the screen
   
   LOAD_ONCE {
      image = new Image("images/missile.png");
   }

   x = (s == SIDE_LEFT) ? 0 : o->GetWidth() - 1;

   // Pick spaces at random until we find one that's empty
   do {
      y = rand() % o->GetHeight();
   } while (o->IsFilled(x, y));
   
   ObjectGrid::Offset(x, y, &dx, &dy);

   angle = (s == SIDE_LEFT) ? 90 : 270;
}

void Missile::Draw() const
{
   if (viewport->PointInScreen(dx, dy, ObjectGrid::OBJ_GRID_SIZE,
                               ObjectGrid::OBJ_GRID_SIZE)
       && state != DESTROYED) {
      image->Draw(dx - viewport->GetXAdjust(), dy - viewport->GetYAdjust(), angle);
   }

   exhaust.Draw((double)viewport->GetXAdjust(),
                (double)viewport->GetYAdjust());
}

bool Missile::CheckCollison(const Ship& ship)
{
   // A bounding box collision isn't exactly accurate but should
   // work OK
   bool collided = ship.BoxCollision(dx, dy, image->GetWidth(), image->GetHeight());
   if (collided)
      state = DESTROYED;
   return collided;
}

void Missile::Move(const Ship& ship)
{
   switch (state) {
   case FIXED: MoveFixed(ship); break;
   case FLYING: MoveFlying(); break;
   case DESTROYED: MoveDestroyed(); break;
   }
}

void Missile::MoveFixed(const Ship& ship)
{
   // Decide whether to fire or not
   int missileMidX = dx + image->GetWidth()/2;
   int missileMidY = dy + image->GetHeight()/2;

   int xDistance = abs(static_cast<int>(ship.GetX()) - missileMidX);
   int yDistance = abs(static_cast<int>(ship.GetY()) - missileMidY);
   
   if (xDistance <= HORIZ_FIRE_RANGE && yDistance <= VERT_FIRE_RANGE)
      state = FLYING;

   exhaust.Process(false);
}

void Missile::MoveFlying()
{
   dx += speed * sin(angle * M_PI/180);
   dy += speed * cos(angle * M_PI/180);

   exhaust.xpos = dx + image->GetWidth()/2
      - (image->GetWidth()/2)*sin(angle*(M_PI/180));
   exhaust.ypos = dy + image->GetHeight()/2
      + (image->GetHeight()/2)*cos(angle*(M_PI/180));
   exhaust.Process(true);

   if (speed < MAX_SPEED)
      speed += ACCEL;

   if (dx > viewport->GetLevelWidth() || dy > viewport->GetLevelHeight()
       || dx + image->GetWidth() < 0 || dy < 0)
      state = DESTROYED;
}

void Missile::MoveDestroyed()
{
   exhaust.Process(false);
}
