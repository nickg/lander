/*
 * Menu.hpp - Defintition of main menu screen.
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

#ifndef INC_MENU_HPP
#define INC_MENU_HPP

#include "Screens.hpp"

#define MENU_FADE_SPEED		0.1f
#define NUM_STARS			512
#define HINT_DISPLAY_TIME	140


/* 
 * The menu screen.
 */
class MainMenu : public Screen
{
public:
	MainMenu() : m_hasloaded(false) { }
	virtual ~MainMenu() { }

	void Load();
	void Process();
	void Display();
private:
	enum MenuState { msFadeIn, msInMenu, msFadeToStart,
		msFadeToHigh, msFadeToOpt, msFadeToExit };
		
	TextureQuad logo, start, highscore, options, exit;
	float m_fade, m_starsel, m_highsel, m_optsel, m_exitsel;
	bool m_hasloaded;
	GLuint uStartTexture, uHighTexture, uOptionsTexture, uExitTexture;
	GLuint uStarTexture;
	MenuState m_state;
	
	int m_hint_timeout, m_hintidx;

	// Stars
	struct Star 
	{
		TextureQuad quad;
		float scale;
		bool active;
		float angle, xpos, ypos;
	} stars[NUM_STARS];
	int m_stardelay;
	float m_starrotate;
};

#endif
