//  ObjectGrid.cpp -- Grid where static objects may be placed.
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

#include "ObjectGrid.hpp"


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
