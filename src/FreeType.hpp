/*
 * FreeType.hpp - Definition of FreeType wrapper class.
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

/* 
 * Wrapper around the GNU FreeType font library. Code adapted from NeHe.
 * tutorial #43. 
 */
class FreeType 
{
    public:
    FreeType();
    ~FreeType();
	
    // Public methods
    static FreeType &GetInstance();
    void LoadFont(int index, string face, unsigned int h);
    void Print(int index, int x, int y, const char *fmt, ...);
    void PrintRotate(int index, int width, float x, float y, float xrot, float yrot, float zrot, const char *fmt, ...);
    int GetStringWidth(int index, const char *s);
    private:
    // Holds data about a single font
    struct FreeTypeFont 
    {
        float	height;			// The height of the font in pixels
        GLuint	*textures;		// Array of texture IDs
        GLuint	listBase;		// Display list base
        unsigned short *widths;	// The widths of each character
    };

    // Private methods
    int NextPowerOf2(int a);
    void MakeDisplayList(FT_Face face, char ch, GLuint listBase, GLuint *texBase, unsigned short *widths);
    void CreateFont(FreeTypeFont *font, string filename, unsigned int height);
    void DeleteFont(FreeTypeFont *font);

    FT_Library library;
    map<int, FreeTypeFont *> fonts; 
};

#endif
