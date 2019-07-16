//
// Screens.hpp - Definition of abstract class to represent game screens.
// Copyright (C) 2006, 2011  Nick Gasson
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

#ifndef INC_SCREENS_HPP
#define INC_SCREENS_HPP

#include "Platform.hpp"
#include "TestDriver.hpp"

//
// A screen within the game that can be displayed.
//
class Screen {
public:
   Screen() = default;
   Screen(const Screen&) = delete;

   virtual ~Screen() { }
   virtual void Load() { }
   virtual void Display() { }
   virtual void Process() { }

   virtual const char *GetName() const = 0;
};


//
// Manages the transition between the various screens in the game.
//
class ScreenManager {
public:
   static ScreenManager& GetInstance();

   void AddScreen(Screen* ptr);
   void SetTestDriver(TestDriver *driver);
   void SelectScreen(const char *id);
   void Process();
   void Display();
   void RemoveAllScreens();
   Screen* GetScreenById(const char *id) const;
   Screen* GetActiveScreen() const;

private:
   Screen* SearchScreenById(const char *id) const;

   static const int MAX_SCREENS = 8;

   Screen *m_screens[MAX_SCREENS];
   int m_screenCount = 0;
   Screen *m_active = nullptr;
   TestDriver *m_testDriver;
};

#endif
