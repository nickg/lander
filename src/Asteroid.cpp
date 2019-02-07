//
//  Asteroid.cpp -- Randomly generated asteroid.
//  Copyright (C) 2008-2019  Nick Gasson
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

Asteroid::Asteroid(int x, int y, int width, int surftex)
   : StaticObject(x, y, width, 4),
     m_texture(LoadTexture(SurfaceFileName(surftex)))
{
   assert(width > 0);

   const int texLoopInit = random() % 10;

   int change, texloop = texLoopInit;

   // Build up Polys
   for (int i = 0; i < width; i++) {
      // Set Poly parameters
      uppolys[i].texwidth = 0.1f;
      uppolys[i].texX = texloop / 10.0f;
      if (++texloop == 10)
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
   texloop = texLoopInit;
   for (int i = 0; i < width; i++) {
      // Set Poly parameters
      downpolys[i].texwidth = 0.1;
      downpolys[i].texX = ((float)texloop) / 10;
      if (++texloop == 10)
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

Asteroid::~Asteroid()
{
}

const char *Asteroid::SurfaceFileName(int textureId)
{
   assert(textureId >= 0 && textureId < Surface::NUM_SURF_TEX);

   static const char* fileNames[Surface::NUM_SURF_TEX] = {
      "images/dirt_surface2.png",
      "images/snow_surface2.png",
      "images/red_rock_surface2.png",
      "images/rock_surface2.png"
   };

   return fileNames[textureId];
}

void Asteroid::GenerateDisplayList(int texidx)
{
   VertexI *vertexBuf = new VertexI[width * 8];

   for (int i = 0; i < width; i++) {
      const VertexI vertices[] = {
         // Up
         { uppolys[i].points[0].x, uppolys[i].points[0].y,
           uppolys[i].texX, 0.0f },
         { uppolys[i].points[1].x, uppolys[i].points[1].y,
           uppolys[i].texX, 1.0f },
         { uppolys[i].points[2].x, uppolys[i].points[2].y,
           uppolys[i].texX + uppolys[i].texwidth, 1.0f },
         { uppolys[i].points[3].x, uppolys[i].points[3].y,
           uppolys[i].texX + uppolys[i].texwidth, 0.0f },

         // Down
         { downpolys[i].points[0].x,
           downpolys[i].points[0].y + 2 * OBJ_GRID_SIZE,
           downpolys[i].texX, 0.0f },
         { downpolys[i].points[1].x,
           downpolys[i].points[1].y + 2 * OBJ_GRID_SIZE,
           downpolys[i].texX, 1.0f },
         { downpolys[i].points[2].x,
           downpolys[i].points[2].y + 2 * OBJ_GRID_SIZE,
           downpolys[i].texX + downpolys[i].texwidth, 1.0f },
         { downpolys[i].points[3].x,
           downpolys[i].points[3].y + 2 * OBJ_GRID_SIZE,
           downpolys[i].texX + downpolys[i].texwidth, 0.0f }
      };

      copy(vertices, vertices + 8, vertexBuf + i*8);
   };

   m_vbo = VertexBuffer::Make(vertexBuf, width * 8);

   delete[] vertexBuf;
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
   float ix = xpos*OBJ_GRID_SIZE - viewadjust_x;
   float iy = ypos*OBJ_GRID_SIZE - viewadjust_y + OBJ_GRID_TOP;

   OpenGL& opengl = OpenGL::GetInstance();

   opengl.Reset();
   opengl.SetTranslation(ix, iy);

   glBindTexture(GL_TEXTURE_2D, m_texture->GetGLTexture());

   opengl.Draw(m_vbo);
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
