//  Mine.hpp -- Floating space mine things.
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
