//  Texture.cpp -- Wrapper for OpenGL textures.
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

#include "Texture.hpp"
#include "OpenGL.hpp"

#include <SDL_image.h>

Texture::Texture(const char* file)
{
   SDL_Surface* surface = IMG_Load(LocateResource(file));
   if (NULL == surface) {
      ostringstream os;
      os << "Failed to load image: " << IMG_GetError();
      throw runtime_error(os.str());
   }

   if (!IsPowerOfTwo(surface->w))
      cerr << "Warning: " << file << " width not a power of 2" << endl;
   if (!IsPowerOfTwo(surface->h))
      cerr << "Warning: " << file << " height not a power of 2" << endl;

   if (!OpenGL::GetInstance().IsTextureSizeSupported(surface->w, surface->h))
      cerr << "Warning: " << file << " bigger than max OpenGL texture" << endl;

   int ncols = surface->format->BytesPerPixel;
   GLenum texture_format;
   if (ncols == 4) {
      // Contains an alpha channel
      if (surface->format->Rmask == 0x000000ff)
         texture_format = GL_RGBA;
      else
         texture_format = GL_BGRA;
   }
   else if (ncols == 3) {
      if (surface->format->Rmask == 0x000000ff)
         texture_format = GL_RGB;
      else
         texture_format = GL_BGR;
   }
   else {
      ostringstream os;
      os << "Unsupported image colour format: " << file;
      throw runtime_error(os.str());
   }

   width = surface->w;
   height = surface->h;
   
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   // Load the surface's data into the texture
   glTexImage2D(GL_TEXTURE_2D, 0, ncols, surface->w, surface->h, 0,
                texture_format, GL_UNSIGNED_BYTE, surface->pixels);

   SDL_FreeSurface(surface);
}

Texture::~Texture()
{
   glDeleteTextures(1, &texture);
}


bool Texture::IsPowerOfTwo(int n)
{
   int pop = 0;
   for (unsigned i = 0, bit = 1;
        i < sizeof(int)*8;
        i++, bit <<= 1) {
      if (n & bit)
         pop++;
   }
   return pop == 1;
}
