//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "Key.hpp"
#include "OpenGL.hpp"
#include "Viewport.hpp"
#include "ObjectGrid.hpp"
#include "Ship.hpp"

#include <string>
#include <cassert>

Key::Key(bool active, int xpos, int ypos, ArrowColour acol)
   : StaticObject(xpos, ypos, 1, 1),
     active(active),
     image(KeyFileName(acol), 32, 32, KEY_FRAMES),
     arrow(ArrowFileName(acol))
{
   alpha = active ? 1.0 : 0.0;
}

void Key::DrawKey(Viewport* viewport)
{
   int draw_x = xpos*OBJ_GRID_SIZE - viewport->GetXAdjust();
   int draw_y = ypos*OBJ_GRID_SIZE - viewport->GetYAdjust() + OBJ_GRID_TOP;
   image.Draw(draw_x, draw_y, 0.0, 1.0, alpha);

   m_rotateAnim += KEY_ROTATION_SPEED * OpenGL::GetInstance().GetTimeScale();

   if (m_rotateAnim > 1.0f) {
      image.NextFrame();
      m_rotateAnim = 0.0f;
   }

   if (!active && alpha > 0.0f)
      alpha -= 0.02f;
}

void Key::DrawArrow(Viewport* viewport) const
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

void Key::DrawIcon(int offset, float minAlpha) const
{
   double drawAlpha = alpha > minAlpha ? alpha : minAlpha;
   image.DrawFrame(5, offset, 10, 0.0, 1.0, drawAlpha);
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
