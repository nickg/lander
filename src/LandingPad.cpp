/*  LandingPad.cpp -- Where the player can land.
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

#include "LandingPad.hpp"
#include "DataFile.hpp"

extern DataFile *g_pData;

Texture LandingPad::s_landtex, LandingPad::s_nolandtex;

// TODO: No! get this from the surface class
#define SURFACE_SIZE 20
#define MAX_SURFACE_HEIGHT  300

LandingPad::LandingPad(Viewport *v, int index, int length)
   : viewport(v), index(index), length(length)
{
   quad.x = index * SURFACE_SIZE;
   quad.width = length * SURFACE_SIZE;
   quad.height = 16;
   quad.uTexture = s_landtex;
}

/*
 * Loads landing pad graphics.
 */
void LandingPad::Load()
{
   OpenGL &opengl = OpenGL::GetInstance();

   s_landtex = opengl.LoadTextureAlpha(g_pData, "LandingPad.bmp");
   s_nolandtex = opengl.LoadTextureAlpha(g_pData, "LandingPadRed.bmp");
} 

/*
 * Draws the landing pad in the current frame.
 *	locked -> If true, pads a drawn with the red texture.
 */
void LandingPad::Draw(bool locked)
{
   quad.uTexture = locked ? s_nolandtex : s_landtex;
   quad.x = index * SURFACE_SIZE - viewport->GetXAdjust();
   quad.y = viewport->GetLevelHeight() - viewport->GetYAdjust()
      - MAX_SURFACE_HEIGHT + ypos;
   OpenGL::GetInstance().Draw(&quad);
}
