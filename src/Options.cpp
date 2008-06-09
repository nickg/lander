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

Options::Options()
   : state(optFadeIn),
     helpFont(LocateResource("Default_Font.ttf"), 14)
{
   
}

void Options::Load()
{
   
}

void Options::ProcessFadeIn()
{

}

void Options::ProcessMain()
{

}

void Options::ProcessFadeOut()
{

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

void Options::Display()
{
   // Delegate star drawing to the menu
   static_cast<MainMenu*>
      (ScreenManager::GetInstance().GetScreenById("MAIN MENU"))
      ->DisplayStars();
}

