//
// Copyright (C) 2006-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "Menu.hpp"
#include "Input.hpp"
#include "OpenGL.hpp"
#include "Game.hpp"
#include "HighScores.hpp"
#include "InterfaceSounds.hpp"

#include <cmath>

const double MenuStar::ROTATE_SPEED(0.005);
const double MenuStar::ENLARGE_RATE(0.001);
const double MenuStar::INIT_SCALE(0.01);
const double MenuStar::SPEED(4.0);

const int MainMenu::OPTIONS_OFFSET(128);
const unsigned MainMenu::MAX_STARS(80);

const double MenuOption::SEL_ENLARGE(1.2);
const double MenuOption::UNSEL_DIM(0.5);

Image* MenuStar::starImage = NULL;

MainMenu::MainMenu()
   : startOpt("images/start_option.png", OPTIONS_OFFSET, 0),
     scoreOpt("images/score_option.png", OPTIONS_OFFSET, 1),
     optionsOpt("images/options_option.png", OPTIONS_OFFSET, 2),
     exitOpt("images/exit_option.png", OPTIONS_OFFSET, 3),
     titleImage("images/title.png"),
     hintFont(LocateResource("fonts/VeraBd.ttf"), 11)
{

}

void MainMenu::Load()
{
   // Set fade in state
   state = msFadeIn;
   fade = 0.0;
   bigness = 1.0;

   // Show a new hint
   m_hintTimeout = 0.0f;
   hintidx = 0;

   selOption = optStart;
}

void MainMenu::Process()
{
   Input& input = Input::GetInstance();
   OpenGL& opengl = OpenGL::GetInstance();
   ScreenManager& sm = ScreenManager::GetInstance();

   const OpenGL::TimeScale timeScale = opengl.GetTimeScale();

   // Stop user doing something when they're not supposed to
   if (state == msInMenu) {
      // Look at keys
      if (input.QueryAction(Input::DOWN)) {
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

         InterfaceSounds::PlayBleep();
         input.ResetAction(Input::DOWN);
      }
      else if (input.QueryAction(Input::UP)) {
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

         InterfaceSounds::PlayBleep();
         input.ResetAction(Input::UP);
      }
      else if (input.QueryAction(Input::FIRE)) {
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

         InterfaceSounds::PlaySelect();
         input.ResetAction(Input::FIRE);
      }
      else if (input.QueryResetAction(Input::SCREENSHOT))
         OpenGL::GetInstance().DeferScreenShot();
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
         fade += MENU_FADE_SPEED * timeScale;
   }
   else if (state == msFadeToStart) {
      // Apply fade
      if (fade <= 0.0) {
         // Move to the game screen
         sm.SelectScreen("GAME");
         Game* g = static_cast<Game*>(sm.GetScreenById("GAME"));
         g->NewGame();
      }
      else {
         fade -= MENU_FADE_SPEED * timeScale;
         bigness += 0.5f;
      }
   }
   else if (state == msFadeToHigh) {
      // Apply fade
      if (fade <= 0.0f) {
         // Move to the high score screen
         HighScores* hs = static_cast<HighScores*>(sm.GetScreenById("HIGH SCORES"));
         hs->DisplayScores();
      }
      else {
         fade -= MENU_FADE_SPEED * timeScale;
         bigness += 0.5f;
      }
   }
   else if (state == msFadeToOpt) {
      // Apply fade
      if (fade <= 0.0f) {
         // Move to the options screen
         sm.SelectScreen("OPTIONS");
      }
      else {
         fade -= MENU_FADE_SPEED * timeScale;
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
         fade -= MENU_FADE_SPEED * timeScale;
         bigness += 0.5f;
      }
   }

   MoveStars();
}

void MainMenu::MoveStars()
{
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

//
// This is also used by the options screen.
//
void MainMenu::DisplayStars() const
{
   for (const MenuStar& star : stars)
      star.Display();
}

void MainMenu::Display()
{
   OpenGL& opengl = OpenGL::GetInstance();

   DisplayStars();

   // Draw logo and menu items
   startOpt.Display(selOption == optStart, bigness, fade);
   scoreOpt.Display(selOption == optScore, bigness, fade);
   optionsOpt.Display(selOption == optOptions, bigness, fade);
   exitOpt.Display(selOption == optExit, bigness, fade);

   int title_x = (opengl.GetWidth() - titleImage.GetWidth()) / 2;
   int title_y = 100;
   titleImage.Draw(title_x, title_y, 0.0, 1.0, fade);

   // Draw some hint texts
   const int numhints = 7;
   const char* hints[] = {
      i18n("Use the arrow keys to rotate the ship"),
      i18n("Press the up arrow to fire the thruster"),
      i18n("Smaller landing pads give you more points"),
      i18n("Press P to pause the game"),
      i18n("Press escape to self destruct"),
      i18n("You can only land safely when then speed bar is green"),
      i18n("Collect the spinning rings to unlock the landing pads")
   };

   if (m_hintTimeout <= 0.0f) {
      hintidx = rand() % numhints;
      m_hintTimeout = HINT_DISPLAY_TIME;
   }
   else
      m_hintTimeout -= opengl.GetTimeScale();

   int x = (opengl.GetWidth() - hintFont.GetStringWidth(hints[hintidx])) / 2;
   int y = opengl.GetHeight() - 120;
   hintFont.SetColour(0.0f, 1.0f, 0.0f, fade);
   hintFont.Print(x, y, hints[hintidx]);
}

double MenuStar::starRotate = 0.0;

MenuStar::MenuStar()
   : scale(INIT_SCALE)
{
   if (NULL == starImage)
      starImage = new Image("images/star.png");

   const int screenWidth = OpenGL::GetInstance().GetWidth();
   const int screenHeight = OpenGL::GetInstance().GetHeight();

   do {
      x = (double)(rand()%(screenWidth/2) + screenWidth/4);
      y = (double)(rand()%(screenHeight/2) + screenHeight/4);
   } while (y == screenHeight/2 || x == screenWidth/2);

   double ratio = (y - screenHeight/2) / (x - screenWidth/2);
   angle = atan(ratio);
}

void MenuStar::Display(double fade) const
{
   starImage->Draw(x, y, starRotate, scale);
}

bool MenuStar::Move()
{
   const OpenGL::TimeScale timeScale = OpenGL::GetInstance().GetTimeScale();

   if (x > OpenGL::GetInstance().GetWidth() / 2) {
      x += SPEED * cos(angle) * timeScale;
      y += SPEED * sin(angle) * timeScale;
   }
   else {
      x -= SPEED * cos(angle) * timeScale;
      y -= SPEED * sin(angle) * timeScale;
   }

   scale += ENLARGE_RATE * timeScale;
   starRotate += ROTATE_SPEED * timeScale;

   // Has it left the screen?
   return (x > OpenGL::GetInstance().GetWidth()
           || y > OpenGL::GetInstance().GetHeight()
           || x + starImage->GetWidth() < 0
           || y + starImage->GetWidth() < 0);
}

MenuOption::MenuOption(const char* imgFile, int off, int order)
   : m_image(imgFile),
     m_off(off),
     m_order(order)
{
}

void MenuOption::Display(bool selected, double bigness, double fade) const
{
   OpenGL& opengl = OpenGL::GetInstance();
   int x = (opengl.GetWidth() - m_image.GetWidth()) / 2;
   int y = (opengl.GetHeight() - m_off) / 2 + (m_order*m_image.GetHeight());
   double scale = selected ? (bigness > SEL_ENLARGE ? bigness : SEL_ENLARGE) : 1.0;
   double white = selected ? 1.0 : UNSEL_DIM;
   m_image.Draw(x, y, 0.0, scale, fade, white);
}
