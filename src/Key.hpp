//  Key.hpp -- A key the player has to collect.
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

#ifndef INC_KEY_HPP
#define INC_KEY_HPP

#include "GameObjFwd.hpp"
#include "AnimatedImage.hpp"
#include "ObjectGrid.hpp"

enum ArrowColour { acBlue, acRed, acYellow, acPink, acGreen };

class Key : public StaticObject {
public:
   Key(bool active, int xpos, int ypos, ArrowColour acol);
   
   void DrawKey(Viewport* viewport);
   void DrawArrow(Viewport* viewport);
   void DrawIcon(int offset, float minAlpha);
   bool CheckCollision(Ship& ship) const;

   void Collected() { active = false; }
   
private:
   static const int KEY_FRAMES = 18;
   static const int KEY_ROTATION_SPEED = 2;
   static const int ARROW_SIZE = 32;
   
   int rotcount;
   float alpha;
   bool active;
   AnimatedImage image;
   Image arrow;

   static string KeyFileName(ArrowColour col);
   static string ArrowFileName(ArrowColour col);
};

#endif
