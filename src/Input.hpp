//
// Copyright (C) 2006-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#pragma once

#include "Platform.hpp"

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
   void FakeAction(Action a);

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
   string text;	// Text read so far
   int maxchar;			  	// Maximum number of characters to read

   // Record joystick state
   bool joyLeft, joyRight, joyUp, joyDown, joyButton0, joyButton1;

   Action m_fakeAction = NUM_ACTIONS;

   static const int JOYSTICK_DEADZONE = 3500;
};
