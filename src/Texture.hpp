//  Texture.hpp -- Wrapper for OpenGL textures.
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

#pragma once

#include "Platform.hpp"

class TextureHolder;

class Texture {
public:
   Texture() = default;
   Texture(const Texture&) = delete;
   Texture(Texture&&);
   ~Texture();

   Texture& operator=(Texture&& other);

   void Bind(int textureUnit=0);

   GLuint GetGLTexture() const;

   int GetWidth() const;
   int GetHeight() const;

   static Texture Load(const string& fileName);
   static Texture Make(int width, int height, const GLubyte *data,
                       GLuint fmt, GLuint filter=GL_LINEAR);

private:
   Texture(TextureHolder *holder, bool owner);

   TextureHolder *m_holder = nullptr;
   bool m_owner = false;
};
