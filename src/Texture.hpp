//  Texture.hpp -- Wrapper for OpenGL textures.
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

#ifndef INC_TEXTURE_HPP
#define INC_TEXTURE_HPP

#include "Platform.hpp"

class Texture {
public:
   Texture(const char* file);
   virtual ~Texture();

   GLuint GetGLTexture() const { return texture; }
   
   int GetWidth() const { return width; }
   int GetHeight() const { return height; }

   friend Texture* loadTexture(const string& fileName);
   
private:
   GLuint texture;
   int width, height;
   
   static bool IsPowerOfTwo(int n);
};

Texture* LoadTexture(const string& fileName);

#endif
