/*
 * DataFile.hpp - Definition of DataFile class.
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

#ifndef INC_DATAFILE_HPP
#define INC_DATAFILE_HPP

#include "Platform.hpp"
#include "Bitmap.hpp"
#include "File.hpp"

/* 
 * Encapsulates a data file. The DAT file format consists of a single header
 * block, followed by one or more index records, followed by raw file data.
 */
class DataFile 
{
public:
    DataFile(const char *filename);
    ~DataFile();

    void SelectFile(const char *filename);
    size_t GetFileSize();
    void Rewind();
    void ReadAll(void *buf);
    void Read(void *buf, int bytes);
    Bitmap *LoadBitmap();

private:

#pragma pack(2)

    // File header
    struct DataFileHeader 
    {
        int headersz, filecount;
    } fileheader;

    // Data entry
    struct DataFileIndex
    {
        int indexsz, offset, filesz;
        static const int INDEX_NAME_SZ = 128;
        char name[INDEX_NAME_SZ];
    } *entries;

#pragma pack()

    File file;
    int currindex;
};

#endif
