//
// Copyright (C) 2009-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "InterfaceSounds.hpp"
#include "SoundEffect.hpp"

#include <string>

void InterfaceSounds::PlayBleep()
{
   static SoundEffect bleepSound(LocateResource("sounds/bleep.wav"));

   bleepSound.Play();
}

void InterfaceSounds::PlaySelect()
{
   static SoundEffect selectSound(LocateResource("sounds/select.wav"));

   selectSound.Play();
}
