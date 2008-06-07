/*
 * FreeType.hpp -- A wrapper around FreeType.
 * Copyright (C) 2006  Nick Gasson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INC_FREETYPE_HPP
#define INC_FREETYPE_HPP

#include "Platform.hpp"

class Font {
public:
   Font(string filename, unsigned int h);
   ~Font();

   void Print(int x, int y, const char *fmt, ...);
   int GetStringWidth(const char *fmt, ...);
private:   
   int NextPowerOf2(int a);
   void MakeDisplayList(FT_Face face, char ch, GLuint listBase,
                        GLuint *texBase, unsigned short *widths);
   void SplitIntoLines(vector<string> &lines, const char *fmt, va_list ap);
   
   GLuint *textures;
   GLuint listBase;
   float height;
   unsigned short *widths;
   char *buf;

   static const int MAX_TXT_BUF = 1024;
   
   static int fontRefCount;
   static FT_Library library;
};


#endif
