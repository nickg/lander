/*
 * Menu.cpp -- Implementation of main menu screen.
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

const float MenuStar::ROTATE_SPEED(0.005f);
const float MenuStar::ENLARGE_RATE(0.003f);
const float MenuStar::INIT_SCALE(0.1f);
const float MenuStar::SPEED(4.0f);
const int MenuStar::TEXTURE_SIZE(20);

extern DataFile *g_pData;

/* Loads menu data */
void MainMenu::Load()
{
   OpenGL &opengl = OpenGL::GetInstance();

   // Load textures
   if (!hasloaded) {
      uStartTexture = opengl.LoadTextureAlpha(g_pData, "StartOption.bmp");
      uHighTexture = opengl.LoadTextureAlpha(g_pData, "HighScoreOption.bmp");
      uOptionsTexture = opengl.LoadTextureAlpha(g_pData, "Options.bmp");
      uExitTexture = opengl.LoadTextureAlpha(g_pData, "ExitOption.bmp");
      hasloaded = true;
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
   state = msFadeIn;
   fade = 0.0f;

   // Set default selections
   starsel = 1.5f;
   optsel = 1.0f;
   highsel = 1.0f;
   exitsel = 1.0f;
	
   // Show a new hint
   hint_timeout = 0;
   hintidx = 0;
}

void MainMenu::Process()
{
   Input &input = Input::GetInstance();
   OpenGL &opengl = OpenGL::GetInstance();
   ScreenManager &sm = ScreenManager::GetInstance();

   // Stop user doing something when they're not supposed to
   if (state == msInMenu) {
      // Look at keys
      if (input.GetKeyState(SDLK_DOWN) || input.QueryJoystickAxis(1) > 0) {
         // Move the selection down
         if (starsel > 1.4f) {
            starsel = 1.0f;
            highsel = 1.5f;
         }
         else if (highsel > 1.4f) {
            highsel = 1.0f;
            optsel = 1.5f;
         }
         else if (optsel > 1.4f) {
            optsel = 1.0f;
            exitsel = 1.5f;
         }
         input.ResetKey(SDLK_DOWN);
         //opengl.di.ResetProp(DIJ_YAXIS);
      }
      else if (input.GetKeyState(SDLK_UP) /*|| opengl.di.QueryJoystick(DIJ_YAXIS) < 0*/) {
         // Move the selection up
         if (highsel > 1.4f) {
            highsel = 1.0f;
            starsel = 1.5f;
         }
         else if (optsel > 1.4f) {
            optsel = 1.0f;
            highsel = 1.5f;
         }
         else if (exitsel > 1.4f) {
            exitsel = 1.0f;
            optsel = 1.5f;
         }
         input.ResetKey(SDLK_UP);
         //opengl.di.ResetProp(DIJ_YAXIS);
      }
      else if (input.GetKeyState(SDLK_RETURN)
               /*|| opengl.di.QueryJoystick(DIJ_BUTTON0) || opengl.di.QueryJoystick(DIJ_BUTTON1)*/ ) {
         // Select this option
         if (starsel > 1.4f)
            state = msFadeToStart;
         else if (highsel > 1.4f)
            state = msFadeToHigh;
         else if (optsel > 1.4f)
            state = msFadeToOpt;
         else if (exitsel > 1.4f)
            state = msFadeToExit;
         input.ResetKey(SDLK_RETURN);
         /*opengl.di.ResetProp(DIJ_BUTTON0);
           opengl.di.ResetProp(DIJ_BUTTON1);*/
      }
   }
   
   // See what menu state we're in
   if (state == msFadeIn) {
      // Apply the fade to the menu items
      if (fade >= 1.0f) {
         // Switch to the next state
         state = msInMenu;
         fade = 1.0f;
      }
      else
         fade += MENU_FADE_SPEED;
   }	
   else if (state == msFadeToStart) {
      // Apply fade
      if (fade <= 0.0f) {
         // Move to the game screen
         sm.SelectScreen("GAME");
         Game *g = static_cast<Game*>(sm.GetScreenById("GAME"));
         g->NewGame();
      }
      else {
         fade -= MENU_FADE_SPEED;
         starsel += 0.5f;
      }
   }
   else if (state == msFadeToHigh) {
      // Apply fade
      if (fade <= 0.0f) {
         // Move to the high score screen
         HighScores *hs = static_cast<HighScores*>(sm.GetScreenById("HIGH SCORES"));
         hs->DisplayScores();
      }
      else {
         fade -= MENU_FADE_SPEED;
         highsel += 0.5f;
      }
   }
   else if (state == msFadeToOpt) {
      // Apply fade
      if (fade <= 0.0f) {
         // Move to the options screen
         // TODO!
         sm.SelectScreen("MAIN MENU");
      }
      else {
         fade -= MENU_FADE_SPEED;
         optsel += 0.5f;
      }
   }
   else if (state == msFadeToExit) {
      // Apply fade
      if (fade <= 0.0f) {
         // Exit the game
         opengl.Stop();
      }
      else {
         fade -= MENU_FADE_SPEED;
         exitsel += 0.5f;
      }
   }

   // Move the stars
   StarListIt it = stars.begin();
   while (it != stars.end()) {
      if ((*it).Move()) {
         // Star has left the screen
         it = stars.erase(it);
      }
      else
         ++it;
   }

   // Maybe create a new star
   if (stars.size() < MAX_STARS) 
      stars.push_back(MenuStar()); 
}

void MainMenu::Display()
{
   OpenGL &opengl = OpenGL::GetInstance();


   for (StarListIt it = stars.begin(); it != stars.end(); ++it) {
      (*it).Display();
   }
   
   // Draw logo and menu items
   opengl.DrawBlendScale(&start, fade, starsel);
   opengl.DrawBlendScale(&highscore, fade, highsel);
   opengl.DrawBlendScale(&options, fade, optsel);
   opengl.DrawBlendScale(&exit, fade, exitsel);

   FreeType &ft = FreeType::GetInstance();

   opengl.Colour(0.0f, 1.0f, 0.0f, fade);
   const char *stitle = i18n("LUNAR  LANDER");
   ft.Print(ftHollow, (opengl.GetWidth() - ft.GetStringWidth(ftHollow, stitle))/2, 100, stitle);	
   // Draw some hint texts
   const int numhints = 7;
   const char *hints[][2] = {
      { "Use  the  arrow  keys  to  rotate  the  ship", "" },
      { "Press  the  up  arrow  to  fire  the  thruster", "" },
      { "Smaller  landing  pads  give  you  more  points", "" },
      { "Press  P  to  pause  the  game", "" },
      { "Press  escape  to  self  destruct", "" },
      { "You  can  only  land  safely  when  the", "speed  bar  is  green" },
      { "Collect  the  spinning  rings  to", "unlock  the  landing  pads" }
   };

   if (hint_timeout == 0) {
      hintidx = rand() % numhints;
      hint_timeout = HINT_DISPLAY_TIME;
   }
   else
      hint_timeout--;
   
   ft.Print(ftNormal,
            (opengl.GetWidth() - ft.GetStringWidth(ftNormal, hints[hintidx][0])) / 2,
            opengl.GetHeight() - 120,
            hints[hintidx][0]);
   ft.Print(ftNormal,
            (opengl.GetWidth() - ft.GetStringWidth(ftNormal, hints[hintidx][1])) / 2,
            opengl.GetHeight() - 100,
            hints[hintidx][1]);
}

bool MenuStar::hasLoaded = false;
float MenuStar::starRotate = 0.0f;
GLuint MenuStar::uStarTexture = 0;

MenuStar::MenuStar()
   : active(false),
     scale(INIT_SCALE)
{
   OpenGL &opengl = OpenGL::GetInstance();
   
   if (!hasLoaded) {
      uStarTexture = opengl.LoadTextureAlpha(g_pData, "Star.bmp");
      hasLoaded = true;
   }

   const int screenWidth = opengl.GetWidth();
   const int screenHeight = opengl.GetHeight();

   pos = Position((float)(rand()%(screenWidth/2) + screenWidth/4),
                  (float)(rand()%(screenHeight/2) + screenHeight/4));

   float ratio = (pos.GetY() - screenHeight/2) / (pos.GetX() - screenWidth/2);
   float angle = atanf(ratio);
   vel = Velocity::Project(SPEED, angle);
}

void MenuStar::Display(float fade)
{
   TextureQuad quad(pos.GetX(), pos.GetY(), TEXTURE_SIZE, TEXTURE_SIZE,
                    uStarTexture);
   OpenGL::GetInstance().DrawRotateBlendScale(&quad, starRotate, fade, scale);
   starRotate += ROTATE_SPEED;
}

bool MenuStar::Move()
{
   if (pos.GetX() > OpenGL::GetInstance().GetWidth() / 2)
      pos += vel;
   else
      pos -= vel;
   
   scale += ENLARGE_RATE;

   // Has it left the screen?
   return (pos.GetX() > OpenGL::GetInstance().GetWidth()
           || pos.GetY() > OpenGL::GetInstance().GetHeight()
           || pos.GetX() + TEXTURE_SIZE < 0
           || pos.GetY() + TEXTURE_SIZE < 0);
}
