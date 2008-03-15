/*
 * Lander.hpp - Definition of core game classes.
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
#include "Screens.hpp"
#include "DataFile.hpp"
#include "Input.hpp"
#include "Strings.hpp"

#include "Menu.hpp"
#include "HighScores.hpp"

#include "Viewport.hpp"
#include "ObjectGrid.hpp"
#include "Asteroid.hpp"
#include "Ship.hpp"

// Different fonts to be loaded
enum FontType { ftNormal, ftBig, ftScore, ftHollow, ftScoreName, ftLarge };

/* Constants */
enum DIRECTIONS { UP, RIGHT, DOWN, LEFT, NODIR };

/* Macros */
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)


/*
 * A landing pad where the player tries to land.
 */
class LandingPad
{
public:
   LandingPad() {}
   ~LandingPad() {}
	
   static void Load();
	
   void Reset(int index, int length); 
   void Draw(int viewadjust_x, int viewadjust_y, int levelheight, bool locked);
   void SetYPos(int ypos) { this->ypos = ypos; }
	
   int GetLength() const { return length; }
   int GetIndex() const { return index; }

private:
   TextureQuad quad;
   int index, length, ypos;
    
   static Texture s_landtex, s_nolandtex;
};


class Game : public Screen
{
public:
   Game();
   virtual ~Game();
    
   void Load();
   void Process();
   void Display();
   void NewGame();
   void StartLevel(int nLevel);

private:
   
   
   void BounceShip();
   void ExplodeShip();

   Viewport viewport;
   Ship ship;
   int death_timeout, level, fuel, maxfuel, lives;
   bool hasloaded, bThrusting, bDebugMode;
   float flGravity, starrotate, fade_alpha, life_alpha;
   Explosion explosion;
   SmokeTrail exhaust;
   Poly *surface;
   TextureQuad fade, levcomp, speedmeter, fuelmeter, smallship, gameover, paused;
   ColourQuad speedbar;
   int score, newscore, nextnewlife;
   int countdown_timeout, leveltext_timeout, levelcomp_timeout;

   enum GameState { gsNone, gsInGame, gsExplode, gsGameOver, gsDeathWait, 
                    gsFadeIn, gsFadeToDeath, gsFadeToRestart, gsLevelComplete, gsPaused };
   GameState state;

   ObjectGrid objgrid;
	
   // Number of available _surface textures
   static const int NUM_SURF_TEX = 5;

   // Textures
   GLuint uStarTexture, uSurfaceTexture[NUM_SURF_TEX], uSurf2Texture[NUM_SURF_TEX], uFadeTexture;
   GLuint uLevComTexture, uSpeedTexture, uBlueKey[18], uRedKey[18], uGreenKey[18], uPinkKey[18], uYellowKey[18];
   GLuint uBlueArrow, uPinkArrow, uRedArrow, uYellowArrow, uGreenArrow;
   GLuint uGatewayTexture, uFuelMeterTexture, uFuelBarTexture, uMineTexture[36], uShipSmallTexture;
   GLuint uGameOver, uPausedTexture;
    
    
   // Stars
   static const int MAX_GAME_STARS = 2048;
   struct Star 
   {
      TextureQuad quad;
      float scale;
      int xpos, ypos;
   } stars[MAX_GAME_STARS];
   int nStarCount;

   // Landing pads
   static const int MAX_PADS = 3;
   LandingPad pads[MAX_PADS];
   int nLandingPads;
    
   // Keys
   static const int MAX_KEYS = 5;
   struct Key 
   {
      int xpos, ypos;
      TextureQuad frame[18], arrow;
      int current, rotcount;
      float alpha;
      bool active;
   } keys[MAX_KEYS];
   int nKeysRemaining, nKeys;

   // Asteroids
   static const int MAX_ASTEROIDS = 50;
   Asteroid asteroids[MAX_ASTEROIDS];
   int asteroidcount;

   // Electric gate things
   static const int MAX_GATEWAYS = 4;
   static const int MAX_GATEWAY_LENGTH = 10;
   struct Gateway 
   {
      int xpos, ypos, length, timer;
      bool vertical;
      TextureQuad icon;
   } gateways[MAX_GATEWAYS];
   int gatewaycount;

   // Space mines
   static const int MAX_MINES = 5;
   struct Mine 
   {
      int xpos, ypos, current, rotcount, dir, displace_x, displace_y, movedelay;
      int movetimeout;
      TextureQuad frame[36];
   } mines[MAX_MINES];
   int minecount;
};

#endif
