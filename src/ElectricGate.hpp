/*  ElectricGate.hpp -- Electric gateway thingys.
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

#ifndef INC_ELECTRICGATE_HPP
#define INC_ELECTRICGATE_HPP

#include "ObjectGrid.hpp"
#include "OpenGL.hpp"
#include "Ship.hpp"
#include "Viewport.hpp"

class ElectricGate : public StaticObject {
public:
   ElectricGate(Viewport *v, int length, bool vertical, int x, int y);

   bool CheckCollision(Ship &ship);
   void Draw();
   
private:
   static const int GATEWAY_ACTIVE = 30;
   
   int length, timer;
   bool vertical;
   TextureQuad icon;
   Viewport *viewport;

   static GLuint uGatewayTexture;
};

#endif