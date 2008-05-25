/*  LandingPad.hpp -- Where the player can land.
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

#ifndef INC_LANDINGPAD_HPP
#define INC_LANDINGPAD_HPP

#include "Viewport.hpp"
#include "Texture.hpp"
#include "OpenGL.hpp"

class LandingPad
{
public:
   LandingPad(Viewport *v, int index, int length);

   void Draw(bool locked);
   void SetYPos(int ypos) { this->ypos = ypos; }
	
   int GetLength() const { return length; }
   int GetIndex() const { return index; }

private:
   TextureQuad quad;
   int index, length, ypos;
   Viewport *viewport;
   
   static Texture *landTexture, *noLandTexture;
};

typedef vector<LandingPad> LandingPadList;
typedef LandingPadList::iterator LandingPadListIt;

#endif
