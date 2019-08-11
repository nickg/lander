//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INC_CONFIGFILE_HPP
#define INC_CONFIGFILE_HPP

#include "Platform.hpp"

#include <string>
#include <map>

class ConfigFile {
public:
   ConfigFile(const string& filename = "config");
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
