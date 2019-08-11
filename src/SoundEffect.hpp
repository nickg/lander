//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
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
