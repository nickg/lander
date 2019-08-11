//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "Platform.hpp"
#include "Missile.hpp"
#include "Image.hpp"
#include "ObjectGrid.hpp"
#include "Ship.hpp"
#include "OpenGL.hpp"

#include <cmath>
#include <string>

SoundEffect* Missile::fireSound(NULL);

const double Missile::ACCEL(0.1);
const double Missile::MAX_SPEED(5.0);
const int Missile::HORIZ_FIRE_RANGE(600);
const int Missile::VERT_FIRE_RANGE(50);

Missile::Missile(ObjectGrid* o, Viewport* v, Side s)
   : viewport(v), speed(0.0), state(FIXED),
     image("images/missile.png")
{
   // This constructor builds a missile attached to the side of the screen

   if (NULL == fireSound)
      fireSound = new SoundEffect(LocateResource("sounds/missile.wav"),
                                  60); // Volume

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
      image.Draw(dx - viewport->GetXAdjust(), dy - viewport->GetYAdjust(), angle);
   }

   exhaust.Draw((double)viewport->GetXAdjust(),
                (double)viewport->GetYAdjust());
}

bool Missile::CheckCollison(const Ship& ship)
{
   // A bounding box collision isn't exactly accurate but should
   // work OK
   bool collided = ship.BoxCollision(dx, dy, image.GetWidth(), image.GetHeight());
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
   int missileMidX = dx + image.GetWidth()/2;
   int missileMidY = dy + image.GetHeight()/2;

   int xDistance = abs(static_cast<int>(ship.GetX()) - missileMidX);
   int yDistance = abs(static_cast<int>(ship.GetY()) - missileMidY);

   if (xDistance <= HORIZ_FIRE_RANGE && yDistance <= VERT_FIRE_RANGE) {
      state = FLYING;
      fireSound->Play();
   }

   exhaust.Process(false);
}

void Missile::MoveFlying()
{
   const OpenGL::TimeScale timeScale = OpenGL::GetInstance().GetTimeScale();

   dx += speed * timeScale * sin(angle * M_PI/180);
   dy += speed * timeScale * cos(angle * M_PI/180);

   exhaust.xpos = dx + image.GetWidth()/2
      - (image.GetWidth()/2)*sin(angle*(M_PI/180));
   exhaust.ypos = dy + image.GetHeight()/2
      + (image.GetHeight()/2)*cos(angle*(M_PI/180));
   exhaust.Process(true);

   if (speed < MAX_SPEED)
      speed += ACCEL;

   if (dx > viewport->GetLevelWidth() || dy > viewport->GetLevelHeight()
       || dx + image.GetWidth() < 0 || dy < 0)
      state = DESTROYED;
}

void Missile::MoveDestroyed()
{
   exhaust.Process(false);
}
