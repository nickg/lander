/*
 * Bitmap.cpp - Implementation of Bitmap wrapper class.
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

#include "Bitmap.hpp"

/*
 * Constructs a new bitmap object.
 *	f -> Stream to read data from.
 * Throws std::runtime_error on failure.
 */
Bitmap::Bitmap(File *f)
    : loaded(false), data(NULL)
{
    // Read file header
    f->Read(&fileh, sizeof(BitmapFileHeader));
    LittleEndian16(fileh.magic);
    if (fileh.magic != BITMAP_MAGIC_NUMBER)
        throw std::runtime_error("Bitmap has invalid magic number");

    // Read info header
    f->Read(&infoh, sizeof(BitmapInfoHeader));
	
    // Bitmaps use big endian
    LittleEndian32(infoh.width);
    LittleEndian32(infoh.height);
    LittleEndian16(infoh.planes);
    LittleEndian16(infoh.bitcount);
    LittleEndian32(infoh.imagesz);
    LittleEndian32(infoh.clrused);
    LittleEndian32(infoh.clrimp);
	
    width = infoh.width;
    height = infoh.height;
    depth = infoh.bitcount;

    if (depth != 24)
        throw std::runtime_error("Only 24-bit bitmaps are supported");

    // Calculate the size of the image with padding
    int datasize = width * height * (infoh.bitcount / 8);

    // Allocate memory
    unsigned char *tempdata = new unsigned char[datasize];

    // Read entire image
    f->Read(tempdata, sizeof(unsigned char) * datasize);

    // Calculate final width
    int bytewidth = (width * depth) / 8;
    int padwidth = bytewidth;
    while (padwidth % 4 != 0)
        padwidth++;

    // Convert data to useable format
    int diff = width * height * RGB_BYTE_SIZE;
    data = new unsigned char[diff];

    int offset = padwidth - bytewidth;

    for (int i = 0; i < datasize; i += RGB_BYTE_SIZE) {
            if ((i+1) % padwidth == 0)
                i += offset;

            *(data+i+2) = *(tempdata+i);
            *(data+i+1) = *(tempdata+i+1);
            *(data+i) = *(tempdata+i+2);
	}

    // Free temporary storage
    delete[] tempdata;

    loaded = true;
}

/*
 * Destroys a bitmap object.
 */
Bitmap::~Bitmap()
{
    if (data)
        delete[] data;
}
