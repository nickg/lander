//  Key.cpp -- A key the player has to collect.
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

#include "Key.hpp"
#include "OpenGL.hpp"
#include "Viewport.hpp"
#include "ObjectGrid.hpp"
#include "Ship.hpp"

Key::Key(bool active, int xpos, int ypos, ArrowColour acol)
   : StaticObject(xpos, ypos, 1, 1),
     active(active),
     image(KeyFileName(acol), 32, 32, KEY_FRAMES),
     arrow(ArrowFileName(acol))
{  
   alpha = active ? 1.0 : 0.0;

   rotcount = KEY_ROTATION_SPEED;
}

void Key::DrawKey(Viewport* viewport)
{
   int draw_x = xpos*OBJ_GRID_SIZE - viewport->GetXAdjust();
   int draw_y = ypos*OBJ_GRID_SIZE - viewport->GetYAdjust() + OBJ_GRID_TOP;
   image.Draw(draw_x, draw_y, 0.0, 1.0, alpha);
   if (--rotcount == 0) {
      image.NextFrame();
      rotcount = KEY_ROTATION_SPEED;
   }

   if (!active && alpha > 0.0f)
      alpha -= 0.02f;
}

void Key::DrawArrow(Viewport* viewport)
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

      arrow.Draw(ax, ay, angle);
   }
}

void Key::DrawIcon(int offset, float minAlpha)
{
   const int prevFrame = image.GetFrame();
   image.SetFrame(5);
   
   double drawAlpha = alpha > minAlpha ? alpha : minAlpha;
   image.Draw(offset, 10, 0.0, 1.0, drawAlpha);

   image.SetFrame(prevFrame);   
}

bool Key::CheckCollision(Ship& ship) const
{
   bool collide = ship.BoxCollision
      (xpos*OBJ_GRID_SIZE + 3,
       ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP + 3,
       OBJ_GRID_SIZE - 6,
       OBJ_GRID_SIZE - 6);
   return active && collide;
}

string Key::KeyFileName(ArrowColour col)
{
   switch (col) {
   case acBlue:
      return "images/keyblue.png";
   case acRed:
      return "images/keyred.png";
   case acYellow:
      return "images/keyyellow.png";
   case acPink:
      return "images/keypink.png";
   case acGreen:
      return "images/keygreen.png";
   default:
      assert(false);
   }
}

string Key::ArrowFileName(ArrowColour col)
{
   switch (col) {
   case acBlue:
      return "images/arrowblue.png";
   case acRed:
      return "images/arrowred.png";
   case acYellow:
      return "images/arrowyellow.png";
   case acPink:
      return "images/arrowpink.png";
   case acGreen:
      return "images/arrowgreen.png";
   default:
      assert(false);
   }
}
