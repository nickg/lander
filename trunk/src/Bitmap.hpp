/*
 * Bitmap.hpp - Definition of Bitmap wrapper class.
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

#ifndef INC_BITMAP_HPP
#define INC_BITMAP_HPP

#include "Platform.hpp"
#include "File.hpp"

/*
 * A cross platform loader for Windows bitmap files. This is used by 
 * the DataFile class to load images.
 */
class Bitmap
{
public:
	Bitmap(File *f);
	~Bitmap();

	bool HasLoaded() const { return loaded; }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	int GetDepth() const { return depth; }
	unsigned char *GetData() const { return data; }

private:

	// Constants
	static const short BITMAP_MAGIC_NUMBER = 19778;
	static const short RGB_BYTE_SIZE = 3;
	
#pragma pack(2)

	// The header present in every bitmap file
	struct BitmapFileHeader
	{
		uint16_t magic;		// Should be BITMAP_MAGIC_NUMBER
		uint32_t filesz;	// Length of file in bytes
		uint16_t reserved1;	// Not used
		uint16_t reserved2;	// Not used
		uint32_t dataoff;	// Start of bitmap data
	};
	
	// Defines colours and dimensions
	struct BitmapInfoHeader
	{
		uint32_t size;		// Size of this header
		uint32_t width;		// Width of image in pixels
		uint32_t height;	// Height of image in pixels
		uint16_t planes;	// Must be 1
		uint16_t bitcount;	// Must be 1, 4, 8, or 24
		uint32_t compress;	// Ignored
		uint32_t imagesz;	// Size of image in bytes
		uint32_t xpelspm;	// Horizontal resolution
		uint32_t ypelspm;	// Vertical resolution
		uint32_t clrused;	// Colour indexes used
		uint32_t clrimp;	// Size of colour table
	};

#pragma pack()

	BitmapFileHeader fileh;
	BitmapInfoHeader infoh;
	
	bool loaded;
	int width, height, depth;

	unsigned char *data;
};

#endif
