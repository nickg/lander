//
// OpenGL.hpp - Definition of OpenGL wrapper class.
// Copyright (C) 2006-2019  Nick Gasson
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#pragma once

#include "Platform.hpp"
#include "Geometry.hpp"
#include "Texture.hpp"

#include <vector>

template <typename T>
struct Vertex {
   T x, y;
   float tx, ty;
};

typedef Vertex<int> VertexI;
typedef Vertex<float> VertexF;

class VertexBuffer {
public:
   static VertexBuffer Make(const VertexI *vertices, int count,
                            GLenum mode=GL_QUADS);
   static VertexBuffer Make(const VertexF *vertices, int count,
                            GLenum mode=GL_QUADS);
   static VertexBuffer MakeQuad(int width, int height);
   static VertexBuffer Invalid();

   VertexBuffer() = default;
   VertexBuffer(VertexBuffer&& other);
   ~VertexBuffer();

   VertexBuffer& operator=(VertexBuffer&& other);

private:
   friend class OpenGL;

   VertexBuffer(GLuint stride, GLuint vertType, GLuint texType,
                GLvoid *texOffset, int count, GLenum mode);
   VertexBuffer(const VertexBuffer&) = delete;

   GLuint m_vbo = 0;
   GLuint m_stride = 0;
   GLuint m_vertType = 0;
   GLuint m_texType = 0;
   GLvoid *m_texOffset = nullptr;
   int m_count = 0;
   GLenum m_mode = GL_QUADS;
};

struct Colour {
   float r, g, b, a;

   static Colour Make(float r, float g, float b, float a=1.0f);

   static const Colour WHITE;
   static const Colour BLACK;
};

//
// A wrapper around common 2D OpenGL functions.
//
class OpenGL {
public:
   static OpenGL& GetInstance();

   void Init(int width, int height, int depth, bool fullscreen);
   void Stop();
   void Run();
   void SkipDisplay();
   int GetFPS();

   void Draw(const VertexBuffer& vbo, int start, int count);
   void Draw(const VertexBuffer& vbo);
   void Reset();
   void SetTranslation(float x, float y);
   void SetScale(float scale);
   void SetScale(float scaleX, float scaleY);
   void SetRotation(float angle);
   void SetColour(float r, float g, float b, float a=1.0f);
   void SetColour(const Colour& colour);
   void SetTexture(GLuint texture);
   void SetTexture(const Texture& texture);
   void SetBlendFunc(GLenum sfactor, GLenum dfactor);

   int GetWidth() const { return screen_width; }
   int GetHeight() const { return screen_height; }

   typedef float TimeScale;

   TimeScale GetTimeScale() const;

   void DeferScreenShot();

   bool SetVideoMode(bool fullscreen, int width, int height);

   class BindVertexBuffer {
   public:
      explicit BindVertexBuffer(const VertexBuffer& vbo);
      ~BindVertexBuffer();
      BindVertexBuffer(const BindVertexBuffer&) = delete;

   private:
      bool m_didBind;
   };

   struct Resolution {
      const int width, height;
      const bool allow_fullscreen;
   };
   typedef std::vector<Resolution> ResolutionList;
   void EnumResolutions(ResolutionList& out) const;

   bool IsTextureSizeSupported(int width, int height, int ncols=4,
                               GLenum format=GL_RGBA);

   static void CheckError(const char *text=NULL);

   static const GLuint INVALID_TEXTURE = 0xFFFFFFFF;

private:
   OpenGL();
   OpenGL(const OpenGL&) = delete;
   ~OpenGL();

   GLvoid ResizeGLScene(GLsizei width, GLsizei height);
   bool InitGL();
   void DrawGLScene();
   void TakeScreenShot() const;
   void AddShader(GLuint program, const char* text, GLenum type);
   void CompileShaders();
   GLuint GetUniformLocation(const char *name);

   static const int VIRTUAL_FRAME_RATE = 35;

   // Window related variables
   int screen_width, screen_height;
   bool fullscreen;
   bool running, active, dodisplay;
   int sdl_flags;
   SDL_Window *m_window;
   SDL_GLContext m_glcontext;
   GLuint m_program = 0;
   GLuint m_translateLocation = 0;
   GLuint m_scaleLocation = 0;
   GLuint m_colourLocation = 0;
   GLuint m_angleLocation = 0;

   // Frame rate variables
   int fps_lastcheck, fps_framesdrawn, fps_rate;
   TimeScale m_timeScale;

   bool deferredScreenShot;
};
