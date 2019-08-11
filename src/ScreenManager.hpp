//
// Copyright (C) 2006-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
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
   void SelectScreen(const string& id);
   void Process();
   void Display();
   void RemoveAllScreens();
   Screen* GetScreenById(const string& id) const;
   Screen* GetActiveScreen() const;

private:
   Screen* SearchScreenById(const string& id) const;

   static const int MAX_SCREENS = 8;

   Screen *m_screens[MAX_SCREENS];
   int m_screenCount = 0;
   Screen *m_active = nullptr;
   TestDriver *m_testDriver;
};

#endif
