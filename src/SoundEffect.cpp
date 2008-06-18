/*  SoundEffect.cpp -- SDL Mixer wrapper.
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

#include "SoundEffect.hpp"

int SoundEffect::loadCount(0);
int SoundEffect::audioRate(22050);
int SoundEffect::audioChannels(2);
int SoundEffect::audioBuffers(4096);
Uint16 SoundEffect::audioFormat(AUDIO_S16);
bool SoundEffect::enabled(true);

SoundEffect::SoundEffect(const char *filename)
   : sound(NULL), channel(-1)
{
   if (++loadCount == 1) {
      
      if (Mix_OpenAudio(audioRate, audioFormat, audioChannels, audioBuffers)) {
         ostringstream ss;
         ss << "Failed to open audio: ";
         ss << Mix_GetError();
         throw runtime_error(ss.str());
      }

      // Get the actual settings used
      Mix_QuerySpec(&audioRate, &audioFormat, &audioChannels);
   }

   if (!(sound = Mix_LoadWAV(filename))) {
      ostringstream ss;
      ss << "Error loading " << filename << ": ";
      ss << Mix_GetError();
      throw runtime_error(ss.str());
   }
}

SoundEffect::~SoundEffect()
{
   if (channel != -1)
      Mix_HaltChannel(channel);

   Mix_FreeChunk(sound);
   
   if (--loadCount == 0)
      Mix_CloseAudio();
}

void SoundEffect::Play()
{
   if (enabled)
      channel = Mix_PlayChannel(-1, sound, 0);
}
