//
// Menu.hpp -- Defintition of main menu screen.
// Copyright (C) 2006-2019  Nick Gasson
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

#ifndef INC_MENU_HPP
#define INC_MENU_HPP

#include "ScreenManager.hpp"
#include "Image.hpp"
#include "Font.hpp"
#include "SoundEffect.hpp"

class MenuStar {
public:
   MenuStar();

   bool Move();
   void Display(double fade=1.0) const;

private:
   static const double ROTATE_SPEED, ENLARGE_RATE, INIT_SCALE, SPEED;

   double scale;
   double x, y, angle;

   static double starRotate;
   static Image* starImage;
};

class MenuOption {
public:
   MenuOption(const char* imgFile, int off, int order);

   void Display(bool selected, double bigness, double fade) const;

private:
   Image m_image;
   int m_off;
   int m_order;

   static const double SEL_ENLARGE, UNSEL_DIM;
};

class MainMenu : public Screen {
public:
   MainMenu();

   void Load();
   void Process();
   void Display();
   void DisplayStars() const;
   void MoveStars();

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
   int hintidx;
   float m_hintTimeout = 0.0f;

   static const int OPTIONS_OFFSET;
   static constexpr float HINT_DISPLAY_TIME = 140.0f;
   static constexpr double MENU_FADE_SPEED = 0.1;

   static const unsigned MAX_STARS;
   typedef vector<MenuStar> StarList;
   typedef StarList::iterator StarListIt;
   vector<MenuStar> stars;
};

#endif
