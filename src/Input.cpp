/*
 * Input.cpp - Implementation of SDL input wrapper.
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

#include "Input.hpp"
#include "OpenGL.hpp"

/* 
 * Create a new input manager. Never call this directly: use GetInstance.
 */
Input::Input()
   : joystick(NULL), textinput(false), joyLeft(false), joyRight(false),
     joyUp(false), joyDown(false), joyButton0(false), joyButton1(false)
{
   // Start SDL joystick handling subsystem
   if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
      throw runtime_error("Unable to initialise SDL: " + string(SDL_GetError()));
   }

   cout << "Found " << SDL_NumJoysticks() << " joysticks" << endl;

   for (int i = 0; i < SDL_NumJoysticks(); i++)
      cout << i << ": " << SDL_JoystickName(i) << endl;
   
   // Only use the first joystick
   if (SDL_NumJoysticks() > 0)	{
      SDL_JoystickEventState(SDL_ENABLE);
      joystick = SDL_JoystickOpen(0);
   }

   for (int i = 0; i < NUM_ACTIONS; i++)
      actionIgnore[i] = 0;
}


/* 
 * Destroys an input manager.
 */
Input::~Input()
{
   if (joystick != NULL)
      SDL_JoystickClose(joystick);
}


/*
 * Returns the only instance of Input.
 */
Input &Input::GetInstance()
{
   static Input g_input;

   return g_input;	
}


/* 
 * Updates the current input state. Note that this also processes SDL_QUIT messages.
 */
void Input::Update()
{
   SDL_Event e;

   while (SDL_PollEvent(&e))	{
      switch (e.type)	{
			case SDL_QUIT:
         // End the game
         OpenGL::GetInstance().Stop();
         break;

			case SDL_KEYDOWN:
         // Type a character in text input mode
         if (textinput) {
            if ((e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) 
                || (e.key.keysym.sym == SDLK_SPACE)) {
               char ch = (char)e.key.keysym.sym;
               text += shift ? toupper(ch) : ch;
            }
            else if (e.key.keysym.sym == SDLK_LSHIFT
                     || e.key.keysym.sym == SDLK_RSHIFT) {
               shift = true;
            }
            else if (e.key.keysym.sym == SDLK_BACKSPACE && text.length() > 0)	{
               text.erase(text.length() - 1, 1);
            }
         }
         break;

      case SDL_KEYUP:
         if (e.key.keysym.sym == SDLK_LSHIFT
                     || e.key.keysym.sym == SDLK_RSHIFT) {
            shift = false;
         }
         break;

			case SDL_JOYAXISMOTION:
         // Joystick was moved
         if ((e.jaxis.value < -3500) || (e.jaxis.value > 3500)) {
            if (e.jaxis.axis == 0) {
               // Left-right movement code goes here
               if (e.jaxis.value < 0)
                  joyLeft = true;
               else
                  joyRight = true;
            }
				
            if (e.jaxis.axis == 1) {
               // Up-Down movement code goes here
               if (e.jaxis.value < 0)
                  joyUp = true;
               else
                  joyDown = true;
            }
         }
         else {
            joyLeft = false;
            joyRight = false;
            joyUp = false;
            joyDown = false;
         }
         break;

			case SDL_JOYBUTTONDOWN:
         // Joystick button was pressed
         // Button is e.jbutton.button
         switch (e.jbutton.button) {
         case 0:
            joyButton0 = true;
            break;
         case 1:
            joyButton1 = true;
            break;
         }
         break;

			case SDL_JOYBUTTONUP:
         // Joystick button was released
         switch (e.jbutton.button) {
         case 0:
            joyButton0 = false;
            break;
         case 1:
            joyButton1 = false;
            break;
         }
         break;
      }
   }

   for (int i = 0; i < NUM_ACTIONS; i++) {
      if (actionIgnore[i] > 0)
         actionIgnore[i]--;
   }
}

// Returns true if the action a is being perfomed.
// Either on the keyboard on the first joystick.
bool Input::QueryAction(Action a) const
{
   int numkeys;
   Uint8 *keystate;

   if (actionIgnore[a] > 0)
      return false;
   
   keystate = SDL_GetKeyState(&numkeys);
   
   switch (a) {
   case UP:
      return (keystate[SDLK_UP] != 0) || joyUp;
   case DOWN:
      return (keystate[SDLK_DOWN] != 0) || joyDown;
   case LEFT:
      return (keystate[SDLK_LEFT] != 0) || joyLeft;
   case RIGHT:
      return (keystate[SDLK_RIGHT] != 0) || joyRight;
   case FIRE:
      return (keystate[SDLK_RETURN] != 0) || joyButton0 || joyButton1;
   case SKIP:
      return (keystate[SDLK_SPACE] != 0) || joyButton0;
   case ABORT:
      return keystate[SDLK_ESCAPE] != 0;
   case DEBUG:
      return keystate[SDLK_d] != 0;
   case PAUSE:
      return keystate[SDLK_p] != 0;
   case THRUST:
      return (keystate[SDLK_UP] != 0) || joyButton1;
   default:
      assert(false);
   }
}

// Waits RESET_TIMEOUT frames before registering action a again.
void Input::ResetAction(Action a)
{
   assert(a < NUM_ACTIONS);

   actionIgnore[a] = RESET_TIMEOUT;
}


/*
 * Starts reading keyboard data into a buffer.
 *	max -> Maximum number of characters to read.
 */
void Input::OpenCharBuffer(int max)
{
   assert(!textinput);

   shift = false;
   maxchar = max;
   text = "";
   textinput = true;
}

/*
 * Closes the character buffer and returns to normal key mode.
 */
void Input::CloseCharBuffer()
{
   assert(textinput);

   textinput = false;
}


/*
 * Returns a pointer to the data read in text input mode.
 */
const char *Input::GetInput() const
{
   return text.c_str();
}

