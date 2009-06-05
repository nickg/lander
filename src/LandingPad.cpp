//  LandingPad.cpp -- Where the player can land.
//  Copyright (C) 2008  Nick Gasson
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

#include "LandingPad.hpp"
#include "Surface.hpp"
#include "OpenGL.hpp"
#include "Texture.hpp"
#include "Viewport.hpp"

LandingPad::LandingPad(Viewport* v, int index, int length)
   : index(index), length(length), viewport(v),
     landTexture_(LoadTexture("images/landingpad.png")),
     noLandTexture_(LoadTexture("images/landingpadred.png"))
{   
   quad.x = index * Surface::SURFACE_SIZE;
   quad.width = length * Surface::SURFACE_SIZE;
   quad.height = 16;
   quad.uTexture = landTexture_->GetGLTexture();
}

//
// Draws the landing pad in the current frame.
//	locked -> If true, pads a drawn with the red texture.
//
void LandingPad::Draw(bool locked)
{
   quad.uTexture = (locked ? noLandTexture_ : landTexture_)->GetGLTexture();
   quad.x = index * Surface::SURFACE_SIZE - viewport->GetXAdjust();
   quad.y = viewport->GetLevelHeight() - viewport->GetYAdjust()
      - Surface::MAX_SURFACE_HEIGHT + ypos;
   OpenGL::GetInstance().Draw(&quad);
}
