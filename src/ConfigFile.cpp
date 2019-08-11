//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "ConfigFile.hpp"

#include <stdexcept>
#include <sstream>
#include <fstream>

ConfigFile::ConfigFile(const string& filename)
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
   Flush();
}

void ConfigFile::Flush()
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

      dirty = false;
   }
}

bool ConfigFile::has(const string& key) const
{
   return settings.find(key) != settings.end();
}

const string& ConfigFile::get(const string& key)
{
   if (!has(key))
      Die("%s not in config file", key.c_str());
   else
      return settings[key];
}

const string& ConfigFile::get_string(const string& key, const string& def)
{
   if (has(key))
      return get(key);
   else {
      put(key, def);
      return def;
   }
}

int ConfigFile::get_int(const string& key, int def)
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

bool ConfigFile::get_bool(const string& key, bool def)
{
   if (has(key)) {
      const string& value = get(key);
      if (value == "true")
         return true;
      else if (value == "false")
         return false;
      else
         Die("'%s' is not boolean", value.c_str());
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
