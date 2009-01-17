//
// OpenGL.cpp - Implementation of OpenGL wrapper class.
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

#include "OpenGL.hpp"
#include "Input.hpp"
#include "ScreenManager.hpp"

OpenGL::OpenGL()
   : screen_width(0), screen_height(0),
     fullscreen(false), running(false), active(true),
     dodisplay(true),
     fps_lastcheck(0), fps_framesdrawn(0), fps_rate(0)
{
   // Start random number generator
   srand((unsigned)time(NULL));
}

//
// Returns the singleton instance of OpenGL.
//
OpenGL &OpenGL::GetInstance()
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
      RuntimeError("Unable to initialise SDL: " + SDLErrorString());
   atexit(SDL_Quit);

   SetVideoMode(fullscreen, width, height);
    
   SDL_WM_SetCaption(WINDOW_TITLE, NULL);

   SDL_ShowCursor(SDL_DISABLE);

   // Start OpenGL
   if (!InitGL())
      RuntimeError("Initialisation failed.");
}

void OpenGL::RuntimeError(string mess)
{
   throw runtime_error(mess);
}

bool OpenGL::SetVideoMode(bool fullscreen, int width, int height)
{
   bool resized = !(width == screen_width && height == screen_height);
   
   screen_height = height;
   screen_width = width;
   this->fullscreen = fullscreen;

   sdl_flags = SDL_OPENGL;
   if (fullscreen)
      sdl_flags |= SDL_FULLSCREEN;
   
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   if (SDL_SetVideoMode(screen_width, screen_height, 0, sdl_flags) == NULL)
      RuntimeError("Unable to create OpenGL screen: " + SDLErrorString());;
    
   ResizeGLScene(screen_width, screen_height);

   return resized;
}

//
// Return the last SDL error as a C++ string.
//
string OpenGL::SDLErrorString()
{
   return string(SDL_GetError());
}

void OpenGL::Run()
{
   unsigned int tick_start, tick_now;

   running = true;
   active = true;

   const unsigned window = 1000/FRAME_RATE;

   // Loop until program ends
   do {
      tick_start = SDL_GetTicks();

      Input::GetInstance().Update();

      // Process user input
      ScreenManager::GetInstance().Process();

      // Draw the next frame
      if (active)
         DrawGLScene();

      // Limit the frame rate
      tick_now = SDL_GetTicks();
      if (tick_now > tick_start + window) {
         //cout << "Missed window by " << tick_now - tick_start - window
         //     << "ms (skipping next frame)" << endl;
         SkipDisplay();
      }
      else {
         while (tick_now < tick_start + window)	{
            msleep(tick_start + window - tick_now);
            tick_now = SDL_GetTicks();
         }
      }
   } while (running);
}

void OpenGL::DrawGLScene()
{
   // Render the scene
   if (dodisplay) {
      // Clear the screen
      glClear(GL_COLOR_BUFFER_BIT);
      glLoadIdentity();
      
      ScreenManager::GetInstance().Display();
      SDL_GL_SwapBuffers();

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

void OpenGL::Viewport(int x, int y, int width, int height)
{
   glViewport(x, y, width, height);
}

void OpenGL::Draw(Renderable *r)
{
   glDisable(GL_BLEND);
   glLoadIdentity();
   r->TranslateTo();
   r->ApplyColour();
   r->Render();
}

void OpenGL::Draw(Poly *cp)
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

void OpenGL::DrawRotate(Renderable *r, float angle)
{
   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);
   glLoadIdentity();
   r->TranslateTo();
   glRotatef(angle, 0.0f, 0.0f, 1.0f);
   r->ApplyColour();
   r->Render();
}

void OpenGL::DrawBlend(Renderable *r, float alpha)
{
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glLoadIdentity();
   r->TranslateTo();
   r->ApplyColour(alpha);
   r->Render();
}

void OpenGL::DrawRotateBlend(Renderable *r, float angle, float alpha)
{
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glLoadIdentity();
   r->TranslateTo();
   glRotatef(angle, 0.0f, 0.0f, 1.0f);
   r->ApplyColour(alpha);
   r->Render();
}

void OpenGL::DrawScale(Renderable *r, float factor)
{
   glDisable(GL_BLEND);
   glLoadIdentity();
   r->TranslateTo();
   glScalef(factor, factor, 0);
   r->ApplyColour();
   r->Render();
}

void OpenGL::DrawRotateScale(Renderable *r, float angle, float factor)
{
   glDisable(GL_BLEND);
   glLoadIdentity();
   r->TranslateTo();
   glScalef(factor, factor, 0);
   glRotatef(angle, 0.0f, 0.0f, 1.0f);
   r->ApplyColour();
   r->Render();
}

void OpenGL::DrawBlendScale(Renderable *r, float alpha, float factor)
{
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glLoadIdentity();
   r->TranslateTo();
   glScalef(factor, factor, 0);
   r->ApplyColour();
   r->Render();
}

void OpenGL::DrawRotateBlendScale(Renderable *r, float angle, float alpha, float factor)
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

   // All went OK
   return true;
}

GLvoid OpenGL::ResizeGLScene(GLsizei width, GLsizei height)
{
   if (height == 0) height = 1;
	
   // Reset viewport
   glViewport(0, 0, width, height);

   // Set it to 2D mode
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0f, (GLfloat)screen_width, (GLfloat)screen_height, 0.0f);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glPushMatrix();
}


//
// Displays a message to the user. On Windows this will appear as a message
// box. On other platforms the message will be printed to stdout.
//
void OpenGL::MsgBox(const char *text, const char *title)
{
#ifdef WIN32
   MessageBox(NULL, text, title, MB_OK | MB_ICONINFORMATION);
#else
   printf("%s\n", text);
#endif
}


//
// Displays an error message to the user. On Windows this will appear
// as a popup error message. On other platforms the message will be
// printed to stderr.
//
void OpenGL::ErrorMsg(const char *text, const char *title)
{
#ifdef WIN32
   MessageBox(NULL, text, title, MB_OK | MB_ICONEXCLAMATION);
#else
   fprintf(stderr, "%s\n", text);
#endif	
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

//
// Generate, but do not display the next frame. 
//
void OpenGL::SkipDisplay()
{
   dodisplay = false;
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
