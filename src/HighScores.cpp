//
// Copyright (C) 2006-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "HighScores.hpp"
#include "Input.hpp"
#include "InterfaceSounds.hpp"

#include <fstream>
#include <algorithm>
#include <filesystem>

const float HighScores::FADE_IN_SPEED(0.2f);
const float HighScores::FADE_OUT_SPEED(-0.02f);

HighScores::HighScores()
   : hscoreImage("images/hscore.png"),
     largeFont(LocateResource("fonts/VeraBd.ttf"), 15),
     scoreNameFont(LocateResource("fonts/VeraBd.ttf"), 14),
     fwBang(LocateResource("sounds/firework_1.wav"))
{

}

//
// Called just before the screen is to be displayed. Resets variables
// and loads data.
//
void HighScores::Load()
{
   // Set state
   state = hssDisplay;
}

//
// Processes user input. Called at the start of each frame.
//
void HighScores::Process()
{
   Input& input = Input::GetInstance();
   OpenGL& opengl = OpenGL::GetInstance();

   const OpenGL::TimeScale timeScale = opengl.GetTimeScale();

   // Check for input
   if (state == hssDisplay) {
      if (input.QueryAction(Input::SKIP)
          || input.QueryAction(Input::ABORT)
          || input.QueryAction(Input::FIRE)) {

         // Go back to main menu
         fade = FADE_OUT_SPEED;
         for (int i = 0; i < MAX_FIREWORKS; i++) {
            fw[i].em->maxspeed = 200;
            fw[i].speed = 0;
            fw[i].em->createrate = 2.0f;
            fw[i].timeout = 5;
            fw[i].em->life = 0.5f;
         }

         input.ResetAction(Input::SKIP);
         input.ResetAction(Input::FIRE);
         input.ResetAction(Input::ABORT);
         InterfaceSounds::PlaySelect();
      }
   }
   else if (state == hssEnterName)	{
      if (input.QueryAction(Input::FIRE)
          && input.GetInput().size() > 0) {

         // Enter name into high score chart
         scoreFile.Insert(input.GetInput(), newscore);
         input.CloseCharBuffer();
         scoreFile.Save();
         state = hssDisplay;
         flAlpha = 0.0f;
         fade = FADE_IN_SPEED;

         input.ResetAction(Input::FIRE);
         InterfaceSounds::PlaySelect();
      }
   }

   // Process fireworks
   for (int i = 0; i < MAX_FIREWORKS; i++) {
      if (fw[i].active)	{
         fw[i].y -= fw[i].speed * timeScale;
         fw[i].em->xpos = (float)fw[i].x;
         fw[i].em->ypos = (float)fw[i].y;

         if (fw[i].y < fw[i].life && fw[i].timeout < 0) {
            // Blow it up
            fw[i].em->maxspeed = 200;
            fw[i].speed = 0;
            fw[i].em->createrate = 2.0f;
            fw[i].timeout = 5;
            fw[i].em->life = 0.5f;

            fwBang.Play();
         }
         else if (fw[i].timeout > 0)	{
            if (--fw[i].timeout == 0)
               fw[i].active = false;
         }
      }
      else if (rand() % 30 == 0 && fade >= -0.005f)	{
         fw[i].x = rand() % (opengl.GetWidth());
         fw[i].y = opengl.GetHeight();
         fw[i].em->r = (rand() % 100) / 100.0f;
         fw[i].em->g = (rand() % 100) / 100.0f;
         fw[i].em->b = (rand() % 100) / 100.0f;
         fw[i].em->life = 0.2f;
         fw[i].speed = 2 + rand() % 2;
         fw[i].life = rand() % (opengl.GetHeight()-100) + 100;
         fw[i].timeout = -1;
         fw[i].em->createrate = 64.0f;
         fw[i].em->maxspeed = 10.0f;
         fw[i].em->xpos = (float)fw[i].x;
         fw[i].em->ypos = (float)fw[i].y;
         fw[i].active = true;
      }

      fw[i].em->Process(fw[i].active);
   }

   // Fade in or out
   if (fade >= 0.001f)	{
      flAlpha += fade * timeScale;
      if (flAlpha >= 1.0f)
         fade = 0.000000f;
   }
   else if (fade <= -0.001f)	{
      flAlpha += fade * timeScale;
      if (flAlpha < 0.0f)	{
         fade = 0.000000f;

         // Fade to something else
         if (state == hssDisplay)
            ScreenManager::GetInstance().SelectScreen("MAIN MENU");
      }
   }
}

//
// Renders the next frame.
//
void HighScores::Display()
{
   OpenGL& opengl = OpenGL::GetInstance();

   // Draw the fireworks
   for (int i = 0; i < MAX_FIREWORKS; i++)
      fw[i].em->Draw(0, 0);

   // Draw scores
   if (state == hssDisplay) {
      int x = (opengl.GetWidth() - 280) / 2;
      int y = (opengl.GetHeight() - 250) / 2;

      scoreNameFont.SetColour(0.0f, 1.0f, 0.0f, flAlpha);
      for (int i = 0; i < 10; i++) {
         scoreNameFont.Print(x, y + 22*i, scoreFile[i].GetName());
         scoreNameFont.Print(x + 230, y + 22*i, "%d", scoreFile[i].GetScore());
      }
   }

   // Draw other stuff
   const char* hsnext = i18n("Press SPACE or FIRE to return");
   if (state == hssDisplay) {
      int title_x = (opengl.GetWidth() - hscoreImage.GetWidth()) / 2;
      int title_y = 50;
      hscoreImage.Draw(title_x, title_y, 0.0, 1.0, flAlpha);

      int x = (opengl.GetWidth() - largeFont.GetStringWidth(hsnext)) / 2;
      int y = opengl.GetHeight() - 50;

      largeFont.SetColour(0.0f, 0.5f, 1.0f, flAlpha);
      largeFont.Print(x, y, hsnext);
   }
   else if (state == hssEnterName)	{
      Input& input = Input::GetInstance();

      const char* hsscore = i18n("Well done - You got a high score");
      int x = (opengl.GetWidth() - largeFont.GetStringWidth(hsscore)) / 2;
      largeFont.SetColour(0.0f, 1.0f, 0.0f, flAlpha);
      largeFont.Print(x, 100, hsscore);

      const char* hscont = i18n("Press ENTER or FIRE to continue");
      x = (opengl.GetWidth() - largeFont.GetStringWidth(hscont)) / 2;
      int y = opengl.GetHeight() - 60;
      largeFont.Print(x, y, hscont);

      const string name(input.GetInput());
      const char* hsname = i18n("Name?  %s");
      x = (opengl.GetWidth() - largeFont.GetStringWidth(hsname, name.c_str())) / 2;
      y = (opengl.GetHeight() - 50) / 2;
      largeFont.SetColour(0.8f, 0.0f, 1.0f, flAlpha);
      largeFont.Print(x, y, hsname, name.c_str());
   }
}

//
// Loads the highsores from disk.
//
void HighScores::LoadHighScores()
{
   scoreFile.Load();
}

//
// Writes high scores to disk.
//
void HighScores::WriteHighScores()
{
   scoreFile.Save();
}

//
// Displays the highest scores screen to the user.
//
void HighScores::DisplayScores()
{
   LoadHighScores();
   ScreenManager::GetInstance().SelectScreen("HIGH SCORES");
   state = hssDisplay;

   for (int i = 0; i < MAX_FIREWORKS; i++)	{
      fw[i].em->Reset();
      fw[i].active = false;
   }

   // Fade in
   flAlpha = 0.0f;
   fade = FADE_IN_SPEED;
}

//
// Check to see if the player has a high score.
//
void HighScores::CheckScore(int score)
{
   LoadHighScores();
   ScreenManager::GetInstance().SelectScreen("HIGH SCORES");

   if (score > scoreFile[9].GetScore()) {
      // New high score
      state = hssEnterName;
      Input::GetInstance().OpenCharBuffer(ScoreFile::ScoreEntry::MAX_NAME);
      newscore = score;
   }
   else
      state = hssDisplay;

   for (int i = 0; i < MAX_FIREWORKS; i++)	{
      fw[i].em->Reset();
      fw[i].active = false;
   }

   // Fade in
   flAlpha = 0.0f;
   fade = FADE_IN_SPEED;
}

ScoreFile::ScoreFile()
   : needsWrite(false), scores(NUM_SCORES, ScoreEntry("Nobody", 0))
{

}

ScoreFile::~ScoreFile()
{
   if (needsWrite)
      Save();
}

string ScoreFile::GetHighScoreFile()
{
   return GetConfigDir() + "scores";
}

bool ScoreFile::ScoreEntry::operator<(const ScoreFile::ScoreEntry& rhs) const
{
   return this->GetScore() > rhs.GetScore();
}

void ScoreFile::Sort()
{
   std::sort(scores.begin(), scores.end());
}

void ScoreFile::Load()
{
#ifndef EMSCRIPTEN
   // Check for file's existence
   string hsname(GetHighScoreFile());
   if (!filesystem::exists(hsname)) {
      // Write a dummy score file
      Save();
   }
   else {
      // Open highscores file
      ifstream fin(hsname.c_str());
      for (ScoreEntryVecIt it = scores.begin(); it != scores.end(); ++it)
         (*it).ReadFromStream(fin);

      Sort();
   }
#endif
}

void ScoreFile::Save()
{
   if (!needsWrite)
      return;

#ifndef EMSCRIPTEN
   ofstream fout(GetHighScoreFile().c_str());
   for (ScoreEntryVecIt it = scores.begin(); it != scores.end(); ++it)
      (*it).WriteOnStream(fout);
#endif
}

void ScoreFile::Insert(const string& name, int score)
{
   scores[9] = ScoreEntry(name.c_str(), score);
   Sort();
   needsWrite = true;
}

ScoreFile::ScoreEntry::ScoreEntry(const char* name, int score)
   : score(score)
{
   strncpy(this->name, name, MAX_NAME - 1);
}

void ScoreFile::ScoreEntry::WriteOnStream(ostream& os)
{
   os.write(reinterpret_cast<const char*>(&score), sizeof(int));
   os.write(name, MAX_NAME);
}

void ScoreFile::ScoreEntry::ReadFromStream(istream& is)
{
   is.read(reinterpret_cast<char*>(&score), sizeof(int));
   is.read(name, MAX_NAME);
}
