//
// Copyright (C) 2006-2020  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "Platform.hpp"
#include "Game.hpp"
#include "Menu.hpp"
#include "HighScores.hpp"
#include "Options.hpp"
#include "ConfigFile.hpp"
#include "SoundEffect.hpp"

#include <iostream>
#include <filesystem>

#include <SDL_main.h>

namespace {
   MainMenu*   menu = NULL;
   Game*       game = NULL;
   HighScores* scores = NULL;
   Options*    options = NULL;
}

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

static void RecreateScreens()
{
   DestroyScreens();

   menu = new MainMenu();
   game = new Game();
   scores = new HighScores();
   options = new Options();

   ScreenManager& sm = ScreenManager::GetInstance();
   sm.AddScreen(menu);
   sm.AddScreen(game);
   sm.AddScreen(scores);
   sm.AddScreen(options);
}

#ifdef UNIX
static void MigrateConfigFiles()
{
   // Earlier versions of Lander stored config files directly in the
   // user's home directory. Now use use the XDG-compliant .config/lander
   // directory but we should move old configs and high scores there first

   using std::filesystem::path;

   const path cfg = GetConfigDir();
   const path home = getenv("HOME");

   const path old_config = home / ".lander.config";
   const path old_scores = home / ".lander.scores";

   if (exists(old_config))
      rename(old_config, cfg / "config");

   if (exists(old_scores))
      rename(old_scores, cfg / "scores");
}
#endif

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

   const int DEFAULT_HRES = 1024;
   const int DEFAULT_VRES = 768;
   const int DEFAULT_FSCREEN = false;
   const int DEFAULT_SOUND = true;

   cout << "Lunar Lander " << VERSION << endl << endl
        << "Copyright (C) 2006-2019 Nick Gasson" << endl
        << "This program comes with ABSOLUTELY NO WARRANTY. This is free "
        << "software, and" << endl
        << "you are welcome to redistribute it under certain conditions. "
        << "See the GNU" << endl
        << "General Public Licence for details." << endl << endl;

#ifdef UNIX
   MigrateConfigFiles();
#endif

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

   // Create the game window
   OpenGL& opengl = OpenGL::GetInstance();
   opengl.Init(width, height, depth, fullscreen);

   RecreateScreens();

   if (argc == 2 && strcmp(argv[1], "test") == 0)
      ScreenManager::GetInstance().SetTestDriver(makeSanityTestDriver());

   // Run the game
   ScreenManager::GetInstance().SelectScreen("MAIN MENU");
   opengl.Run();

   DestroyScreens();
   Texture::UnloadAll();

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
   using filesystem::path;

   static path datadir;

   if (datadir.empty()) {
      const char *meson_src = getenv("MESON_SOURCE_ROOT");
      if (meson_src != NULL) {
         cout << "Using data from source directory: " << meson_src << endl;
         datadir = path(meson_src) / "data";
      }
      else {
         cout << "Using data from installation directory: " << DATADIR << endl;
         datadir = DATADIR;
      }
   }

   return (datadir / file).string();
#else
   return file;
#endif
}

string GetConfigDir()
{
#if defined UNIX
   using filesystem::path;

   path p;
   const char *config = getenv("XDG_CONFIG_HOME");
   if (config == NULL || *config == '\0') {
      const char *home = getenv("HOME");
      if (home == NULL)
         Die("HOME not set");

      p = home;
      p /= ".config";
   }
   else
      p = config;

   p /= "lander";
   create_directories(p);

   return p.string() + "/";
#elif defined WIN32
   using filesystem::path;

   path appdata(getenv("APPDATA"));
   appdata /= "doof.me.uk";
   appdata /= "Lander";
   create_directories(appdata);
   return appdata.string() + "\\";
#elif defined EMSCRIPTEN
   return "";
#else
#error "Need to port GetConfigDir to this platform"
#endif
}

void Die(const char *fmt, ...)
{
   va_list ap;
   va_start(ap, fmt);

#ifdef WIN32
   int len = _vscprintf(fmt, ap);
   char *buf = new char[len + 1];
   vsprintf_s(buf, len + 1, fmt, ap);

   fputs(buf, stderr);
   fputs("\r\n", stderr);
   fflush(stderr);

   MessageBox(NULL, buf, "Runtime Error", MB_OK | MB_ICONSTOP);

   delete[] buf;
#else
   vfprintf(stderr, fmt, ap);
   fprintf(stderr, "\n");
   fflush(stderr);
#endif

   va_end(ap);
   exit(EXIT_FAILURE);
}
