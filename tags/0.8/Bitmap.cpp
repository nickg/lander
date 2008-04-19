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
    : m_loaded(false), m_data(NULL)
{
    // Read file header
    f->Read(&m_fileh, sizeof(BitmapFileHeader));
    LittleEndian16(m_fileh.magic);
    if (m_fileh.magic != BITMAP_MAGIC_NUMBER)
        throw std::runtime_error("Bitmap has invalid magic number");

    // Read info header
    f->Read(&m_infoh, sizeof(BitmapInfoHeader));
	
    // Bitmaps use big endian
    LittleEndian32(m_infoh.width);
    LittleEndian32(m_infoh.height);
    LittleEndian16(m_infoh.planes);
    LittleEndian16(m_infoh.bitcount);
    LittleEndian32(m_infoh.imagesz);
    LittleEndian32(m_infoh.clrused);
    LittleEndian32(m_infoh.clrimp);
	
    m_width = m_infoh.width;
    m_height = m_infoh.height;
    m_depth = m_infoh.bitcount;

    if (m_depth != 24)
        throw std::runtime_error("Only 24-bit bitmaps are supported");

    // Calculate the size of the image with padding
    int datasize = m_width * m_height * (m_infoh.bitcount / 8);

    // Allocate memory
    unsigned char *tempdata = new unsigned char[datasize];

    // Read entire image
    f->Read(tempdata, sizeof(unsigned char) * datasize);

    // Calculate final width
    int bytewidth = (m_width * m_depth) / 8;
    int padwidth = bytewidth;
    while (padwidth % 4 != 0)
        padwidth++;

    // Convert data to useable format
    int diff = m_width * m_height * RGB_BYTE_SIZE;
    m_data = new unsigned char[diff];

    int offset = padwidth - bytewidth;

    for (int i = 0; i < datasize; i += RGB_BYTE_SIZE) {
            if ((i+1) % padwidth == 0)
                i += offset;

            *(m_data+i+2) = *(tempdata+i);
            *(m_data+i+1) = *(tempdata+i+1);
            *(m_data+i) = *(tempdata+i+2);
	}

    // Free temporary storage
    delete[] tempdata;

    m_loaded = true;
}

/*
 * Destroys a bitmap object.
 */
Bitmap::~Bitmap()
{
    if (m_data)
        delete[] m_data;
}
