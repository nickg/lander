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

#include "Game.hpp"

#define DEBUG_WINDOW
#define DEBUG_WIN_X 800
#define DEBUG_WIN_Y 600

#include <SDL_main.h>

/* 
 * Entry point.
 */
int main(int argc, char **argv)
{
   int width, height, depth;
   bool fullscreen;

   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);
   
   // Get current resolution from Windows
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
      // Create the game window
      OpenGL &opengl = OpenGL::GetInstance();
      opengl.Init(width, height, depth, fullscreen);

      // Create screens
      MainMenu *mm = new MainMenu();
      Game *g = new Game();
      HighScores *hs = new HighScores();

      ScreenManager &sm = ScreenManager::GetInstance();
      sm.AddScreen("MAIN MENU", mm);
      sm.AddScreen("GAME", g);
      sm.AddScreen("HIGH SCORES", hs);

      // Run the game
      sm.SelectScreen("MAIN MENU");
      opengl.Run();
      
      sm.RemoveAllScreens();
      delete mm;
      delete g;
      delete hs;
   }
   catch (std::runtime_error e) {
#ifdef WIN32
      MessageBox(NULL, e.what(), "Runtime Error", MB_OK | MB_ICONSTOP);
#else /* #ifdef WIN32 */
      fprintf(stderr, i18n("Runtime Error: %s\n"), e.what());
#endif /* #ifdef WIN32 */
   }

   return 0;
}

/*
 * Find a filename in the installation tree.
 */
const char *LocateResource(const char *file)
{
#ifdef MACOSX
   static char path[MAX_RES_PATH];
   
   CFURLRef resURL;
   CFBundleRef mainBundle;
   CFStringRef cfBase, cfExt, cfPath;
    
   cfBase = CFStringCreateWithCString(NULL, base, kCFStringEncodingASCII);
   cfExt = CFStringCreateWithCString(NULL, ext, kCFStringEncodingASCII);
    
   mainBundle = CFBundleGetMainBundle();
    
   resURL = CFBundleCopyResourceURL(mainBundle, cfBase, cfExt, NULL);
    
   if (resURL == NULL)
      throw runtime_error("Failed to locate " + string(base) + "." + string(ext));
	
   cfPath = CFURLCopyPath(resURL);
    
   CFStringGetCString(cfPath, path, MAX_RES_PATH, kCFStringEncodingASCII);

   return patch;
#endif
   
#ifdef DATADIR
   static char path[PATH_MAX];
   snprintf(path, PATH_MAX, "%s/%s", DATADIR, file);
   return path;   
#else
   return file;
#endif
}

bool FileExists(const char *file)
{
#ifdef UNIX
   struct stat buf;
   return stat(file, &buf) == 0;
#else
   FILE *f = fopen(file, "r");
   if (NULL == f)
      return false;
   else {
      fclose(f);
      return true;
   }
#endif   
}
