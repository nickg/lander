//
// OpenGL.hpp - Definition of OpenGL wrapper class.
// Copyright (C) 2006  Nick Gasson
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

#ifndef INC_OPENGL_HPP
#define INC_OPENGL_HPP

#include "Platform.hpp"
#include "Geometry.hpp"

#include <vector>

#define WINDOW_TITLE "Lunar Lander"
#define FRAME_RATE 35

struct Renderable {
   Renderable(int x, int y, int width, int height,
              float r, float g, float b);
   virtual ~Renderable() {}

   virtual void Render() = 0;

   void TranslateTo();
   void ApplyColour(float alpha=1.0f);

   int x, y, width, height;
   float red, green, blue;
};

struct ColourQuad : Renderable {
   ColourQuad(int x=0, int y=0, int width=0, int height=0,
              float r=1, float g=1, float b=1);
   void Render();

};

//
// A polygon with four points and a texture.
//
struct TextureQuad : Renderable {
   TextureQuad(int qx=0, int qy=0, int width=0, int height=0, GLuint tex=0,
               float r=1, float g=1, float b=1);
   void Render();

   GLuint uTexture;
};


//
// A generic polygon.
//
struct Poly {
   Poly()
      : pointcount(0), xpos(0), ypos(0), uTexture(0), texX(0), texwidth(1.0f)
   { }

   static const int MAX_POINTS = 4;
   Point points[MAX_POINTS];	// MAX_PointS=4 at the moment (!)
   int pointcount, xpos, ypos;
   GLuint uTexture;
   float texX, texwidth;
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

   // Renderer functions
   void Draw(Renderable* r);
   void Draw(Poly* cp);
   void DrawRotate(Renderable* r, float angle);
   void DrawBlend(Renderable* r, float alpha);
   void DrawRotateBlend(Renderable* r, float angle, float alpha);
   void DrawScale(Renderable* r, float factor);
   void DrawRotateScale(Renderable* r, float angle, float factor);
   void DrawBlendScale(Renderable* r, float alpha, float factor);
   void DrawRotateBlendScale(Renderable* r, float angle, float alpha, float factor);

   void Reset();
   void Translate(float x, float y);
   void Scale(float scale);
   void Rotate(float angle);
   void Colour(float r, float g, float b, float a);
   void Colour(float r, float g, float b);

   int GetWidth() const { return screen_width; }
   int GetHeight() const { return screen_height; }

   typedef float TimeScale;

   TimeScale GetTimeScale() const;

   void DeferScreenShot();

   bool SetVideoMode(bool fullscreen, int width, int height);

   struct Resolution {
      Resolution(int w, int h);
      Resolution(const std::pair<int, int>& p);

      const int width, height;
   };
   void EnumResolutions(vector<Resolution>& out) const;

   bool IsTextureSizeSupported(int width, int height, int ncols=4,
                               GLenum format=GL_RGBA);

   static void CheckError(const char *text=NULL);

   static const GLuint INVALID_TEXTURE = 0xFFFFFFFF;

private:
   OpenGL();
   ~OpenGL();

   GLvoid ResizeGLScene(GLsizei width, GLsizei height);
   GLuint BindTexture(unsigned char* data, int width, int height, int fmt);
   GLubyte* BuildAlphaChannel(const unsigned char* pixels, int width, int height);
   bool InitGL();
   void DrawGLScene();
   void TakeScreenShot() const;
   void AddShader(GLuint program, const char* text, GLenum type);
   void CompileShaders();
   GLuint GetUniformLocation(const char *name);

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

#endif
