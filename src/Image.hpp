/*  Image.hpp -- Wrapper for loading and displaying images
 *  Copyright (C) 2008  Nick Gasson
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INC_IMAGE_HPP
#define INC_IMAGE_HPP

#include "Platform.hpp"

class Image {
public:
   Image(const char *file);
   virtual ~Image();

   virtual void Draw(int x, int y, double rotate=0.0, double scale=1.0,
                     double alpha=1.0, double white=1.0) const;

   virtual int GetWidth() const { return width; }
   virtual int GetHeight() const { return height; }
  
protected:
   GLuint texture;
   int width, height;

private:
   static bool IsPowerOfTwo(int n);
};

#endif