//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "Fade.hpp"
#include "OpenGL.hpp"

#include <cassert>

const float Fade::DEFAULT_FADE_SPEED(0.05f);

Fade::Fade(float s)
   : m_state(fNone),
     m_alpha(0.0f),
     m_speed(s)
{
   const int w = OpenGL::GetInstance().GetWidth();
   const int h = OpenGL::GetInstance().GetHeight();

   const VertexI vertices[4] = {
      { 0, 0, 0.0f, 0.0f },
      { 0, h, 0.0f, 1.0f },
      { w, h, 1.0f, 1.0f },
      { w, 0, 1.0f, 0.0f }
   };

   m_vbo = VertexBuffer::Make(vertices, 4);
}

void Fade::BeginFadeIn()
{
   assert(m_state == fNone);

   m_state = fIn;
   m_alpha = 1.0f;
}

void Fade::BeginFadeOut()
{
   assert(m_state == fNone);

   m_state = fOut;
   m_alpha = 0.0f;
}

bool Fade::Process()
{
   const OpenGL::TimeScale timeScale = OpenGL::GetInstance().GetTimeScale();

   switch (m_state) {
   case fOut:
      m_alpha += min(m_speed * timeScale, 1.0f);
      if (m_alpha >= 1.0f) {
         m_state = fNone;
         return true;
      }
      else
         return false;
   case fIn:
      m_alpha -= max(m_speed * timeScale, 0.0f);
      if (m_alpha <= 0.0f) {
         m_state = fNone;
         return true;
      }
      else
         return false;
   default:
      assert(false);
   }
}

void Fade::Display() const
{
   OpenGL& opengl = OpenGL::GetInstance();

   opengl.Reset();
   opengl.SetColour(0.0f, 0.0f, 0.0f, m_alpha);
   opengl.Draw(m_vbo);
}
