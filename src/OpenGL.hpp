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
#include "DataFile.hpp"
#include "FreeType.hpp"

/* Constants */
#define WINDOW_TITLE "Lunar Lander"
#define FRAME_RATE 35
#define PI 3.1415926535f

typedef GLuint Texture;


/* A coloured quad */
class ColourQuad
{
    public:
    ColourQuad(int x=0, int y=0, int width=0, int height=0, float r=1, float g=1, float b=1)
        : m_x(x), m_y(y), m_width(width), m_height(height), m_red(r), 
          m_green(g), m_blue(b)
    { }
    int m_x, m_y, m_width, m_height;
    float m_red, m_green, m_blue;
};


/* 
 * A polygon with four points and a texture.
 */
struct TextureQuad
{
    TextureQuad(int qx=0, int qy=0, int width=0, int height=0, GLuint tex=0,
                float r=1, float g=1, float b=1)
        : x(qx), y(qy), width(width), height(height), red(r), 
          green(g), blue(b), uTexture(tex)
    { }
    int x, y, width, height;
    float red, green, blue;
    Texture uTexture;
};


/*
 * A generic polygon.
 */
struct Poly
{
    Poly() 
        : pointcount(0), xpos(0), ypos(0), uTexture(0), texX(0), texwidth(1.0f)
    { }
	
    static const int MAX_POINTS = 4;
    struct Point 
    {
        int x, y;
    } points[MAX_POINTS];	// MAX_PointS=4 at the moment (!)
    int pointcount, xpos, ypos;
    GLuint uTexture;
    float texX, texwidth;
};

/* 
 * A wrapper around common 2D OpenGL functions.
 */
class OpenGL
{
public:
    // Public methods
    OpenGL();
    ~OpenGL();

    static OpenGL &GetInstance();

    void Init(int width, int height, int depth, bool fullscreen);
    void MsgBox(const char *text, const char *title="Message");
    void ErrorMsg(const char *text, const char *title="Error");
    void Stop();
    void Run();
    void SkipDisplay();
    int GetFPS();

    GLuint LoadTexture(const char *filename);
    GLuint LoadTexture(DataFile *p_data, const char *filename);
    GLuint LoadTextureAlpha(const char *filename);
    GLuint LoadTextureAlpha(DataFile *p_data, const char *filename);

    void Viewport(int x, int y, int width, int height);

    // Renderer functions
    void Draw(ColourQuad *cq);
    void Draw(TextureQuad *tq);
    void Draw(Poly *cp);
    void DrawRotate(ColourQuad *cq, float angle);
    void DrawRotate(TextureQuad *tq, float angle);
    void DrawBlend(ColourQuad *cq, float alpha);
    void DrawBlend(TextureQuad *tq, float alpha);
    void DrawRotateBlend(ColourQuad *cq, float angle, float alpha);
    void DrawRotateBlend(TextureQuad *tq, float angle, float alpha);
    void DrawScale(ColourQuad *cq, float factor);
    void DrawScale(TextureQuad *tq, float factor);
    void DrawRotateScale(ColourQuad *cq, float angle, float factor);
    void DrawRotateScale(TextureQuad *tq, float angle, float factor);
    void DrawBlendScale(ColourQuad *cq, float alpha, float factor);
    void DrawBlendScale(TextureQuad *tq, float alpha, float factor);
    void DrawRotateBlendScale(ColourQuad *cq, float angle, float alpha, float factor);
    void DrawRotateBlendScale(TextureQuad *tq, float angle, float alpha, float factor);

    inline int GetWidth()
    { 
        return m_screen_width;
    }

    inline int GetHeight()
    { 
        return m_screen_height;
    }

    inline void SelectTexture(GLuint uTexture)
    {
        glBindTexture(GL_TEXTURE_2D, uTexture);
    }

    inline void ClearColour(float r, float g, float b)
    { 
        glClearColor(r, g, b, 0.0f); 
    }

    inline void Colour(float r, float g, float b)
    { 
        glColor3f(r, g, b); 
    }

    inline void Colour(float r, float g, float b, float a)
    { 
        glColor4f(r, g, b, a); 
    }

    inline void EnableTexture() 
    { 
        if (!m_textureon)
            { 
                glEnable(GL_TEXTURE_2D); 
                m_textureon = true; 
            } 
    }

    inline void DisableTexture() 
    { 
        if (m_textureon)
            { 
                glDisable(GL_TEXTURE_2D); 
                m_textureon = false; 
            } 
    }

    inline void EnableBlending() 
    { 
        if (!m_blendon)
            { 
                glEnable(GL_BLEND); 
                m_blendon = true; 
            } 
    }

    inline void DisableBlending() 
    { 
        if (m_blendon)
            { 
                glDisable(GL_BLEND);
                m_blendon = false;
            }
    } 
	
    inline void EnableDepthBuffer() 
    { 
        if (!m_depthon)
            {
                glEnable(GL_DEPTH_TEST); 
                m_depthon = true; 
            } 
    }
	
    inline void DisableDepthBuffer() 
    { 
        if (m_depthon)
            { 
                glDisable(GL_DEPTH_TEST); 
                m_depthon = false; 
            } 
    } 
	
    static const Texture INVALID_TEXTURE = 0xFFFFFFFF;

private:
    // Private methods
    GLvoid ResizeGLScene(GLsizei width, GLsizei height);
    GLuint BindTexture(unsigned char *data, int width, int height, int fmt);
    GLubyte *BuildAlphaChannel(const unsigned char *pixels, int width, int height);
    bool InitGL();
    void DrawGLScene();
    string SDLErrorString();
    void RuntimeError(string mess);

    // Window related variables
    int m_screen_width, m_screen_height, m_screen_depth;
    bool m_fullscreen;
    bool m_running, m_active, m_dodisplay;

    // State variables
    bool m_textureon, m_blendon, m_depthon;

    // Frame rate variables
    int m_fps_lastcheck, m_fps_framesdrawn, m_fps_rate;
};

#endif
