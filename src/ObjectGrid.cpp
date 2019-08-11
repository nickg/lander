//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "ObjectGrid.hpp"

#include <cassert>


ObjectGrid::ObjectGrid()
  : grid(NULL), width(0), height(0)
{

}

ObjectGrid::~ObjectGrid()
{
   if (grid)
      delete[] grid;
}

//
// Allocates a free space in the object grid.
//	x, y -> Output x, y, co-ordinates.
// Returns falce if area could not be allocated.
//
bool ObjectGrid::AllocFreeSpace(int& x, int& y)
{
   int timeout = 10000;

   // Keep generating points until we find a free space
   do {
      if (--timeout == 0)
         return false;

      x = rand() % width;
      y = rand() % height;
   } while (grid[x + (y * width)]);

   grid[x + (y * width)] = true;

   return true;
}

//
// Allocates a free space in the object grid.
//	x, y -> Output x, y co-ordinates.
//	width, height -> Size of desired space.
// Returns false if area could not be allocated.
//
bool ObjectGrid::AllocFreeSpace(int& x, int& y, int width, int height)
{
   bool isOk;
   int counter_x, counter_y;
   int timeout = 10000;

   // Keep generating points until we find a free space
   do {
      if (--timeout == 0)
         return false;

      x = rand() % (this->width - width);
      y = rand() % (this->height - height);

      // Check this position
      isOk = true;
      for (counter_x = x; counter_x < x + width; counter_x++) {
         for (counter_y = y; counter_y < y + height; counter_y++) {
            if (grid[counter_x + (counter_y * this->width)])
               isOk = false;
         }
      }
   } while (!isOk);

   for (counter_x = x; counter_x < x + width; counter_x++) {
      for (counter_y = y; counter_y < y + height; counter_y++)
         grid[counter_x + (counter_y * this->width)] = true;
   }

   return true;
}

//
// Marks the square at (x, y) as no longer in use.
//
void ObjectGrid::UnlockSpace(int x, int y)
{
   assert(x < width);
   assert(y < height);
   grid[x + (y * width)] = false;
}

//
// Creates a new blank object grid.
//
void ObjectGrid::Reset(int width, int height)
{
   assert(width > 0);
   assert(height > 0);

   if (grid)
      delete[] grid;

   this->width = width;
   this->height = height;

   grid = new bool[width * height];

   memset(grid, 0, width * height * sizeof(bool));
}

//
// Returns true if there is an object at the specified co-ordinates.
//
bool ObjectGrid::IsFilled(int x, int y) const
{
   assert(x < width);
   assert(y < height);
   return grid[x + (width * y)];
}

void ObjectGrid::Offset(int ox, int oy, int* x, int* y)
{
   // Find the absolute coordinates of object grid point
   // (ox, oy) and store them in (x, y)
   *x = ox*OBJ_GRID_SIZE;
   *y = OBJ_GRID_TOP + oy*OBJ_GRID_SIZE;
}

bool StaticObject::ObjectInScreen(Viewport* viewport) const
{
   return viewport->ObjectInScreen(xpos, ypos, width, height);
}
