//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
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
   : sound(NULL),
     channel(-1)
{
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
}

SoundEffect::~SoundEffect()
{
   if (channel != -1)
      Mix_HaltChannel(channel);

   if (enabled)
      Mix_FreeChunk(sound);

   if (--loadCount == 0)
      Mix_CloseAudio();
}

void SoundEffect::Play()
{
   if (enabled)
      channel = Mix_PlayChannel(-1, sound, 0);
}
