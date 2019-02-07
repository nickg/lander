//
// FreeType.hpp -- A wrapper around FreeType.
// Copyright (C) 2006-2019  Nick Gasson
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#pragma once

#include "Platform.hpp"
#include "OpenGL.hpp"

#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

class Font {
public:
   Font(const string& filename, unsigned int h);
   Font(const Font&) = delete;
   ~Font();

   void SetColour(float r, float g, float b, float a=1.0f);
   void Print(int x, int y, const char* fmt, ...);
   int GetStringWidth(const char* fmt, ...);
private:
   int NextPowerOf2(unsigned a);
   int SplitIntoLines(const char* fmt, va_list ap);

   static const int MAX_CHAR = 128;
   static const int MAX_TXT_BUF = 1024;

   VertexBuffer m_vbo;
   GLuint       m_texture;
   const float  m_height;
   unsigned     m_widths[MAX_CHAR];
   char        *m_buf;
   Colour       m_colour;

   static int fontRefCount;
   static FT_Library library;
};
