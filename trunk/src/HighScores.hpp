/*
 * HighScores.hpp -- Definition of high score screen.
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

#ifndef INC_HIGHSCORES_HPP
#define INC_HIGHSCORES_HPP

#include "Screens.hpp"

#define HS_FADE_IN_SPEED	0.2f
#define HS_FADE_OUT_SPEED	-0.02f

class HighScores : public Screen {
public:
   HighScores() : hasloaded(false) { }
   virtual ~HighScores() { }
	
   void Load();
   void Process();
   void Display();
   void LoadHighScores();
   void WriteHighScores();
   void DisplayScores();
   void CheckScore(int score);
   
private:
   void SortScores();
   void SwapScores(int a, int b);
	
   enum HighScoreState { hssDisplay, hssEnterName };
	
   bool hasloaded;
   int newscore;
   TextureQuad hscore;
   float flAlpha, fade;
   HighScoreState state;
	
   GLuint uHighScore;
	
   // An entry in the highscores chart
   static const int NUM_SCORES = 10;
   struct ScoreEntry
   {
      static const int MAX_NAME_LEN = 32;
      char name[MAX_NAME_LEN];
      int score;
   } scores[NUM_SCORES];
	
   // Fireworks
   static const int MAX_FIREWORKS = 7;
   class Firework {
   public:
      Firework() : active(false)
      { 
         em = new Emitter(-100, -100, 1.0f, 1.0f, 1.0f);
      }
		
      ~Firework()
      { 
         delete em; 
      }
		
      Emitter *em;
      int x, y, speed, life, timeout;
      bool active;
   } fw[MAX_FIREWORKS];
};


#endif
