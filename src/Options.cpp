/*  Options.cpp -- The options screen.
 *  Copyright (C) 2008  Nick Gasson
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Options.hpp"
#include "Menu.hpp"
#include "OpenGL.hpp"

const double Options::FADE_SPEED = 0.1;

Options::Options()
   : state(optFadeIn),
     helpFont(LocateResource("Default_Font.ttf"), 12),
     fadeAlpha(0.0)
{
   
}

void Options::Load()
{
   state = optFadeIn;
   fadeAlpha = 0.0;
}

void Options::ProcessFadeIn()
{
   fadeAlpha += FADE_SPEED;
   
   if (fadeAlpha >= 1.0) {
      state = optMain;
      fadeAlpha = 1.0;
   }
}

void Options::ProcessMain()
{

}

void Options::ProcessFadeOut()
{
   fadeAlpha -= FADE_SPEED;

   if (fadeAlpha <= 0.0) {
      fadeAlpha = 0.0;

      ScreenManager::GetInstance().SelectScreen("MAIN MENU");
   }
}

void Options::Process()
{
   static_cast<MainMenu*>
      (ScreenManager::GetInstance().GetScreenById("MAIN MENU"))
      ->MoveStars();

   switch (state) {
   case optFadeIn:
      ProcessFadeIn();
      break;
   case optMain:
      ProcessMain();
      break;
   case optFadeOut:
      ProcessFadeOut();
      break;
   }
}

void Options::DisplayHelpText()
{
   int x, y;

   int screen_w = OpenGL::GetInstance().GetWidth();
   int screen_h = OpenGL::GetInstance().GetHeight();
   
   glColor4d(0.0, 1.0, 0.0, fadeAlpha);

   const char *help1 = i18n("Use UP and DOWN to select options");
   x = (screen_w - helpFont.GetStringWidth(help1)) / 2;
   y = screen_h - 100;
   helpFont.Print(x, y, help1);
      
   const char *help2 = i18n("Use LEFT and RIGHT to change values");
   x = (screen_w - helpFont.GetStringWidth(help2)) / 2;
   y += 25;
   helpFont.Print(x, y, help2);

   const char *help3 = i18n("Press FIRE or RETURN to exit");
   x = (screen_w - helpFont.GetStringWidth(help3)) / 2;
   y += 25;
   helpFont.Print(x, y, help3);   
}

void Options::Display()
{
   // Delegate star drawing to the menu
   static_cast<MainMenu*>
      (ScreenManager::GetInstance().GetScreenById("MAIN MENU"))
      ->DisplayStars();

   DisplayHelpText();
}

