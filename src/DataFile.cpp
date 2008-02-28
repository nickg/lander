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
    : m_file(filename), m_currindex(0)
{
    // Read the header
    m_file.Read(&m_fileheader, sizeof(DataFileHeader));
    
    // Fix the endianess
    LittleEndian32(m_fileheader.headersz);
    LittleEndian32(m_fileheader.filecount);
    
    if (m_fileheader.headersz != sizeof(DataFileHeader))
        throw runtime_error("Invalid data file: " + string(filename));

    // Read the entries
    m_entries = new DataFileIndex[m_fileheader.filecount];
    m_file.Read(m_entries, sizeof(DataFileIndex) * m_fileheader.filecount);
	
    // Fix the endianness of the entries
    for (int i = 0; i < m_fileheader.filecount; i++) {
        LittleEndian32(m_entries[i].filesz);
        LittleEndian32(m_entries[i].indexsz);
        LittleEndian32(m_entries[i].offset);
    }
}


/* 
 * Seeks to the start of the current file.
 */
void DataFile::Rewind()
{
    m_file.Seek(m_entries[m_currindex].offset);
}


/*
 * Selects a file in the archive.
 *	filename -> Name of file to select.
 * Throws std::runtime_error if file not present.
 */
void DataFile::SelectFile(const char *filename)
{
    int i = 0;

    while (i < m_fileheader.filecount && strcmp(m_entries[i].name, filename))
        i++;

    if (i == m_fileheader.filecount) {
        throw runtime_error("File " + string(filename) + " not present in data file");
    }
    else {
        m_currindex = i;
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
    m_file.Read(buf, m_entries[m_currindex].filesz);
    Rewind();
}


/* 
 * Reads part of the active file.
 *	buf -> Buffer to hold output data.
 *	bytes -> Number of bytes to read.
 */
void DataFile::Read(void *buf, int bytes)
{
    m_file.Read(buf, bytes);
}


/*
 * Reads a bitmap from the currently selected file.
 * The Bitmap constructor may throw a std::runtime_error.
 */
Bitmap *DataFile::LoadBitmap()
{
    Rewind();
    Bitmap *b = new Bitmap(&m_file);
    Rewind();

    return b;
}


/* 
 * Frees am_y memory used by the data file.
 */
DataFile::~DataFile()
{
    // Free memory
    delete[] m_entries;
}


/*
 * Returns the size of the file in bytes.
 */
size_t DataFile::GetFileSize()
{
    return m_entries[m_currindex].filesz;
}
