//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#pragma once

#include "Platform.hpp"
#include "Texture.hpp"
#include "OpenGL.hpp"

class Image {
public:
   explicit Image(const string& fileName);
   Image(const Image&) = delete;
   Image(Image&&) = default;
   virtual ~Image() = default;

   void Draw(int x, int y, float rotate=0.0f, float scale=1.0f,
             float alpha=1.0f, float white=1.0f) const;

   int GetWidth() const;
   int GetHeight() const;

protected:
   const Texture& GetTexture() const { return m_texture; }

private:
   Texture m_texture;
   VertexBuffer m_vbo;
};
