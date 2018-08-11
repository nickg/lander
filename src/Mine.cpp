//
//  Mine.cpp -- Floating space mine things.
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

#include "Mine.hpp"
#include "OpenGL.hpp"
#include "Ship.hpp"

#include <string>

Mine::Mine(ObjectGrid* o, Viewport* v, int x, int y)
   : StaticObject(x, y),
     objgrid(o),
     viewport(v),
     image("images/mine.png", 64, 64, MINE_FRAME_COUNT)
{
   dir = dirNone;
   movetimeout = 1;

   // Free space on object grid
   objgrid->UnlockSpace(xpos, ypos);
   objgrid->UnlockSpace(xpos + 1, ypos);
   objgrid->UnlockSpace(xpos + 1, ypos + 1);
   objgrid->UnlockSpace(xpos, ypos + 1);
}

void Mine::Move()
{
   if (fabs(m_displaceX) >= ObjectGrid::OBJ_GRID_SIZE
       || fabs(m_displaceY) >= ObjectGrid::OBJ_GRID_SIZE
       || dir == dirNone) {

      switch (dir) {
      case dirUp:
         ypos -= 1;
         break;
      case dirDown:
         ypos += 1;
         break;
      case dirLeft:
         xpos -= 1;
         break;
      case dirRight:
         xpos += 1;
         break;
      case dirNone:
         break;	// Do nothing
      }

      m_displaceX = 0.0f;
      m_displaceY = 0.0f;

      // Change direction
      bool ok = false;
      int nextx = 0, nexty = 0, timeout = 5;
      do {
         if (timeout < 5 || movetimeout == 0) {
            dir = (Direction)(rand() % 4);
            movetimeout = 5;
         }
         else {
            movetimeout--;
         }

         switch (dir) {
         case dirUp:
            nexty = ypos - 1;
            nextx = xpos;
            break;
         case dirDown:
            nexty = ypos + 1;
            nextx = xpos;
            break;
         case dirLeft:
            nexty = ypos;
            nextx = xpos - 1;
            break;
         case dirRight:
            nexty = ypos;
            nextx = xpos + 1;
            break;
         case dirNone:
         default:
            nextx = xpos;
            nexty = ypos;
         }

         // Check if this is ok
         ok = !(nextx + 1 >= objgrid->GetWidth() || nextx < 0
                || nexty + 1 >= objgrid->GetHeight() || nexty < 0
                || objgrid->IsFilled(nextx, nexty)
                || objgrid->IsFilled(nextx + 1, nexty)
                || objgrid->IsFilled(nextx + 1, nexty + 1)
                || objgrid->IsFilled(nextx, nexty + 1));
         timeout--;
      } while (!ok && timeout > 0);

      if (timeout == 0)
         dir = dirNone;
   }

   const float timeScale = OpenGL::GetInstance().GetTimeScale();
   const float delta = MINE_MOVE_SPEED * timeScale;

   switch(dir) {
   case dirUp: m_displaceY -= delta; break;
   case dirDown: m_displaceY += delta; break;
   case dirLeft: m_displaceX -= delta; break;
   case dirRight: m_displaceX += delta; break;
   default: break;
   }

   m_rotateAnim += MINE_ROTATION_SPEED * timeScale;

   int currentFrame = static_cast<int>(m_rotateAnim);
   if (currentFrame >= MINE_FRAME_COUNT) {
      currentFrame = 0;
      m_rotateAnim = 0.0f;
   }

   image.SetFrame(currentFrame);
}

bool Mine::CheckCollision(const Ship& ship) const
{
   return ship.BoxCollision
      (xpos*OBJ_GRID_SIZE + 3 + static_cast<int>(m_displaceX),
       ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP + 6 + static_cast<int>(m_displaceY),
       OBJ_GRID_SIZE*2 - 6,
       OBJ_GRID_SIZE*2 - 12);
}

void Mine::Draw() const
{
   int draw_x = xpos*OBJ_GRID_SIZE + static_cast<int>(m_displaceX)
      - viewport->GetXAdjust();
   int draw_y = ypos*OBJ_GRID_SIZE + static_cast<int>(m_displaceY)
      - viewport->GetYAdjust() + OBJ_GRID_TOP;
   image.Draw(draw_x, draw_y);
}
