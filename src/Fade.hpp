//  Fade.hpp -- Generic fade in/out effect.
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

#ifndef INC_FADE_HPP
#define INC_FADE_HPP

#include "Platform.hpp"

class Fade {
public:
   static const float DEFAULT_FADE_SPEED;
   
   Fade(float s = DEFAULT_FADE_SPEED);

   void BeginFadeIn();
   void BeginFadeOut();
   void Display();
   bool Process();
private:
   enum State { fNone, fIn, fOut };

   State state;
   float alpha, speed;
};

#endif
