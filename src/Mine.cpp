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
#include "LoadOnce.hpp"

AnimatedImage* Mine::image = NULL;

Mine::Mine(ObjectGrid* o, Viewport* v, int x, int y)
   : objgrid(o), viewport(v)
{
   LOAD_ONCE {
      image = new AnimatedImage("images/mine.png", 64, 64, MINE_FRAME_COUNT);
   }
   
   current = 0;
   rotcount = MINE_ROTATION_SPEED;
   displace_x = 0;
   displace_y = 0;
   movedelay = MINE_MOVE_SPEED;
   dir = dirNone;
   movetimeout = 1;

   xpos = x;
   ypos = y;

   // Free space on object grid
   objgrid->UnlockSpace(xpos, ypos);
   objgrid->UnlockSpace(xpos + 1, ypos);
   objgrid->UnlockSpace(xpos + 1, ypos + 1);
   objgrid->UnlockSpace(xpos, ypos + 1);
}

void Mine::Move()
{
   if (displace_x % ObjectGrid::OBJ_GRID_SIZE == 0
       && displace_y % ObjectGrid::OBJ_GRID_SIZE == 0) {
      
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
      displace_x = 0;
      displace_y = 0;
      movedelay = 1;
         
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
         ok = nextx >= objgrid->GetWidth() || nextx < 0 
            || nexty > objgrid->GetHeight() || nexty < 0 
            || objgrid->IsFilled(nextx, nexty)
            || objgrid->IsFilled(nextx + 1, nexty)
            || objgrid->IsFilled(nextx + 1, nexty + 1)
            || objgrid->IsFilled(nextx, nexty + 1);
         ok = !ok;
         timeout--;
      } while (!ok && timeout > 0);
         
      if (timeout == 0)
         dir = dirNone;				
   }
      
   if (--movedelay == 0) {
      switch(dir) {
      case dirUp: displace_y--; break;
      case dirDown: displace_y++; break;
      case dirLeft: displace_x--; break;
      case dirRight: displace_x++; break;
      default: break;
      }
      movedelay = MINE_MOVE_SPEED;
   }

   if (--rotcount == 0) {
      current = (current + 1) % MINE_FRAME_COUNT;
      rotcount = MINE_ROTATION_SPEED;
   }
}

bool Mine::CheckCollision(Ship& ship)
{
   return ship.BoxCollision
      (xpos*OBJ_GRID_SIZE + 3 + displace_x,
       ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP + 6 + displace_y,
       OBJ_GRID_SIZE*2 - 6,
       OBJ_GRID_SIZE*2 - 12); 
}

void Mine::Draw() const
{
   int draw_x = xpos*OBJ_GRID_SIZE + displace_x
      - viewport->GetXAdjust();
   int draw_y = ypos*OBJ_GRID_SIZE + displace_y
      - viewport->GetYAdjust() + OBJ_GRID_TOP;
   image->SetFrame(current);
   image->Draw(draw_x, draw_y);
}
