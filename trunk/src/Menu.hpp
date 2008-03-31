/*
 * Menu.hpp -- Defintition of main menu screen.
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

#include "ScreenManager.hpp"
#include "Mechanics.hpp"

#define MENU_FADE_SPEED		0.1f
#define HINT_DISPLAY_TIME	140


class MenuStar {
public:
   MenuStar();
   
   bool Move();
   void Display(float fade=1.0f);

private:
   static const float ROTATE_SPEED, ENLARGE_RATE, INIT_SCALE, SPEED;
   static const int TEXTURE_SIZE;
   
   TextureQuad quad;
   float scale;
   bool active;
   Position pos;
   Velocity vel;

   static float starRotate;
   static bool hasLoaded;
   static GLuint uStarTexture;
};

class MainMenu : public Screen {
public:
   MainMenu() : hasloaded(false) { }
   virtual ~MainMenu() { }

   void Load();
   void Process();
   void Display();
private:
   enum MenuState { msFadeIn, msInMenu, msFadeToStart,
                    msFadeToHigh, msFadeToOpt, msFadeToExit };
		
   TextureQuad logo, start, highscore, options, exit;
   float fade, starsel, highsel, optsel, exitsel;
   bool hasloaded;
   GLuint uStartTexture, uHighTexture, uOptionsTexture, uExitTexture;
   MenuState state;
	
   int hint_timeout, hintidx;

   static const int MAX_STARS = 80;
   typedef vector<MenuStar> StarList;
   typedef StarList::iterator StarListIt;
   vector<MenuStar> stars;
};


#endif
