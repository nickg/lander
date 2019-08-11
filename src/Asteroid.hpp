//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#pragma once

#include "GameObjFwd.hpp"
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
