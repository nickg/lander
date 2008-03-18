/*
 * Main.cpp - Program entry point.
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

#include "Lander.hpp"

#define DEBUG_WINDOW
#define DEBUG_WIN_X 800
#define DEBUG_WIN_Y 600

/* Globals */
DataFile *g_pData;

#include <SDL_main.h>

/* 
 * Entry point.
 */
int main(int argc, char **argv)
{
   int width, height, depth;
   bool fullscreen;

   // Get current resolution from windows
#ifdef DEBUG_WINDOW 
   width = DEBUG_WIN_X;
   height = DEBUG_WIN_Y;
   fullscreen = false;
#else
   // width = GetSystemMetrics(SM_CXSCREEN);
   // height = GetSystemMetrics(SM_CYSCREEN);
   width = 800;
   height = 600;
   fullscreen = true;
#endif

#ifdef WIN32
   // Work out colour depth
   HDC hDesktopDC = GetDC(GetDesktopWindow());
   depth = GetDeviceCaps(hDesktopDC, BITSPIXEL);
   ReleaseDC(GetDesktopWindow(), hDesktopDC);
#else
   // Use default colour depth
   depth = 0;
#endif

   try {
      // Open data file
      g_pData = new DataFile(File::LocateResource("lander", "dat"));

      // Create the game window
      OpenGL &opengl = OpenGL::GetInstance();
      opengl.Init(width, height, depth, fullscreen);

      // Create screens
      MainMenu mm;
      Game g;
      HighScores hs;

      ScreenManager &sm = ScreenManager::GetInstance();
      sm.AddScreen("MAIN MENU", &mm);
      sm.AddScreen("GAME", &g);
      sm.AddScreen("HIGH SCORES", &hs);

      // Load fonts
      FreeType &ft = FreeType::GetInstance();
      ft.LoadFont(ftNormal, File::LocateResource("Default_Font", "ttf"), 11);
      ft.LoadFont(ftBig, File::LocateResource("Default_Font", "ttf"), 20);
      ft.LoadFont(ftScore, File::LocateResource("Default_Font", "ttf"), 16);
      ft.LoadFont(ftHollow, File::LocateResource("Hollow_Font", "ttf"), 40);
      ft.LoadFont(ftScoreName, File::LocateResource("Default_Font", "ttf"), 14);
      ft.LoadFont(ftLarge, File::LocateResource("Default_Font", "ttf"), 15);
      
      // Run the game
      sm.SelectScreen("MAIN MENU");
      opengl.Run();

      // Close data file
      delete g_pData;
   }
   catch (std::runtime_error e) {
#ifdef WIN32
      MessageBox(NULL, e.what(), "Runtime Error", MB_OK | MB_ICONSTOP);
#else /* #ifdef WIN32 */
      fprintf(stderr, "Runtime Error: %s\n", e.what());
#endif /* #ifdef WIN32 */
   }

   return 0;
}
