//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
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
