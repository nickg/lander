//
//  ElectricGate.hpp -- Electric gateway thingys.
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

#ifndef INC_ELECTRICGATE_HPP
#define INC_ELECTRICGATE_HPP

#include "ObjectGrid.hpp"
#include "Viewport.hpp"
#include "Image.hpp"
#include "GameObjFwd.hpp"

#include <list>

//
// A line strip used for rendering lightning.
//
class LightLineStrip {
public:
   LightLineStrip();

   void Draw(int x, int y) const;
   void SwapXandY(bool b) { swapXandY = b; }
   void Build(const VertexF *vertices, int count);
private:
   void DrawWithOffset(int x, int y, float off, float r, float g, float b,
                       float a) const;

   bool swapXandY;
   VertexBuffer m_vbo;
   Texture m_texture;
};

class Lightning {
public:
   void Build(int length, bool vertical);
   void Draw(int x, int y) const;
private:
   LightLineStrip m_line;
};


class ElectricGate : public StaticObject {
public:
   ElectricGate(Viewport* v, int length, bool vertical, int x, int y);

   bool CheckCollision(Ship& ship);
   void Draw();

private:
   static constexpr float GATEWAY_ACTIVE = 30.0f;

   int length;
   bool vertical;
   Viewport* viewport;
   Lightning lightning;
   Image gateImage;
   float m_timer = 0.0f;
};

#endif
