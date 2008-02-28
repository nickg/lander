/*
 * Bitmap.cpp - Implementation of main menu screen.
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

#include "Lander.hpp"

/* Globals */
extern DataFile *g_pData;

/* Loads menu data */
void MainMenu::Load()
{
    OpenGL &opengl = OpenGL::GetInstance();

    // Load textures
    if (!m_hasloaded)
	{
            uStartTexture = opengl.LoadTextureAlpha(g_pData, "StartOption.bmp");
            uHighTexture = opengl.LoadTextureAlpha(g_pData, "HighScoreOption.bmp");
            uOptionsTexture = opengl.LoadTextureAlpha(g_pData, "Options.bmp");
            uExitTexture = opengl.LoadTextureAlpha(g_pData, "ExitOption.bmp");
            uStarTexture = opengl.LoadTextureAlpha(g_pData, "Star.bmp");
	}

    // Create start button
    start.x = (opengl.GetWidth() - 256) / 2;
    start.y = (opengl.GetHeight() - 128) / 2;
    start.height = 32;
    start.width = 256;
    start.uTexture = uStartTexture;

    // Create high score button
    highscore.x = (opengl.GetWidth() - 512) / 2;
    highscore.y = start.y + 32;
    highscore.height = 32;
    highscore.width = 512;
    highscore.uTexture = uHighTexture;

    // Create options button
    options.x = (opengl.GetWidth() - 256) / 2;
    options.y = start.y + 64;
    options.height = 32;
    options.width = 256;
    options.uTexture = uOptionsTexture;

    // Create exit button
    exit.x = (opengl.GetWidth() - 128) / 2;
    exit.y = start.y + 96;
    exit.height = 32;
    exit.width = 128;
    exit.uTexture = uExitTexture;

    // Set fade in state
    m_state = msFadeIn;
    m_fade = 0.0f;

    // Reset the stars
    for (int i = 0; i < NUM_STARS; i++)
	{
            stars[i].active = false;
            stars[i].quad.uTexture = uStarTexture;
            stars[i].scale = 0.5f;
	}
    m_stardelay = 1;
    m_starrotate = 0.0f;

    // Set default selections
    m_starsel = 1.5f;
    m_optsel = 1.0f;
    m_highsel = 1.0f;
    m_exitsel = 1.0f;
	
    // Show a new hint
    m_hint_timeout = 0;
    m_hintidx = 0;
}

/* Processes user input */
void MainMenu::Process()
{
    Input &input = Input::GetInstance();
    OpenGL &opengl = OpenGL::GetInstance();
    ScreenManager &sm = ScreenManager::GetInstance();
    int i;

    // Stop user doing something when they're not supposed to
    if (m_state == msInMenu)
	{
            // Look at keys
            if (input.GetKeyState(SDLK_DOWN) || input.QueryJoystickAxis(1) > 0)
		{
                    // Move the selection down
                    if (m_starsel > 1.4f)
			{
                            m_starsel = 1.0f;
                            m_highsel = 1.5f;
			}
                    else if (m_highsel > 1.4f)
			{
                            m_highsel = 1.0f;
                            m_optsel = 1.5f;
			}
                    else if (m_optsel > 1.4f)
			{
                            m_optsel = 1.0f;
                            m_exitsel = 1.5f;
			}
                    input.ResetKey(SDLK_DOWN);
                    //opengl.di.ResetProp(DIJ_YAXIS);
		}
            else if (input.GetKeyState(SDLK_UP) /*|| opengl.di.QueryJoystick(DIJ_YAXIS) < 0*/)
		{
                    // Move the selection up
                    if (m_highsel > 1.4f)
			{
                            m_highsel = 1.0f;
                            m_starsel = 1.5f;
			}
                    else if (m_optsel > 1.4f)
			{
                            m_optsel = 1.0f;
                            m_highsel = 1.5f;
			}
                    else if (m_exitsel > 1.4f)
			{
                            m_exitsel = 1.0f;
                            m_optsel = 1.5f;
			}
                    input.ResetKey(SDLK_UP);
                    //opengl.di.ResetProp(DIJ_YAXIS);
		}
            else if (input.GetKeyState(SDLK_RETURN)
                     /*|| opengl.di.QueryJoystick(DIJ_BUTTON0) || opengl.di.QueryJoystick(DIJ_BUTTON1)*/ )
		{
                    // Select this option
                    if (m_starsel > 1.4f)
                        m_state = msFadeToStart;
                    else if (m_highsel > 1.4f)
                        m_state = msFadeToHigh;
                    else if (m_optsel > 1.4f)
                        m_state = msFadeToOpt;
                    else if (m_exitsel > 1.4f)
                        m_state = msFadeToExit;
                    input.ResetKey(SDLK_RETURN);
                    /*opengl.di.ResetProp(DIJ_BUTTON0);
                      opengl.di.ResetProp(DIJ_BUTTON1);*/
		}
	}

    // See what menu state we're in
    if (m_state == msFadeIn)
	{
            // Apply the fade to the menu items
            if (m_fade >= 1.0f)
		{
                    // Switch to the next state
                    m_state = msInMenu;
                    m_fade = 1.0f;
		}
            else
                m_fade += MENU_FADE_SPEED;
	}	
    else if (m_state == msFadeToStart)
	{
            // Apply fade
            if (m_fade <= 0.0f)
		{
                    // Move to the game screen
                    sm.SelectScreen("GAME");
                    Game *g = static_cast<Game*>(sm.GetScreenById("GAME"));
                    g->NewGame();
		}
            else
		{
                    m_fade -= MENU_FADE_SPEED;
                    m_starsel += 0.5f;
		}
	}
    else if (m_state == msFadeToHigh)
	{
            // Apply fade
            if (m_fade <= 0.0f)
		{
                    // Move to the high score screen
                    HighScores *hs = static_cast<HighScores*>(sm.GetScreenById("HIGH SCORES"));
                    hs->DisplayScores();
		}
            else
		{
                    m_fade -= MENU_FADE_SPEED;
                    m_highsel += 0.5f;
		}
	}
    else if (m_state == msFadeToOpt)
	{
            // Apply fade
            if (m_fade <= 0.0f)
		{
                    // Move to the options screen
                    // TODO!
                    sm.SelectScreen("MAIN MENU");
		}
            else
		{
                    m_fade -= MENU_FADE_SPEED;
                    m_optsel += 0.5f;
		}
	}
    else if (m_state == msFadeToExit)
	{
            // Apply fade
            if (m_fade <= 0.0f)
		{
                    // Exit the game
                    opengl.Stop();
		}
            else
		{
                    m_fade -= MENU_FADE_SPEED;
                    m_exitsel += 0.5f;
		}
	}

    // Move the stars
    for (i = 0; i < NUM_STARS; i++)
	{
            if (stars[i].active)
		{
                    if (stars[i].xpos > opengl.GetWidth()/2)
			{
                            stars[i].xpos += 4 * (float)cos(stars[i].angle);
                            stars[i].ypos += 4 * (float)sin(stars[i].angle);
			}
                    else
			{
                            stars[i].xpos -= 4 * (float)cos(stars[i].angle);
                            stars[i].ypos -= 4 * (float)sin(stars[i].angle);
			}
                    stars[i].quad.x = (int)stars[i].xpos;
                    stars[i].quad.y = (int)stars[i].ypos;
                    stars[i].scale += 0.003f;

                    // See if it left the screen
                    if ((stars[i].quad.x > opengl.GetWidth()) || 
                        (stars[i].quad.y > opengl.GetHeight()) ||
                        (stars[i].quad.x + stars[i].quad.width < 0) ||
                        (stars[i].quad.y + stars[i].quad.height < 0))
                        stars[i].active = false;
		}
	}

    // Create a new star
    if (--m_stardelay == 0)
	{
            for (i = 0; i < NUM_STARS; i++)
		{
                    if (!stars[i].active)
			{
                            stars[i].quad.height = 20;
                            stars[i].quad.width = 20;
                            stars[i].xpos = (float)(rand()%(opengl.GetWidth()/2)+opengl.GetWidth()/4);
                            stars[i].ypos = (float)(rand()%(opengl.GetHeight()/2)+opengl.GetHeight()/4);
                            stars[i].quad.x = (int)stars[i].xpos;
                            stars[i].quad.y = (int)stars[i].ypos;
                            stars[i].scale = 0.1f;
                            if (stars[i].xpos-opengl.GetWidth()/2 < -0.01f || stars[i].xpos-opengl.GetWidth()/2 > 0.01f)
				{
                                    stars[i].angle = (float)atan((stars[i].ypos-opengl.GetHeight()/2)/(stars[i].xpos-opengl.GetWidth()/2));
                                    stars[i].active = true;
				}
                            break;
			}
		}
            m_stardelay = 1;
	}
}

/* Displays scene to user */
void MainMenu::Display()
{
    int i;

    OpenGL &opengl = OpenGL::GetInstance();

    // Display stars
    for (i = 0; i < NUM_STARS; i++) {
        if (stars[i].active) {
            if (m_state == msFadeIn || m_state == msInMenu)
                opengl.DrawRotateScale(&stars[i].quad, m_starrotate, stars[i].scale);
            else
                opengl.DrawRotateBlendScale(&stars[i].quad, m_starrotate, m_fade, stars[i].scale);
            m_starrotate += 0.005f;
        }
    }

    // Draw logo and menu items
    opengl.DrawBlendScale(&start, m_fade, m_starsel);
    opengl.DrawBlendScale(&highscore, m_fade, m_highsel);
    opengl.DrawBlendScale(&options, m_fade, m_optsel);
    opengl.DrawBlendScale(&exit, m_fade, m_exitsel);

    FreeType &ft = FreeType::GetInstance();

    opengl.Colour(0.0f, 1.0f, 0.0f, m_fade);
    ft.Print(ftHollow, (opengl.GetWidth() - ft.GetStringWidth(ftHollow, S_TITLE))/2, 60, S_TITLE);
	
    // This is a huge hack to neatly format revision data from subversion
    const char *rev = "$Rev$";
    char revstr[4];
    revstr[0] = rev[6];
    revstr[1] = rev[7];
    revstr[2] = rev[8];
    revstr[3] = '\0';
	
    ft.Print
	(
         ftNormal,
         10,
         opengl.GetHeight() - 20,
         S_VERSION,
         revstr
         );

    // Draw some hint texts
    const int numhints = 7;
    const char *hints[][2] = 
	{
            { "Use  the  arrow  keys  to  rotate  the  ship", "" },
            { "Press  the  up  arrow  to  fire  the  thruster", "" },
            { "Smaller  landing  pads  give  you  more  points", "" },
            { "Press  P  to  pause  the  game", "" },
            { "Press  escape  to  self  destruct", "" },
            { "You  can  only  land  safely  when  the", "speed  bar  is  green" },
            { "Collect  the  spinning  rings  to", "unlock  the  landing  pads" }
	};

    if (m_hint_timeout == 0)
	{
            m_hintidx = rand() % numhints;
            m_hint_timeout = HINT_DISPLAY_TIME;
	}
    else
        m_hint_timeout--;
		
    ft.Print
	(
         ftNormal,
         (opengl.GetWidth() - ft.GetStringWidth(ftNormal, hints[m_hintidx][0])) / 2,
         opengl.GetHeight() - 140,
         hints[m_hintidx][0]
         );
    ft.Print
	(
         ftNormal,
         (opengl.GetWidth() - ft.GetStringWidth(ftNormal, hints[m_hintidx][1])) / 2,
         opengl.GetHeight() - 120,
         hints[m_hintidx][1]
         );
}
