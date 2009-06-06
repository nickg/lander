//
// Platform.hpp -- Platform specific includes.
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

#ifndef INC_PLATFORM_HPP
#define INC_PLATFORM_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define DATADIR "."
#endif

// Windows specific constants and includes */
#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "SDL.h"

#include <GL\gl.h>							
#include <GL\glu.h>	

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#include <boost/tr1/memory.hpp>

#define msleep(n) Sleep(n)

#define i18n(x) x

#define PATH_MAX  256

// Don't have these on Windows for some reason */
#ifndef GL_BGRA
#define GL_BGRA 0
#endif
#ifndef GL_BGR
#define GL_BGR 0
#endif

// Define safe snprintf, etc. for MSCV8 */
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

#else /* #ifdef WIN32 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// Probably using GCC
#include <tr1/memory>

#endif /* #ifdef WIN32 */

// Mac OS X specifics */
#ifdef MACOSX

#include <GL/gl.h>
#include <GL/glu.h>

#define msleep(n) usleep((n)*1000)

#define i18n(x) x

#include "SDL.h"

#endif /* #ifdef MACOSX */

// Linux specifics */
#ifdef LINUX

#include <GL/gl.h>
#include <GL/glu.h>

#include <libintl.h>
#define i18n(x) gettext(x)

#define msleep(n) usleep((n)*1000)

#include "SDL.h"

#endif /* #ifdef LINUX */

using namespace std;
using namespace std::tr1;

void RecreateScreens();
string LocateResource(const string& file);
string GetConfigDir();

#endif /* #ifdef INC_PLATFORM_HPP */
