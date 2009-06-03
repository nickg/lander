//
// Input.hpp -- Definition of input manager class.
// Copyright (C) 2006  Nick Gasson
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#ifndef INC_INPUT_HPP
#define INC_INPUT_HPP

#include "Platform.hpp"

#include <sstream>

// 
// A singleton class to manage SDL input.
//
class Input {
public:
   // Possible inputs
   enum Action {
      UP, DOWN, LEFT, RIGHT, FIRE,
      SKIP, ABORT, DEBUG, PAUSE, THRUST,
      SCREENSHOT,
      NUM_ACTIONS // Must be last
   };
   
   static Input& GetInstance();

   bool QueryAction(Action a) const;
   bool QueryResetAction(Action a);
   void ResetAction(Action a);
   void Update();

   void OpenCharBuffer(int max=256);
   void CloseCharBuffer();
   string GetInput() const;

private:
   Input();
   ~Input();
   
   static const int RESET_TIMEOUT;		// Frames between key presses

   SDL_Joystick* joystick;

   int actionIgnore[NUM_ACTIONS];   // Timeout for actions being ignored

   bool shift;
   bool textinput;			// Is a character buffer open?
   ostringstream text;	// Text read so far
   int maxchar;			  	// Maximum number of characters to read

   // Record joystick state
   bool joyLeft, joyRight, joyUp, joyDown, joyButton0, joyButton1;
};

#endif
