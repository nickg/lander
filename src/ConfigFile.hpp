//
//  ConfigFile.hpp -- Settings persistence.
//  Copyright (C) 2008-2009  Nick Gasson
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef INC_CONFIGFILE_HPP
#define INC_CONFIGFILE_HPP

#include "Platform.hpp"

class ConfigFile {
public:
   ConfigFile(string filename = ".lander.config");
   ~ConfigFile();

   void Flush();
   
   bool has(const string& key) const;

   const string& get(const string& key);
   const string& get_string(const string& key, const string& def = "");
   int get_int(const string& key, int def = 0);
   bool get_bool(const string& key, bool def = false);
   
   void put(string key, string value);
   void put(string key, int value);
   void put(string key, bool value);
private:
   map<string, string> settings;
   bool dirty;
   string filename;
};

#endif
