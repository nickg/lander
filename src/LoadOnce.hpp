/*  LoadOnce.hpp -- Macros for static initialisation.
 *  Copyright (C) 2008  Nick Gasson
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INC_LOADONCE_HPP
#define INC_LOADONCE_HPP

/*
 * The global variable _forceReload can be used to force all images
 * to reload. This works around an SDL bug (?) on Windows where all
 * texture information is lost after the resolution is changed.
 *
 * This is really a bit of a hack, but quite cool, eh? The variable
 * _forceReload gets incremented each time we have to reload the
 * images, and the static variable _hasLoaded in each constructor
 * stores the value of _forceReload the last time the images were
 * loaded -- we reload if the values differ.
 */

extern int _forceReload;

#define LOAD_ONCE \
   static int _hasLoaded = 0; \
   if ((_forceReload > _hasLoaded) && (_hasLoaded = _forceReload))

#endif
