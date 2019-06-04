//
//  ElectricGate.cpp -- Electric gateway thingys.
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

#include "ElectricGate.hpp"
#include "Ship.hpp"
#include "OpenGL.hpp"

#include <string>

ElectricGate::ElectricGate(Viewport* v, int length, bool vertical, int x, int y)
   : StaticObject(x, y), length(length), vertical(vertical), viewport(v),
     gateImage("images/gateway.png")
{
   lightning.Build(length * OBJ_GRID_SIZE, vertical);

   m_timer = rand() % 70 + 10;
}

bool ElectricGate::CheckCollision(Ship& ship)
{
   int dx = vertical ? 0 : length;
   int dy = vertical ? length : 0;
   if (m_timer > GATEWAY_ACTIVE) {
      bool collide1 = ship.BoxCollision
         (xpos*OBJ_GRID_SIZE,
          ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP,
          OBJ_GRID_SIZE,
          OBJ_GRID_SIZE);

      bool collide2 = ship.BoxCollision
         ((xpos + dx)*OBJ_GRID_SIZE,
          (ypos + dy)*OBJ_GRID_SIZE + OBJ_GRID_TOP,
          OBJ_GRID_SIZE,
          OBJ_GRID_SIZE);

      return collide1 || collide2;
   }
   else {
      return ship.BoxCollision
         (xpos*OBJ_GRID_SIZE,
          ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP,
          (dx + 1)*OBJ_GRID_SIZE,
          (dy + 1)*OBJ_GRID_SIZE);
   }
}

void ElectricGate::Draw()
{
   // Draw first sphere
   int draw_x = xpos*OBJ_GRID_SIZE - viewport->GetXAdjust();
   int draw_y = ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP - viewport->GetYAdjust();
   gateImage.Draw(draw_x, draw_y);

   // Draw second sphere
   if (vertical) {
      draw_x = xpos*OBJ_GRID_SIZE - viewport->GetXAdjust();
      draw_y = (ypos+length)*OBJ_GRID_SIZE + OBJ_GRID_TOP - viewport->GetYAdjust();
   }
   else {
      draw_x = (xpos+length)*OBJ_GRID_SIZE - viewport->GetXAdjust();
      draw_y = ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP - viewport->GetYAdjust();
   }
   gateImage.Draw(draw_x, draw_y);

   // Draw the electricity stuff
   m_timer -= OpenGL::GetInstance().GetTimeScale();
   if (m_timer < GATEWAY_ACTIVE) {
      float x = xpos*OBJ_GRID_SIZE + 16 - viewport->GetXAdjust();
      float y = ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP + 16 - viewport->GetYAdjust();

      lightning.Draw(x, y);

      if (static_cast<int>(m_timer) % 5 == 0)
         lightning.Build(length * OBJ_GRID_SIZE, vertical);

      // Reset timer
      if (m_timer < 0.0f)
         m_timer = 100.0f;
   }
}

void Lightning::Build(int length, bool vertical)
{
   m_line.SwapXandY(vertical);

   const int POINT_STEP = 20;
   int npoints = (length / POINT_STEP) + 1;
   float delta = (float)length / (float)(npoints - 1);

   VertexF *vertices = new VertexF[npoints];

   const float SWING_SIZE = 5;
   const float MAX_OUT = 25;
   float y = 0;
   for (int i = 0; i < npoints; i++) {
      if (i == npoints - 1)
         y = 0;

      if (vertical)
         vertices[i] = VertexF{y, i * delta};
      else
         vertices[i] = VertexF{i * delta, y};

      float swing = rand() % 2 == 0 ? -1 : 1;
      y += swing * SWING_SIZE * (float)(rand() % 4);
      if (y > MAX_OUT)
         y = MAX_OUT - swing * SWING_SIZE;
      else if (y < -MAX_OUT)
         y = -MAX_OUT + swing * SWING_SIZE;
   }

   m_line.Build(vertices, npoints);

   delete[] vertices;
}

void Lightning::Draw(int x, int y) const
{
   m_line.Draw(x, y);
}

LightLineStrip::LightLineStrip()
   : swapXandY(false)
{
   GLubyte white = 255;
   m_texture = Texture::Make(1, 1, &white, GL_LUMINANCE);
}

void LightLineStrip::Build(const VertexF *vertices, int count)
{
   m_vbo = VertexBuffer::Make(vertices, count, GL_LINE_STRIP);
}

void LightLineStrip::Draw(int x, int y) const
{
   DrawWithOffset(x, y, 0, 1, 1, 1, 1);

   DrawWithOffset(x, y, 1, 0.8, 0.8, 1, 0.8);
   DrawWithOffset(x, y, -1, 0.8, 0.8, 1, 0.8);

   DrawWithOffset(x, y, 2, 0.6, 0.6, 1, 0.6);
   DrawWithOffset(x, y, -2, 0.6, 0.6, 1, 0.6);

   DrawWithOffset(x, y, 3, 0.4, 0.4, 1, 0.4);
   DrawWithOffset(x, y, -3, 0.4, 0.4, 1, 0.4);

   DrawWithOffset(x, y, 4, 0.2, 0.2, 1, 0.2);
   DrawWithOffset(x, y, -4, 0.2, 0.2, 1, 0.2);
}

void LightLineStrip::DrawWithOffset(int x, int y, float off, float r,
                                    float g, float b, float a) const
{
   OpenGL& opengl = OpenGL::GetInstance();

   opengl.Reset();
   opengl.SetColour(r, g, b, a);
   opengl.SetTranslation(x + (swapXandY ? off : 0), y + (swapXandY ? 0 : off));
   opengl.SetTexture(m_texture);
   opengl.Draw(m_vbo);
}
