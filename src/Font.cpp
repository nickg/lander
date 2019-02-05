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

int Font::fontRefCount = 0;
FT_Library Font::library;

// TODO
//  * Just one big texture
//  * Use ints for VBO instead of float


Font::Font(const string& filename, unsigned int h)
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

   glGenBuffers(1, &m_vbo);
   glGenTextures(MAX_CHAR, m_textures);

   // Generate the characters
   float *vertexBuf = new float[MAX_CHAR * VERTEX_SIZE * 4];
   for (int i = 0; i < MAX_CHAR; i++)
      MakeDisplayList(face, i, vertexBuf + i * VERTEX_SIZE * 4);

   glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_CHAR * VERTEX_SIZE * 4,
                vertexBuf, GL_STATIC_DRAW);

   delete[] vertexBuf;

   // Free face data
   FT_Done_Face(face);
}

Font::~Font()
{
   glDeleteBuffers(1, &m_vbo);
   glDeleteTextures(128, m_textures);

   delete[] buf;

   if (--fontRefCount == 0) {
      FT_Done_FreeType(library);
   }
}

int Font::NextPowerOf2(int a)
{
   int rval = 1;

   while (rval < a)
      rval <<= 1;

   return rval;
}

void Font::MakeDisplayList(FT_Face face, char ch, float *vertexBuf)
{
   // Load the character's glyph
   if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT))
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

   // Make the width and height a power of 2
   const unsigned width = NextPowerOf2(bitmap.width);
   const unsigned height = NextPowerOf2(bitmap.rows);

   // Allocate memory for the texture data
   GLubyte* expandedData = new GLubyte[2 * width * height];

   // Fill in the bitmap's extended data
   for (unsigned j = 0; j < height; j++) {
      for (unsigned i = 0; i < width; i++) {
         expandedData[2*(i+j*width)] = expandedData[2*(i+j*width)+1] =
            (i >= bitmap.width || j >= bitmap.rows)
            ? 0
            : bitmap.buffer[i + bitmap.width*j];
      }
   }

   // Set texture parameters
   glBindTexture(GL_TEXTURE_2D, m_textures[(int)ch]);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   // Create the texture
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expandedData);

   // Free expanded data
   delete[] expandedData;

   const float tx = (float)bitmap.width / (float)width;
   const float ty = (float)bitmap.rows / (float)height;

   // Insert some space between characters
   const float x = bitmapGlyph->left;

   // Move down a bit to accomodate characters such as p and q
   const float y = -bitmapGlyph->top;

   const float vertices[4 * VERTEX_SIZE] = {
      x, y + bitmap.rows, 0.0f, ty,
      x, y, 0.0f, 0.0f,
      x + bitmap.width, y, tx, 0.0f,
      x + bitmap.width, y + bitmap.rows, tx, ty
   };

   copy(vertices, vertices + 4 * VERTEX_SIZE, vertexBuf);

   m_widths[(int)ch] = (short)face->glyph->advance.x >> 6;

   FT_Done_Glyph(glyph);
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

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                         reinterpret_cast<const void *>(2 * sizeof(float)));

   // Draw the text
   const unsigned nlines = lines.size();
   for (unsigned i = 0; i < nlines; i++) {
      float offset = 0.0f;
      for (char ch : lines[i]) {
         if ((int)ch < MAX_CHAR) {
            opengl.Translate(x + offset, y - h*i);
            glBindTexture(GL_TEXTURE_2D, m_textures[(int)ch]);
            glDrawArrays(GL_QUADS, ch * 4, 4);

            offset += m_widths[(int)ch];
         }
      }
   }

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
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
