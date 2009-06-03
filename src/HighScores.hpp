//
// HighScores.hpp -- Definition of high score screen.
// Copyright (C) 2006  Nick Gasson
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#ifndef INC_HIGHSCORES_HPP
#define INC_HIGHSCORES_HPP

#include "ScreenManager.hpp"
#include "OpenGL.hpp"
#include "Emitter.hpp"
#include "Image.hpp"
#include "Font.hpp"
#include "SoundEffect.hpp"

class ScoreFile {
public:
   ScoreFile();
   ~ScoreFile();

   void Load();
   void Save();
   void Insert(const char* name, int score);

   // An entry in the highscores chart
   class ScoreEntry {
   public:
      ScoreEntry(const char* name, int score);

      static const int MAX_NAME = 16;

      const char* GetName() const { return name; }
      int GetScore() const { return score; }
      
      void WriteOnStream(ostream& os);
      void ReadFromStream(istream& is);

   private:
      char name[MAX_NAME];
      int score;    
   };
   
   static const int NUM_SCORES = 10;
   
   const ScoreEntry& operator[](int n) const { return scores[n]; }
      
private:
   void Sort();
   void SwapScores(int a, int b);
   
   static string GetHighScoreFile();
   
   bool needsWrite;
   typedef vector<ScoreEntry> ScoreEntryVec;
   typedef ScoreEntryVec::iterator ScoreEntryVecIt; 
   ScoreEntryVec scores;;
};

class HighScores : public Screen {
public:
   HighScores();
   virtual ~HighScores() { }
	
   void Load();
   void Process();
   void Display();
   void LoadHighScores();
   void WriteHighScores();
   void DisplayScores();
   void CheckScore(int score);
   
private:	
   enum HighScoreState { hssDisplay, hssEnterName };
	
   int newscore;
   Image hscoreImage;
   float flAlpha, fade;
   HighScoreState state;
   ScoreFile scoreFile;
   Font largeFont, scoreNameFont;
   SoundEffect fwBang;

   static const float FADE_IN_SPEED, FADE_OUT_SPEED;
   
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
		
      Emitter* em;
      int x, y, speed, life, timeout;
      bool active;
   } fw[MAX_FIREWORKS];
};


#endif
