//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "Options.hpp"
#include "Menu.hpp"
#include "OpenGL.hpp"
#include "Input.hpp"
#include "SoundEffect.hpp"
#include "InterfaceSounds.hpp"

#include <cassert>
#include <sstream>

const double Options::FADE_SPEED = 0.1;

Options::Options()
   : state(optFadeIn),
     helpFont(LocateResource("fonts/VeraBd.ttf"), 12),
     itemFont(LocateResource("fonts/VeraBd.ttf"), 16),
     fadeAlpha(0.0),
     selected(0)
{
   Item fullscreen = { "Fullscreen" };
   fullscreen.values.push_back("Yes");
   fullscreen.values.push_back("No");
   fullscreen.active = (cfile.get_bool("fullscreen") ? 0 : 1);

   vector<OpenGL::Resolution> available;
   OpenGL::GetInstance().EnumResolutions(available);

   const int min_width = 640;
   const int min_height = 480;

   Item resolution = { "Resolution", 0 };
   for (const auto& r : available) {
      if (r.width >= min_width && r.height >= min_height) {
         ostringstream ss;
         ss << r.width << "x" << r.height;

         resolution.values.push_back(ss.str());
      }
   }

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

   Item sound = { "Sound Effects" };
   sound.values.push_back("On");
   sound.values.push_back("Off");
   sound.active = (cfile.get_bool("sound") ? 0 : 1);

   Item startLevel = { "Start Level" };
   for (int i = 1; i <= 20; i++) {
      ostringstream ss;
      ss << i;
      startLevel.values.push_back(ss.str());
   }
   startLevel.active = cfile.get_int("level", 1) - 1;

   items.push_back(fullscreen);
   items.push_back(resolution);
   items.push_back(sound);
   items.push_back(startLevel);
}

void Options::Load()
{
   state = optFadeIn;
   fadeAlpha = 0.0;
   selected = 0;
}

void Options::ProcessFadeIn()
{
   fadeAlpha += FADE_SPEED * OpenGL::GetInstance().GetTimeScale();

   if (fadeAlpha >= 1.0) {
      state = optMain;
      fadeAlpha = 1.0;
   }
}

void Options::ProcessMain()
{
   Input& input = Input::GetInstance();

   if (input.QueryAction(Input::FIRE)) {
      state = optFadeOut;
      InterfaceSounds::PlaySelect();
   }
   else if (input.QueryAction(Input::UP)) {
      if (selected > 0)
         selected--;
      input.ResetAction(Input::UP);
      InterfaceSounds::PlayBleep();
   }
   else if (input.QueryAction(Input::DOWN)) {
      if (selected + 1 < items.size())
         selected++;
      input.ResetAction(Input::DOWN);
      InterfaceSounds::PlayBleep();
   }
   else if (input.QueryAction(Input::LEFT)) {
      Item& item = items[selected];
      if (item.active == 0)
         item.active = item.values.size() - 1;
      else
         item.active = (item.active - 1) % item.values.size();
      input.ResetAction(Input::LEFT);
      InterfaceSounds::PlayBleep();
   }
   else if (input.QueryAction(Input::RIGHT)) {
      Item& item = items[selected];
      item.active = (item.active + 1) % item.values.size();
      input.ResetAction(Input::RIGHT);
      InterfaceSounds::PlayBleep();
   }
}

void Options::Apply()
{
   int hres = 0, vres = 0;
   bool fullscreen = false;

   for (ItemListIt it = items.begin(); it != items.end(); ++it) {
      if ((*it).name == "Fullscreen") {
         fullscreen = (*it).active == 0;

         cfile.put("fullscreen", fullscreen);
      }
      else if ((*it).name == "Resolution") {
         ParseResolutionString((*it).values[(*it).active], &hres, &vres);

         cfile.put("hres", hres);
         cfile.put("vres", vres);
      }
      else if ((*it).name == "Sound Effects") {
         bool sound = (*it).active == 0;
         SoundEffect::SetEnabled(sound);

         cfile.put("sound", sound);
      }
      else if ((*it).name == "Start Level") {
         istringstream ss((*it).values[(*it).active]);
         int level;
         ss >> level;

         cfile.put("level", level);
      }
   }

   cfile.Flush();

   assert(hres > 0 && vres > 0);

// Changing the video mode on Windows invalidiates all the OpenGL textures
// A temporary workaround is to require the user to restart the program
#ifndef WIN32
   OpenGL::GetInstance().SetVideoMode(fullscreen, hres, vres);
#endif
}

void Options::ProcessFadeOut()
{
   fadeAlpha -= FADE_SPEED * OpenGL::GetInstance().GetTimeScale();

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
         itemFont.SetColour(0.0f, 1.0f, 0.0f, fadeAlpha);
      else
         itemFont.SetColour(0.0f, 0.5f, 0.0f, fadeAlpha);
      int x = (screen_w - all_w) / 2;
      itemFont.Print(x, y, (*it).name.c_str());

      if (i == selected)
         itemFont.SetColour(0.0f, 0.7f, 1.0f, fadeAlpha);
      else
         itemFont.SetColour(0.0f, 0.2f, 0.7f, fadeAlpha);
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

   helpFont.SetColour(0.0f, 1.0f, 0.0f, fadeAlpha);

   // TODO: Remove this once texture loading bug is fixed
#ifdef WIN32
   const int vertOffset = 125;
#else
   const int vertOffset = 100;
#endif

   const char* help1 = i18n("Use UP and DOWN to select options");
   x = (screen_w - helpFont.GetStringWidth(help1)) / 2;
   y = screen_h - vertOffset;
   helpFont.Print(x, y, help1);

   const char* help2 = i18n("Use LEFT and RIGHT to change values");
   x = (screen_w - helpFont.GetStringWidth(help2)) / 2;
   y += 25;
   helpFont.Print(x, y, help2);

   const char* help3 = i18n("Press FIRE or RETURN to exit");
   x = (screen_w - helpFont.GetStringWidth(help3)) / 2;
   y += 25;
   helpFont.Print(x, y, help3);

   // Temporary work around for texture reloading problem on Windows
#ifdef WIN32
   const char* help4 = i18n("Please restart the game to change the resolution");
   x = (screen_w - helpFont.GetStringWidth(help4)) / 2;
   y += 25;
   helpFont.Print(x, y, help4);
#endif
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

void Options::ParseResolutionString(const string& str, int* hres, int* vres) const
{
   char x;
   istringstream ss(str);
   ss >> *hres;
   ss >> x;
   assert(x == 'x');
   ss >> *vres;
}
