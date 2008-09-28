/*  ObjectGrid.hpp -- Grid where static objects may be placed.
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

#ifndef INC_OBJECTGRID_HPP
#define INC_OBJECTGRID_HPP

#include "Platform.hpp"
#include "Geometry.hpp"
#include "Viewport.hpp"

class ObjectGrid {
public:
   ObjectGrid();
   ~ObjectGrid();
    
   void Reset(int width, int height);
   bool AllocFreeSpace(int &x, int &y);
   bool AllocFreeSpace(int &x, int &y, int width, int height);
   void UnlockSpace(int x, int y);
	
   bool IsFilled(int x, int y) const;
   int GetWidth() const { return width; }
   int GetHeight() const { return height; }

   static void Offset(int ox, int oy, int *x, int *y);

   static const int OBJ_GRID_SIZE = 32;
   static const int OBJ_GRID_TOP = 100;
   
private:   
   bool *grid;
   int width, height;
};


class StaticObject {
public:
   StaticObject(int xpos, int ypos, int width=1, int height=1)
      : xpos(xpos), ypos(ypos), width(width), height(height) {}
   StaticObject() { StaticObject(0, 0, 1, 1); }

   bool ObjectInScreen(Viewport *viewport);
   
   int GetX() const { return xpos; }
   int GetY() const { return ypos; }
   int GetWidth() const { return width; }
   int GetHeight() const { return height; }

protected:
   int xpos, ypos, width, height;

   static const int OBJ_GRID_SIZE = ObjectGrid::OBJ_GRID_SIZE;
   static const int OBJ_GRID_TOP = ObjectGrid::OBJ_GRID_TOP;
};

#endif
