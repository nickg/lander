//
// Copyright (C) 2006-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "ScreenManager.hpp"
#include "OpenGL.hpp"

#include <string>
#include <cassert>

ScreenManager& ScreenManager::GetInstance()
{
   static ScreenManager sm;
   return sm;
}

void ScreenManager::SetTestDriver(TestDriver *driver)
{
   assert(m_testDriver == nullptr);
   m_testDriver = driver;
}

void ScreenManager::AddScreen(const string& id, Screen* ptr)
{
   if (m_screens.find(id) != m_screens.end())
      Die("Screen already registered: %s", id.c_str());

   m_screens[id] = ptr;
}

void ScreenManager::SelectScreen(const string& id)
{
   ScreenMapIt it = m_screens.find(id);

   if (it == m_screens.end())
      Die("Screen does not exist: %s", id.c_str());

   m_active = (*it).second;
   m_active->Load();

   m_screens[id] = m_active;

   // Allow the new screen to generate a frame
   OpenGL::GetInstance().SkipDisplay();
}

Screen* ScreenManager::GetScreenById(const string& id) const
{
   ScreenMap::const_iterator it;

   it = m_screens.find(id);
   if (it == m_screens.end())
      Die("Screen %s does not exist", id.c_str());
   else
      return (*it).second;
}

void ScreenManager::Process()
{
   if (m_active != nullptr) {
      if (m_testDriver != nullptr)
         m_testDriver->Poll();

      m_active->Process();
   }
}

void ScreenManager::Display()
{
   if (m_active != nullptr)
      m_active->Display();
}

void ScreenManager::RemoveAllScreens()
{
   m_screens.clear();
}
