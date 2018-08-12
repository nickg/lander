//  Image.cpp -- Wrapper for loading and displaying images
//  Copyright (C) 2008-2009  Nick Gasson
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
#include "Texture.hpp"

Image::Image(const string& fileName)
{
   m_texture = LoadTexture(fileName);

   glGenBuffers(1, &m_vbo);

   const int width = GetWidth();
   const int height = GetHeight();

   const int vertices[][2] = {
      { -(width/2), -(height/2) },
      { -(width/2), height/2 },
      { width/2, height/2 },
      { width/2, -height/2 }
   };

   glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

Image::~Image()
{
   glDeleteBuffers(1, &m_vbo);
}

void Image::Draw(int x, int y, double rotate, double scale,
                 double alpha, double white) const
{
   int width = GetWidth();
   int height = GetHeight();

   glEnable(GL_TEXTURE_2D);
   glEnable(GL_BLEND);
   glBindTexture(GL_TEXTURE_2D, m_texture->GetGLTexture());
   glLoadIdentity();
   glTranslated((double)(x + width/2), (double)(y + height/2), 0.0);
   glScaled(scale, scale, 0);
   glRotated(rotate, 0.0, 0.0, 1.0);
   glColor4d(white, white, white, alpha);

   #if 0
   glBegin(GL_QUADS);
   glTexCoord2d(0.0, 0.0); glVertex2i(-(width/2), -(height/2));
   glTexCoord2d(0.0, 1.0); glVertex2i(-(width/2), height/2);
   glTexCoord2d(1.0, 1.0); glVertex2i(width/2, height/2);
   glTexCoord2d(1.0, 0.0); glVertex2i(width/2, -(height/2));
   glEnd();
#else
   glDisable(GL_TEXTURE_2D);  // XXX

   OpenGL& opengl = OpenGL::GetInstance();

   opengl.Colour(white, white, white, alpha);
   opengl.Translate(x + width/2, y + height/2);
   opengl.Scale(scale);

   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
   glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 0, 0);
#if 1
   glDrawArrays(GL_QUADS, 0, 4);
#else
   GLubyte indices[] = {0,1,2, // first triangle (bottom left - top left - top right)
                     0,2,3}; // second triangle (bottom left - top right - bottom right)
   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
#endif
   glDisableVertexAttribArray(0);

#endif
}

int Image::GetWidth() const
{
   return m_texture->GetWidth();
}

int Image::GetHeight() const
{
   return m_texture->GetHeight();
}
