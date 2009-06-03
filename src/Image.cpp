//  Image.cpp -- Wrapper for loading and displaying images
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

#include "Image.hpp"
#include "OpenGL.hpp"

Image::Image(const string& file)
   : Texture(file)
{
   
}

Image::~Image()
{
   
}

void Image::Draw(int x, int y, double rotate, double scale,
                 double alpha, double white) const
{
   int width = GetWidth();
   int height = GetHeight();
   
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_BLEND);
   glBindTexture(GL_TEXTURE_2D, GetGLTexture());
   glLoadIdentity();
   glTranslated((double)(x + width/2), (double)(y + height/2), 0.0);
   glScaled(scale, scale, 0);
   glRotated(rotate, 0.0, 0.0, 1.0);
   glColor4d(white, white, white, alpha);
   glBegin(GL_QUADS);
   glTexCoord2d(0.0, 0.0); glVertex2i(-(width/2), -(height/2));
   glTexCoord2d(0.0, 1.0); glVertex2i(-(width/2), height/2);
   glTexCoord2d(1.0, 1.0); glVertex2i(width/2, height/2);
   glTexCoord2d(1.0, 0.0); glVertex2i(width/2, -(height/2));
   glEnd();
}

