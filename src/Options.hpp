//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INC_OPTIONS_HPP
#define INC_OPTIONS_HPP

#include "Platform.hpp"
#include "ScreenManager.hpp"
#include "Font.hpp"
#include "ConfigFile.hpp"

class Options : public Screen {
public:
   Options();

   void Load();
   void Process();
   void Display();

   const char *GetName() const override { return "OPTIONS"; }

private:
   void ProcessFadeIn();
   void ProcessMain();
   void ProcessFadeOut();

   void DisplayHelpText();
   void DisplayItems();

   string MakeResolutionString(int hres, int vres) const;
   void ParseResolutionString(const string& str, int* hres, int* vres) const;

   void Apply();

   static const double FADE_SPEED;

   enum OptionState {
      optFadeIn, optMain, optFadeOut
   };

   typedef vector<string> StringList;
   typedef StringList::iterator StringListIt;

   struct Item {
      string name;
      unsigned int active;
      StringList values;
   };
   typedef vector<Item> ItemList;
   typedef ItemList::iterator ItemListIt;

   OptionState state;
   Font helpFont, itemFont;
   double fadeAlpha;
   ItemList items;
   unsigned int selected;
   ConfigFile cfile;
};

#endif
