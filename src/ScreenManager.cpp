//
// Copyright (C) 2006-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "ScreenManager.hpp"
#include "OpenGL.hpp"

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

void ScreenManager::AddScreen(Screen* ptr)
{
   if (m_screenCount == MAX_SCREENS)
      Die("Too many screens");

   if (SearchScreenById(ptr->GetName()) != nullptr)
      Die("Screen already registered: %s", ptr->GetName());

   m_screens[m_screenCount++] = ptr;
}

Screen* ScreenManager::GetActiveScreen() const
{
   if (m_active == nullptr)
      Die("No active screen");

   return m_active;
}

void ScreenManager::SelectScreen(const char *id)
{
   m_active = GetScreenById(id);
   m_active->Load();

   // Allow the new screen to generate a frame
   OpenGL::GetInstance().SkipDisplay();
}

Screen* ScreenManager::SearchScreenById(const char *id) const
{
   for (int i = 0; i < m_screenCount; i++) {
      if (strcmp(id, m_screens[i]->GetName()) == 0)
         return m_screens[i];
   }

   return nullptr;
}

Screen* ScreenManager::GetScreenById(const char *id) const
{
   Screen *s = SearchScreenById(id);
   if (s == nullptr)
      Die("Screen %s does not exist", id);

   return s;
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
   for (int i = 0; i < m_screenCount; i++)
      m_screens[i] = nullptr;
   m_active = nullptr;
   m_screenCount = 0;
}
