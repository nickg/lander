//
// OpenGL.cpp - Implementation of OpenGL wrapper class.
// Copyright (C) 2006-2009  Nick Gasson
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

#include "OpenGL.hpp"
#include "Input.hpp"
#include "ScreenManager.hpp"

#include <ctime>
#include <iostream>
#include <cassert>
#include <set>

static const char *g_vertexShader =
   "#version 130\n"
   "in vec2 Position;\n"
   "in vec2 TexCoord;\n"
   "uniform vec2 WindowSize;\n"
   "uniform vec2 Translate;\n"
   "uniform float Scale;\n"
   "out vec2 TexCoord0;\n"
   "void main()\n"
   "{\n"
   "   vec2 tmp = Position * Scale + Translate;\n"
   "   vec2 winscale = vec2(WindowSize.x / 2, WindowSize.y / 2);\n"
   "   tmp -= winscale;\n"
   "   tmp /= winscale;\n"
   "   gl_Position = vec4(tmp.x, -tmp.y, 0.0, 1.0);\n"
   "   TexCoord0 = TexCoord;\n"
   "}\n";

static const char *g_fragmentShader =
   "#version 130\n"
   "in vec2 TexCoord0;\n"
   "out vec4 FragColor;\n"
   "uniform vec4 Colour;\n"
   "uniform sampler2D Sampler;\n"
   "void main()\n"
   "{\n"
   "   FragColor = texture2D(Sampler, TexCoord0.st) * vec4(Colour);\n"
   "}\n";

OpenGL::OpenGL()
   : screen_width(0), screen_height(0),
     fullscreen(false), running(false), active(true),
     dodisplay(true),
     m_window(NULL),
     m_glcontext(NULL),
     fps_lastcheck(0), fps_framesdrawn(0), fps_rate(0),
     m_timeScale(0.0),
     deferredScreenShot(false)
{
   // Start random number generator
   srand((unsigned)time(NULL));
}

//
// Returns the singleton instance of OpenGL.
//
OpenGL& OpenGL::GetInstance()
{
   static OpenGL opengl;

   return opengl;
}

//
// Called before the game is started. Creates a new window and performs ay
// necessary initialisation.
//
void OpenGL::Init(int width, int height, int depth, bool fullscreen)
{
   // Start SDL
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
      Die("Unable to initialise SDL: %s", SDL_GetError());
   atexit(SDL_Quit);

   SetVideoMode(fullscreen, width, height);

   SDL_ShowCursor(SDL_DISABLE);
}

bool OpenGL::SetVideoMode(bool fullscreen, int width, int height)
{
   bool resized = !(width == screen_width && height == screen_height);

   screen_height = height;
   screen_width = width;
   this->fullscreen = fullscreen;

   sdl_flags = SDL_WINDOW_OPENGL;
   if (fullscreen)
      sdl_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

   if (m_window == NULL) {
      m_window = SDL_CreateWindow(WINDOW_TITLE,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  screen_width, screen_height,
                                  sdl_flags);

      if (m_window == NULL)
         Die("Failed to create window: %s", SDL_GetError());

      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

      if ((m_glcontext = SDL_GL_CreateContext(m_window)) == NULL)
         Die("Failed to create GL context: %s", SDL_GetError());

      SDL_GL_SetSwapInterval(1);

      InitGL();
   }
   else {
      SDL_SetWindowSize(m_window, screen_width, screen_height);
      SDL_SetWindowFullscreen(m_window, sdl_flags);
   }

   ResizeGLScene(screen_width, screen_height);

   return resized;
}

void OpenGL::AddShader(GLuint program, const char* text, GLenum type)
{
   GLuint obj = glCreateShader(type);
   if (obj == 0)
      Die("Error creating shader type %d", type);

   const GLchar* p[1] = { text };
   GLint lengths[1];
   lengths[0]= strlen(text);
   glShaderSource(obj, 1, p, lengths);

   glCompileShader(obj);

   GLint success;
   glGetShaderiv(obj, GL_COMPILE_STATUS, &success);
   if (!success) {
      GLchar infoLog[1024];
      glGetShaderInfoLog(obj, sizeof(infoLog), NULL, infoLog);
      Die("Error compiling shader type %d: %s", type, infoLog);
   }

   glAttachShader(program, obj);
}

void OpenGL::CompileShaders()
{
   m_program = glCreateProgram();
   if (m_program == 0)
      Die("Error creating shader program");

   AddShader(m_program, g_vertexShader, GL_VERTEX_SHADER);
   AddShader(m_program, g_fragmentShader, GL_FRAGMENT_SHADER);

   GLint success = 0;
   GLchar errorLog[1024] = { 0 };

   glLinkProgram(m_program);
   glGetProgramiv(m_program, GL_LINK_STATUS, &success);
   if (success == 0) {
      glGetProgramInfoLog(m_program, sizeof(errorLog), NULL, errorLog);
      Die("Error linking shader program: %s", errorLog);
   }

   glValidateProgram(m_program);
   glGetProgramiv(m_program, GL_VALIDATE_STATUS, &success);
   if (!success) {
      glGetProgramInfoLog(m_program, sizeof(errorLog), NULL, errorLog);
      Die("Invalid shader program: %s", errorLog);
   }

   glUseProgram(m_program);

   m_translateLocation = GetUniformLocation("Translate");
   m_scaleLocation = GetUniformLocation("Scale");
   m_colourLocation = GetUniformLocation("Colour");

   //glUniform1i(GetUniformLocation("Sampler"), 0);
}

GLuint OpenGL::GetUniformLocation(const char *name)
{
   GLuint location = glGetUniformLocation(m_program, name);
   if (location == 0xffffffff)
      Die("Failed to get uniform location: %s", name);

   return location;
}

void OpenGL::Run()
{
   running = true;
   active = true;

   unsigned lastTick = 0;

   // Loop until program ends
   do {
      const unsigned tickStart = SDL_GetTicks();
      if (lastTick == 0)
         m_timeScale = 1.0;
      else {
         const float delta = tickStart - lastTick;
         m_timeScale = delta / (1000.0f / FRAME_RATE);
      }

      Input::GetInstance().Update();

      // Process user input
      ScreenManager::GetInstance().Process();

      // Draw the next frame
      if (active)
         DrawGLScene();

      lastTick = tickStart;
   } while (running);
}

OpenGL::TimeScale OpenGL::GetTimeScale() const
{
   return m_timeScale;
}

// Take a screenshot at the end of this frame
void OpenGL::DeferScreenShot()
{
   deferredScreenShot = true;
}

void OpenGL::TakeScreenShot() const
{
   const string fileName("Lander.bmp");

   SDL_Surface* temp = SDL_CreateRGBSurface
      (SDL_SWSURFACE, screen_width, screen_height, 24,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
       0x000000FF, 0x0000FF00, 0x00FF0000, 0
#else
       0x00FF0000, 0x0000FF00, 0x000000FF, 0
#endif
       );
   assert(temp);

   const int w = screen_width;
   const int h = screen_height;
   unsigned char* pixels = new unsigned char[3 * w * h];

   glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);

   for (int i = 0; i < h; i++)
      memcpy(((char*)temp->pixels) + temp->pitch * i, pixels + 3*w * (h-i-1), w*3);
   delete[] pixels;

   SDL_SaveBMP(temp, fileName.c_str());
   SDL_FreeSurface(temp);

   cout << "Wrote screen shot to " << fileName << endl;
}

void OpenGL::CheckError(const char *text)
{
   GLenum error = glGetError();
   if (error == GL_NO_ERROR)
      return;

   if (text != NULL)
      fprintf(stderr, "%s: ", text);

   switch (error) {
   case GL_INVALID_VALUE:
      Die("OpenGL error: GL_INVALID_VALUE");
   case GL_INVALID_OPERATION:
      Die("OpenGL error: GL_INVALID_OPERATION");
   default:
      Die("OpenGL error: %d", error);
   }
}

void OpenGL::DrawGLScene()
{
   // Render the scene
   if (dodisplay) {
      // Clear the screen
      glClear(GL_COLOR_BUFFER_BIT);

      glUseProgram(m_program);
      Reset();

      ScreenManager::GetInstance().Display();

      CheckError("DrawGLScene");

      SDL_GL_SwapWindow(m_window);

      if (deferredScreenShot) {
         TakeScreenShot();
         deferredScreenShot = false;
      }

      fps_framesdrawn++;
   }
   else
      dodisplay = true;

   // Calculate frame rate
   if (SDL_GetTicks() - fps_lastcheck >= 1000)	{
      fps_lastcheck = SDL_GetTicks();
      fps_rate = fps_framesdrawn;
      fps_framesdrawn = 0;

#ifdef SHOW_FPS
      const int TITLE_BUF_LEN = 256;
      char buf[TITLE_BUF_LEN];

      if (!fullscreen) {
         snprintf(buf, TITLE_BUF_LEN, "%s {%dfps}", WINDOW_TITLE, fps_rate);
         SDL_WM_SetCaption(buf, NULL);
      }
#endif /* #ifdef SHOW_FPS */
   }
}

void OpenGL::Draw(Renderable* r)
{
   glDisable(GL_BLEND);
   glLoadIdentity();
   r->TranslateTo();
   r->ApplyColour();
   r->Render();
}

void OpenGL::Draw(Poly* cp)
{
   glDisable(GL_BLEND);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, cp->uTexture);
   glLoadIdentity();
   glTranslatef((float)cp->xpos, (float)cp->ypos, 0.0f);
   glColor3f(1.0f, 1.0f, 1.0f);
   glBegin(GL_QUADS);
   glTexCoord2f(cp->texX, 0.0f);
   glVertex2i(cp->points[0].x, cp->points[0].y);
   glTexCoord2f(cp->texX, 1.0f);
   glVertex2i(cp->points[1].x, cp->points[1].y);
   glTexCoord2f(cp->texX + cp->texwidth, 1.0f);
   glVertex2i(cp->points[2].x, cp->points[2].y);
   glTexCoord2f(cp->texX + cp->texwidth, 0.0f);
   glVertex2i(cp->points[3].x, cp->points[3].y);
   glEnd();
}

void OpenGL::DrawRotate(Renderable* r, float angle)
{
   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);
   glLoadIdentity();
   r->TranslateTo();
   glRotatef(angle, 0.0f, 0.0f, 1.0f);
   r->ApplyColour();
   r->Render();
}

void OpenGL::DrawBlend(Renderable* r, float alpha)
{
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glLoadIdentity();
   r->TranslateTo();
   r->ApplyColour(alpha);
   r->Render();
}

void OpenGL::DrawRotateBlend(Renderable* r, float angle, float alpha)
{
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glLoadIdentity();
   r->TranslateTo();
   glRotatef(angle, 0.0f, 0.0f, 1.0f);
   r->ApplyColour(alpha);
   r->Render();
}

void OpenGL::DrawScale(Renderable* r, float factor)
{
   glDisable(GL_BLEND);
   glLoadIdentity();
   r->TranslateTo();
   glScalef(factor, factor, 0);
   r->ApplyColour();
   r->Render();
}

void OpenGL::DrawRotateScale(Renderable* r, float angle, float factor)
{
   glDisable(GL_BLEND);
   glLoadIdentity();
   r->TranslateTo();
   glScalef(factor, factor, 0);
   glRotatef(angle, 0.0f, 0.0f, 1.0f);
   r->ApplyColour();
   r->Render();
}

void OpenGL::DrawBlendScale(Renderable* r, float alpha, float factor)
{
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glLoadIdentity();
   r->TranslateTo();
   glScalef(factor, factor, 0);
   r->ApplyColour();
   r->Render();
}

void OpenGL::DrawRotateBlendScale(Renderable* r, float angle, float alpha, float factor)
{
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glLoadIdentity();
   r->TranslateTo();
   glScalef(factor, factor, 0);
   glRotatef(angle, 0.0f, 0.0f, 1.0f);
   r->ApplyColour(alpha);
   r->Render();
}

OpenGL::~OpenGL()
{
   if (m_program != 0)
      glDeleteProgram(m_program);

   if (m_glcontext != NULL)
      SDL_GL_DeleteContext(m_glcontext);

   if (m_window != NULL)
      SDL_DestroyWindow(m_window);
}

bool OpenGL::IsTextureSizeSupported(int width, int height, int ncols, GLenum format)
{
   glTexImage2D(GL_PROXY_TEXTURE_2D, 0, ncols, width, height, 0, format,
                GL_UNSIGNED_BYTE, NULL);
   glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
   glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

   return height != 0 && width != 0;
}

bool OpenGL::InitGL()
{
   cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;

   GLenum res = glewInit();
   if (res != GLEW_OK)
      Die("Error: glewInit failed: %s", glewGetErrorString(res));

   // Clear any error bit (this seems to be required on Windows??)
   glGetError();

   CompileShaders();

   // Set options
   glShadeModel(GL_SMOOTH);			        // Enable smooth shading
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// Black background
   glClearDepth(1.0f);					// Depth buffer setup
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// Basic blending function
   glEnable(GL_ALPHA_TEST);				// Enable alpha testing
   glAlphaFunc(GL_GREATER, 0.0);			// Alpha testing function
   glDisable(GL_NORMALIZE);
   glDisable(GL_DEPTH_TEST);

   CheckError("InitGL");

   // All went OK
   return true;
}

GLvoid OpenGL::ResizeGLScene(GLsizei width, GLsizei height)
{
   if (height == 0) height = 1;

   glUseProgram(m_program);

   GLuint windowSizeU = GetUniformLocation("WindowSize");
   glUniform2f(windowSizeU, width, height);

   CheckError("ResizeGLScene");
}

void OpenGL::Reset()
{
   Translate(0.0f, 0.0f);
   Scale(0.0f);
   Colour(1.0f, 1.0f, 1.0f);
}

void OpenGL::Translate(float x, float y)
{
   glUniform2f(m_translateLocation, x, y);
}

void OpenGL::Scale(float scale)
{
   glUniform1f(m_scaleLocation, scale);
}

void OpenGL::Colour(float r, float g, float b, float a)
{
   glUniform4f(m_colourLocation, r, g, b, a);
}

void OpenGL::Colour(float r, float g, float b)
{
   Colour(r, g, b, 1.0f);
}

int OpenGL::GetFPS()
{
   return fps_rate;
}

//
// Stops processing after the current frame.
//
void OpenGL::Stop()
{
   running = false;
}

void OpenGL::SkipDisplay()
{
   dodisplay = false;
}

void OpenGL::EnumResolutions(std::vector<Resolution>& out) const
{
   std::set<std::pair<int, int>> accum;

   const int numDisplayModes = SDL_GetNumDisplayModes(0);
   for (int i = 0; i < numDisplayModes; i++) {
      SDL_DisplayMode mode;
      if (SDL_GetDisplayMode(0, i, &mode) != 0)
         Die("SDL_GetDisplayMode failed: %s", SDL_GetError());

      accum.insert(std::make_pair(mode.w, mode.h));
   }

   std::copy(accum.rbegin(), accum.rend(), std::back_inserter(out));
}

Renderable::Renderable(int x, int y, int width, int height,
                       float r, float g, float b)
   : x(x), y(y), width(width), height(height), red(r),
     green(g), blue(b)
{

}

void Renderable::TranslateTo()
{
   glTranslatef((float)(x + width/2), (float)(y + height/2), 0.0f);
}

void Renderable::ApplyColour(float alpha)
{
   glColor4f(red, green, blue, alpha);
}

TextureQuad::TextureQuad(int qx, int qy, int width, int height, GLuint tex,
                         float r, float g, float b)
   : Renderable(qx, qy, width, height, r, g, b), uTexture(tex)
{

}

void TextureQuad::Render()
{
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, uTexture);
   glBegin(GL_QUADS);
   glTexCoord2f(0.0f, 1.0f); glVertex2i(-(width/2), -(height/2));
   glTexCoord2f(0.0f, 0.0f); glVertex2i(-(width/2), height/2);
   glTexCoord2f(1.0f, 0.0f); glVertex2i(width/2, height/2);
   glTexCoord2f(1.0f, 1.0f); glVertex2i(width/2, -(height/2));
   glEnd();
}

ColourQuad::ColourQuad(int x, int y, int width, int height,
                       float r, float g, float b)
   : Renderable(x, y, width, height, r, g, b)
{

}

void ColourQuad::Render()
{
   glDisable(GL_TEXTURE_2D);
   glBegin(GL_QUADS);
   glTexCoord2f(0.0f, 1.0f); glVertex2i(-(width/2), -(height/2));
   glTexCoord2f(0.0f, 0.0f); glVertex2i(-(width/2), height/2);
   glTexCoord2f(1.0f, 0.0f); glVertex2i(width/2, height/2);
   glTexCoord2f(1.0f, 1.0f); glVertex2i(width/2, -(height/2));
   glEnd();
}

OpenGL::Resolution::Resolution(int w, int h)
   : width(w), height(h)
{

}

OpenGL::Resolution::Resolution(const std::pair<int, int>& p)
   : width(p.first), height(p.second)
{

}
