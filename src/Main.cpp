//
// Main.cpp - Program entry point.
// Copyright (C) 2006-2009  Nick Gasson
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

#include "Platform.hpp"
#include "Game.hpp"
#include "Menu.hpp"
#include "HighScores.hpp"
#include "Options.hpp"
#include "ConfigFile.hpp"
#include "SoundEffect.hpp"

#include <iostream>

#ifdef MACOSX
namespace CF {
#include "CoreFoundation/CoreFoundation.h"
}
#endif

#include <SDL_main.h>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

namespace {
   MainMenu* menu = NULL;
   Game* game = NULL;
   HighScores* scores = NULL;
   Options* options = NULL;

   void DestroyScreens()
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
}

//
// Recreate all the screens. After a resolution switch for
// example.
//
void RecreateScreens()
{
   DestroyScreens();

   menu = new MainMenu();
   game = new Game();
   scores = new HighScores();
   options = new Options();
   
   ScreenManager& sm = ScreenManager::GetInstance();
   sm.AddScreen("MAIN MENU", menu);
   sm.AddScreen("GAME", game);
   sm.AddScreen("HIGH SCORES", scores);
   sm.AddScreen("OPTIONS", options);
}

// 
// Entry point.
//
int main(int argc, char **argv)
{
   int width, height, depth;
   bool fullscreen;

#ifdef LOCALEDIR
   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);
#endif

   const int DEFAULT_HRES = 800;
   const int DEFAULT_VRES = 600;
   const int DEFAULT_FSCREEN = false;
   const int DEFAULT_SOUND = true;

   cout << "Lunar Lander " << VERSION << endl << endl
        << "Copyright (C) 2006-2009  Nick Gasson" << endl
        << "This program comes with ABSOLUTELY NO WARRANTY. This is free software, and" << endl
        << "you are welcome to redistribute it under certain conditions. See the GNU" << endl
        << "General Public Licence for details." << endl << endl;

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
      OpenGL& opengl = OpenGL::GetInstance();
      opengl.Init(width, height, depth, fullscreen);

      RecreateScreens();
      
      // Run the game
      ScreenManager::GetInstance().SelectScreen("MAIN MENU");
      opengl.Run();

      DestroyScreens();
   }
   catch (const std::runtime_error& e) {
#ifdef WIN32
      MessageBox(NULL, e.what(), "Runtime Error", MB_OK | MB_ICONSTOP);
#else /* #ifdef WIN32 */
      fprintf(stderr, i18n("Runtime Error: %s\n"), e.what());
#endif /* #ifdef WIN32 */
   }

   return 0;
}

//
// Find a filename in the installation tree.
//
string LocateResource(const string& file)
{
#ifdef MACOSX
   using namespace CF;
   
   static char path[PATH_MAX];
   
   CFURLRef resURL;
   CFBundleRef mainBundle;
   CFStringRef cfBase, cfExt, cfPath;
   
   const char* ext = "";
   char* copy = strdup(file.c_str());
   if (char* p = strrchr(copy, '.')) {
		*p = '\0';
		ext = ++p;
   }
    
   cfBase = CFStringCreateWithCString(NULL, copy, kCFStringEncodingASCII);
   cfExt = CFStringCreateWithCString(NULL, ext, kCFStringEncodingASCII);
   
	free(copy); 
	
   mainBundle = CFBundleGetMainBundle();
    
   resURL = CFBundleCopyResourceURL(mainBundle, cfBase, cfExt, NULL);
    
   if (resURL == NULL)
      throw runtime_error("Failed to locate " + string(file));
	
   cfPath = CFURLCopyPath(resURL);
    
   CFStringGetCString(cfPath, path, PATH_MAX, kCFStringEncodingASCII);

   return path;
#endif
   
#ifdef DATADIR
   return boost::lexical_cast<string>(DATADIR) + "/" + file;   
#else
   return file;
#endif
}

string GetConfigDir()
{
#ifdef UNIX
   return string(getenv("HOME")) + "/";
#else
#ifdef WIN32
   using namespace boost::filesystem;

   path appdata(getenv("APPDATA"));
   appdata /= "doof.me.uk";
   appdata /= "Lander";
   create_directories(appdata);
   return appdata.file_string() + "\\";
#else
#error "Need to port GetConfigDir to this platform"
#endif
#endif
}
