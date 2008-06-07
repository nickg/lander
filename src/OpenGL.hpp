/*
 * OpenGL.hpp - Definition of OpenGL wrapper class.
 * Copyright (C) 2006  Nick Gasson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INC_OPENGL_HPP
#define INC_OPENGL_HPP

#include "Platform.hpp"
#include "Geometry.hpp"

#define WINDOW_TITLE "Lunar Lander"
#define FRAME_RATE 35
#define PI 3.1415926535f

class Renderable {
public:
   Renderable(int x, int y, int width, int height,
              float r, float g, float b);
   virtual ~Renderable() {}
   
   virtual void Render() = 0;

   void TranslateTo();
   void ApplyColour(float alpha=1.0f);

   int x, y, width, height;
   float red, green, blue;
};

class ColourQuad : public Renderable {
public:
   ColourQuad(int x=0, int y=0, int width=0, int height=0,
              float r=1, float g=1, float b=1);
   void Render();
   
};

/* 
 * A polygon with four points and a texture.
 */
class TextureQuad : public Renderable {
public:
   TextureQuad(int qx=0, int qy=0, int width=0, int height=0, GLuint tex=0,
               float r=1, float g=1, float b=1);
   void Render();
   
   GLuint uTexture;
};


/*
 * A generic polygon.
 */
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

/* 
 * A wrapper around common 2D OpenGL functions.
 */
class OpenGL {
public:
   static OpenGL &GetInstance();

   void Init(int width, int height, int depth, bool fullscreen);
   void MsgBox(const char *text, const char *title="Message");
   void ErrorMsg(const char *text, const char *title="Error");
   void Stop();
   void Run();
   void SkipDisplay();
   int GetFPS();

   void Viewport(int x, int y, int width, int height);

   // Renderer functions
   void Draw(Renderable *r);
   void Draw(Poly *cp);
   void DrawRotate(Renderable *r, float angle);
   void DrawBlend(Renderable *r, float alpha);
   void DrawRotateBlend(Renderable *r, float angle, float alpha);
   void DrawScale(Renderable *r, float factor);
   void DrawRotateScale(Renderable *r, float angle, float factor);
   void DrawBlendScale(Renderable *r, float alpha, float factor);
   void DrawRotateBlendScale(Renderable *r, float angle, float alpha, float factor);

   int GetWidth() const { return screen_width; }
   int GetHeight() const { return screen_height; }

   void SelectTexture(GLuint uTexture);
   void ClearColour(float r, float g, float b);
   void Colour(float r, float g, float b, float a=1.0f);
   
   void EnableTexture();
   void DisableTexture();
   void EnableBlending();
   void DisableBlending();
   void EnableDepthBuffer();
   void DisableDepthBuffer();

   bool IsTextureSizeSupported(int width, int height, int ncols=4,
                               GLenum format=GL_RGBA);
     	
   static const GLuint INVALID_TEXTURE = 0xFFFFFFFF;

private:
   OpenGL();
   ~OpenGL();
   
   GLvoid ResizeGLScene(GLsizei width, GLsizei height);
   GLuint BindTexture(unsigned char *data, int width, int height, int fmt);
   GLubyte *BuildAlphaChannel(const unsigned char *pixels, int width, int height);
   bool InitGL();
   void DrawGLScene();
   string SDLErrorString();
   void RuntimeError(string mess);

   // Window related variables
   int screen_width, screen_height, screen_depth;
   bool fullscreen;
   bool running, active, dodisplay;

   // State variables
   bool textureon, blendon, depthon;

   // Frame rate variables
   int fps_lastcheck, fps_framesdrawn, fps_rate;
};

#endif
