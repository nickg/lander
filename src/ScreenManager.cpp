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
{
   active.loaded = false;
   active.ptr = NULL;
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
   if (screens.find(id) != screens.end())
      throw runtime_error("Screen already registered: " + id);

   ScreenData sd;
   sd.loaded = false;
   sd.ptr = ptr;

   screens[id] = sd;
}

void ScreenManager::SelectScreen(const string& id)
{
   ScreenMapIt it = screens.find(id);

   if (it == screens.end())
      throw runtime_error("Screen does not exist: " + id);
	
   active = (*it).second;
	
   active.ptr->Load();
   active.loaded = true;
   screens[id] = active;
   
   // Allow the new screen to generate a frame
   OpenGL::GetInstance().SkipDisplay();
}

Screen* ScreenManager::GetScreenById(const string& id) const
{
   ScreenMap::const_iterator it;

   it = screens.find(id);
   if (it == screens.end())
      throw runtime_error("Screen " + id + " does not exist");
   else
      return (*it).second.ptr;
}

void ScreenManager::Process()
{
   if (active.ptr != NULL)	{
      assert(active.loaded);
      active.ptr->Process();
   }
}

void ScreenManager::Display()
{
   if (active.ptr != NULL) {
      assert(active.loaded);
      active.ptr->Display();
   }
}

void ScreenManager::RemoveAllScreens()
{
   screens.clear();
}
