//
// Copyright (C) 2006-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
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
   Texture      m_texture;
   const float  m_height;
   unsigned     m_widths[MAX_CHAR];
   char        *m_buf;
   Colour       m_colour;

   static int fontRefCount;
   static FT_Library library;
};
