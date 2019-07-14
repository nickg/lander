//
//  Viewport.cpp -- The area of the screen the player can see.
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

#include "Platform.hpp"
#include "Viewport.hpp"
#include "OpenGL.hpp"
#include "ObjectGrid.hpp"

Viewport::Viewport()
   : adjustX(0), adjustY(0), levelWidth(0), levelHeight(0)
{
}

void Viewport::SetXAdjust(int x)
{
   const int screenWidth = OpenGL::GetInstance().GetWidth();

   adjustX = x;
   if (adjustX < 0)
      adjustX = 0;
   else if (adjustX > levelWidth - screenWidth)
      adjustX = levelWidth - screenWidth;
}

void Viewport::SetYAdjust(int y)
{
   const int screenHeight = OpenGL::GetInstance().GetHeight();

   adjustY = y;
   if (adjustY < 0)
      adjustY = 0;
   else if (adjustY > levelHeight - screenHeight)
      adjustY = levelHeight - screenHeight;
}

//
// Works out whether or not a point is visible.
//	xpos, ypos -> Absolute co-ordinates.
//	width, height -> Size of object.
//
bool Viewport::PointInScreen(int xpos, int ypos, int width, int height)
{
   const int screenWidth = OpenGL::GetInstance().GetWidth();
   const int screenHeight = OpenGL::GetInstance().GetHeight();

   return ((xpos + width > adjustX && xpos - adjustX < screenWidth)
           && (ypos + height > adjustY && ypos - adjustY < screenHeight));
}

//
// Works out whether or not an object is visible.
//	xpos, ypos -> Grid co-ordinates.
//	width, height -> Size of object in grid squares.
//
bool Viewport::ObjectInScreen(int xpos, int ypos, int width, int height)
{
   return PointInScreen(xpos * ObjectGrid::OBJ_GRID_SIZE,
                        ypos * ObjectGrid::OBJ_GRID_SIZE + ObjectGrid::OBJ_GRID_TOP,
                        width * ObjectGrid::OBJ_GRID_SIZE,
                        height * ObjectGrid::OBJ_GRID_SIZE);
}
