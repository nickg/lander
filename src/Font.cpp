//
// Copyright (C) 2006-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "Font.hpp"
#include "OpenGL.hpp"

#include <string>
#include <stdexcept>
#include <cassert>

int Font::fontRefCount = 0;
FT_Library Font::library;

Font::Font(const string& filename, unsigned int h)
   : m_height(h),
     m_colour(Colour::WHITE)
{
   if (++fontRefCount == 1) {
      if (FT_Init_FreeType(&library))
         Die("FT_Init_FreeType failed");
   }

   m_buf = new char[MAX_TXT_BUF];

   // Create the face
   FT_Face face;
   if (FT_New_Face(library, filename.c_str(), 0, &face))
      Die("FT_New_Face failed, file name: %s", filename.c_str());

   // FreeType measures font sizes in 1/64ths of a pixel...
   FT_Set_Char_Size(face, h<<6, h<<6, 96, 96);

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

   m_texture = Texture::Make(textureWidth, cellSize, textureData,
                             GL_LUMINANCE_ALPHA, GL_NEAREST);

   m_vbo = VertexBuffer::Make(vertexBuf, MAX_CHAR * 4);

   delete[] vertexBuf;
   delete[] textureData;

   // Free face data
   FT_Done_Face(face);
}

Font::~Font()
{
   delete[] m_buf;

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

int Font::SplitIntoLines(const char* fmt, va_list ap)
{
   if (fmt == NULL)
      m_buf[0] = '\0';
   else
      vsnprintf(m_buf, MAX_TXT_BUF, fmt, ap);

   int nlines = 1;
   for (char *p = m_buf; *p != '\0'; p++) {
      if (*p == '\n') {
         *p = '\0';
         nlines++;
      }
   }

   return nlines;
}

void Font::Print(int x, int y, const char* fmt, ...)
{
   OpenGL& opengl = OpenGL::GetInstance();

   float h = m_height / 0.63f;   // Add some space between lines

   va_list ap;
   va_start(ap, fmt);
   const int nlines = SplitIntoLines(fmt, ap);
   va_end(ap);

   opengl.Reset();
   opengl.SetColour(m_colour);
   opengl.SetTexture(m_texture);

   OpenGL::BindVertexBuffer bind(m_vbo);

   const char *p = m_buf;
   for (int i = 0; i < nlines; i++) {
      float offset = 0.0f;
      for (; *p != '\0'; p++) {
         if ((int)*p < MAX_CHAR) {
            opengl.SetTranslation(x + offset, y - h*i);
            opengl.Draw(m_vbo, *p * 4, 4);

            offset += m_widths[(int)*p];
         }
      }
      ++p;
   }
}

int Font::GetStringWidth(const char* fmt, ...)
{
   va_list ap;
   va_start(ap, fmt);
   const int nlines = SplitIntoLines(fmt, ap);
   va_end(ap);

   int maxlen = 0;
   const char *p = m_buf;
   for (int i = 0; i < nlines; i++) {
      int len = 0;
      for (; *p != '\0'; p++)
         len += m_widths[static_cast<int>(*p)];
      ++p;

      if (len > maxlen)
         maxlen = len;
   }

   return maxlen;
}

void Font::SetColour(float r, float g, float b, float a)
{
   m_colour = Colour::Make(r, g, b, a);
}
