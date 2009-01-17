//  Viewport.hpp -- The area of the screen the player can see.
//  Copyright (C) 2008  Nick Gasson
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

#ifndef INC_VIEWPORT_HPP
#define INC_VIEWPORT_HPP

#include "Platform.hpp"

class Viewport {
public:
   Viewport();

   int GetXAdjust() const { return adjustX; }
   int GetYAdjust() const { return adjustY; }
   void SetXAdjust(int x);
   void SetYAdjust(int y);

   int GetLevelWidth() const { return levelWidth; }
   int GetLevelHeight() const { return levelHeight; }
   void SetLevelWidth(int w) { levelWidth = w; }
   void SetLevelHeight(int h) { levelHeight = h; }

   bool ObjectInScreen(int xpos, int ypos, int width, int height);
   bool PointInScreen(int xpos, int ypos, int width, int height);
   
private:
   int adjustX, adjustY;
   int screenWidth, screenHeight;
   int levelWidth, levelHeight;
};

#endif
