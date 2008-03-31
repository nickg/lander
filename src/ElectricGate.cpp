/*  ElectricGate.cpp -- Electric gateway thingys.
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

#include "ElectricGate.hpp"
#include "DataFile.hpp"

extern DataFile *g_pData;

GLuint ElectricGate::uGatewayTexture = 0;

ElectricGate::ElectricGate(Viewport *v, int length, bool vertical, int x, int y)
   : length(length), vertical(vertical), viewport(v)
{
   xpos = x;
   ypos = y;
   
   icon.width = OBJ_GRID_SIZE;
   icon.height = OBJ_GRID_SIZE;
   icon.uTexture = uGatewayTexture;
   timer = rand() % 70 + 10;
}

void ElectricGate::Load()
{
   OpenGL &opengl = OpenGL::GetInstance();
   uGatewayTexture = opengl.LoadTextureAlpha(g_pData, "Gateway.bmp");
}

bool ElectricGate::CheckCollision(Ship &ship)
{
   int dx = vertical ? 0 : length;
   int dy = vertical ? length : 0;
   if (timer > GATEWAY_ACTIVE) {
      bool collide1 = ship.BoxCollision
         (xpos*OBJ_GRID_SIZE,
          ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP,
          OBJ_GRID_SIZE,
          OBJ_GRID_SIZE);
			
      bool collide2 = ship.BoxCollision
         ((xpos + dx)*OBJ_GRID_SIZE,
          (ypos + dy)*OBJ_GRID_SIZE + OBJ_GRID_TOP,
          OBJ_GRID_SIZE,
          OBJ_GRID_SIZE);
		
      return collide1 || collide2;
   }
   else {
      return ship.BoxCollision
         (xpos*OBJ_GRID_SIZE, 
          ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP,
          (dx + 1)*OBJ_GRID_SIZE,
          (dy + 1)*OBJ_GRID_SIZE); 
   }
}

void ElectricGate::Draw()
{
   OpenGL &opengl = OpenGL::GetInstance();
   
   // Draw first sphere
   icon.x = xpos*OBJ_GRID_SIZE - viewport->GetXAdjust();
   icon.y = ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP - viewport->GetYAdjust();
   opengl.Draw(&icon);

   // Draw second sphere
   if (vertical) {
      icon.x = xpos*OBJ_GRID_SIZE - viewport->GetXAdjust();
      icon.y = (ypos+length)*OBJ_GRID_SIZE + OBJ_GRID_TOP - viewport->GetYAdjust();
   }
   else {
      icon.x = (xpos+length)*OBJ_GRID_SIZE - viewport->GetXAdjust();
      icon.y = ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP - viewport->GetYAdjust();
   }
   opengl.Draw(&icon);

   // Draw the electricity stuff
   if (--timer < GATEWAY_ACTIVE) { 
      int x, y, deviation;
      
      opengl.DisableTexture();
      
      for (int j = 0; j < 10; j++) {
         deviation = 0;
         for (int k = 0; k < length; k++) {
            glLoadIdentity();
            glBegin(GL_LINE_STRIP);
            float r = 0.0f + (float)(rand()%5)/10.0f;
            float g = 0.0f + (float)(rand()%5)/10.0f;
            float b = 1.0f - (float)(rand()%5)/10.0f;
            float a = 1.0f - (float)(rand()%5)/10.0f;
            
            glColor4f(r, g, b, a);
            if (vertical) {
               x = xpos*OBJ_GRID_SIZE + 16 + deviation - viewport->GetXAdjust();
               y = (ypos+k)*OBJ_GRID_SIZE + OBJ_GRID_TOP + 16 - viewport->GetYAdjust();
               glVertex2i(x, y);
               if (k == length-1)
                  deviation = 0;
               else
                  deviation += rand()%20 - 10;
               x = xpos*OBJ_GRID_SIZE + 16 + deviation - viewport->GetXAdjust();
               y += OBJ_GRID_SIZE;
               glVertex2i(x, y);
            }
            else {
               x = (xpos+k)*OBJ_GRID_SIZE + 16 - viewport->GetXAdjust();
               y = ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP + 16 + deviation
                  - viewport->GetYAdjust();
               glVertex2i(x, y);
               if (k == length-1)
                  deviation = 0;
               else
                  deviation += rand()%20 - 10;
               y = ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP + 16 + deviation
                  - viewport->GetYAdjust();
               x += OBJ_GRID_SIZE;
               glVertex2i(x, y);
            }
            glEnd();
         }
      }
      
      // Reset timer 
      if (timer < 0)
         timer = 100;
   }
}


