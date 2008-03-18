/*
 * Screen.cpp - Implementation of the ScreenManager class.
 * Copyright (C) 2006  Nick Gasson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "ScreenManager.hpp"
#include "OpenGL.hpp"

/*
 * Instantiates the ScreenManager class. Never calls this directly. 
 */
ScreenManager::ScreenManager()
{
	m_active.loaded = false;
	m_active.ptr = NULL;
}


/*
 * Unloads all loaded m_screens.
 */
ScreenManager::~ScreenManager()
{
	ScreenMap::iterator it;

	for (it = m_screens.begin(); it != m_screens.end(); ++it)
	{
		if ((*it).second.ptr != NULL && (*it).second.loaded)
			(*it).second.ptr->Unload();
	}
}


/*
 * Returns the single instance of ScreenManager.
 */
ScreenManager &ScreenManager::GetInstance()
{
	static ScreenManager sm;

	return sm;
}


/*
 * Registers a new screen with the screen manager.
 *	id -> String identifier to reference the screen.
 *	ptr -> Pointer to instance of Screen class.
 */
void ScreenManager::AddScreen(const char *id, Screen *ptr)
{
	if (m_screens.find(string(id)) != m_screens.end())
		throw runtime_error("Screen already registered: " + string(id));

	ScreenData sd;
	sd.loaded = false;
	sd.ptr = ptr;

	m_screens[string(id)] = sd;
}


/* 
 * Changes the currently active screen.
 *	id -> Screen identifier to switch to.
 */
void ScreenManager::SelectScreen(const char *id)
{
	ScreenMap::iterator it;

	it = m_screens.find(string(id));

	if (it == m_screens.end())
		throw runtime_error("Screen does not exist: " + string(id));
	
	if (m_active.ptr != NULL)
	{
		m_active.ptr->Unload();
		m_active.loaded = false;
	}

	m_active = (*it).second;
	
	m_active.ptr->Load();
	m_active.loaded = true;
	m_screens[id] = m_active;

	// Allow the new screen to generate a frame
	OpenGL::GetInstance().SkipDisplay();
}


/*
 * Finds a screen given an identifier.
 */
Screen *ScreenManager::GetScreenById(const char *id) const
{
	ScreenMap::const_iterator it;

	it = m_screens.find(string(id));
	if (it == m_screens.end())
		throw runtime_error("Screen " + string(id) + " does not exist");
	else
		return (*it).second.ptr;
}


/*
 * Calls the Process method of the active screen.
 */
void ScreenManager::Process()
{
	if (m_active.ptr != NULL)
	{
		assert(m_active.loaded);
		m_active.ptr->Process();
	}
}


/*
 * Calls the Display method of the active screen.
 */
void ScreenManager::Display()
{
	if (m_active.ptr != NULL)
	{
		assert(m_active.loaded);
		m_active.ptr->Display();
	}
}
