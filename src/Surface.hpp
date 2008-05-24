/*  Surface.hpp -- Randomly generated planet surface.
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

#ifndef INC_SURFACE_HPP
#define INC_SURFACE_HPP

#include "OpenGL.hpp"
#include "Viewport.hpp"
#include "LandingPad.hpp"
#include "Ship.hpp"

class Surface {
public:
   Surface(Viewport *v);
   ~Surface();

   static void Load();

   void Generate(int surftex, LandingPadList &pads);
   bool CheckCollisions(Ship &ship, LandingPadList &pads, int *padIndex);
   void Display();

   static const int NUM_SURF_TEX = 5;  // Number of available surface textures
   static const int SURFACE_SIZE = 20;
   static const int MAX_SURFACE_HEIGHT = 300;
   static const int VARIANCE = 50;     // Bumpyness of landscape
   
private:
   static GLuint uSurfaceTexture[NUM_SURF_TEX];
   
   Viewport *viewport;
   Poly *surface;
};

#endif
