//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "LandingPad.hpp"
#include "Surface.hpp"
#include "OpenGL.hpp"
#include "Texture.hpp"
#include "Viewport.hpp"

#include <string>

LandingPad::LandingPad(Viewport* v, int index, int length)
   : index(index), length(length), viewport(v),
     m_landTexture(Texture::Load("images/landingpad.png")),
     m_noLandTexture(Texture::Load("images/landingpadred.png"))
{
   const int width = length * Surface::SURFACE_SIZE;
   const int height = 16;

   const VertexI vertices[4] = {
      { 0, height, 0.0f, 0.0f },
      { 0, 0, 0.0f, 1.0f },
      { width, 0, 1.0f, 1.0f },
      { width, height, 1.0f, 0.0f }
   };

   m_vbo = VertexBuffer::Make(vertices, 4);
}

//
// Draws the landing pad in the current frame.
//	locked -> If true, pads a drawn with the red texture.
//
void LandingPad::Draw(bool locked) const
{
   OpenGL& opengl = OpenGL::GetInstance();

   opengl.Reset();
   opengl.SetTexture(locked ? m_noLandTexture : m_landTexture);
   opengl.SetTranslation(index * Surface::SURFACE_SIZE - viewport->GetXAdjust(),
                         viewport->GetLevelHeight() - viewport->GetYAdjust()
                         - Surface::MAX_SURFACE_HEIGHT + ypos);
   opengl.Draw(m_vbo);
}
