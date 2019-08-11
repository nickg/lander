//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INC_FADE_HPP
#define INC_FADE_HPP

#include "Platform.hpp"
#include "OpenGL.hpp"

class Fade {
public:
   static const float DEFAULT_FADE_SPEED;

   Fade(float s = DEFAULT_FADE_SPEED);

   void BeginFadeIn();
   void BeginFadeOut();
   void Display() const;
   bool Process();
private:
   enum State { fNone, fIn, fOut };

   State        m_state;
   float        m_alpha;
   const float  m_speed;
   VertexBuffer m_vbo;
};

#endif
