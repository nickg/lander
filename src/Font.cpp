//
// Font.cpp -- A wrapper around FreeType.
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

#include "Font.hpp"
#include "OpenGL.hpp"

#include <string>
#include <stdexcept>
#include <cassert>

int Font::fontRefCount = 0;
FT_Library Font::library;

Font::Font(const string& filename, unsigned int h)
   : m_colour(Colour::WHITE)
{
   if (++fontRefCount == 1) {
      if (FT_Init_FreeType(&library))
         Die("FT_Init_FreeType failed");
   }

   buf = new char[MAX_TXT_BUF];
   height = (float)h;

   // Create the face
   FT_Face face;
   if (FT_New_Face(library, filename.c_str(), 0, &face))
      Die("FT_New_Face failed, file name: %s", filename.c_str());

   // FreeType measures font sizes in 1/64ths of a pixel...
   FT_Set_Char_Size(face, h<<6, h<<6, 96, 96);

   glGenTextures(1, &m_texture);

   const unsigned cellSize = NextPowerOf2(h * 2);
   const unsigned textureWidth = NextPowerOf2(cellSize * MAX_CHAR);
   const float normCellSize = 1.0f / MAX_CHAR;

   GLubyte *textureData = new GLubyte[2 * cellSize * textureWidth];
   Vertex<float> *vertexBuf = new Vertex<float>[MAX_CHAR * 4];

   // Generate the characters
   for (int i = 0; i < MAX_CHAR; i++) {
      // Load the character's glyph
      if (FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT))
         Die("FT_Load_Glyph failed");

      // Store the face's glyph in a glyph object
      FT_Glyph glyph;
      if (FT_Get_Glyph(face->glyph, &glyph))
         Die("FT_Get_Glyph failed");

      // Convert the glyph to a bitmap
      FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
      FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;

      // Get a reference to the bitmap
      FT_Bitmap& bitmap = bitmapGlyph->bitmap;

      assert(bitmap.width <= cellSize);
      assert(bitmap.rows <= cellSize);

      // Convert greyscale bitmap to lumiance and alpha channel
      for (unsigned y = 0; y < cellSize; y++) {
         for (unsigned x = 0; x < cellSize; x++) {
            const int offset = 2 * (x + i*cellSize + y*textureWidth);
            textureData[offset] = textureData[offset + 1] =
               (x >= bitmap.width || y >= bitmap.rows)
               ? 0
               : bitmap.buffer[x + bitmap.width*y];
         }
      }

      const float tx = i * normCellSize;
      const float tw = ((float)bitmap.width / (float)cellSize) * normCellSize;
      const float th = (float)bitmap.rows / (float)cellSize;

      // Insert some space between characters
      const float x = bitmapGlyph->left;

      // Move down a bit to accomodate characters such as p and q
      const float y = -bitmapGlyph->top;

      const Vertex<float> vertices[4] = {
         { x, y + bitmap.rows, tx, th},
         { x, y, tx, 0.0f },
         { x + bitmap.width, y, tx + tw, 0.0f },
         { x + bitmap.width, y + bitmap.rows, tx + tw, th }
      };

      copy(vertices, vertices + 4, vertexBuf + i * 4);

      m_widths[i] = face->glyph->advance.x >> 6;

      FT_Done_Glyph(glyph);
   }

   glBindTexture(GL_TEXTURE_2D, m_texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, cellSize, 0,
                GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, textureData);

   m_vbo = VertexBuffer::Make(vertexBuf, MAX_CHAR * 4);

   delete[] vertexBuf;
   delete[] textureData;

   // Free face data
   FT_Done_Face(face);
}

Font::~Font()
{
   glDeleteTextures(1, &m_texture);

   delete[] buf;

   if (--fontRefCount == 0) {
      FT_Done_FreeType(library);
   }
}

int Font::NextPowerOf2(unsigned a)
{
   a--;
   a |= a >> 1;
   a |= a >> 2;
   a |= a >> 4;
   a |= a >> 8;
   a |= a >> 16;
   a++;
   return a;
}

void Font::SplitIntoLines(vector<string> &lines, const char* fmt, va_list ap)
{
   if (fmt == NULL)
      *buf = 0;
   else
      vsnprintf(buf, MAX_TXT_BUF, fmt, ap);

   const char* start_line = buf, *c;
   for (c = buf; *c; c++) {
      if (*c == '\n')	{
         string line;
         for (const char* n = start_line; n < c; n++)
            line.append(1, *n);
         lines.push_back(line);
         start_line = c+1;
      }
   }

   if (start_line) {
      string line;
      for (const char* n = start_line; n < c; n++)
         line.append(1, *n);
      lines.push_back(line);
   }
}

void Font::Print(int x, int y, const char* fmt, ...)
{
   OpenGL& opengl = OpenGL::GetInstance();

   float h = height / 0.63f;   // Add some space between lines

   vector<string> lines;
   va_list ap;
   va_start(ap, fmt);
   SplitIntoLines(lines, fmt, ap);
   va_end(ap);

   opengl.Reset();
   opengl.Colour(m_colour);

   glBindTexture(GL_TEXTURE_2D, m_texture);

   OpenGL::BindVertexBuffer bind(m_vbo);

   // Draw the text
   const unsigned nlines = lines.size();
   for (unsigned i = 0; i < nlines; i++) {
      float offset = 0.0f;
      for (char ch : lines[i]) {
         if ((int)ch < MAX_CHAR) {
            opengl.Translate(x + offset, y - h*i);
            opengl.Draw(m_vbo, ch * 4, 4);

            offset += m_widths[(int)ch];
         }
      }
   }
}

int Font::GetStringWidth(const char* fmt, ...)
{
   va_list ap;
   vector<string> lines;

   va_start(ap, fmt);
   SplitIntoLines(lines, fmt, ap);
   va_end(ap);

   int maxlen = 0;
   for (const string& line : lines) {
      int len = 0;
      for (const char ch : line)
         len += m_widths[static_cast<int>(ch)];

      if (len > maxlen)
         maxlen = len;
   }

   return maxlen;
}

void Font::SetColour(float r, float g, float b, float a)
{
   m_colour = Colour::Make(r, g, b, a);
}
