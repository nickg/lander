//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INC_KEY_HPP
#define INC_KEY_HPP

#include "GameObjFwd.hpp"
#include "AnimatedImage.hpp"
#include "ObjectGrid.hpp"

enum ArrowColour { acBlue, acRed, acYellow, acPink, acGreen };

class Key : public StaticObject {
public:
   Key(bool active, int xpos, int ypos, ArrowColour acol);

   void DrawKey(Viewport* viewport);
   void DrawArrow(Viewport* viewport) const;
   void DrawIcon(int offset, float minAlpha) const;
   bool CheckCollision(Ship& ship) const;

   void Collected() { active = false; }

private:
   static const int KEY_FRAMES = 18;
   static constexpr float KEY_ROTATION_SPEED = 1.0f;
   static const int ARROW_SIZE = 32;

   float m_rotateAnim = 0.0f;
   float alpha;
   bool active;
   AnimatedImage image;
   Image arrow;

   static string KeyFileName(ArrowColour col);
   static string ArrowFileName(ArrowColour col);
};

#endif
