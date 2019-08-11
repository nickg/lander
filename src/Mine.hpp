//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INC_MINE_HPP
#define INC_MINE_HPP

#include "ObjectGrid.hpp"
#include "Viewport.hpp"
#include "AnimatedImage.hpp"
#include "GameObjFwd.hpp"

class Mine : public StaticObject {
public:
   Mine(ObjectGrid* o, Viewport* v, int x, int y);

   void Move();
   void Draw() const;
   bool CheckCollision(const Ship& ship) const;

   static const int MINE_FRAME_COUNT = 18;

private:
   static constexpr float MINE_ROTATION_SPEED = 0.1f;
   static constexpr float MINE_MOVE_SPEED = 0.5f;

   enum Direction { dirUp, dirRight, dirDown, dirLeft, dirNone };

   ObjectGrid* objgrid;
   Viewport* viewport;
   Direction dir;
   int movetimeout;
   float m_displaceX = 0.0f, m_displaceY = 0.0f;
   float m_rotateAnim = 0.0f;

   AnimatedImage image;
};

#endif
