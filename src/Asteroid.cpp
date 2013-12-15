//
//  Asteroid.cpp -- Randomly generated asteroid.
//  Copyright (C) 2008-2013  Nick Gasson
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

#include "Asteroid.hpp"
#include "OpenGL.hpp"
#include "Ship.hpp"

#include <string>
#include <cassert>
#include <stdexcept>

namespace {
   // Called by shared_ptr when no more references to a display
   // list are held
   void displayListDeleter(GLuint* u)
   {
      glDeleteLists(*u, 1);
      delete u;
   }
}

Asteroid::Asteroid(int x, int y, int width, int surftex)
   : StaticObject(x, y, width, 4),
     surfaceTexture(LoadTexture(SurfaceFileName(surftex))),
     displayList(new GLuint, displayListDeleter)
{
   assert(width > 0);

   *displayList = glGenLists(1);

   int change, texloop=0;

   // Build up Polys
   for (int i = 0; i < width; i++) {
      // Set Poly parameters
      uppolys[i].texwidth = 0.1;
      uppolys[i].texX = ((float)texloop)/10;
      if (texloop++ == 10)
         texloop = 0;

      // Lower left vertex
      uppolys[i].points[0].x = i * OBJ_GRID_SIZE;
      uppolys[i].points[0].y = 2 * OBJ_GRID_SIZE;

      // Upper left vertex
      uppolys[i].points[1].x = i * OBJ_GRID_SIZE;
      if (i == 0)
         uppolys[i].points[1].y = rand() % (2 * OBJ_GRID_SIZE);
      else
         uppolys[i].points[1].y = uppolys[i - 1].points[2].y;

      // Upper right vertex
      uppolys[i].points[2].x = (i + 1) * OBJ_GRID_SIZE;
      do
         change = uppolys[i].points[1].y + (rand() % AS_VARIANCE) - (AS_VARIANCE / 2);
      while (change < 0 || change > 2 * OBJ_GRID_SIZE);
      uppolys[i].points[2].y = change;

      // Lower right vertex
      uppolys[i].points[3].x = (i + 1) * OBJ_GRID_SIZE;
      uppolys[i].points[3].y = 2 * OBJ_GRID_SIZE;
   }

   // Taper last poly
   uppolys[width - 1].points[2].y = 2 * OBJ_GRID_SIZE;
   uppolys[0].points[1].y = 2 * OBJ_GRID_SIZE;

   // Build down Polys
   texloop = 0;
   for (int i = 0; i < width; i++) {
      // Set Poly parameters
      downpolys[i].texwidth = 0.1;
      downpolys[i].texX = ((float)texloop) / 10;
      if (texloop++ == 10)
         texloop = 0;

      // Upper left vertex
      downpolys[i].points[0].x = i * OBJ_GRID_SIZE;
      downpolys[i].points[0].y = 0;

      // Lower left vertex
      downpolys[i].points[1].x = i * OBJ_GRID_SIZE;
      if (i == 0)
         downpolys[i].points[1].y = rand() % (2 * OBJ_GRID_SIZE);
      else
         downpolys[i].points[1].y = downpolys[i - 1].points[2].y;

      // Lower right vertex
      downpolys[i].points[2].x = (i + 1) * OBJ_GRID_SIZE;
      do
         change = downpolys[i].points[1].y + (rand() % AS_VARIANCE) - (AS_VARIANCE / 2);
      while (change < 0 || change > 2 * OBJ_GRID_SIZE);
      downpolys[i].points[2].y = change;

      // Upper right vertex
      downpolys[i].points[3].x = (i + 1) * OBJ_GRID_SIZE;
      downpolys[i].points[3].y = 0;
   }

   // Taper last poly
   downpolys[width-1].points[2].y = 0;
   downpolys[0].points[1].y = 0;

   GenerateDisplayList(surftex);
}

Asteroid::Asteroid(const Asteroid& other)
   : StaticObject(other),
     displayList(other.displayList)
{
   copy(other.uppolys, other.uppolys + MAX_ASTEROID_WIDTH, uppolys);
   copy(other.downpolys, other.downpolys + MAX_ASTEROID_WIDTH, downpolys);
}

Asteroid::~Asteroid()
{

}

// Return the file name for the corresponding surface texture
string Asteroid::SurfaceFileName(int textureId)
{
   assert(textureId >= 0 && textureId < Surface::NUM_SURF_TEX);

   static const char* fileNames[] = {
      "images/dirt_surface2.png",
      "images/snow_surface2.png",
      "images/red_rock_surface2.png",
      "images/rock_surface2.png"
   };

   return fileNames[textureId];
}

void Asteroid::GenerateDisplayList(int texidx)
{
   glNewList(*displayList, GL_COMPILE);

   glDisable(GL_BLEND);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, surfaceTexture->GetGLTexture());
   glColor4d(1.0, 1.0, 1.0, 1.0);

   for (int i = 0; i < width; i++) {
      // Up
      glBegin(GL_QUADS);
        glTexCoord2d(uppolys[i].texX, 0.0);
        glVertex2i(uppolys[i].points[0].x, uppolys[i].points[0].y);
        glTexCoord2d(uppolys[i].texX, 1.0);
        glVertex2i(uppolys[i].points[1].x, uppolys[i].points[1].y);
        glTexCoord2d(uppolys[i].texX + uppolys[i].texwidth, 1.0);
        glVertex2i(uppolys[i].points[2].x, uppolys[i].points[2].y);
        glTexCoord2d(uppolys[i].texX + uppolys[i].texwidth, 0.0);
        glVertex2i(uppolys[i].points[3].x, uppolys[i].points[3].y);
      glEnd();

      glTranslated(0.0, 2.0*OBJ_GRID_SIZE, 0.0);

      // Down
      glBegin(GL_QUADS);
        glTexCoord2d(downpolys[i].texX, 0.0);
        glVertex2i(downpolys[i].points[0].x, downpolys[i].points[0].y);
        glTexCoord2d(downpolys[i].texX, 1.0);
        glVertex2i(downpolys[i].points[1].x, downpolys[i].points[1].y);
        glTexCoord2d(downpolys[i].texX + downpolys[i].texwidth, 1.0);
        glVertex2i(downpolys[i].points[2].x, downpolys[i].points[2].y);
        glTexCoord2d(downpolys[i].texX + downpolys[i].texwidth, 0.0);
        glVertex2i(downpolys[i].points[3].x, downpolys[i].points[3].y);
      glEnd();

      glTranslated(0.0, -2.0*OBJ_GRID_SIZE, 0.0);
   }

   glEndList();
}

//
// Returns the line segment which defines the top of the given Poly.
//
LineSegment Asteroid::GetUpBoundary(int poly) const
{
   return LineSegment
      (xpos*OBJ_GRID_SIZE + uppolys[poly].points[1].x,
       ypos*OBJ_GRID_SIZE + uppolys[poly].points[1].y + OBJ_GRID_TOP,
       xpos*OBJ_GRID_SIZE + uppolys[poly].points[2].x,
       ypos*OBJ_GRID_SIZE + uppolys[poly].points[2].y + OBJ_GRID_TOP);
}

//
// Returns the line segment which defines the bottom of the given Poly.
//
LineSegment Asteroid::GetDownBoundary(int poly) const
{
   return LineSegment
      (xpos*OBJ_GRID_SIZE + downpolys[poly].points[1].x,
       (ypos+2)*OBJ_GRID_SIZE + downpolys[poly].points[1].y + OBJ_GRID_TOP,
       xpos*OBJ_GRID_SIZE + downpolys[poly].points[2].x,
       (ypos+2)*OBJ_GRID_SIZE + downpolys[poly].points[2].y + OBJ_GRID_TOP);
}

void Asteroid::Draw(int viewadjust_x, int viewadjust_y) const
{
   if (!displayList) {
      // This asteroid has been copied and lost its display
      // list reference
      throw runtime_error("Asteroid::Draw called on invalid asteroid copy");
   }

   double ix = xpos*OBJ_GRID_SIZE - viewadjust_x;
   double iy = ypos*OBJ_GRID_SIZE - viewadjust_y + OBJ_GRID_TOP;

   glLoadIdentity();
   glTranslated(ix, iy, 0.0);
   glCallList(*displayList);
}

bool Asteroid::CheckCollision(const Ship& ship) const
{
   // Look at polys
   for (int k = 0; k < GetWidth(); k++) {
      LineSegment l1 = GetUpBoundary(k);
      LineSegment l2 = GetDownBoundary(k);

      if (ship.HotSpotCollision(l1) || ship.HotSpotCollision(l2))
         return true;
   }
   return false;
}
