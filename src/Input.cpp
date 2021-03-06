//
// Copyright (C) 2006-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "Input.hpp"
#include "OpenGL.hpp"

#include <iostream>
#include <cassert>

const int Input::RESET_TIMEOUT(7);

//
// Create a new input manager. Never call this directly: use GetInstance.
//
Input::Input()
   : joystick(NULL), textinput(false), joyLeft(false), joyRight(false),
     joyUp(false), joyDown(false), joyButton0(false), joyButton1(false)
{
   // Start SDL joystick handling subsystem
   if (SDL_Init(SDL_INIT_JOYSTICK) < 0)
      Die("Unable to initialise SDL: %s", SDL_GetError());

   cout << "Found " << SDL_NumJoysticks() << " joysticks" << endl;

   for (int i = 0; i < SDL_NumJoysticks(); i++)
      cout << i << ": " << SDL_JoystickNameForIndex(i) << endl;

   // Only use the first joystick
   if (SDL_NumJoysticks() > 0)  {
      SDL_JoystickEventState(SDL_ENABLE);
      joystick = SDL_JoystickOpen(0);
   }

   for (int i = 0; i < NUM_ACTIONS; i++)
      actionIgnore[i] = 0;
}


//
// Destroys an input manager.
//
Input::~Input()
{
   if (joystick != NULL)
      SDL_JoystickClose(joystick);
}


//
// Returns the only instance of Input.
//
Input& Input::GetInstance()
{
   static Input g_input;

   return g_input;
}


//
// Updates the current input state. Note that this also processes SDL_QUIT messages.
//
void Input::Update()
{
   m_fakeAction = NUM_ACTIONS;

   SDL_Event e;
   while (SDL_PollEvent(&e))    {
      switch (e.type)   {
      case SDL_QUIT:
         // End the game
         OpenGL::GetInstance().Stop();
         break;

      case SDL_KEYDOWN:
         // Type a character in text input mode
         if (textinput) {
            if ((e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z)
                || (e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9)
                || (e.key.keysym.sym == SDLK_SPACE)) {
               char ch = (char)e.key.keysym.sym;
               ch = shift ? toupper(ch) : ch;
               text += ch;
            }
            else if (e.key.keysym.sym == SDLK_LSHIFT
                     || e.key.keysym.sym == SDLK_RSHIFT) {
               shift = true;
            }
            else if (e.key.keysym.sym == SDLK_BACKSPACE) {
               if (!text.empty())
                  text.erase(text.end() - 1);
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
         if (e.jaxis.axis == 0) {
            if (abs(e.jaxis.value) < JOYSTICK_DEADZONE) {
               joyLeft = false;
               joyRight = false;
            }
            else {
               joyLeft = e.jaxis.value < 0;
               joyRight = e.jaxis.value > 0;
            }
         }
         else if (e.jaxis.axis == 1) {
            if (abs(e.jaxis.value) < JOYSTICK_DEADZONE) {
               joyUp = false;
               joyDown = false;
            }
            else {
               joyUp = e.jaxis.value < 0;
               joyDown = e.jaxis.value > 0;
            }
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

// Query an action and reset if fired.
bool Input::QueryResetAction(Action a)
{
   bool on = QueryAction(a);
   if (on)
      ResetAction(a);
   return on;
}

// Returns true if the action a is being perfomed.
// Either on the keyboard on the first joystick.
bool Input::QueryAction(Action a) const
{
   int numkeys;
   const Uint8* keystate;

   if (actionIgnore[a] > 0)
      return false;

   if (a == m_fakeAction)
      return true;

   keystate = SDL_GetKeyboardState(&numkeys);

   switch (a) {
   case UP:
      return (keystate[SDL_SCANCODE_UP] != 0) || joyUp;
   case DOWN:
      return (keystate[SDL_SCANCODE_DOWN] != 0) || joyDown;
   case LEFT:
      return (keystate[SDL_SCANCODE_LEFT] != 0) || joyLeft;
   case RIGHT:
      return (keystate[SDL_SCANCODE_RIGHT] != 0) || joyRight;
   case FIRE:
      return (keystate[SDL_SCANCODE_RETURN] != 0) || joyButton0 || joyButton1;
   case SKIP:
      return (keystate[SDL_SCANCODE_SPACE] != 0) || joyButton0;
   case ABORT:
      return keystate[SDL_SCANCODE_ESCAPE] != 0;
   case DEBUG:
      return keystate[SDL_SCANCODE_D] != 0;
   case PAUSE:
      return keystate[SDL_SCANCODE_P] != 0;
   case THRUST:
      return (keystate[SDL_SCANCODE_UP] != 0) || joyButton1;
   case SCREENSHOT:
      return keystate[SDL_SCANCODE_PRINTSCREEN] != 0;
   default:
      return false;
   }
}

// Waits RESET_TIMEOUT frames before registering action a again.
void Input::ResetAction(Action a)
{
   assert(a < NUM_ACTIONS);

   actionIgnore[a] = RESET_TIMEOUT;
}

void Input::FakeAction(Action a)
{
   m_fakeAction = a;
}

//
// Starts reading keyboard data into a buffer.
//      max -> Maximum number of characters to read.
//
void Input::OpenCharBuffer(int max)
{
   assert(!textinput);

   shift = false;
   maxchar = max;
   text = "";
   textinput = true;
}

//
// Closes the character buffer and returns to normal key mode.
//
void Input::CloseCharBuffer()
{
   assert(textinput);

   textinput = false;
}


//
// Returns a pointer to the data read in text input mode.
//
string Input::GetInput() const
{
   return text;
}
