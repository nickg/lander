//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#pragma once

#include "Platform.hpp"

class TextureHolder;

class Texture {
public:
   Texture() = default;
   Texture(const Texture&) = delete;
   Texture(Texture&&);
   ~Texture();

   Texture& operator=(Texture&& other);

   void Bind(int textureUnit=0);

   GLuint GetGLTexture() const;

   int GetWidth() const;
   int GetHeight() const;

   static Texture Load(const string& fileName);
   static Texture Make(int width, int height, const GLubyte *data,
                       GLuint fmt, GLuint filter=GL_LINEAR);
   static void UnloadAll();

private:
   Texture(TextureHolder *holder, bool owner);

   TextureHolder *m_holder = nullptr;
   bool m_owner = false;
};
