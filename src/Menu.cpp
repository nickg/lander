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

#include "Menu.hpp"
#include "Input.hpp"
#include "OpenGL.hpp"
#include "Lander.hpp"
#include "HighScores.hpp"

const double MenuStar::ROTATE_SPEED(0.005);
const double MenuStar::ENLARGE_RATE(0.001);
const double MenuStar::INIT_SCALE(0.01);
const double MenuStar::SPEED(4.0);

const int MainMenu::OPTIONS_OFFSET(128);
const int MainMenu::HINT_DISPLAY_TIME(140);
const double MainMenu::MENU_FADE_SPEED(0.1);
const unsigned MainMenu::MAX_STARS(80);

const double MenuOption::SEL_ENLARGE(1.2);
const double MenuOption::UNSEL_DIM(0.5);

Image *MenuStar::starImage = NULL;

MainMenu::MainMenu()
   : startOpt("images/start_option.png", OPTIONS_OFFSET, 0),
     scoreOpt("images/score_option.png", OPTIONS_OFFSET, 1),
     optionsOpt("images/options_option.png", OPTIONS_OFFSET, 2),
     exitOpt("images/exit_option.png", OPTIONS_OFFSET, 3),
     titleImage("images/title.png")
{
   
}

void MainMenu::Load()
{
   // Set fade in state
   state = msFadeIn;
   fade = 0.0;
   bigness = 1.0;

   // Show a new hint
   hint_timeout = 0;
   hintidx = 0;

   selOption = optStart;
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
         switch (selOption) {
         case optStart:
            selOption = optScore;
            break;
         case optScore:
            selOption = optOptions;
            break;
         case optOptions:
            selOption = optExit;
            break;
         default:
            break;
         }
         
         input.ResetKey(SDLK_DOWN);
         //opengl.di.ResetProp(DIJ_YAXIS);
      }
      else if (input.GetKeyState(SDLK_UP) /*|| opengl.di.QueryJoystick(DIJ_YAXIS) < 0*/) {
         // Move the selection up
         switch (selOption) {
         case optScore:
            selOption = optStart;
            break;
         case optOptions:
            selOption = optScore;
            break;
         case optExit:
            selOption = optOptions;
            break;
         default:
            break;
         }
         
         input.ResetKey(SDLK_UP);
         //opengl.di.ResetProp(DIJ_YAXIS);
      }
      else if (input.GetKeyState(SDLK_RETURN)
               /*|| opengl.di.QueryJoystick(DIJ_BUTTON0) || opengl.di.QueryJoystick(DIJ_BUTTON1)*/ ) {
         // Select this option
         switch (selOption) {
         case optStart:
            state = msFadeToStart;
            break;
         case optScore:
            state = msFadeToHigh;
            break;
         case optOptions:
            state = msFadeToOpt;
            break;
         case optExit:
            state = msFadeToExit;
            break;
         }
         
         input.ResetKey(SDLK_RETURN);
         /*opengl.di.ResetProp(DIJ_BUTTON0);
           opengl.di.ResetProp(DIJ_BUTTON1);*/
      }
   }
   
   // See what menu state we're in
   if (state == msFadeIn) {
      // Apply the fade to the menu items
      if (fade >= 1.0) {
         // Switch to the next state
         state = msInMenu;
         fade = 1.0;
      }
      else
         fade += MENU_FADE_SPEED;
   }	
   else if (state == msFadeToStart) {
      // Apply fade
      if (fade <= 0.0) {
         // Move to the game screen
         sm.SelectScreen("GAME");
         Game *g = static_cast<Game*>(sm.GetScreenById("GAME"));
         g->NewGame();
      }
      else {
         fade -= MENU_FADE_SPEED;
         bigness += 0.5f;
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
         bigness += 0.5f;
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
         bigness += 0.5f;
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
         bigness += 0.5f;
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


   for (StarListIt it = stars.begin(); it != stars.end(); ++it)
      (*it).Display();
   
   // Draw logo and menu items
   startOpt.Display(selOption == optStart, bigness, fade);
   scoreOpt.Display(selOption == optScore, bigness, fade);
   optionsOpt.Display(selOption == optOptions, bigness, fade);
   exitOpt.Display(selOption == optExit, bigness, fade);

   FreeType &ft = FreeType::GetInstance();

   int title_x = (opengl.GetWidth() - titleImage.GetWidth()) / 2;
   int title_y = 100;
   titleImage.Draw(title_x, title_y, 0.0, 1.0, fade);
   
   // Draw some hint texts
   const int numhints = 7;
   const char *hints[][2] = {
      { i18n("Use the arrow keys to rotate the ship"), "" },
      { i18n("Press the up arrow to fire the thruster"), "" },
      { i18n("Smaller landing pads give you more points"), "" },
      { i18n("Press P to pause the game"), "" },
      { i18n("Press escape to self destruct"), "" },
      // TODO: automatically split text into multiple lines
      { i18n("You can only land safely when the"),
        i18n("speed bar is green") },
      { i18n("Collect the spinning rings to"),
        i18n("unlock the landing pads") }
   };

   if (hint_timeout == 0) {
      hintidx = rand() % numhints;
      hint_timeout = HINT_DISPLAY_TIME;
   }
   else
      hint_timeout--;

   opengl.Colour(0.0f, 1.0f, 0.0f, fade);   
   ft.Print(ftNormal,
            (opengl.GetWidth() - ft.GetStringWidth(ftNormal, hints[hintidx][0])) / 2,
            opengl.GetHeight() - 120,
            hints[hintidx][0]);
   ft.Print(ftNormal,
            (opengl.GetWidth() - ft.GetStringWidth(ftNormal, hints[hintidx][1])) / 2,
            opengl.GetHeight() - 100,
            hints[hintidx][1]);
}

double MenuStar::starRotate = 0.0;

MenuStar::MenuStar()
   : scale(INIT_SCALE), active(false)
{   
   if (NULL == starImage)
      starImage = new Image("images/star.png");

   const int screenWidth = OpenGL::GetInstance().GetWidth();
   const int screenHeight = OpenGL::GetInstance().GetHeight();

   pos = Position((float)(rand()%(screenWidth/2) + screenWidth/4),
                  (float)(rand()%(screenHeight/2) + screenHeight/4));

   float ratio = (pos.GetY() - screenHeight/2) / (pos.GetX() - screenWidth/2);
   float angle = atanf(ratio);
   vel = Velocity::Project(SPEED, angle);
}

void MenuStar::Display(double fade)
{
   starImage->Draw(pos.GetX(), pos.GetY(), starRotate, scale);
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
           || pos.GetX() + starImage->GetWidth() < 0
           || pos.GetY() + starImage->GetWidth() < 0);
}

MenuOption::MenuOption(const char *imgFile, int off, int order)
   : image(imgFile)
{
   y = (OpenGL::GetInstance().GetHeight() - off) / 2 + (order*image.GetHeight());
}

void MenuOption::Display(bool selected, double bigness, double fade) const
{
   int x = (OpenGL::GetInstance().GetWidth() - image.GetWidth()) / 2;
   double scale = selected ? (bigness > SEL_ENLARGE ? bigness : SEL_ENLARGE) : 1.0;
   double white = selected ? 1.0 : UNSEL_DIM;
   image.Draw(x, y, 0.0, scale, fade, white);
}
