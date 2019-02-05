//
// Screen.cpp - Implementation of the ScreenManager class.
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

#include "ScreenManager.hpp"
#include "OpenGL.hpp"

#include <string>
#include <cassert>
#include <stdexcept>

ScreenManager::ScreenManager()
   : m_active(NULL)
{
}

ScreenManager::~ScreenManager()
{

}

ScreenManager& ScreenManager::GetInstance()
{
   static ScreenManager sm;
   return sm;
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
   if (m_active != NULL)
      m_active->Process();
}

void ScreenManager::Display()
{
   if (m_active != NULL)
      m_active->Display();
}

void ScreenManager::RemoveAllScreens()
{
   m_screens.clear();
}
