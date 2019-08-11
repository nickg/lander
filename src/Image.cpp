//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
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
