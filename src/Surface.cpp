//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "Surface.hpp"
#include "Ship.hpp"

#include <string>

const int Surface::VARIANCE(65);     // Bumpyness of landscape
const int Surface::MAX_SURFACE_HEIGHT(300);
const int Surface::MIN_SURFACE_HEIGHT(10);
const int Surface::SURFACE_SIZE(20);

Surface::Surface(Viewport* v)
   : viewport(v),
     surface(NULL)
{
   surfTexture[0] = Texture::Load("images/dirt_surface.png");
   surfTexture[1] = Texture::Load("images/snow_surface.png");
   surfTexture[2] = Texture::Load("images/red_rock_surface.png");
   surfTexture[3] = Texture::Load("images/rock_surface.png");
}

Surface::~Surface()
{
   if (surface)
      delete[] surface;
}

void Surface::Generate(int surftex, LandingPadList& pads)
{
   if (surface)
      delete[] surface;

   int nPolys = viewport->GetLevelWidth()/SURFACE_SIZE;
   surface = new SurfaceSection[nPolys];

   texidx = surftex;

   int texloop = 0;
   for (int i = 0; i < nPolys; i++) {
      surface[i].texX = ((double)texloop)/10.0;
      if (++texloop == 10)
         texloop = 0;
      surface[i].texwidth = 0.1f;

      surface[i].points[0].x = 0;
      surface[i].points[0].y = MAX_SURFACE_HEIGHT;

      // See if we want to place a landing pad here
      LandingPad* padHere = NULL;
      for (LandingPadListIt it = pads.begin(); it != pads.end(); ++it) {
         for (int k = 0; k < (*it).GetLength(); k++) {
            if ((*it).GetIndex() + k == i) {
               padHere = &(*it);
               goto out;
            }
         }
      }

   out:
      int change;
      if (NULL == padHere)  {
         // Genereate height randomly
         if (i != 0)
            change = surface[i-1].points[2].y;
         else
            change = rand()%MAX_SURFACE_HEIGHT;
         surface[i].points[1].x = 0;
         surface[i].points[1].y = change;

         do
            change = surface[i].points[1].y + (rand()%VARIANCE-(VARIANCE/2));
         while (change > MAX_SURFACE_HEIGHT || change < MIN_SURFACE_HEIGHT);
         surface[i].points[2].x = SURFACE_SIZE;
         surface[i].points[2].y = change;
      }
      else {
         // Make flat terrain for landing pad
         if (i != 0)
            change = surface[i-1].points[2].y;
         else {
            do
               change = rand()%MAX_SURFACE_HEIGHT;
            while (change > MAX_SURFACE_HEIGHT || change < MIN_SURFACE_HEIGHT);
         }
         surface[i].points[1].x = 0;
         surface[i].points[1].y = change;
         surface[i].points[2].x = SURFACE_SIZE;
         surface[i].points[2].y = change;

         padHere->SetYPos(change);
      }

      surface[i].points[3].x = SURFACE_SIZE;
      surface[i].points[3].y = MAX_SURFACE_HEIGHT;
   }

   VertexI *vertexBuf = new VertexI[4 * nPolys];

   for (int i = 0; i < nPolys; i++) {
      const VertexI vertices[4] = {
         { surface[i].points[0].x, surface[i].points[0].y,
           surface[i].texX, 0.0f },
         { surface[i].points[1].x, surface[i].points[1].y,
           surface[i].texX, 1.0f },
         { surface[i].points[2].x, surface[i].points[2].y,
           surface[i].texX + surface[i].texwidth, 1.0f },
         { surface[i].points[3].x, surface[i].points[3].y,
           surface[i].texX + surface[i].texwidth, 0.0f }
      };

      copy(vertices, vertices + 4, vertexBuf + i*4);
   }

   m_vbo = VertexBuffer::Make(vertexBuf, 4 * nPolys);

   delete[] vertexBuf;
}

void Surface::Display() const
{
   int left = viewport->GetXAdjust()/SURFACE_SIZE;
   int right = left + OpenGL::GetInstance().GetWidth()/SURFACE_SIZE + 1;
   int max = viewport->GetLevelWidth()/SURFACE_SIZE;
   if (right > max)
      right = max;

   OpenGL& opengl = OpenGL::GetInstance();

   opengl.Reset();
   opengl.SetTexture(surfTexture[texidx]);

   for (int i = left; i < right; i++) {
      float xpos = i*SURFACE_SIZE - viewport->GetXAdjust();
      float ypos = viewport->GetLevelHeight()
         - viewport->GetYAdjust() - MAX_SURFACE_HEIGHT;

      opengl.SetTranslation(xpos, ypos);
      opengl.Draw(m_vbo, i * 4, 4);
   }
}

//
// Returns true if ship has collided with surface. Sets padIndex to the index
// of pad if the player hit it, -1 otherwise.
//
bool Surface::CheckCollisions(Ship& ship, LandingPadList& pads, int* padIndex)
{
   LineSegment l;
   int lookmin = (int)(ship.GetX()/SURFACE_SIZE) - 2;
   int lookmax = (int)(ship.GetX()/SURFACE_SIZE) + 2;
   if (lookmin < 0)	lookmin = 0;
   if (lookmax >= viewport->GetLevelWidth()/SURFACE_SIZE)
      lookmax = viewport->GetLevelWidth()/SURFACE_SIZE - 1;

   if (ship.GetY() < viewport->GetLevelHeight() - MAX_SURFACE_HEIGHT)
      return false;

   *padIndex = -1;

   for (int i = lookmin; i <= lookmax; i++) {
      l.p1.x = i*SURFACE_SIZE;
      l.p1.y = viewport->GetLevelHeight() - MAX_SURFACE_HEIGHT + surface[i].points[1].y;
      l.p2.x = (i+1)*SURFACE_SIZE;
      l.p2.y = viewport->GetLevelHeight() - MAX_SURFACE_HEIGHT + surface[i].points[2].y;

      // Look through each hot spot and check for collisions
      if (ship.HotSpotCollision(l)) {
         // See if this is a landing pad
         int j = 0;
         for (LandingPadListIt it = pads.begin();
              it != pads.end();
              ++it, ++j) {
            LandingPad& pad = *it;
            for (int m = 0; m < pad.GetLength(); m++) {
               if (pad.GetIndex() + m == i) {
                  // Hit a landing pad
                  *padIndex = j;
                  goto out;
               }
            }
         }
      out:
         return true;
      }
   }
   return false;
}
