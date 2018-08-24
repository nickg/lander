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

   const float width = GetWidth();
   const float height = GetHeight();

   const float vertices[][4] = {
      { -(width/2), -(height/2), 0.0f, 0.0f },
      { -(width/2), height/2, 0.0f, 1.0f },
      { width/2, height/2, 1.0f, 1.0f },
      { width/2, -height/2, 1.0f, 0.0f }
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

   glBindTexture(GL_TEXTURE_2D, m_texture->GetGLTexture());
   glLoadIdentity();
   glTranslated((double)(x + width/2), (double)(y + height/2), 0.0);
   glScaled(scale, scale, 0);
   glRotated(rotate, 0.0, 0.0, 1.0);
   glColor4d(white, white, white, alpha);

   OpenGL& opengl = OpenGL::GetInstance();

   opengl.Colour(white, white, white, alpha);
   opengl.Translate(x + width/2, y + height/2);
   opengl.Scale(scale);

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                         reinterpret_cast<const void *>(2 * sizeof(float)));
   glDrawArrays(GL_QUADS, 0, 4);
   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
}

int Image::GetWidth() const
{
   return m_texture->GetWidth();
}

int Image::GetHeight() const
{
   return m_texture->GetHeight();
}
