//
// Copyright (C) 2006-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INC_PLATFORM_HPP
#define INC_PLATFORM_HPP

#include "config.h"

#ifndef DATADIR
#define DATADIR "."
#endif

////////////////////////////////////////////////////////////
// Windows
#if defined WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "SDL.h"

#include <GL/glew.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define i18n(x) x

#ifndef PATH_MAX
#define PATH_MAX  260
#endif

// Don't have these on Windows for some reason
#ifndef GL_BGRA
#define GL_BGRA 0
#endif
#ifndef GL_BGR
#define GL_BGR 0
#endif

// Define safe snprintf, etc. for MSCV8
#if (_MSC_VER >= 1400)

#define USE_FOPEN_S

#ifndef snprintf
#define snprintf sprintf_s
#endif

#undef vsnprintf
#define vsnprintf(b, l, s, a) vsnprintf_s(b, l, (l)-1, s, a)

#undef strncpy
#define strncpy strncpy_s

#else /* MSVC7 or earlier */

#ifndef snprintf
#define snprintf _snprintf
#endif

#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif

#endif /* #if (_MSC_VER >= 1400) */

////////////////////////////////////////////////////////////
// Mac OS X
#elif defined MACOSX

#include <GL/gl.h>

#define i18n(x) x

#include "SDL.h"

////////////////////////////////////////////////////////////
// Linux
#elif defined LINUX

//#define GL_GLEXT_PROTOTYPES
//#include <GL/gl.h>
//#include <GLES2/gl2.h>
#include <GL/glew.h>

#include <libintl.h>
#define i18n(x) gettext(x)

#include "SDL.h"

////////////////////////////////////////////////////////////
// Emscripten
#elif defined EMSCRIPTEN

#include <GL/gl.h>

#include "SDL.h"

#define i18n(x) x

#endif

#include <string>

using namespace std;

string LocateResource(const string& file);
string GetConfigDir();

void Die(const char *fmt, ...)
   __attribute__((noreturn, format(printf, 1, 2)));

#endif /* #ifdef INC_PLATFORM_HPP */
