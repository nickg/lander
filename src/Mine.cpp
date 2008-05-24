/*  Mine.cpp -- Floating space mine things.
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

#include "Mine.hpp"
#include "OpenGL.hpp"
#include "DataFile.hpp"
#include "LoadOnce.hpp"

extern DataFile *g_pData;

GLuint Mine::uMineTexture[Mine::MINE_FRAME_COUNT];

Mine::Mine(ObjectGrid *o, Viewport *v, int x, int y)
   : objgrid(o), viewport(v)
{
   LOAD_ONCE {
      OpenGL &opengl = OpenGL::GetInstance();
      const int TEX_NAME_LEN = 128;
      char buf[TEX_NAME_LEN];
      
      for (int i = 0; i < MINE_FRAME_COUNT; i++) {
         snprintf(buf, TEX_NAME_LEN, "mine%d.bmp", i*5);
         uMineTexture[i] = opengl.LoadTextureAlpha(g_pData, buf);
      }
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

   // Allocate frame images
   for (int j = 0; j < 36; j++) {
      frame[j].width = OBJ_GRID_SIZE*2;
      frame[j].height = OBJ_GRID_SIZE*2;
      frame[j].uTexture = uMineTexture[j];
   }

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
      }
      movedelay = MINE_MOVE_SPEED;
   }
}

bool Mine::CheckCollision(Ship &ship)
{
   return ship.BoxCollision
      (xpos*OBJ_GRID_SIZE + 3 + displace_x,
       ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP + 6 + displace_y,
       OBJ_GRID_SIZE*2 - 6,
       OBJ_GRID_SIZE*2 - 12); 
}

void Mine::Draw()
{
   frame[current].x = xpos*OBJ_GRID_SIZE + displace_x
      - viewport->GetXAdjust();
   frame[current].y = ypos*OBJ_GRID_SIZE + displace_y
      - viewport->GetYAdjust() + OBJ_GRID_TOP;		
   OpenGL::GetInstance().Draw(&frame[current]);
   if (--rotcount == 0) {
      if (++current == 18)
         current = 0;
      rotcount = MINE_ROTATION_SPEED;
   }
}
