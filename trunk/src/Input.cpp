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
: joystick(NULL), m_textinput(false)
{
	// Start SDL joystick handling subsystem
	if (SDL_Init(SDL_INIT_JOYSTICK) < 0) 
	{
		throw runtime_error("Unable to initialise SDL: " + string(SDL_GetError()));
	}

	// Only use the first joystick
	if (SDL_NumJoysticks() > 0)
	{
		SDL_JoystickEventState(SDL_DISABLE);	// TODO: enable when I have a joystick to test!
		//joystick = SDL_JoystickOpen(0);
	}

	for (int i = 0; i < NUM_KEYS; i++)
		m_ignore[i] = 0;
	
	for (int i = 0; i < NUM_BUTTONS; i++)
		m_jignore[i] = 0;
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
	int i;

	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
			case SDL_QUIT:
				// End the game
				OpenGL::GetInstance().Stop();
				break;

			case SDL_KEYDOWN:
				// Type a character in text input mode
				if (m_textinput)
				{
					if ((e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) 
						|| (e.key.keysym.sym == SDLK_SPACE))
					{
						m_text += (char)e.key.keysym.sym;
					}
					else if (e.key.keysym.sym == SDLK_BACKSPACE && m_text.length() > 0)
					{
						m_text.erase(m_text.length() - 1, 1);
					}
				}
				break;

			case SDL_JOYAXISMOTION:
				// Joystick was moved
				if ((e.jaxis.value < -3200) || (e.jaxis.value > 3200)) 
				{
					if (e.jaxis.axis == 0) 
					{
						// Left-right movement code goes here
					}
				
					if (e.jaxis.axis == 1) 
					{
						// Up-Down movement code goes here
					}
				}
				break;

			case SDL_JOYBUTTONDOWN:
				// Joystick button was pressed
				// Button is e.jbutton.button
				break;

			case SDL_JOYBUTTONUP:
				// Joystick button was released
				break;
		}
	}

	for (i = 0; i < NUM_KEYS; i++)
	{
		if (m_ignore[i] > 0)
			m_ignore[i]--;
	}
}


/*
 * Returns the state of one key. Result true if currently pressed.
 */
bool Input::GetKeyState(int key)
{
	int numkeys;
	Uint8 *keystate;

	if (m_ignore[key])
		return false;

	keystate = SDL_GetKeyState(&numkeys);
	assert(key < numkeys);

	return keystate[key] != 0;
}


/* 
 * Waits RESET_TIMEOUT frames before registering a particular keypress again.
 */
void Input::ResetKey(int key)
{
	assert(key < NUM_KEYS);

	m_ignore[key] = RESET_TIMEOUT;
}


/*
 * Waits RESET_TIMEOUT frames before registering a particular button press again.
 */
void Input::ResetJoystickButton(int button)
{
	assert(button < NUM_BUTTONS);
	
	m_jignore[button] = RESET_TIMEOUT;
}


/*
 * Starts reading keyboard data into a buffer.
 *	max -> Maximum number of characters to read.
 */
void Input::OpenCharBuffer(int max)
{
	assert(!m_textinput);

	m_maxchar = max;
	m_text = "";
	m_textinput = true;
}

/*
 * Closes the character buffer and returns to normal key mode.
 */
void Input::CloseCharBuffer()
{
	assert(m_textinput);

	m_textinput = false;
}


/*
 * Returns a pointer to the data read in text input mode.
 */
const char *Input::GetInput() const
{
	return m_text.c_str();
}


/*
 * Returns the current value of a joystick axis.
 *	axis -> 0 = horizontal, 1 = vertical.
 */
int Input::QueryJoystickAxis(int axis)
{
	return 0;
}


/*
 * Returns the current state of a joystick button.
 *	button -> Numbered from zero.
 */
bool Input::QueryJoystickButton(int button)
{
	return false;
}

