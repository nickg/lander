/*  Surface.cpp -- Randomly generated planet surface.
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

#include "Surface.hpp"
#include "LoadOnce.hpp"

const int Surface::VARIANCE = 65;     // Bumpyness of landscape
const int Surface::MAX_SURFACE_HEIGHT = 300;
const int Surface::SURFACE_SIZE = 20;

Texture *Surface::surfTexture[Surface::NUM_SURF_TEX];

Surface::Surface(Viewport *v)
   : surface(NULL), viewport(v)
{
   LOAD_ONCE {
      surfTexture[0] = new Texture("images/grass_surface.png");
      surfTexture[1] = new Texture("images/dirt_surface.png");
      surfTexture[2] = new Texture("images/snow_surface.png");
      surfTexture[3] = new Texture("images/red_rock_surface.png");
      surfTexture[4] = new Texture("images/rock_surface.png");
   }
}

Surface::~Surface()
{
   if (surface)
      delete[] surface;
}

void Surface::Generate(int surftex, LandingPadList &pads)
{
   if (surface)
      delete[] surface;

   int nPolys = viewport->GetLevelWidth()/SURFACE_SIZE;
   surface = new Poly[nPolys];

   int texloop = 0;
   for (int i = 0; i < nPolys; i++) {
      surface[i].pointcount = 4;
      surface[i].xpos = i * SURFACE_SIZE;
      surface[i].ypos = viewport->GetLevelHeight() - MAX_SURFACE_HEIGHT;
      surface[i].uTexture = surfTexture[surftex]->GetGLTexture();
      surface[i].texX = ((float)texloop)/10;
      if (texloop++ == 10)
         texloop = 0;
      surface[i].texwidth = 0.1f;

      surface[i].points[0].x = 0;
      surface[i].points[0].y = MAX_SURFACE_HEIGHT;
		
      // See if we want to place a landing pad here
      LandingPad *padHere = NULL;
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
         while (change > MAX_SURFACE_HEIGHT || change < 0);
         surface[i].points[2].x = SURFACE_SIZE;
         surface[i].points[2].y = change;
      }
      else {
         // Make flat terrain for landing pad
         if (i != 0)
            change = surface[i-1].points[2].y;
         else
            change = rand()%MAX_SURFACE_HEIGHT;
         surface[i].points[1].x = 0;
         surface[i].points[1].y = change;
         surface[i].points[2].x = SURFACE_SIZE;
         surface[i].points[2].y = change;
         
         padHere->SetYPos(change);
      }
      
      surface[i].points[3].x = SURFACE_SIZE;
      surface[i].points[3].y = MAX_SURFACE_HEIGHT;
   }
}

void Surface::Display()
{
   OpenGL &opengl = OpenGL::GetInstance();
   for (int i = 0; i < viewport->GetLevelWidth()/SURFACE_SIZE; i++) {
      surface[i].xpos = i*SURFACE_SIZE - viewport->GetXAdjust();
      surface[i].ypos = viewport->GetLevelHeight()
         - viewport->GetYAdjust() - MAX_SURFACE_HEIGHT;
      opengl.Draw(&surface[i]);
   }
}

/*
 * Returns true if ship has collided with surface. Sets padIndex to the index
 * of pad if the player hit it, -1 otherwise.
 */
bool Surface::CheckCollisions(Ship &ship, LandingPadList &pads, int *padIndex)
{
   LineSegment l;
   int lookmin = (int)(ship.GetX()/SURFACE_SIZE) - 2;
   int lookmax = (int)(ship.GetX()/SURFACE_SIZE) + 2;
   if (lookmin < 0)	lookmin = 0;
   if (lookmax >= viewport->GetLevelWidth()/SURFACE_SIZE)
      lookmax = viewport->GetLevelWidth()/SURFACE_SIZE - 1;

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
            LandingPad &pad = *it;
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
