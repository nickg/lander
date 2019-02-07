//
//  Asteroid.hpp -- Randomly generated asteroid.
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

#pragma once

#include "GameObjFwd.hpp"
#include "GraphicsFwd.hpp"
#include "Surface.hpp"
#include "ObjectGrid.hpp"

#include <memory>

class Asteroid : public StaticObject {
public:
   Asteroid(int x, int y, int width, int surftex);
   Asteroid(Asteroid&& other) = default;
   Asteroid(const Asteroid& other) = delete;
   ~Asteroid();

   void Draw(int viewadjust_x, int viewadjust_y) const;
   bool CheckCollision(const Ship& ship) const;
   LineSegment GetUpBoundary(int poly) const;
   LineSegment GetDownBoundary(int poly) const;

   static const int MAX_ASTEROID_WIDTH = 15;

private:
   static const int AS_VARIANCE = 64;

   void GenerateDisplayList(int texidx);
   static const char *SurfaceFileName(int textureId);

   Texture m_texture;
   VertexBuffer m_vbo;

   struct AsteroidSection {
      float texX, texwidth;
      Point points[4];
   };
   AsteroidSection uppolys[MAX_ASTEROID_WIDTH], downpolys[MAX_ASTEROID_WIDTH];
};
