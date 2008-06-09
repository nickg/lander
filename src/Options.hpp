/*  Options.hpp -- The options screen.
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

#ifndef INC_OPTIONS_HPP
#define INC_OPTIONS_HPP

#include "Platform.hpp"
#include "ScreenManager.hpp"
#include "Font.hpp"

class Options : public Screen {
public:
   Options();
   virtual ~Options() {}

   void Load();
   void Process();
   void Display();
private:
   void ProcessFadeIn();
   void ProcessMain();
   void ProcessFadeOut();
   
   void DisplayHelpText();
   void DisplayItems();

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
};

#endif
