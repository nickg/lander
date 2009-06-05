//
//  Asteroid.hpp -- Randomly generated asteroid.
//  Copyright (C) 2008  Nick Gasson
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

#ifndef INC_ASTEROID_HPP
#define INC_ASTEROID_HPP

#include "GameObjFwd.hpp"
#include "GraphicsFwd.hpp"
#include "Surface.hpp"
#include "ObjectGrid.hpp"

#include <tr1/memory>

using namespace std::tr1;

class Asteroid : public StaticObject {
public:
   Asteroid(int x, int y, int width, int surftex);
   Asteroid(const Asteroid& other);
   ~Asteroid();
	
   void Draw(int viewadjust_x, int viewadjust_y) const;
   bool CheckCollision(const Ship& ship) const;
   LineSegment GetUpBoundary(int poly) const;
   LineSegment GetDownBoundary(int poly) const;

   static const int MAX_ASTEROID_WIDTH = 15;
	
private:
   static const int AS_VARIANCE = 64;

   void GenerateDisplayList(int texidx);

   static Texture* surfTexture[Surface::NUM_SURF_TEX];
   shared_ptr<GLuint> displayList_;
   
   struct AsteroidSection {
      double texX, texwidth;
      Point points[4];
   };
   AsteroidSection uppolys[MAX_ASTEROID_WIDTH], downpolys[MAX_ASTEROID_WIDTH];
};


#endif
