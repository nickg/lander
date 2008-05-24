/*
 * Input.hpp -- Definition of input manager class.
 * Copyright (C) 2006  Nick Gasson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INC_INPUT_HPP
#define INC_INPUT_HPP

#include "Platform.hpp"

/* 
 * A singleton class to manage SDL input.
 */
class Input {
public:
   static Input &GetInstance();

   void Update();
   bool GetKeyState(int key);
   void ResetKey(int key);
   int QueryJoystickAxis(int axis);
   bool QueryJoystickButton(int button);
   void ResetJoystickButton(int button);

   void OpenCharBuffer(int max=256);
   void CloseCharBuffer();
   const char *GetInput() const;

private:
   Input();
   ~Input();
   
   static const int NUM_KEYS = 512;		// Maximum number of input keys
   static const int RESET_TIMEOUT = 5;		// Frames between key presses
   static const int NUM_BUTTONS = 16;		// Maximum number of joystick buttons

   SDL_Joystick *joystick;

   int ignore[NUM_KEYS];		// Timeout for keys being ignored
   int jignore[NUM_BUTTONS];	// Timeout for buttons being ignored

   bool textinput;			// Is a character buffer open?
   string text;				// Text read so far
   int maxchar;				// Maximum number of characters to read

};

#endif
