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
#include "Image.hpp"
#include "Font.hpp"


class MenuStar {
public:
   MenuStar();
   
   bool Move();
   void Display(double fade=1.0);

private:
   static const double ROTATE_SPEED, ENLARGE_RATE, INIT_SCALE, SPEED;
   
   double scale;
   bool active;
   Position pos;
   Velocity vel;

   static double starRotate;
   static Image *starImage;
};

class MenuOption {
public:
   MenuOption(const char *imgFile, int off, int order);

   void Display(bool selected, double bigness, double fade) const;
   
private:
   Image image;
   int y;

   static const double SEL_ENLARGE, UNSEL_DIM;
};

class MainMenu : public Screen {
public:
   MainMenu();
   virtual ~MainMenu() {}

   void Load();
   void Process();
   void Display();
   
private:
   enum MenuState { msFadeIn, msInMenu, msFadeToStart,
                    msFadeToHigh, msFadeToOpt, msFadeToExit };
   enum SelOption { optStart, optScore, optOptions, optExit };
   
   MenuOption startOpt, scoreOpt, optionsOpt, exitOpt;
   SelOption selOption;
   double fade, bigness;
   MenuState state;
   Image titleImage;
   Font hintFont;
	
   int hint_timeout, hintidx;

   static const int OPTIONS_OFFSET, HINT_DISPLAY_TIME;
   static const double MENU_FADE_SPEED;
   
   static const unsigned MAX_STARS;
   typedef vector<MenuStar> StarList;
   typedef StarList::iterator StarListIt;
   vector<MenuStar> stars;
};


#endif
