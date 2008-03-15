/*  Mine.hpp -- Floating space mine things.
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

#ifndef INC_MINE_HPP
#define INC_MINE_HPP

#include "ObjectGrid.hpp"
#include "Ship.hpp"
#include "Viewport.hpp"

class Mine : public StaticObject {
public:
   Mine(ObjectGrid *o, Viewport *v, int x, int y);

   static void Load();

   void Move();
   void Draw();
   bool CheckCollision(Ship &ship);

   static const int MINE_FRAME_COUNT = 36;
 
private:
   static const int MINE_ROTATION_SPEED = 5;
   static const int MINE_MOVE_SPEED = 1;
   
   static GLuint uMineTexture[MINE_FRAME_COUNT];

   enum Direction { dirUp, dirRight, dirDown, dirLeft, dirNone };

   ObjectGrid *objgrid;
   Viewport *viewport;
   Direction dir;
   int current, rotcount, movetimeout;
   int displace_x, displace_y, movedelay;
   TextureQuad frame[MINE_FRAME_COUNT];
};

#endif
