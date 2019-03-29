//
//  SoundEffect.hpp -- SDL Mixer wrapper.
//  Copyright (C) 2008-2019  Nick Gasson
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

#ifndef INC_SOUNDEFFECT_HPP
#define INC_SOUNDEFFECT_HPP

#include "Platform.hpp"

#ifndef EMSCRIPTEN
#include <SDL_mixer.h>
#else
#include <SDL/SDL_mixer.h>
#endif

class SoundEffect {
public:
   SoundEffect(const string& filename, Uint8 volume = MIX_MAX_VOLUME);
   SoundEffect(const SoundEffect&) = delete;
   ~SoundEffect();

   static void SetEnabled(bool state) { enabled = state; }

   void Play();
private:
   Mix_Chunk* sound;
   int channel;

   static int loadCount;
   static int audioChannels, audioBuffers, audioRate;
   static Uint16 audioFormat;

   static bool enabled;
};

#endif
