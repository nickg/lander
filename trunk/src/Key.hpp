/*  Key.hpp -- A key the player has to collect.
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

#ifndef INC_KEY_HPP
#define INC_KEY_HPP

#include "ObjectGrid.hpp"
#include "Viewport.hpp"
#include "Ship.hpp"

enum ArrowColour { acBlue, acRed, acYellow, acPink, acGreen };

class Key : public StaticObject {
public:
   Key();

   static void Load();
   
   void Reset(bool active, int xpos, int ypos, ArrowColour acol);
   void DrawKey(Viewport *viewport);
   void DrawArrow(Viewport *viewport);
   void DrawIcon(int offset, float minAlpha);
   bool CheckCollision(Ship &ship) const;

   void Collected() { active = false; }
   
private:
   static const int KEY_FRAMES = 18;
   static const int KEY_ROTATION_SPEED = 2;
   
   static GLuint uBlueKey[KEY_FRAMES], uRedKey[KEY_FRAMES];
   static GLuint uGreenKey[KEY_FRAMES], uPinkKey[KEY_FRAMES];
   static GLuint uYellowKey[KEY_FRAMES];
   static GLuint uBlueArrow, uPinkArrow, uRedArrow, uYellowArrow, uGreenArrow;

   TextureQuad frame[KEY_FRAMES], arrow;
   int current, rotcount;
   float alpha;
   bool active;
};

#endif
