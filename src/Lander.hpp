/*
 * Lander.hpp -- Definition of core game classes.
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

#ifndef INC_LANDER_HPP
#define INC_LANDER_HPP

#include "Platform.hpp"
#include "OpenGL.hpp" 
#include "Emitter.hpp"
#include "ScreenManager.hpp"
#include "Input.hpp"

#include "Menu.hpp"
#include "HighScores.hpp"

#include "Viewport.hpp"
#include "ObjectGrid.hpp"
#include "Asteroid.hpp"
#include "Ship.hpp"
#include "LandingPad.hpp"
#include "Surface.hpp"
#include "Mine.hpp"
#include "ElectricGate.hpp"
#include "Key.hpp"

// Different fonts to be loaded
enum FontType { ftNormal, ftBig, ftScore, ftHollow, ftScoreName, ftLarge };

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

class FuelMeter {
public:
   FuelMeter();
   
   void Display();

   void Refuel(int howmuch);
   bool OutOfFuel() const;
   void BurnFuel();

   int GetFuel() const { return fuel; }
   
private:
   static Image *fuelMeterImage;
   static Texture *fuelBarTexture;

   static const int FUELBAR_Y;
   
   int fuel, maxfuel;
   TextureQuad border;
};

class SpeedMeter {
public:
   SpeedMeter(Ship *ship);

   bool SafeLandingSpeed() const;
   void Display();

private:
   static Image *speedMeterImage;

   static const float LAND_SPEED;
   
   ColourQuad speedbar;
   Ship *ship;
};

class Game : public Screen {
public:
   Game();
   virtual ~Game();
    
   void Load();
   void Process();
   void Display();
   void NewGame();
   void StartLevel(int nLevel);

private:
   static const float TURN_ANGLE, DEATH_SPIN_RATE;
   static const int FUEL_BASE, FUEL_PER_LEVEL;
   static const int SCORE_PAD_SIZE, SCORE_LEVEL, SCORE_FUEL_DIV;
   static const int SCORE_Y;
   
   void ExplodeShip();
   void CalculateScore(int padIndex);

   Viewport viewport;
   Ship ship;
   Surface surface;
   ObjectGrid objgrid;
   FuelMeter fuelmeter;
   SpeedMeter speedmeter;
   int death_timeout, level, lives;
   bool bDebugMode;
   float flGravity, starrotate, fade_alpha, life_alpha;
   TextureQuad fade;
   int score, newscore, nextnewlife;
   int countdown_timeout, leveltext_timeout, levelcomp_timeout;

   enum GameState { gsNone, gsInGame, gsExplode, gsGameOver, gsDeathWait, 
                    gsFadeIn, gsFadeToDeath, gsFadeToRestart, gsLevelComplete,
                    gsPaused };
   GameState state;

   Texture fadeTexture;

   Image levelComp, smallShip;
   Image starImage;
    
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
   Asteroid asteroids[MAX_ASTEROIDS];
   int asteroidcount;

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
};

#endif
