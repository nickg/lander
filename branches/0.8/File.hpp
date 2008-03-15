/*
 * File.hpp - Definition of File wrapper class.
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

#ifndef INC_FILE_HPP
#define INC_FILE_HPP

#include "Platform.hpp"

/*
 * A wrapper around file I/O operations.
 */
class File
{
public:
  File(const char *path, bool readonly=true);
  ~File();
  
  void Read(void *buf, size_t bytes);
  void Write(const void *buf, size_t bytes);
  void Seek(size_t offset);
  
  static bool Exists(const char *path);
  static const char *LocateResource(const char *base, const char *ext);
private:
  static const int MAX_RES_PATH = 256;
  
  FILE *m_stream;
  string m_path;
};

#endif
