/*
 * OpenGL.cpp - Implementation of OpenGL wrapper class.
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

#include "OpenGL.hpp"
#include "Input.hpp"
#include "ScreenManager.hpp"

OpenGL::OpenGL()
   : screen_width(0), screen_height(0), screen_depth(0),
     fullscreen(false), running(false), active(true),
     dodisplay(true), textureon(false), blendon(false), depthon(false),
     fps_lastcheck(0), fps_framesdrawn(0), fps_rate(0)
{
   // Start random number generator
   srand((unsigned)time(NULL));
}

/*
 * Returns the singleton instance of OpenGL.
 */
OpenGL &OpenGL::GetInstance()
{
   static OpenGL opengl;

   return opengl;
}

/* 
 * Called before the game is started. Creates a new window and performs ay 
 * necessary initialisation.
 */
void OpenGL::Init(int width, int height, int depth, bool fullscreen)
{
   // Store parameters
   screen_height = height;
   screen_width = width;
   screen_depth = depth;
   this->fullscreen = fullscreen;

   // Start SDL
   if (SDL_Init(SDL_INIT_VIDEO) < 0)
      RuntimeError("Unable to initialise SDL: " + SDLErrorString());
   atexit(SDL_Quit);

   int flags = SDL_OPENGL;
   if (fullscreen)
      flags |= SDL_FULLSCREEN;
    
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   if (SDL_SetVideoMode(screen_width, screen_height, 0, flags) == NULL)
      RuntimeError("Unable to create OpenGL screen: " + SDLErrorString());;

   SDL_WM_SetCaption(WINDOW_TITLE, NULL);

   if (fullscreen)
      SDL_ShowCursor(0);
    
   ResizeGLScene(screen_width, screen_height);

   // Start OpenGL
   if (!InitGL())
      RuntimeError("Initialisation failed.");
}

void OpenGL::RuntimeError(string mess)
{
   throw runtime_error(mess);
}

/*
 * Return the last SDL error as a C++ string.
 */
string OpenGL::SDLErrorString()
{
   return string(SDL_GetError());
}

void OpenGL::Run()
{
   unsigned int tick_start, tick_now;

   running = true;
   active = true;

   // Loop until program ends
   do {
      tick_start = SDL_GetTicks();

      Input::GetInstance().Update();

      // Process user input
      ScreenManager::GetInstance().Process();

      // Draw the next frame
      if (active)	{
         DrawGLScene();
         SDL_GL_SwapBuffers();
      }

      // Limit the frame rate
      tick_now = SDL_GetTicks();
      while (tick_now < tick_start + 1000/FRAME_RATE)	{
         msleep(tick_start + 1000/FRAME_RATE - tick_now);
         tick_now = SDL_GetTicks();
      }
   } while (running);
}

void OpenGL::DrawGLScene()
{
   // Clear the screen
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();

   // Render the scene
   if (dodisplay)
      ScreenManager::GetInstance().Display();
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
   fps_framesdrawn++;
}

void OpenGL::Viewport(int x, int y, int width, int height)
{
   glViewport(x, y, width, height);
}

void OpenGL::Draw(Renderable *r)
{
   DisableBlending();
   glLoadIdentity();
   r->TranslateTo();
   r->ApplyColour();
   r->Render();
}

void OpenGL::Draw(Poly *cp)
{
   DisableBlending();
   EnableTexture();
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
   DisableBlending();
   DisableTexture();
   glLoadIdentity();
   r->TranslateTo();
   glRotatef(angle, 0.0f, 0.0f, 1.0f);
   r->ApplyColour();
   r->Render();
}

void OpenGL::DrawBlend(Renderable *r, float alpha)
{
   DisableDepthBuffer();
   EnableBlending();
   glLoadIdentity();
   r->TranslateTo();
   r->ApplyColour(alpha);
   r->Render();
}

void OpenGL::DrawRotateBlend(Renderable *r, float angle, float alpha)
{
   DisableDepthBuffer();
   EnableBlending();
   glLoadIdentity();
   r->TranslateTo();
   glRotatef(angle, 0.0f, 0.0f, 1.0f);
   r->ApplyColour(alpha);
   r->Render();
}

void OpenGL::DrawScale(Renderable *r, float factor)
{
   DisableBlending();
   glLoadIdentity();
   r->TranslateTo();
   glScalef(factor, factor, 0);
   r->ApplyColour();
   r->Render();
}

void OpenGL::DrawRotateScale(Renderable *r, float angle, float factor)
{
   DisableBlending();
   glLoadIdentity();
   r->TranslateTo();
   glScalef(factor, factor, 0);
   glRotatef(angle, 0.0f, 0.0f, 1.0f);
   r->ApplyColour();
   r->Render();
}

void OpenGL::DrawBlendScale(Renderable *r, float alpha, float factor)
{
   DisableDepthBuffer();
   EnableBlending();
   glLoadIdentity();
   r->TranslateTo();
   glScalef(factor, factor, 0);
   r->ApplyColour();
   r->Render();
}

void OpenGL::DrawRotateBlendScale(Renderable *r, float angle, float alpha, float factor)
{
   DisableDepthBuffer();
   EnableBlending();
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

GLuint OpenGL::BindTexture(unsigned char *data, int width, int height, int fmt)
{
   GLuint result;

   // Generate the texture
   glGenTextures(1, &result);
   glBindTexture(GL_TEXTURE_2D, result);

   int sz = (fmt == GL_RGBA ? 4 : 3);
	
   // Bind textures
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D, 0, sz, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);

   return result;
}

GLubyte *OpenGL::BuildAlphaChannel(const unsigned char *pixels, int width, int height)
{
   // Convert RGB to RGBA - (0,255,0) is transparent
   GLubyte* RGBA_pixels = new GLubyte[width * height * 4];
   for (int i = 0; i < width * height; i++) {
      RGBA_pixels[i*4] = pixels[i*3];
      RGBA_pixels[i*4+1] = pixels[i*3+1];
      RGBA_pixels[i*4+2] = pixels[i*3+2];
        
      if (pixels[i*3+0] == 0 && pixels[i*3+1] == 255 && pixels[i*3+2] == 0)
         RGBA_pixels[i*4+3] = 0;
      else 		
         RGBA_pixels[i*4+3] = 255;
   }

   return RGBA_pixels;
}

GLuint OpenGL::LoadTextureAlpha(const char *filename)
{
   GLuint result;
   GLubyte *RGBA_pixels;
   Bitmap *bmp;

   // Load the image
   try {
      File f(filename);
      bmp = new Bitmap(&f);
   }
   catch (std::runtime_error e) {
      ErrorMsg(e.what());
      throw std::runtime_error("Invalid texture: " + std::string(filename));
   }

   // Construct the alpha channel and generate the texture
   RGBA_pixels = BuildAlphaChannel(bmp->GetData(), bmp->GetWidth(), bmp->GetHeight());	
   result = BindTexture(RGBA_pixels, bmp->GetWidth(), bmp->GetHeight(), GL_RGBA);

   // Free image data
   delete bmp;
   delete[] RGBA_pixels;

   return result;
}

GLuint OpenGL::LoadTextureAlpha(DataFile *p_data, const char *filename)
{
   GLuint result;
   GLubyte *RGBA_pixels;
   Bitmap *bmp;

   p_data->SelectFile(filename);
	
   try {
      bmp = p_data->LoadBitmap();
   }
   catch (std::runtime_error e) {
      ErrorMsg(e.what());
      throw std::runtime_error("Invalid texture: " + std::string(filename));
   }

   // Build an alpha channel and create the texture
   RGBA_pixels = BuildAlphaChannel(bmp->GetData(), bmp->GetWidth(), bmp->GetHeight());
   result = BindTexture(RGBA_pixels, bmp->GetWidth(), bmp->GetHeight(), GL_RGBA);

   // Free image data
   delete bmp;
   delete[] RGBA_pixels;

   return result;
}

GLuint OpenGL::LoadTexture(const char *filename)
{
   Bitmap *bmp;
   GLuint result;

   // Load the image
   try {
      File f(filename);
      bmp = new Bitmap(&f);
   }
   catch (std::runtime_error e) {
      ErrorMsg(e.what());
      throw std::runtime_error("Invalid texture: " + std::string(filename));
   }

   result = BindTexture(bmp->GetData(), bmp->GetWidth(), bmp->GetHeight(), GL_RGB);

   // Free image data
   delete bmp;

   return result;
}

GLuint OpenGL::LoadTexture(DataFile *p_data, const char *filename)
{
   GLuint result;
   Bitmap *bmp;

   p_data->SelectFile(filename);
	
   try	{
      bmp = p_data->LoadBitmap();
   }
   catch (std::runtime_error e) {
      ErrorMsg(e.what());
      throw std::runtime_error("Invalid texture: " + std::string(filename));
   }

   result = BindTexture(bmp->GetData(), bmp->GetWidth(), bmp->GetHeight(), GL_RGB);

   // Free image data
   delete bmp;

   return result;
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
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Nicest perspective correction
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// Basic blending function
   glEnable(GL_ALPHA_TEST);				// Enable alpha testing
   glAlphaFunc(GL_GREATER, 0.0);			// Alpha testing function

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


/*
 * Displays a message to the user. On Windows this will appear as a message
 * box. On other platforms the message will be printed to stdout.
 */
void OpenGL::MsgBox(const char *text, const char *title)
{
#ifdef WIN32
   MessageBox(NULL, text, title, MB_OK | MB_ICONINFORMATION);
#else
   printf("%s\n", text);
#endif
}


/*
 * Displays an error message to the user. On Windows this will appear
 * as a popup error message. On other platforms the message will be
 * printed to stderr.
 */
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

/*
 * Stops processing after the current frame.
 */
void OpenGL::Stop()
{
   running = false;
}

/*
 * Generate, but do not display the next frame. 
 */
void OpenGL::SkipDisplay()
{
   dodisplay = false;
}

void OpenGL::SelectTexture(Texture uTexture)
{
   glBindTexture(GL_TEXTURE_2D, uTexture);
}

void OpenGL::ClearColour(float r, float g, float b)
{
   glClearColor(r, g, b, 0.0f); 
}

void OpenGL::Colour(float r, float g, float b, float a)
{
   glColor4f(r, g, b, a); 
}

void OpenGL::EnableTexture() 
{ 
   if (!textureon) { 
      glEnable(GL_TEXTURE_2D); 
      textureon = true; 
   } 
}

void OpenGL::DisableTexture() 
{ 
   if (textureon) { 
      glDisable(GL_TEXTURE_2D); 
      textureon = false; 
   } 
}

void OpenGL::EnableBlending() 
{ 
   if (!blendon) { 
      glEnable(GL_BLEND); 
      blendon = true; 
   } 
}

void OpenGL::DisableBlending() 
{ 
   if (blendon) { 
      glDisable(GL_BLEND);
      blendon = false;
   }
} 
	
void OpenGL::EnableDepthBuffer() 
{ 
   if (!depthon) {
      glEnable(GL_DEPTH_TEST); 
      depthon = true; 
   } 
}

void OpenGL::DisableDepthBuffer() 
{ 
   if (depthon) { 
      glDisable(GL_DEPTH_TEST); 
      depthon = false; 
   } 
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

TextureQuad::TextureQuad(int qx, int qy, int width, int height, Texture tex,
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
