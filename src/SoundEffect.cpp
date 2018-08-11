//  SoundEffect.cpp -- SDL Mixer wrapper.
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

#include "SoundEffect.hpp"
#include "ConfigFile.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

int SoundEffect::loadCount(0);
int SoundEffect::audioRate(44100);
int SoundEffect::audioChannels(2);
int SoundEffect::audioBuffers(1024);
Uint16 SoundEffect::audioFormat(AUDIO_S16);
bool SoundEffect::enabled(true);

SoundEffect::SoundEffect(const string& filename, Uint8 volume)
#ifndef EMSCRIPTEN
   : sound(NULL),
     channel(-1)
#endif
{
#ifndef EMSCRIPTEN
   if (++loadCount == 1) {

      if (Mix_OpenAudio(audioRate, audioFormat, audioChannels, audioBuffers)) {
         cerr << "Failed to open audio: " << Mix_GetError() << endl;
         cerr << "(Disabling sound effects)" << endl;

         SetEnabled(false);
         return;
      }

      // Get the actual settings used
      Mix_QuerySpec(&audioRate, &audioFormat, &audioChannels);
   }

   if (!enabled) return;

   if (!(sound = Mix_LoadWAV(filename.c_str())))
      Die("Error loading %s: %s", filename.c_str(), Mix_GetError());

   sound->volume = volume;
#endif
}

SoundEffect::~SoundEffect()
{
#ifndef EMSCRIPTEN
   if (channel != -1)
      Mix_HaltChannel(channel);

   if (enabled)
      Mix_FreeChunk(sound);

   if (--loadCount == 0)
      Mix_CloseAudio();
#endif
}

void SoundEffect::Play()
{
#ifndef EMSCRIPTEN
   if (enabled)
      channel = Mix_PlayChannel(-1, sound, 0);
#endif
}
