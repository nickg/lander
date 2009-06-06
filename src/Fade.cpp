//  Fade.cpp -- Generic fade in/out effect.
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

#include "Fade.hpp"
#include "OpenGL.hpp"

#include <cassert>

const double Fade::DEFAULT_FADE_SPEED(0.05);

Fade::Fade(double s)
   : state(fNone), alpha(0.0), speed(s)
{
   
}

void Fade::BeginFadeIn()
{
   assert(state == fNone);

   state = fIn;
   alpha = 1.0;
}

void Fade::BeginFadeOut()
{
   assert(state == fNone);

   state = fOut;
   alpha = 0.0;
}

bool Fade::Process()
{
   switch (state) {
   case fOut:
      alpha += speed;
      if (alpha >= 1.0) {
         state = fNone;
         return true;
      }
      else
         return false;
   case fIn:
      alpha -= speed;
      if (alpha <= 0.0) {
         state = fNone;
         return true;
      }
      else
         return false;
   default:
      assert(false);
   }
}

void Fade::Display()
{
   double w = OpenGL::GetInstance().GetWidth();
   double h = OpenGL::GetInstance().GetHeight();

   glEnable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);
   glColor4d(0.0, 0.0, 0.0, alpha);
   glLoadIdentity();
   glBegin(GL_QUADS);
   glVertex3d(0.0, 0.0, 0.0);
   glVertex3d(0.0, h, 0.0);
   glVertex3d(w, h, 0.0);
   glVertex3d(w, 0.0, 0.0);
   glEnd();
}
