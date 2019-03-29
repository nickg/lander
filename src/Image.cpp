//  Image.cpp -- Wrapper for loading and displaying images
//  Copyright (C) 2008-2019  Nick Gasson
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
   : m_texture(Texture::Load(fileName))
{
   const int width = GetWidth();
   const int height = GetHeight();

   const VertexI vertices[4] = {
      { -(width/2), -(height/2), 0.0f, 0.0f },
      { -(width/2), height/2, 0.0f, 1.0f },
      { width/2, height/2, 1.0f, 1.0f },
      { width/2, -height/2, 1.0f, 0.0f }
   };

   m_vbo = VertexBuffer::Make(vertices, 4);
}

void Image::Draw(int x, int y, float rotate, float scale,
                 float alpha, float white) const
{
   int width = GetWidth();
   int height = GetHeight();

   OpenGL& opengl = OpenGL::GetInstance();

   opengl.Reset();
   opengl.SetTexture(m_texture);
   opengl.SetColour(white, white, white, alpha);
   opengl.SetTranslation(x + width/2, y + height/2);
   opengl.SetScale(scale);
   opengl.SetRotation(rotate);
   opengl.Draw(m_vbo);
}

int Image::GetWidth() const
{
   return m_texture.GetWidth();
}

int Image::GetHeight() const
{
   return m_texture.GetHeight();
}
