//
// Copyright (C) 2006-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
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

   const char *GetName() const override { return "MAIN MENU"; }

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
