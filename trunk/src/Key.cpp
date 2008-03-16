/*  Key.cpp -- A key the player has to collect.
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

#include "Key.hpp"
#include "OpenGL.hpp"
#include "DataFile.hpp"

extern DataFile *g_pData;

GLuint Key::uBlueKey[KEY_FRAMES], Key::uRedKey[KEY_FRAMES];
GLuint Key::uGreenKey[KEY_FRAMES], Key::uPinkKey[KEY_FRAMES];
GLuint Key::uYellowKey[KEY_FRAMES];
GLuint Key::uBlueArrow, Key::uPinkArrow, Key::uRedArrow;
GLuint Key::uYellowArrow, Key::uGreenArrow;

Key::Key()
{
   Reset(false, 0, 0, acBlue);
}

void Key::Load()
{
   const int TEX_NAME_LEN = 128;
   char buf[TEX_NAME_LEN];

   OpenGL &opengl = OpenGL::GetInstance();
   
   uBlueArrow = opengl.LoadTextureAlpha(g_pData, "BlueArrow.bmp");
   uRedArrow = opengl.LoadTextureAlpha(g_pData, "RedArrow.bmp");
   uGreenArrow = opengl.LoadTextureAlpha(g_pData, "GreenArrow.bmp");
   uYellowArrow = opengl.LoadTextureAlpha(g_pData, "YellowArrow.bmp");
   uPinkArrow = opengl.LoadTextureAlpha(g_pData, "PinkArrow.bmp");
      
   for (int i = 0; i < KEY_FRAMES; i++) {
      snprintf(buf, TEX_NAME_LEN, "keyblue%.2d.bmp", i);
      uBlueKey[i] = opengl.LoadTextureAlpha(g_pData, buf);
      
      snprintf(buf, TEX_NAME_LEN, "keygreen%.2d.bmp", i);
      uGreenKey[i] = opengl.LoadTextureAlpha(g_pData, buf);
         
      snprintf(buf, TEX_NAME_LEN, "keyred%.2d.bmp", i);
      uRedKey[i] = opengl.LoadTextureAlpha(g_pData, buf);
      
      snprintf(buf, TEX_NAME_LEN, "keyyellow%.2d.bmp", i);
      uYellowKey[i] = opengl.LoadTextureAlpha(g_pData, buf);
      
      snprintf(buf, TEX_NAME_LEN, "keypink%.2d.bmp", i);
      uPinkKey[i] = opengl.LoadTextureAlpha(g_pData, buf);
   }
}

void Key::Reset(bool active, int xpos, int ypos, ArrowColour acol)
{
   this->active = active;
   this->xpos = xpos;
   this->ypos = ypos;

   if (active)
      alpha = 1.0f;
   else
      alpha = 0.0f;

   current = 0;
   rotcount = KEY_ROTATION_SPEED;

   // Allocate arrow images
   arrow.width = 32;
   arrow.height = 32;
   switch (acol) {
   case acBlue:
      arrow.uTexture = uBlueArrow;
      break;
   case acRed:
      arrow.uTexture = uRedArrow;
      break;
   case acYellow:
      arrow.uTexture = uYellowArrow;
      break;
   case acPink:
      arrow.uTexture = uPinkArrow;
      break;
   case acGreen:
      arrow.uTexture = uGreenArrow;
      break;
   }

   // Allocate frame images
   for (int j = 0; j < KEY_FRAMES; j++) {
      frame[j].width = OBJ_GRID_SIZE;
      frame[j].height = OBJ_GRID_SIZE;

      switch (acol) {
      case acBlue:
         frame[j].uTexture = uBlueKey[j];
         break;
      case acRed:
         frame[j].uTexture = uRedKey[j];
         break;
      case acYellow:
         frame[j].uTexture = uYellowKey[j];
         break;
      case acPink:
         frame[j].uTexture = uPinkKey[j];
         break;
      case acGreen:
         frame[j].uTexture = uGreenKey[j];
         break;
      }
   }
}

void Key::DrawKey(Viewport *viewport)
{
   if (active) {
      frame[current].x = xpos*OBJ_GRID_SIZE - viewport->GetXAdjust();
      frame[current].y = ypos*OBJ_GRID_SIZE - viewport->GetYAdjust() + OBJ_GRID_TOP;		
      OpenGL::GetInstance().Draw(&frame[current]);
      if (--rotcount == 0) {
         if (++current == 18)
            current = 0;
         rotcount = KEY_ROTATION_SPEED;
      }
   }
   else {
      if (alpha > 0.0f) {
         frame[current].x = xpos*OBJ_GRID_SIZE - viewport->GetXAdjust();
         frame[current].y = ypos*OBJ_GRID_SIZE - viewport->GetYAdjust() + OBJ_GRID_TOP;	
         OpenGL::GetInstance().DrawBlend(&frame[current], alpha);
         alpha -= 0.02f;
         if (--rotcount == 0) {
            if (++current == 18)
               current = 0;
            rotcount = KEY_ROTATION_SPEED;
         }
      }
   }
}

void Key::DrawArrow(Viewport *viewport)
{
   if (active && !ObjectInScreen(viewport))	{
      int ax = xpos*OBJ_GRID_SIZE - viewport->GetXAdjust();
      int ay = ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP - viewport->GetYAdjust();
      float angle = 0.0;

      int screenWidth = OpenGL::GetInstance().GetWidth();
      int screenHeight = OpenGL::GetInstance().GetHeight();
      
      if (ax < 0) { 
         ax = 0; 
         angle = 90;
      }
      else if (ax + 32 > screenWidth) { 
         ax = screenWidth - 32;
         angle = 270;
      }
      if (ay < 0) { 
         ay = 0;
         angle = 180;
      }
      else if (ay + 32 > screenHeight) { 
         ay = screenHeight - 32;
         angle = 0;
      }
      
      arrow.x = ax;
      arrow.y = ay;
			
      OpenGL::GetInstance().DrawRotate(&arrow, angle);
   }
}

void Key::DrawIcon(int offset, float minAlpha)
{
   frame[5].x = offset;
   frame[5].y = 10;
   if (active)
      OpenGL::GetInstance().Draw(&frame[5]);
   else
      OpenGL::GetInstance().DrawBlend(&frame[5], alpha > minAlpha ? alpha : minAlpha);
}

bool Key::CheckCollision(Ship &ship) const
{
   bool collide = ship.BoxCollision
      (xpos*OBJ_GRID_SIZE + 3,
       ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP + 3,
       OBJ_GRID_SIZE - 6,
       OBJ_GRID_SIZE - 6);
   return active && collide;
}
