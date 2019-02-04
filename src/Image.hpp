//  Image.hpp -- Wrapper for loading and displaying images
//  Copyright (C) 2008-2019  Nick Gasson
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef INC_IMAGE_HPP
#define INC_IMAGE_HPP

#include "Platform.hpp"
#include "Texture.hpp"

class Image {
public:
   Image(const string& fileName);
   virtual ~Image();

   void Draw(int x, int y, float rotate=0.0f, float scale=1.0f,
             float alpha=1.0f, float white=1.0f) const;

   int GetWidth() const;
   int GetHeight() const;

protected:
   Texture* GetTexture() const { return m_texture; }

private:
   Texture* m_texture = NULL;
   GLuint m_vbo = 0;
};

#endif
