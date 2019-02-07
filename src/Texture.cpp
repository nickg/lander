//  Texture.cpp -- Wrapper for OpenGL textures.
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

#include "Texture.hpp"
#include "OpenGL.hpp"

#include <map>
#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cassert>

#include <SDL_image.h>

using namespace std;

namespace {
   typedef map<string, TextureHolder*> TextureCache;
   TextureCache theCache;
}

class RefCounted {
public:
   RefCounted()
      : m_refcount(1)
   {}

   RefCounted(const RefCounted&) = delete;

   virtual ~RefCounted()
   {
      assert(m_refcount == 0);
   }

   void Ref()
   {
      assert(m_refcount > 0);
      m_refcount++;
   }

   void Unref()
   {
      assert(m_refcount > 0);
      if (--m_refcount == 0)
         delete this;
   }

private:
   int m_refcount;
};

class TextureHolder : public RefCounted {
public:
   TextureHolder(const string& file);
   TextureHolder(int width, int height, const GLubyte *data,
                 GLuint fmt, GLuint filter);
   TextureHolder(const TextureHolder&) = delete;
   ~TextureHolder();

   GLuint GetGLTexture() const { return m_texture; }
   int GetWidth() const { return m_width; }
   int GetHeight() const { return m_height; }

private:
   GLuint m_texture;
   int m_width, m_height;
};

static bool IsPowerOfTwo(int n)
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

TextureHolder::TextureHolder(const string& file)
{
   SDL_Surface* surface = IMG_Load(LocateResource(file).c_str());
   if (NULL == surface)
      Die("Failed to load image: %s", IMG_GetError());

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
   else
      Die("Unsupported image colour format: %s", file.c_str());

   m_width = surface->w;
   m_height = surface->h;

   glGenTextures(1, &m_texture);
   glBindTexture(GL_TEXTURE_2D, m_texture);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   // Load the surface's data into the texture
   glTexImage2D(GL_TEXTURE_2D, 0, ncols, surface->w, surface->h, 0,
                texture_format, GL_UNSIGNED_BYTE, surface->pixels);

   SDL_FreeSurface(surface);
}

TextureHolder::TextureHolder(int width, int height, const GLubyte *data,
                             GLuint fmt, GLuint filter)
{
   glGenTextures(1, &m_texture);
   glBindTexture(GL_TEXTURE_2D, m_texture);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                fmt, GL_UNSIGNED_BYTE, data);
}

TextureHolder::~TextureHolder()
{
   glDeleteTextures(1, &m_texture);
}

Texture Texture::Load(const string& fileName)
{
   TextureCache::iterator it = theCache.find(fileName);
   if (it != theCache.end()) {
      (*it).second->Ref();
      return Texture((*it).second);
   }
   else {
      TextureHolder *holder = new TextureHolder(fileName);
      holder->Ref();
      theCache[fileName] = holder;
      return Texture(holder);
   }
}

Texture Texture::Make(int width, int height, const GLubyte *data,
                      GLuint fmt, GLuint filter)
{
   return Texture(new TextureHolder(width, height, data, fmt, filter));
}

Texture::Texture(TextureHolder *holder)
   : m_holder(holder)
{
}

Texture::Texture(Texture&& other)
   : m_holder(other.m_holder)
{
   other.m_holder = nullptr;
}

Texture::Texture(const Texture& other)
   : m_holder(other.m_holder)
{
   if (m_holder != nullptr)
      m_holder->Ref();
}

Texture::~Texture()
{
   if (m_holder != nullptr)
      m_holder->Unref();
}

Texture& Texture::operator=(const Texture& other)
{
   if (m_holder != nullptr)
      m_holder->Unref();

   m_holder = other.m_holder;

   if (m_holder != nullptr)
      m_holder->Ref();

   return *this;
}

void Texture::Bind(int textureUnit)
{
   glActiveTexture(textureUnit);
   if (m_holder != nullptr)
      glBindTexture(GL_TEXTURE_2D, m_holder->GetGLTexture());
   else
      glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::GetGLTexture() const
{
   if (m_holder == nullptr)
      return 0;
   else
      return m_holder->GetGLTexture();
}

int Texture::GetWidth() const
{
   if (m_holder == nullptr)
      return 0;
   else
      return m_holder->GetWidth();
}

int Texture::GetHeight() const
{
   if (m_holder == nullptr)
      return 0;
   else
      return m_holder->GetHeight();
}
