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
#include "Menu.hpp"
#include "HighScores.hpp"
#include "Options.hpp"
#include "ConfigFile.hpp"
#include "SoundEffect.hpp"

#include <SDL_main.h>

static MainMenu *menu = NULL;
static Game *game = NULL;
static HighScores *scores = NULL;
static Options *options = NULL;

static void DestroyScreens()
{
   ScreenManager::GetInstance().RemoveAllScreens();
   if (menu)
      delete menu;
   if (game)
      delete game;
   if (scores)
      delete scores;
   if (options)
      delete options;
}

/*
 * Recreate all the screens. After a resolution switch for
 * example.
 */
void RecreateScreens()
{
   DestroyScreens();
   
   menu = new MainMenu();
   game = new Game();
   scores = new HighScores();
   options = new Options();
   
   ScreenManager &sm = ScreenManager::GetInstance();
   sm.AddScreen("MAIN MENU", menu);
   sm.AddScreen("GAME", game);
   sm.AddScreen("HIGH SCORES", scores);
   sm.AddScreen("OPTIONS", options);
}

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

   const int DEFAULT_HRES = 800;
   const int DEFAULT_VRES = 600;
   const int DEFAULT_FSCREEN = false;
   const int DEFAULT_SOUND = true;

   {
      ConfigFile cfile;   
      width = cfile.get_int("hres", DEFAULT_HRES);
      height = cfile.get_int("vres", DEFAULT_VRES);
      fullscreen = cfile.get_bool("fullscreen", DEFAULT_FSCREEN);
      SoundEffect::SetEnabled(cfile.get_bool("sound", DEFAULT_SOUND));
   }

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

      RecreateScreens();
      
      // Run the game
      ScreenManager::GetInstance().SelectScreen("MAIN MENU");
      opengl.Run();

      DestroyScreens();
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

string GetConfigDir()
{
#ifdef UNIX
   return string(getenv("HOME")) + "/";
#else
#error "Need to port GetConfigDir to this platform"
#endif
}
