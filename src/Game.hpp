//
// Game.hpp -- Definition of core game classes.
// Copyright (C) 2006-2019  Nick Gasson
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

#pragma once

#include "Platform.hpp"
#include "OpenGL.hpp"
#include "Emitter.hpp"
#include "ScreenManager.hpp"
#include "Font.hpp"
#include "SoundEffect.hpp"
#include "Fade.hpp"

#include "Viewport.hpp"
#include "ObjectGrid.hpp"
#include "Asteroid.hpp"
#include "Ship.hpp"
#include "LandingPad.hpp"
#include "Surface.hpp"
#include "Mine.hpp"
#include "Missile.hpp"
#include "ElectricGate.hpp"
#include "Key.hpp"

// Different fonts to be loaded
enum FontType { ftNormal, ftBig, ftScore, ftScoreName, ftLarge };

class FuelMeter {
public:
   FuelMeter();

   void Display();

   void Refuel(int howmuch);
   bool OutOfFuel() const;
   void BurnFuel();

   int GetFuel() const;

private:
   Image fuelMeterImage;
   Texture fuelBarTexture;
   VertexBuffer m_vbo;

   static const int FUELBAR_Y, FUELBAR_OFFSET;

   int maxfuel;
   float m_fuel = 0.0f;
};

class SpeedMeter {
public:
   SpeedMeter(Ship* ship);

   bool SafeLandingSpeed() const;
   void Display();

private:
   Image speedMeterImage;

   static const float LAND_SPEED;

   VertexBuffer m_vbo;
   Texture m_texture;
   Ship* ship;
};

class Game : public Screen {
public:
   Game();
   virtual ~Game();

   void Load();
   void Process();
   void Display();
   void NewGame();
   void StartLevel();

private:
   static const float TURN_ANGLE, DEATH_SPIN_RATE;
   static const int FUEL_BASE, FUEL_PER_LEVEL;
   static const int SCORE_PAD_SIZE, SCORE_LEVEL, SCORE_FUEL_DIV;
   static const int SCORE_Y, DEATH_TIMEOUT, LEVEL_TEXT_TIMEOUT;
   static const int MAX_SURFACE_HEIGHT, MAX_PAD_SIZE, FUEL_OFFSET;
   static const float SHIP_SPEED, GRAVITY, GAME_FADE_IN_SPEED,
      GAME_FADE_OUT_SPEED, LIFE_ALPHA_BASE;

   void MakeLandingPads();
   void MakeKeys();
   void MakeAsteroids(int surftex);
   void MakeMissiles();
   void MakeGateways();
   void MakeMines();

   void ExplodeShip();
   void EnterDeathWait(int timeout = DEATH_TIMEOUT);
   void CalculateScore(int padIndex);

   static void MakeMultipleOf(int& n, int x, int y);

   Viewport viewport;
   Ship ship;
   Surface surface;
   ObjectGrid objgrid;
   FuelMeter fuelmeter;
   SpeedMeter speedmeter;
   int death_timeout, level, lives;
   bool bDebugMode;
   float flGravity, starrotate, life_alpha;
   int score, newscore, nextnewlife, newscore_width;
   int countdown_timeout, leveltext_timeout, levelcomp_timeout;

   enum GameState { gsNone, gsInGame, gsExplode, gsGameOver, gsDeathWait,
                    gsFadeIn, gsFadeToDeath, gsFadeToRestart, gsLevelComplete,
                    gsPaused };
   GameState state;

   Image levelComp, smallShip;
   Image starImage, gameOver;

   Fade fade;

   Font normalFont, scoreFont, bigFont;

   SoundEffect impactSound, collectSound;

   // Stars
   static const int MAX_GAME_STARS = 2048;
   struct Star {
      double scale;
      int xpos, ypos;
   } stars[MAX_GAME_STARS];
   int nStarCount;

   // Landing pads
   static const int MAX_PADS = 3;
   LandingPadList pads;

   // Keys
   static const int MAX_KEYS = 5;
   typedef vector<Key> KeyList;
   typedef KeyList::iterator KeyListIt;
   KeyList keys;
   int nKeysRemaining, nKeys;

   // Asteroids
   static const int MAX_ASTEROIDS = 50;
   typedef vector<Asteroid> AsteroidList;
   typedef AsteroidList::iterator AsteroidListIt;
   AsteroidList asteroids_;

   // Electric gate things
   static const int MAX_GATEWAYS = 4;
   static const int MAX_GATEWAY_LENGTH = 10;
   typedef vector<ElectricGate> ElectricGateList;
   typedef ElectricGateList::iterator ElectricGateListIt;
   ElectricGateList gateways;

   // Space mines
   static const int MAX_MINES = 5;
   typedef vector<Mine> MineList;
   typedef MineList::iterator MineListIt;
   MineList mines;

   // Missiles
   static const int MAX_MISSILES;
   typedef vector<Missile> MissileList;
   typedef MissileList::iterator MissileListIt;
   MissileList missiles;
};
