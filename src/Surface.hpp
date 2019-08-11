//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#pragma once

#include "GameObjFwd.hpp"
#include "LandingPad.hpp"

class Surface {
public:
   Surface(Viewport* v);
   ~Surface();

   void Generate(int surftex, LandingPadList& pads);
   bool CheckCollisions(Ship& ship, LandingPadList& pads, int* padIndex);
   void Display() const;

   static const int NUM_SURF_TEX = 4;   // Number of available surface textures
   static const int SURFACE_SIZE;
   static const int MAX_SURFACE_HEIGHT;
   static const int MIN_SURFACE_HEIGHT;
   static const int VARIANCE;

private:
   Texture surfTexture[NUM_SURF_TEX];

   int texidx;
   Viewport* viewport;
   VertexBuffer m_vbo;

   struct SurfaceSection {
      float texX, texwidth;
      Point points[4];
   };
   SurfaceSection* surface;
};
