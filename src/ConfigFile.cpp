/*  ConfigFile.cpp -- Settings persistence.
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

#include "ConfigFile.hpp"

ConfigFile::ConfigFile(string filename)
   : dirty(false), filename(filename)
{
   ifstream ifs((GetConfigDir() + filename).c_str(), ifstream::in);
   
   while (ifs.good()) {
      string key, value;
      ifs >> key;
      ifs >> value;

      if (key.size() == 0 || value.size() == 0)
         break;
      
      settings[key] = value;
   }

   ifs.close();
}

ConfigFile::~ConfigFile()
{
   if (dirty) {
      ofstream of((GetConfigDir() + filename).c_str());

      for (map<string, string>::iterator it = settings.begin();
           it != settings.end();
           ++it) {
         of << (*it).first;
         of << " ";
         of << (*it).second;
         of << endl;
      }
   }
}

bool ConfigFile::has(const string &key) const
{
   return settings.find(key) != settings.end();
}

const string &ConfigFile::get(const string &key)
{
   if (!has(key))
      throw runtime_error(key + " not in config file");
   else
      return settings[key];
}

const string &ConfigFile::get_string(const string &key, const string &def)
{
   if (has(key))
      return get(key);
   else {
      put(key, def);
      return def;
   }
}

int ConfigFile::get_int(const string &key, int def)
{
   if (has(key)) {
      istringstream is(get(key));
      int i;
      is >> i;
      return i;
   }
   else {
      ostringstream os;
      os << def;
      put(key, os.str());
      return def;
   }     
}

bool ConfigFile::get_bool(const string &key, bool def)
{
   if (has(key)) {
      const string &value = get(key);
      if (value == "true")
         return true;
      else if (value == "false")
         return false;
      else
         throw runtime_error(value + " is not boolean");
   }
   else {
      put(key, def);
      return def;
   }      
}

void ConfigFile::put(string key, string value)
{
   settings[key] = value;
   dirty = true;
}

void ConfigFile::put(string key, int value)
{
   ostringstream ss;
   ss << value;
   put(key, ss.str());
}

void ConfigFile::put(string key, bool value)
{
   put(key, string(value ? "true" : "false"));
}

