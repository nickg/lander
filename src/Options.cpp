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
#include "Input.hpp"

const double Options::FADE_SPEED = 0.1;

Options::Options()
   : state(optFadeIn),
     helpFont(LocateResource("Default_Font.ttf"), 12),
     itemFont(LocateResource("Default_Font.ttf"), 16),
     fadeAlpha(0.0),
     selected(0)
{
   Item fullscreen = { "Fullscreen" };
   fullscreen.values.push_back("Yes");
   fullscreen.values.push_back("No");
   fullscreen.active = (cfile.get_bool("fullscreen") ? 0 : 1);

   // TODO: It would be better to query SDL for these values
   Item resolution = { "Resolution", 0 };
   resolution.values.push_back("800x600");
   resolution.values.push_back("1024x768");
   resolution.values.push_back("1280x1024");

   int hres = cfile.get_int("hres");
   int vres = cfile.get_int("vres");
   string currentRes = MakeResolutionString(hres, vres);
   int i = 0;
   for (StringListIt it = resolution.values.begin();
        it != resolution.values.end();
        ++it, ++i) {
      if (*it == currentRes) {
         resolution.active = i;
         break;
      }
   }

   items.push_back(fullscreen);
   items.push_back(resolution);
}

void Options::Load()
{
   state = optFadeIn;
   fadeAlpha = 0.0;
   selected = 0;
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
   Input &input = Input::GetInstance();
   
   if (input.GetKeyState(SDLK_RETURN)) {
      state = optFadeOut;
   }
   else if (input.GetKeyState(SDLK_UP)) {
      if (selected > 0)
         selected--;
      input.ResetKey(SDLK_UP);
   }
   else if (input.GetKeyState(SDLK_DOWN)) {
      if (selected + 1 < items.size())
         selected++;
      input.ResetKey(SDLK_DOWN);
   }
   else if (input.GetKeyState(SDLK_LEFT)) {
      Item &item = items[selected];
      if (item.active == 0)
         item.active = item.values.size() - 1;
      else
         item.active = (item.active - 1) % item.values.size();
      input.ResetKey(SDLK_LEFT);
   }
   else if (input.GetKeyState(SDLK_RIGHT)) {
      Item &item = items[selected];
      item.active = (item.active + 1) % item.values.size();
      input.ResetKey(SDLK_RIGHT);
   }
}

void Options::Apply()
{
   for (ItemListIt it = items.begin(); it != items.end(); ++it) {
      if ((*it).name == "Fullscreen") {
         cfile.put("fullscreen", (*it).active == 0);
      }
      if ((*it).name == "Resolution") {
         int hres, vres;
         ParseResolutionString((*it).values[(*it).active], &hres, &vres);

         cfile.put("hres", hres);
         cfile.put("vres", vres);
      }
   }
}

void Options::ProcessFadeOut()
{
   fadeAlpha -= FADE_SPEED;

   if (fadeAlpha <= 0.0) {
      fadeAlpha = 0.0;

      Apply();
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

void Options::DisplayItems()
{
   int screen_w = OpenGL::GetInstance().GetWidth();
   int screen_h = OpenGL::GetInstance().GetHeight();

   const int H_PER_ITEM = 30;
   int item_height = H_PER_ITEM * items.size();
   int y = (screen_h - item_height) / 2;

   unsigned int i = 0;
   for (ItemListIt it = items.begin(); it != items.end(); ++it) {
      assert((*it).active < (*it).values.size());

      int all_w = itemFont.GetStringWidth
         ("%s  %s", (*it).name.c_str(), (*it).values[(*it).active].c_str());
      int name_w = itemFont.GetStringWidth((*it).name.c_str());
      int space_w = itemFont.GetStringWidth("  ");

      if (i == selected)
         glColor4d(0.0, 1.0, 0.0, fadeAlpha);
      else
         glColor4d(0.0, 0.5, 0.0, fadeAlpha);
      int x = (screen_w - all_w) / 2;
      itemFont.Print(x, y, (*it).name.c_str());

      if (i == selected)
         glColor4d(0.0, 0.7, 1.0, fadeAlpha);
      else
         glColor4d(0.0, 0.2, 0.7, fadeAlpha);
      x += name_w + space_w;
      itemFont.Print(x, y, (*it).values[(*it).active].c_str());

      y += H_PER_ITEM;
      i++;
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
   DisplayItems();
}

string Options::MakeResolutionString(int hres, int vres) const
{
   ostringstream ss;
   ss << hres << "x" << vres;
   return ss.str();
}

void Options::ParseResolutionString(const string &str, int *hres, int *vres) const
{
   char x;
   istringstream ss(str);
   ss >> *hres;
   ss >> x;
   assert(x == 'x');
   ss >> *vres;
}
