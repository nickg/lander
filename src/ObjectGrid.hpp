//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

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
   bool AllocFreeSpace(int& x, int& y);
   bool AllocFreeSpace(int& x, int& y, int width, int height);
   void UnlockSpace(int x, int y);

   bool IsFilled(int x, int y) const;
   int GetWidth() const { return width; }
   int GetHeight() const { return height; }

   static void Offset(int ox, int oy, int* x, int* y);

   static const int OBJ_GRID_SIZE = 32;
   static const int OBJ_GRID_TOP  = 100;

private:
   bool* grid;
   int width, height;
};


class StaticObject {
public:
   StaticObject(int xpos, int ypos, int width=1, int height=1)
      : xpos(xpos), ypos(ypos), width(width), height(height) {}
   virtual ~StaticObject() {}

   bool ObjectInScreen(Viewport* viewport) const;

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
