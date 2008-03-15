/*  Asteroid.cpp -- Randomly generated asteroid.
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

#include "Asteroid.hpp"
#include "OpenGL.hpp"

void Asteroid::ConstructAsteroid(int x, int y, int width, Texture texture)
{
   xpos = x;
   ypos = y;
   this->width = width;
   height = 4;

   int change, texloop=0;

   // Build up Polys
   for (int i = 0; i < width; i++) {
      // Set Poly parameters
      uppolys[i].texwidth = 0.1f;
      uppolys[i].texX = ((float)texloop)/10;
      if (texloop++ == 10)
         texloop = 0;
      uppolys[i].pointcount = 4;
      uppolys[i].uTexture = texture;

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
      downpolys[i].texwidth = 0.1f;
      downpolys[i].texX = ((float)texloop) / 10;
      if (texloop++ == 10)
         texloop = 0;
      downpolys[i].pointcount = 4;
      downpolys[i].uTexture = texture;

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
}

/*
 * Returns the line segment which defines the top of the given Poly.
 */
LineSegment Asteroid::GetUpBoundary(int poly)
{
   return LineSegment
      (xpos*OBJ_GRID_SIZE + uppolys[poly].points[1].x,
       ypos*OBJ_GRID_SIZE + uppolys[poly].points[1].y + OBJ_GRID_TOP,
       xpos*OBJ_GRID_SIZE + uppolys[poly].points[2].x,
       ypos*OBJ_GRID_SIZE + uppolys[poly].points[2].y + OBJ_GRID_TOP);
}

/*
 * Returns the line segment which defines the bottom of the given Poly.
 */
LineSegment Asteroid::GetDownBoundary(int poly)
{
   return LineSegment
      (xpos*OBJ_GRID_SIZE + downpolys[poly].points[1].x,
       (ypos+2)*OBJ_GRID_SIZE + downpolys[poly].points[1].y + OBJ_GRID_TOP,
       xpos*OBJ_GRID_SIZE + downpolys[poly].points[2].x,
       (ypos+2)*OBJ_GRID_SIZE + downpolys[poly].points[2].y + OBJ_GRID_TOP);
}

void Asteroid::Draw(int viewadjust_x, int viewadjust_y)
{
   OpenGL &opengl = OpenGL::GetInstance();

   for (int i = 0; i < width; i++) {
      // Up
      uppolys[i].xpos = xpos*OBJ_GRID_SIZE - viewadjust_x;
      uppolys[i].ypos = ypos*OBJ_GRID_SIZE - viewadjust_y + OBJ_GRID_TOP;
      opengl.Draw(&uppolys[i]);

      // Down
      downpolys[i].xpos = xpos*OBJ_GRID_SIZE - viewadjust_x;
      downpolys[i].ypos = (ypos+2)*OBJ_GRID_SIZE - viewadjust_y + OBJ_GRID_TOP;
      opengl.Draw(&downpolys[i]);
   }
} 

