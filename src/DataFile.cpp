/*
 * DataFile.cpp - Implementation of DataFile class.
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

#include "DataFile.hpp"

/* 
 * Opens a data file.
 *	filename -> Name of data file to open.
 * May throw std::runtime_error if file does not exist or is not
 * in the correct format.
 */
DataFile::DataFile(const char *filename)
    : file(filename), currindex(0)
{
    // Read the header
    file.Read(&fileheader, sizeof(DataFileHeader));
    
    // Fix the endianess
    LittleEndian32(fileheader.headersz);
    LittleEndian32(fileheader.filecount);
    
    if (fileheader.headersz != sizeof(DataFileHeader))
        throw runtime_error("Invalid data file: " + string(filename));

    // Read the entries
    entries = new DataFileIndex[fileheader.filecount];
    file.Read(entries, sizeof(DataFileIndex) * fileheader.filecount);
	
    // Fix the endianness of the entries
    for (int i = 0; i < fileheader.filecount; i++) {
        LittleEndian32(entries[i].filesz);
        LittleEndian32(entries[i].indexsz);
        LittleEndian32(entries[i].offset);
    }
}


/* 
 * Seeks to the start of the current file.
 */
void DataFile::Rewind()
{
    file.Seek(entries[currindex].offset);
}


/*
 * Selects a file in the archive.
 *	filename -> Name of file to select.
 * Throws std::runtime_error if file not present.
 */
void DataFile::SelectFile(const char *filename)
{
    int i = 0;

    while (i < fileheader.filecount && strcmp(entries[i].name, filename))
        i++;

    if (i == fileheader.filecount) {
        throw runtime_error("File " + string(filename) + " not present in data file");
    }
    else {
        currindex = i;
        Rewind();
    }
}


/*
 * Reads all the data from a file into buf.
 *	buf -> Buffer to hold output data.
 */
void DataFile::ReadAll(void *buf)
{
    Rewind();
    file.Read(buf, entries[currindex].filesz);
    Rewind();
}


/* 
 * Reads part of the active file.
 *	buf -> Buffer to hold output data.
 *	bytes -> Number of bytes to read.
 */
void DataFile::Read(void *buf, int bytes)
{
    file.Read(buf, bytes);
}


/*
 * Reads a bitmap from the currently selected file.
 * The Bitmap constructor may throw a std::runtime_error.
 */
Bitmap *DataFile::LoadBitmap()
{
    Rewind();
    Bitmap *b = new Bitmap(&file);
    Rewind();

    return b;
}


/* 
 * Frees ay memory used by the data file.
 */
DataFile::~DataFile()
{
    // Free memory
    delete[] entries;
}


/*
 * Returns the size of the file in bytes.
 */
size_t DataFile::GetFileSize()
{
    return entries[currindex].filesz;
}
