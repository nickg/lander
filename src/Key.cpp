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
#include "LoadOnce.hpp"

extern DataFile *g_pData;

GLuint Key::uBlueArrow, Key::uPinkArrow, Key::uRedArrow;
GLuint Key::uYellowArrow, Key::uGreenArrow;

AnimatedImage *Key::blueImage = NULL;
AnimatedImage *Key::redImage = NULL;
AnimatedImage *Key::greenImage = NULL;
AnimatedImage *Key::yellowImage = NULL;
AnimatedImage *Key::pinkImage = NULL;
Image *Key::blueArrow = NULL;
Image *Key::redArrow = NULL;
Image *Key::greenArrow = NULL;
Image *Key::yellowArrow = NULL;
Image *Key::pinkArrow = NULL;

Key::Key(bool active, int xpos, int ypos, ArrowColour acol)
   : StaticObject(xpos, ypos, 1, 1),
     active(active)
{
   // Static initialisation
   LOAD_ONCE {
      OpenGL &opengl = OpenGL::GetInstance();

      blueImage = new AnimatedImage("images/keyblue.png", 32, KEY_FRAMES);
      redImage = new AnimatedImage("images/keyred.png", 32, KEY_FRAMES);
      greenImage = new AnimatedImage("images/keygreen.png", 32, KEY_FRAMES);
      yellowImage = new AnimatedImage("images/keyyellow.png", 32, KEY_FRAMES);
      pinkImage = new AnimatedImage("images/keypink.png", 32, KEY_FRAMES);

      blueArrow = new Image("images/arrowblue.png");
      redArrow = new Image("images/arrowred.png");
      greenArrow = new Image("images/arrowgreen.png");
      yellowArrow = new Image("images/arrowyellow.png");
      pinkArrow = new Image("images/arrowpink.png");
      
      uBlueArrow = opengl.LoadTextureAlpha(g_pData, "BlueArrow.bmp");
      uRedArrow = opengl.LoadTextureAlpha(g_pData, "RedArrow.bmp");
      uGreenArrow = opengl.LoadTextureAlpha(g_pData, "GreenArrow.bmp");
      uYellowArrow = opengl.LoadTextureAlpha(g_pData, "YellowArrow.bmp");
      uPinkArrow = opengl.LoadTextureAlpha(g_pData, "PinkArrow.bmp");
   }
   
   alpha = active ? 1.0 : 0.0;

   current = 0;
   rotcount = KEY_ROTATION_SPEED;

   // Allocate arrow images
   arrow.width = OBJ_GRID_SIZE;
   arrow.height = OBJ_GRID_SIZE;
   switch (acol) {
   case acBlue:
      arrow.uTexture = uBlueArrow;
      image = blueImage;
      break;
   case acRed:
      arrow.uTexture = uRedArrow;
      image = redImage;
      break;
   case acYellow:
      arrow.uTexture = uYellowArrow;
      image = yellowImage;
      break;
   case acPink:
      arrow.uTexture = uPinkArrow;
      image = pinkImage;
      break;
   case acGreen:
      arrow.uTexture = uGreenArrow;
      image = greenImage;
      break;
   }
}

void Key::DrawKey(Viewport *viewport)
{
   int draw_x = xpos*OBJ_GRID_SIZE - viewport->GetXAdjust();
   int draw_y = ypos*OBJ_GRID_SIZE - viewport->GetYAdjust() + OBJ_GRID_TOP;
   image->Draw(draw_x, draw_y, 0.0, 1.0, alpha);
   if (--rotcount == 0) {
      image->NextFrame();
      rotcount = KEY_ROTATION_SPEED;
   }

   if (!active && alpha > 0.0f)
      alpha -= 0.02f;
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
      else if (ax + ARROW_SIZE > screenWidth) { 
         ax = screenWidth - ARROW_SIZE;
         angle = 270;
      }
      if (ay < 0) { 
         ay = 0;
         angle = 180;
      }
      else if (ay + ARROW_SIZE > screenHeight) { 
         ay = screenHeight - ARROW_SIZE;
         angle = 0;
      }
      
      arrow.x = ax;
      arrow.y = ay;
			
      OpenGL::GetInstance().DrawRotate(&arrow, angle);
   }
}

void Key::DrawIcon(int offset, float minAlpha)
{
   int prevFrame = image->GetFrame();
   image->SetFrame(5);
   
   double drawAlpha = alpha > minAlpha ? alpha : minAlpha;
   image->Draw(offset, 10, 0.0, 1.0, drawAlpha);

   image->SetFrame(prevFrame);   
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
