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
#include "Screens.hpp"

/* OpenGL constructor */
OpenGL::OpenGL()
    : m_screen_width(0), m_screen_height(0), m_screen_depth(0), m_fullscreen(false), m_running(false),
      m_active(true), m_dodisplay(true), m_textureon(false), m_blendon(false), m_depthon(false),
      m_fps_lastcheck(0), m_fps_framesdrawn(0), m_fps_rate(0)
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
 * Called before the game is started. Creates a new window and performs am_y 
 * necessary initialisation.
 *   width -> Width of window in pixels.
 *   height -> Height of window in pixels.
 *   depth -> Colour depth in bits per pixel.
 *   fullscreen -> A desktop window is created if false.
 * Throws std::runtime_error on failure.
 */
void OpenGL::Init(int width, int height, int depth, bool fullscreen)
{
    // Store parameters
    m_screen_height = height;
    m_screen_width = width;
    m_screen_depth = depth;
    m_fullscreen = fullscreen;

    // Start SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        RuntimeError("Unable to initialise SDL: " + SDLErrorString());
    atexit(SDL_Quit);

    int flags = SDL_OPENGL;
    if (fullscreen)
        flags |= SDL_FULLSCREEN;
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    if (SDL_SetVideoMode(m_screen_width, m_screen_height, 0, flags) == NULL)
        RuntimeError("Unable to create OpenGL screen: " + SDLErrorString());;

    SDL_WM_SetCaption(WINDOW_TITLE, NULL);

    if (fullscreen)
        SDL_ShowCursor(0);
    
    ResizeGLScene(m_screen_width, m_screen_height);

    // Start OpenGL
    if (!InitGL())
        RuntimeError("Initialisation failed.");
}


/*
 * Throw a runtime error.
 */
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


/* Main game loop */
void OpenGL::Run()
{
    unsigned int tick_start, tick_now;

    m_running = true;
    m_active = true;

    // Loop until program ends
    do
	{
            tick_start = SDL_GetTicks();

            Input::GetInstance().Update();

            // Process user input
            ScreenManager::GetInstance().Process();

            // Draw the next frame
            if (m_active)
		{
                    DrawGLScene();
                    SDL_GL_SwapBuffers();
		}

            // Limit the frame rate
            tick_now = SDL_GetTicks();
            while (tick_now < tick_start + 1000/FRAME_RATE)
		{
                    msleep(tick_start + 1000/FRAME_RATE - tick_now);
                    tick_now = SDL_GetTicks();
		}
	} while (m_running);
}

/* Draws the current scene */
void OpenGL::DrawGLScene()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Render the scene
    if (m_dodisplay)
        ScreenManager::GetInstance().Display();
    else
        m_dodisplay = true;

    // Calculate frame rate
    if (SDL_GetTicks() - m_fps_lastcheck >= 1000)
	{
            m_fps_lastcheck = SDL_GetTicks();
            m_fps_rate = m_fps_framesdrawn;
            m_fps_framesdrawn = 0;

#ifdef _DEBUG
            const int TITLE_BUF_LEN = 256;
            char buf[TITLE_BUF_LEN];

            if (!m_fullscreen)
		{
                    snprintf(buf, TITLE_BUF_LEN, "%s {%dfps}", WINDOW_TITLE, m_fps_rate);
                    SDL_WM_SetCaption(buf, NULL);
		}
#endif /* #ifdef _DEBUG */
	}
    m_fps_framesdrawn++;
}

/* Switches the viewport */
void OpenGL::Viewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}


/* Draws a colored quad */
void OpenGL::Draw(ColourQuad *cq)
{
    DisableBlending();
    DisableTexture();
    glLoadIdentity();
    glTranslatef(cq->m_x + (cq->m_width/2), cq->m_y + (cq->m_height/2), 0.0f);
    glColor3f(cq->m_red, cq->m_green, cq->m_blue);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2i(-(cq->m_width/2), -(cq->m_height/2));
    glTexCoord2f(0.0f, 0.0f); glVertex2i(-(cq->m_width/2), cq->m_height/2);
    glTexCoord2f(1.0f, 0.0f); glVertex2i(cq->m_width/2, cq->m_height/2);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(cq->m_width/2, -(cq->m_height/2));
    glEnd();
}

/* Draws a textured quad */
void OpenGL::Draw(TextureQuad *tq)
{
    DisableBlending();
    EnableTexture();
    glLoadIdentity();
    glTranslatef((float)(tq->x + (tq->width/2)), (float)(tq->y + (tq->height/2)), 0.0f);
    glColor3f(tq->red, tq->green, tq->blue);
    glBindTexture(GL_TEXTURE_2D, tq->uTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2i(-(tq->width/2), -(tq->height/2));
    glTexCoord2f(0.0f, 0.0f); glVertex2i(-(tq->width/2), tq->height/2);
    glTexCoord2f(1.0f, 0.0f); glVertex2i(tq->width/2, tq->height/2);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(tq->width/2, -(tq->height/2));
    glEnd();
}

/* Draws a Poly */
void OpenGL::Draw(Poly *cp)
{
    DisableBlending();
    EnableTexture();
    glBindTexture(GL_TEXTURE_2D, cp->uTexture);
    glLoadIdentity();
    glTranslatef(cp->xpos, cp->ypos, 0.0f);
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

/* Draws a rotated colored quad */
void OpenGL::DrawRotate(ColourQuad *cq, float angle)
{
    DisableBlending();
    DisableTexture();
    glLoadIdentity();
    glTranslatef(cq->m_x + (cq->m_width/2), cq->m_y + (cq->m_height/2), 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glColor3f(cq->m_red, cq->m_green, cq->m_blue);
    glBegin(GL_QUADS);
    glVertex2i(-(cq->m_width/2), -(cq->m_height/2));
    glVertex2i(-(cq->m_width/2), cq->m_height/2);
    glVertex2i(cq->m_width/2, cq->m_height/2);
    glVertex2i(cq->m_width/2, -(cq->m_height/2));
    glEnd();
}

/* Draws a rotated textured quad */
void OpenGL::DrawRotate(TextureQuad *tq, float angle)
{
    DisableBlending();
    EnableTexture();
    glLoadIdentity();
    glTranslatef(tq->x + (tq->width/2), tq->y + (tq->height/2), 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glColor3f(tq->red, tq->green, tq->blue);
    glBindTexture(GL_TEXTURE_2D, tq->uTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); 
    glVertex2i(-(tq->width/2), -(tq->height/2));
    glTexCoord2f(0.0f, 0.0f);
    glVertex2i(-(tq->width/2), tq->height/2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2i(tq->width/2, tq->height/2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2i(tq->width/2, -(tq->height/2));
    glEnd();
}

/* Draws a colored quad */
void OpenGL::DrawBlend(ColourQuad *cq, float alpha)
{
    DisableTexture();
    DisableDepthBuffer();
    EnableBlending();
    glLoadIdentity();
    glTranslatef(cq->m_x + (cq->m_width/2), cq->m_y + (cq->m_height/2), 0.0f);
    glColor4f(cq->m_red, cq->m_green, cq->m_blue, alpha);
    glBegin(GL_QUADS);
    glVertex2i(-(cq->m_width/2), -(cq->m_height/2));
    glVertex2i(-(cq->m_width/2), cq->m_height/2);
    glVertex2i(cq->m_width/2, cq->m_height/2);
    glVertex2i(cq->m_width/2, -(cq->m_height/2));
    glEnd();
}

/* Draws a textured quad */
void OpenGL::DrawBlend(TextureQuad *tq, float alpha)
{
    EnableTexture();
    DisableDepthBuffer();
    EnableBlending();
    glLoadIdentity();
    glTranslatef(tq->x + (tq->width/2), tq->y + (tq->height/2), 0.0f);
    glColor4f(tq->red, tq->green, tq->blue, alpha);
    glBindTexture(GL_TEXTURE_2D, tq->uTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2i(-(tq->width/2), -(tq->height/2));
    glTexCoord2f(0.0f, 0.0f);
    glVertex2i(-(tq->width/2), tq->height/2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2i(tq->width/2, tq->height/2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2i(tq->width/2, -(tq->height/2));
    glEnd();
}

/* Draws a rotated colored quad */
void OpenGL::DrawRotateBlend(ColourQuad *cq, float angle, float alpha)
{
    DisableTexture();
    DisableDepthBuffer();
    EnableBlending();
    glLoadIdentity();
    glTranslatef(cq->m_x + (cq->m_width/2), cq->m_y + (cq->m_height/2), 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glColor4f(cq->m_red, cq->m_green, cq->m_blue, alpha);
    glBegin(GL_QUADS);
    glVertex2i(-(cq->m_width/2), -(cq->m_height/2));
    glVertex2i(-(cq->m_width/2), cq->m_height/2);
    glVertex2i(cq->m_width/2, cq->m_height/2);
    glVertex2i(cq->m_width/2, -(cq->m_height/2));
    glEnd();
}

/* Draws a rotated textured quad */
void OpenGL::DrawRotateBlend(TextureQuad *tq, float angle, float alpha)
{
    EnableTexture();
    DisableDepthBuffer();
    EnableBlending();
    glLoadIdentity();
    glTranslatef(tq->x + (tq->width/2), tq->y + (tq->height/2), 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glColor4f(tq->red, tq->green, tq->blue, alpha);
    glBindTexture(GL_TEXTURE_2D, tq->uTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2i(-(tq->width/2), -(tq->height/2));
    glTexCoord2f(0.0f, 0.0f);
    glVertex2i(-(tq->width/2), tq->height/2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2i(tq->width/2, tq->height/2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2i(tq->width/2, -(tq->height/2));
    glEnd();
}

/* Draws a scaled colored quad */
void OpenGL::DrawScale(ColourQuad *cq, float factor)
{
    DisableBlending();
    DisableTexture();
    glLoadIdentity();
    glTranslatef(cq->m_x + (cq->m_width/2), cq->m_y + (cq->m_height/2), 0.0f);
    glScalef(factor, factor, 0);
    glColor3f(cq->m_red, cq->m_green, cq->m_blue);
    glBegin(GL_QUADS);
    glVertex2i(-(cq->m_width/2), -(cq->m_height/2));
    glVertex2i(-(cq->m_width/2), cq->m_height/2);
    glVertex2i(cq->m_width/2, cq->m_height/2);
    glVertex2i(cq->m_width/2, -(cq->m_height/2));
    glEnd();
}

/* Draws a scaled textured quad */
void OpenGL::DrawScale(TextureQuad *tq, float factor)
{
    DisableBlending();
    EnableTexture();
    glLoadIdentity();
    glTranslatef(tq->x + (tq->width/2), tq->y + (tq->height/2), 0.0f);
    glScalef(factor, factor, 0);
    glColor3f(tq->red, tq->green, tq->blue);
    glBindTexture(GL_TEXTURE_2D, tq->uTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2i(-(tq->width/2), -(tq->height/2));
    glTexCoord2f(0.0f, 0.0f);
    glVertex2i(-(tq->width/2), tq->height/2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2i(tq->width/2, tq->height/2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2i(tq->width/2, -(tq->height/2));
    glEnd();
}

/* Draws a rotated scaled colored quad */
void OpenGL::DrawRotateScale(ColourQuad *cq, float angle, float factor)
{
    DisableBlending();
    DisableTexture();
    glLoadIdentity();
    glTranslatef(cq->m_x + (cq->m_width/2), cq->m_y + (cq->m_height/2), 0.0f);
    glScalef(factor, factor, 0);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glColor3f(cq->m_red, cq->m_green, cq->m_blue);
    glBegin(GL_QUADS);
    glVertex2i(-(cq->m_width/2), -(cq->m_height/2));
    glVertex2i(-(cq->m_width/2), cq->m_height/2);
    glVertex2i(cq->m_width/2, cq->m_height/2);
    glVertex2i(cq->m_width/2, -(cq->m_height/2));
    glEnd();
}

/* Draws a scaled rotated textured quad */
void OpenGL::DrawRotateScale(TextureQuad *tq, float angle, float factor)
{
    DisableBlending();
    EnableTexture();
    glLoadIdentity();
    glTranslatef(tq->x + (tq->width/2), tq->y + (tq->height/2), 0.0f);
    glScalef(factor, factor, 0);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glColor3f(tq->red, tq->green, tq->blue);
    glBindTexture(GL_TEXTURE_2D, tq->uTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2i(-(tq->width/2), -(tq->height/2));
    glTexCoord2f(0.0f, 0.0f); glVertex2i(-(tq->width/2), tq->height/2);
    glTexCoord2f(1.0f, 0.0f); glVertex2i(tq->width/2, tq->height/2);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(tq->width/2, -(tq->height/2));
    glEnd();
}

/* Draws a scaled colored quad */
void OpenGL::DrawBlendScale(ColourQuad *cq, float alpha, float factor)
{
    DisableTexture();
    DisableDepthBuffer();
    EnableBlending();
    glLoadIdentity();
    glTranslatef(cq->m_x, cq->m_y, 0.0f);
    glScalef(factor, factor, 0);
    glTranslatef(cq->m_x + (cq->m_width/2), cq->m_y + (cq->m_height/2), 0.0f);
    glBegin(GL_QUADS);
    glVertex2i(-(cq->m_width/2), -(cq->m_height/2));
    glVertex2i(-(cq->m_width/2), cq->m_height/2);
    glVertex2i(cq->m_width/2, cq->m_height/2);
    glVertex2i(cq->m_width/2, -(cq->m_height/2));
    glEnd();
}

/* Draws a scaled textured quad */
void OpenGL::DrawBlendScale(TextureQuad *tq, float alpha, float factor)
{
    EnableTexture();
    DisableDepthBuffer();
    EnableBlending();
    glLoadIdentity();
    glTranslatef(tq->x + (tq->width/2), tq->y + (tq->height/2), 0.0f);
    glScalef(factor, factor, 0);
    glColor4f(tq->red, tq->green, tq->blue, alpha);
    glBindTexture(GL_TEXTURE_2D, tq->uTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2i(-(tq->width/2), -(tq->height/2));
    glTexCoord2f(0.0f, 0.0f); glVertex2i(-(tq->width/2), tq->height/2);
    glTexCoord2f(1.0f, 0.0f); glVertex2i(tq->width/2, tq->height/2);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(tq->width/2, -(tq->height/2));
    glEnd();
}

/* Draws a rotated scaled colored quad */
void OpenGL::DrawRotateBlendScale(ColourQuad *cq, float angle, float alpha, float factor)
{
    DisableTexture();
    DisableDepthBuffer();
    EnableBlending();
    glLoadIdentity();
    glTranslatef(cq->m_x + (cq->m_width/2), cq->m_y + (cq->m_height/2), 0.0f);
    glScalef(factor, factor, 0);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glColor4f(cq->m_red, cq->m_green, cq->m_blue, alpha);
    glBegin(GL_QUADS);
    glVertex2i(-(cq->m_width/2), -(cq->m_height/2));
    glVertex2i(-(cq->m_width/2), cq->m_height/2);
    glVertex2i(cq->m_width/2, cq->m_height/2);
    glVertex2i(cq->m_width/2, -(cq->m_height/2));
    glEnd();
}

/* Draws a rotated scaled textured quad */
void OpenGL::DrawRotateBlendScale(TextureQuad *tq, float angle, float alpha, float factor)
{
    EnableTexture();
    DisableDepthBuffer();
    EnableBlending();
    glLoadIdentity();
    glTranslatef(tq->x + (tq->width/2), tq->y + (tq->height/2), 0.0f);
    glScalef(factor, factor, 0);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glColor4f(tq->red, tq->green, tq->blue, alpha);
    glBindTexture(GL_TEXTURE_2D, tq->uTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2i(-(tq->width/2), -(tq->height/2));
    glTexCoord2f(0.0f, 0.0f); glVertex2i(-(tq->width/2), tq->height/2);
    glTexCoord2f(1.0f, 0.0f); glVertex2i(tq->width/2, tq->height/2);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(tq->width/2, -(tq->height/2));
    glEnd();
}

/* OpenGL destructor */
OpenGL::~OpenGL()
{

}


/*
 * Internal method used to bind a new OpenGL texture.
 *	data -> RGB pixel data.
 *	width -> Width of image in pixels.
 *	height -> Height of image in pixels.
 *  fmt -> Pixel format.
 * Returns the textured identifier.
 */
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


/*
 * Converts a RGB texture to a RGBA texture with (0,255,0) as the alpha channel.
 *	pixels -> Original pixel data.
 *	width -> Width of image.
 *	height -> Height of image.
 * The return value must be explicitly delete[]ed.
 */
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


/* 
 * Loads a texture from a file and returns a handle to it. An alpha channel
 * will be automatically created.
 *	filename -> File to load texture from.
 * Throws std::runtime_error on failure.
 */
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


/* 
 * Loads an alpha texture from a data file.
 *	p_data -> Pointer to data file.
 *	filename -> Name of file to read data from.
 * Throws std::runtime_error on failure.
 */
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


/* 
 * Loads a texture from a file and returns a handle to it.
 *	filename -> File to load texture from.
 * Throws std::runtime_error on failure.
 */
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


/* 
 * Loads a texture from a data file. 
 *	p_data -> Pointer to data file.
 *	filename -> Name of file to load from data file.
 * Throws std::runtime_error on error.
 */
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

/* Sets up OpenGL */
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

/* Resizes the OpenGL scene */
GLvoid OpenGL::ResizeGLScene(GLsizei width, GLsizei height)
{
    if (height == 0) height = 1;
	
    // Reset viewport
    glViewport(0, 0, width, height);

    // Set it to 2D mode
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0f, (GLfloat)m_screen_width, (GLfloat)m_screen_height, 0.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
}


/*
 * Displays a message to the user. On Windows this will appear as a message
 * box. On other platforms the message will be printed to stdout.
 *   text -> Text to display.
 *   title -> Title for Windows message box (optional).
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
 *   text -> Text to display.
 *   title -> Title for Windows error popup (optional)
 */
void OpenGL::ErrorMsg(const char *text, const char *title)
{
#ifdef WIN32
    MessageBox(NULL, text, title, MB_OK | MB_ICONEXCLAMATION);
#else
    fprintf(stderr, "%s\n", text);
#endif	
}


/*
 * Returns the number of frames processed in the last second.
 */
int OpenGL::GetFPS()
{
    return m_fps_rate;
}


/*
 * Stops processing after the current frame.
 */
void OpenGL::Stop()
{
    m_running = false;
}


/*
 * Generate, but do not display the next frame. 
 */
void OpenGL::SkipDisplay()
{
    m_dodisplay = false;
}
