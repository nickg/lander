//
// Copyright (C) 2006-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#pragma once

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
   void Insert(const string& name, int score);

   // An entry in the highscores chart
   class ScoreEntry {
   public:
      ScoreEntry(const char* name, int score);

      static const int MAX_NAME = 16;

      const char* GetName() const { return name; }
      int GetScore() const { return score; }

      void WriteOnStream(ostream& os);
      void ReadFromStream(istream& is);

      bool operator<(const ScoreFile::ScoreEntry& rhs) const;

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
   ScoreEntryVec scores;
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

   const char *GetName() const override { return "HIGH SCORES"; }

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
