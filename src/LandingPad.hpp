//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INC_LANDINGPAD_HPP
#define INC_LANDINGPAD_HPP

#include "GameObjFwd.hpp"
#include "OpenGL.hpp"

#include <vector>

class LandingPad {
public:
   LandingPad(Viewport* v, int index, int length);

   void Draw(bool locked) const;
   void SetYPos(int ypos) { this->ypos = ypos; }

   int GetLength() const { return length; }
   int GetIndex() const { return index; }

private:
   int index, length, ypos;
   Viewport* viewport;

   Texture m_landTexture;
   Texture m_noLandTexture;
   VertexBuffer m_vbo;
};

typedef vector<LandingPad> LandingPadList;
typedef LandingPadList::iterator LandingPadListIt;

#endif
