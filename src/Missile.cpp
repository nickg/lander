/*  Missile.cpp -- Missiles on asteroids, etc. 
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

#include "Missile.hpp"
#include "LoadOnce.hpp"

Image *Missile::image = NULL;

Missile::Missile(ObjectGrid *o, Viewport *v, Side s)
   : viewport(v), objgrid(o)
{
   // This constructor builds a missile attached to the side of the screen
   
   LOAD_ONCE {
      image = new Image("images/missile.png");
   }

   x = (s == SIDE_LEFT) ? 0 : o->GetWidth() - 1;

   // Pick spaces at random until we find one that's empty
   do {
      y = rand() % o->GetHeight();
   } while (o->IsFilled(x, y));
}

void Missile::Draw() const
{
   if (viewport->ObjectInScreen(x, y, 1, 1)) {
      int dx, dy;
      ObjectGrid::Offset(x, y, &dx, &dy);
      
      image->Draw(dx - viewport->GetXAdjust(),
                  dy - viewport->GetYAdjust());
   }
}
