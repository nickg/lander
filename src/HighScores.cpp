/*
 * Bitmap.cpp - Implementation of high scores screen.
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

#include "HighScores.hpp"
#include "Input.hpp"
#include "FreeType.hpp"

#include "Lander.hpp" // TODO: Remove


extern DataFile *g_pData;


/*
 * Called just before the screen is to be displayed. Resets variables
 * and loads data.
 */
void HighScores::Load()
{
   OpenGL &opengl = OpenGL::GetInstance();

   // Load graphics
   if (!hasloaded) {
      uHighScore = opengl.LoadTextureAlpha(g_pData, "HighScore.bmp");

      hasloaded = true;
   }

   // High score label
   hscore.x = (opengl.GetWidth() - 512) / 2;
   hscore.y = 50;
   hscore.width = 512;
   hscore.height = 64;
   hscore.uTexture = uHighScore;

   // Set state
   state = hssDisplay;
}


/* 
 * Processes user input. Called at the start of each frame.
 */
void HighScores::Process()
{
   Input &input = Input::GetInstance();
   OpenGL &opengl = OpenGL::GetInstance();
   int i;

   // Check for input
   if (state == hssDisplay) {
      if (input.GetKeyState(SDLK_SPACE) 
          || input.QueryJoystickButton(0)
          || input.QueryJoystickButton(0) 
          || input.GetKeyState(SDLK_RETURN) 
          || input.GetKeyState(SDLK_ESCAPE)) {
       
         // Go back to main menu
         fade = HS_FADE_OUT_SPEED;
         for (i = 0; i < MAX_FIREWORKS; i++)
            {
               fw[i].em->maxspeed = 200;
               fw[i].speed = 0;
               fw[i].em->createrate = 2.0f;
               fw[i].timeout = 5;
               fw[i].em->life = 1.0f;
               input.ResetKey(SDLK_RETURN);
               input.ResetKey(SDLK_SPACE);
            }
      }
   }
   else if (state == hssEnterName)	{
      if ((input.GetKeyState(SDLK_RETURN) 
           || input.QueryJoystickButton(0) 
           || input.QueryJoystickButton(1))
          && strlen(input.GetInput()) > 0) {
       
         // Enter name into high score chart
         scoreFile.Insert(input.GetInput(), newscore);
         input.CloseCharBuffer();
         scoreFile.Save();
         state = hssDisplay;
         flAlpha = 0.0f;
         fade = HS_FADE_IN_SPEED;
			
         input.ResetKey(SDLK_RETURN);
         input.ResetJoystickButton(0);
         input.ResetJoystickButton(1);
      }
   }

   // Process fireworks
   for (i = 0; i < MAX_FIREWORKS; i++) {
      if (fw[i].active)	{
         fw[i].y-=fw[i].speed;
         fw[i].em->xpos = (float)fw[i].x;
         fw[i].em->ypos = (float)fw[i].y;
			
         if (fw[i].y < fw[i].life && fw[i].timeout < 0) {
            // Blow it up
            fw[i].em->maxspeed = 200;
            fw[i].speed = 0;
            fw[i].em->createrate = 2.0f;
            fw[i].timeout = 5;
            fw[i].em->life = 1.0f;
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
         fw[i].em->life = 0.5f;
         fw[i].speed = 2 + rand() % 2;
         fw[i].life = rand() % (opengl.GetHeight()-100) + 100;
         fw[i].timeout = -1;
         fw[i].em->createrate = 64.0f;
         fw[i].em->maxspeed = 10.0f;
         fw[i].em->xpos = (float)fw[i].x;
         fw[i].em->ypos = (float)fw[i].y;
         fw[i].active = true;
      }
   }

   // Fade in or out
   if (fade >= 0.001f)	{
      flAlpha += fade;
      if (flAlpha >= 1.0f)
         fade = 0.000000f;
   }
   else if (fade <= -0.001f)	{
      flAlpha += fade;
      if (flAlpha < 0.0f)	{
         fade = 0.000000f;

         // Fade to something else
         if (state == hssDisplay)
            ScreenManager::GetInstance().SelectScreen("MAIN MENU");
      }
   }
}


/* 
 * Renders the next frame.
 */
void HighScores::Display()
{
   OpenGL &opengl = OpenGL::GetInstance();
   FreeType &ft = FreeType::GetInstance();

   // Draw the fireworks
   for (int i = 0; i < MAX_FIREWORKS; i++)	{
      if (fw[i].active)
         fw[i].em->Draw();
      else
         fw[i].em->Draw(0, 0, false);
   }

   // Draw scores
   if (state == hssDisplay) {
      int x = (opengl.GetWidth() - 280) / 2;
      int y = (opengl.GetHeight() - 250) / 2;

      opengl.Colour(0.0f, 1.0f, 0.0f, flAlpha);
      for (int i = 0; i < 10; i++) {
         ft.Print(ftScoreName, x, y + 22*i, "%s", scoreFile[i].GetName());
         ft.Print(ftScoreName, x + 230, y + 22*i, "%d", scoreFile[i].GetScore());
      }	
   }

   // Draw other stuff
   const char *hsnext = i18n("Press  SPACE  or  FIRE  to  return");
   if (state == hssDisplay) {
      opengl.DrawBlend(&hscore, flAlpha);
      opengl.Colour(0.0f, 0.5f, 1.0f, flAlpha);
      ft.Print
         (
          ftLarge, 
          (opengl.GetWidth() - ft.GetStringWidth(ftLarge, hsnext)) / 2, 
          opengl.GetHeight() - 50, 
          hsnext
          );
   }
   else if (state == hssEnterName)	{
      Input &input = Input::GetInstance();

      const char *hsscore = i18n("Well  done  -  You  got  a  high  score");
      opengl.Colour(0.0f, 1.0f, 0.0f, flAlpha);
      ft.Print
         (
          ftLarge, 
          (opengl.GetWidth() - ft.GetStringWidth(ftLarge, hsscore)) / 2,
          100, 
          hsscore
          );

      const char *hscont = i18n("Press  ENTER  or  FIRE  to  continue");
      ft.Print
         (
          ftLarge, 
          (opengl.GetWidth() - ft.GetStringWidth(ftLarge, hscont)) / 2, 
          opengl.GetHeight() - 60, 
          hscont
          );

      const char *hsname = i18n("Name?  %s");
      opengl.Colour(0.8f, 0.0f, 1.0f, flAlpha);
      ft.Print
         (
          ftLarge, 
          (opengl.GetWidth() 
           - ft.GetStringWidth(ftLarge, input.GetInput()) 
           - ft.GetStringWidth(ftLarge, hsname)
           ) / 2, 
          (opengl.GetHeight() - 50) / 2, 
          hsname, 
          input.GetInput()
          );
   }
}

/* 
 * Loads the highsores from disk.
 */
void HighScores::LoadHighScores()
{
   scoreFile.Load();
}

/*
 * Writes high scores to disk.
 */
void HighScores::WriteHighScores()
{
   scoreFile.Save();
}


/* 
 * Displays the highest scores screen to the user.
 */
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
   fade = HS_FADE_IN_SPEED;
}


/* 
 * Check to see if the player has a high score.
 */
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
   fade = HS_FADE_IN_SPEED;
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

bool operator<(const ScoreFile::ScoreEntry &a, const ScoreFile::ScoreEntry &b)
{
   return a.GetScore() > b.GetScore();
}

void ScoreFile::Sort()
{
   sort(scores.begin(), scores.end());
}

void ScoreFile::Load()
{
   // Check for file's existence
   if (!File::Exists(File::LocateResource("Highscores", "dat"))) {
      // Write a dummy score file
      Save();
   }
   else {
      // Open highscores file
      const char *fname = File::LocateResource("Highscores", "dat");
      ifstream fin(fname);
      for (ScoreEntryVecIt it = scores.begin(); it != scores.end(); ++it)
         (*it).ReadFromStream(fin);
      
      Sort();
   }
}

void ScoreFile::Save()
{
   if (!needsWrite)
      return;

   const char *fname = File::LocateResource("Highscores", "dat");
   ofstream fout(fname);
   for (ScoreEntryVecIt it = scores.begin(); it != scores.end(); ++it)
      (*it).WriteOnStream(fout);
}

void ScoreFile::Insert(const char *name, int score)
{
   scores[9] = ScoreEntry(name, score);
   Sort();
   needsWrite = true;
}

ScoreFile::ScoreEntry::ScoreEntry(const char *name, int score)
   : score(score)
{
   strncpy(this->name, name, MAX_NAME);
}

void ScoreFile::ScoreEntry::WriteOnStream(ostream &os)
{
   os.write((const char*)&score, sizeof(int));
   os.write(name, MAX_NAME);
}

void ScoreFile::ScoreEntry::ReadFromStream(istream &is)
{
   is.read((char*)&score, sizeof(int));
   is.read(name, MAX_NAME);
}