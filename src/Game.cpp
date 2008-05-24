/*
 * Game.cpp -- Implementation of core game logic.
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

#include "Lander.hpp"
#include "LoadOnce.hpp"

/*
 * Constants affecting level generation.
 */
#define MAX_SURFACE_HEIGHT  300
#define SHIP_SPEED		      0.15f
#define MAX_PAD_SIZE		    2 
#define FUELBAR_OFFSET		  68
#define GRAVITY             0.035f

/*
 * Constants affecting state transitions.
 */
#define GAME_FADE_IN_SPEED	0.1f	// Rate of alpha change at level start
#define GAME_FADE_OUT_SPEED	0.1f	// Rate of alpha change at level end
#define DEATH_TIMEOUT		    50		// Frames to wait for ending level
#define LIFE_ALPHA_BASE		  2.0f
#define LEVEL_TEXT_TIMEOUT	75

const float Game::TURN_ANGLE(3.0f);
const float Game::DEATH_SPIN_RATE(5.0f);
const int Game::FUEL_BASE(600);
const int Game::FUEL_PER_LEVEL(50);

const int Game::SCORE_PAD_SIZE(10);
const int Game::SCORE_LEVEL(100);
const int Game::SCORE_FUEL_DIV(10);

const int Game::SCORE_Y(30);

const float SpeedMeter::LAND_SPEED(2.0f);

const int FuelMeter::FUELBAR_Y(15);

extern DataFile *g_pData;


Game::Game()
   : ship(&viewport),
     surface(&viewport),
     speedmeter(&ship),
     state(gsNone),
     starImage("images/star.png")
{

}

void Game::Load()
{
   OpenGL &opengl = OpenGL::GetInstance();

   LOAD_ONCE {
      uFadeTexture = opengl.LoadTexture(g_pData, "Fade.bmp");
      uLevComTexture = opengl.LoadTextureAlpha(g_pData, "LevelComplete.bmp");
      uSurf2Texture[0] = opengl.LoadTexture(g_pData, "GrassSurface2.bmp");
      uSurf2Texture[1] = opengl.LoadTexture(g_pData, "DirtSurface2.bmp");
      uSurf2Texture[2] = opengl.LoadTexture(g_pData, "SnowSurface2.bmp");
      uSurf2Texture[3] = opengl.LoadTexture(g_pData, "RedRockSurface2.bmp");
      uSurf2Texture[4] = opengl.LoadTexture(g_pData, "RockSurface2.bmp");
      uShipSmallTexture = opengl.LoadTextureAlpha(g_pData, "ShipSmall.bmp");
   }

   // Create the fade
   fade.x = 0;
   fade.y = 0;
   fade.width = opengl.GetWidth();
   fade.height = opengl.GetHeight();
   fade.uTexture = uFadeTexture;

   // Create the level complete sign
   levcomp.x = (opengl.GetWidth() - 512) / 2;
   levcomp.y = (opengl.GetHeight() - 32) / 2 - 50;
   levcomp.width = 512;
   levcomp.height = 32;
   levcomp.uTexture = uLevComTexture;

   // Create the small ship icon
   smallship.width = 32;
   smallship.height = 32;
   smallship.uTexture = uShipSmallTexture;

   starrotate = 0.0f;
   death_timeout = 0;
   state = gsNone;

   bDebugMode = false;
}

Game::~Game()
{
   
}

void Game::NewGame()
{
   // Reset score, lives, etc.
   score = 0;
   lives = 3;

   // Start the game
   level = 1;
   nextnewlife = 1000;
   StartLevel(level);
}

void Game::CalculateScore(int padIndex)
{
   newscore =
      (level * SCORE_LEVEL)
      + ((MAX_PAD_SIZE + 2 - pads[padIndex].GetLength()) * SCORE_PAD_SIZE)
      + (fuelmeter.GetFuel() / SCORE_FUEL_DIV);
}

void Game::Process()
{
   Input &input = Input::GetInstance();
   OpenGL &opengl = OpenGL::GetInstance();
   
   // Check keys
   if (input.GetKeyState(SDLK_p)) {
      if (state == gsPaused) {
         // Unpause the game
         state = gsInGame;
         input.ResetKey(SDLK_p);
      }
      else if (state == gsInGame) {
         // Pause the game
         state = gsPaused;
         input.ResetKey(SDLK_p);
         ship.ThrustOff();
      }
   }

   // Check for paused state
   if (state == gsPaused)
      return;

   if ((input.GetKeyState(SDLK_UP) || input.QueryJoystickButton(1))
       && !fuelmeter.OutOfFuel() && state == gsInGame) {
      // Thrusting
      ship.ThrustOn();
      ship.Thrust(SHIP_SPEED);
      fuelmeter.BurnFuel();
   }
   else
      ship.ThrustOff();
   
   if ((input.GetKeyState(SDLK_RIGHT) || input.QueryJoystickAxis(0) > 0)
       && state == gsInGame) {
      // Turn clockwise
      ship.Turn(TURN_ANGLE);
   }
   else if ((input.GetKeyState(SDLK_LEFT) || input.QueryJoystickAxis(0) < 0)
            && state == gsInGame) {
      // Turn anti-clockwise
      ship.Turn(-TURN_ANGLE);
   }
   
   if (input.GetKeyState(SDLK_SPACE) && state == gsExplode) {
      // Skip explosion
      state = gsDeathWait; 
      if (lives == 0)
         death_timeout = DEATH_TIMEOUT;
      else	
         death_timeout = 1;
   }

   if (input.GetKeyState(SDLK_ESCAPE) && state == gsInGame) {
      // Quit to main menu
      ExplodeShip();
      lives = 0;
   }

   if (input.GetKeyState(SDLK_d)) {
      // Toggle mode
      bDebugMode = !bDebugMode;
      input.ResetKey(SDLK_d);
   }

   // Move only if not in game over
   if (state == gsInGame || state == gsExplode) {
      ship.ApplyGravity(flGravity);

      // Move the ship (and exhaust and explosion)
      ship.Move();
   }

   // Move mines
   for (MineListIt it = mines.begin(); it != mines.end(); ++it)
      (*it).Move();
   
   // Calculate view adjusts
   ship.CentreInViewport();

   // Check for collisions with surface
   int padIndex;
   if (surface.CheckCollisions(ship, pads, &padIndex)) {
      bool landed = false;
      if (state == gsInGame) {
         if (padIndex != -1) {
            // Hit a landing pad
            int dAngle = ((int)ship.GetAngle()) % 360;
            if ((dAngle >= 330 || dAngle <= 30)
                && speedmeter.SafeLandingSpeed() && nKeysRemaining == 0) {
               // Landed safely
               state = gsLevelComplete;
               CalculateScore(padIndex);
               countdown_timeout = 70;
               landed = true;
            }
         }
         if (!landed) {
            // Crash landed
            ExplodeShip();
            ship.Bounce();
         }
      }
      else if (state == gsExplode) {
         ship.Bounce();
         
         // See if we need to stop the madness
         if (state == gsExplode && -ship.GetYSpeed() < 0.05f) {
            state = gsDeathWait; 
            death_timeout = DEATH_TIMEOUT;
         }
      }
   }
   
   // Check for collisions with asteroids
   LineSegment l1, l2;
   for (int i = 0; i < asteroidcount; i++) {
      if (asteroids[i].ObjectInScreen(&viewport)) {
         if (asteroids[i].CheckCollision(ship)) {
            // Crashed
            if (state == gsInGame) {
               // Destroy the ship
               ExplodeShip();
               ship.Bounce();
            }
            else if (state == gsExplode) {
               ship.Bounce();
               
               // See if we need to stop the madness
               if (state == gsExplode && -ship.GetYSpeed() < 0.05f) {
                  state = gsDeathWait; 
                  death_timeout = DEATH_TIMEOUT;
               }
            }
         }
      }
   }

   // Check for collision with gateways
   for (ElectricGateListIt it = gateways.begin(); it != gateways.end(); ++it) {
      if ((*it).CheckCollision(ship)) {
         if (state == gsInGame) {
            // Destroy the ship
            ExplodeShip();
            ship.Bounce();
         }
         else if (state == gsExplode) {
            // Destroy the ship anyway
            state = gsDeathWait; 
            death_timeout = DEATH_TIMEOUT;
         }
      }
   }

   // Check for collisions with mines
   for (MineListIt it = mines.begin(); it != mines.end(); ++it) {
      if ((*it).CheckCollision(ship)) {
         if (state == gsInGame) {
            // Destroy the ship
            ExplodeShip();
            ship.Bounce();
         }
         else if (state == gsExplode) {
            ship.Bounce();
            
            // See if we need to stop the madness
            if (state == gsExplode && -ship.GetYSpeed() < 0.05f) {
               state = gsDeathWait; 
               death_timeout = DEATH_TIMEOUT;
            }
         }
      }
   }

   // See if the player collected a key
   for (KeyListIt it = keys.begin(); it != keys.end(); ++it) {
      if((*it).CheckCollision(ship)) {
         nKeysRemaining--;
         (*it).Collected();
         objgrid.UnlockSpace((*it).GetX(), (*it).GetY());
      }
   }

   // Entry / exit states
   if (state == gsDeathWait) {
      if (--death_timeout == 0) {
         // Fade out
         if (lives == 0  || (lives == 1 && life_alpha < LIFE_ALPHA_BASE)) {
            state = gsFadeToDeath;
            fade_alpha = LIFE_ALPHA_BASE + 1.0f;
         }
         else if (lives > 0) {
            if (life_alpha < LIFE_ALPHA_BASE) {
               life_alpha = LIFE_ALPHA_BASE + 1.0f;
               lives--;
            }

            state = gsFadeToRestart;
            fade_alpha = 0.0f;
         }
      }
   }
   else if (state == gsGameOver) {
      if (--death_timeout == 0) {
         // Fade out
         state = gsFadeToDeath;
         fade_alpha = 0.0f;
      }
   }
   else if (state == gsFadeIn) {
      // Fade in
      fade_alpha -= GAME_FADE_IN_SPEED;
      if (fade_alpha < 0.0f)
         state = gsInGame;
   }
   else if (state == gsFadeToRestart) {
      // Fade out
      fade_alpha += GAME_FADE_OUT_SPEED;
      if (fade_alpha > 1.0f) {
         // Restart the level
         StartLevel(level);
         opengl.SkipDisplay();
      }
   }
   else if (state == gsFadeToDeath) {
      fade_alpha += GAME_FADE_OUT_SPEED;
      if (fade_alpha > 1.0f) {
         // Return to main menu
         ScreenManager &sm = ScreenManager::GetInstance();
         HighScores *hs = static_cast<HighScores*>(sm.GetScreenById("HIGH SCORES"));
         hs->CheckScore(score);
      }
   }
   else if (state == gsLevelComplete) {
      // Decrease the displayed score
      if (countdown_timeout > 0)
         countdown_timeout--;
      else if (levelcomp_timeout > 0) {
         if (--levelcomp_timeout == 0) {
            level++;
            state = gsFadeToRestart;
         }
      }
      else {
         int dec = level * 2;
         
         // Decrease the score
         newscore -= dec;
         score += dec;
         
         if (score > nextnewlife) {
            lives++;
            nextnewlife *= 2;
         }
         
         if (newscore < 0) {
            // Move to the next level (after a 1s pause)
            score -= -newscore;
            levelcomp_timeout = 40;
         }
      }
   }
   
   // Decrease level text timeout
   if (leveltext_timeout > 0)
      leveltext_timeout--;

   // Spin the ship if we're exploding
   if (state == gsExplode)
      ship.Turn(DEATH_SPIN_RATE);
}

void Game::StartLevel(int level)
{
   // Set level size
   viewport.SetLevelWidth(2000 + 2*Surface::SURFACE_SIZE*level);
   viewport.SetLevelHeight(1500 + 2*Surface::SURFACE_SIZE*level);
   flGravity = GRAVITY;

   // Create the object grid
   int grid_w = viewport.GetLevelWidth() / ObjectGrid::OBJ_GRID_SIZE;
   int grid_h = (viewport.GetLevelHeight() - ObjectGrid::OBJ_GRID_TOP
                 - MAX_SURFACE_HEIGHT - 100) / ObjectGrid::OBJ_GRID_SIZE;
   objgrid.Reset(grid_w, grid_h);

   // Create background stars
   nStarCount = (viewport.GetLevelWidth() * viewport.GetLevelHeight()) / 10000;
   if (nStarCount > MAX_GAME_STARS)
      nStarCount = MAX_GAME_STARS;
   for (int i = 0; i < nStarCount; i++) {
      stars[i].xpos = (int)(rand()%(viewport.GetLevelWidth()/20))*20;
      stars[i].ypos = (int)(rand()%(viewport.GetLevelHeight()/20))*20;
      stars[i].scale = (double)rand()/(double)RAND_MAX/8.0;
   }
   
   // Generate landing pads
   pads.clear();
   int nLandingPads = rand()%MAX_PADS + 1;
   for (int i = 0; i < nLandingPads; i++) {
      int index, length;
      bool overlap;
      do {
         index = rand() % (viewport.GetLevelWidth() / Surface::SURFACE_SIZE);
         length = rand() % MAX_PAD_SIZE + 3;
         
         // Check for overlap
         overlap = false;
         if (index + length > (viewport.GetLevelWidth() / Surface::SURFACE_SIZE))
            overlap = true;
         for (int j = 0; j < i; j++) {
            if (pads[j].GetIndex() == index) 
               overlap = true;
            else if (pads[j].GetIndex() < index
                     && pads[j].GetIndex() + pads[j].GetLength() >= index)
               overlap = true;
            else if (index < pads[j].GetIndex()
                     && index + length >= pads[j].GetIndex())
               overlap = true;
         }
      } while (overlap);
      
      pads.push_back(LandingPad(&viewport, index, length));
   }   

   int surftex = rand() % Surface::NUM_SURF_TEX;
   surface.Generate(surftex, pads);
   
   // Create the keys
   nKeys = (level / 2) + (level % 2);
   if (nKeys > MAX_KEYS)
      nKeys = MAX_KEYS;
   nKeysRemaining = nKeys;
   const ArrowColour acols[MAX_KEYS] =
      { acBlue, acRed, acYellow, acPink, acGreen };
   keys.clear();
   for (int i = 0; i < MAX_KEYS; i++) {
      int xpos, ypos;
      objgrid.AllocFreeSpace(xpos, ypos, 1, 1);
      keys.push_back(Key(i < nKeysRemaining, xpos, ypos, acols[i]));
   }

   // Create the asteroids
   asteroidcount = level*2 + rand()%(level+3);
   if (asteroidcount > MAX_ASTEROIDS)
      asteroidcount = MAX_ASTEROIDS;
   for (int i = 0; i < asteroidcount; i++) {
      // Allocate space, check for timeout
      int x, y, width = rand() % (Asteroid::MAX_ASTEROID_WIDTH - 4) + 4;
      if (!objgrid.AllocFreeSpace(x, y, width, 4)) {
         // Failed to allocate space so don't make any more asteroids
         break;
      }
      
      // Generate the asteroid
      asteroids[i].ConstructAsteroid(x, y, width, uSurf2Texture[surftex]);			
   }
   
   // Create gateways
   int gatewaycount = level/2 + rand()%level - 2;
   gateways.clear();
   if (gatewaycount > MAX_GATEWAYS)
      gatewaycount = MAX_GATEWAYS;
   for (int i = 0; i < gatewaycount; i++) {
      // Allocate space for gateway
      int length = rand()%(MAX_GATEWAY_LENGTH-3) + 3;
      bool vertical = rand() % 2 == 0;
		
      bool result;
      int xpos, ypos;
      if (vertical)
         result = objgrid.AllocFreeSpace(xpos, ypos, 1, length+1);
      else
         result = objgrid.AllocFreeSpace(xpos, ypos, length+1, 1);
      if (!result) {
         // Failed to allocate space so don't make any more gateways
         break;
      }

      gateways.push_back(ElectricGate(&viewport, length, vertical, xpos, ypos));
   }

   // Create mines (MUST BE CREATED LAST)
   //int minecount = level/2 + rand()%level;
   int minecount = 6;
   mines.clear();
   if (minecount > MAX_MINES)
      minecount = MAX_MINES;
   for (int i = 0; i < minecount; i++) {
      // Allocate space for mine
      int xpos, ypos;
      if (!objgrid.AllocFreeSpace(xpos, ypos, 2, 2)) {
         // Failed to allocate space
         minecount = i + 1;
         break;
      }
      mines.push_back(Mine(&objgrid, &viewport, xpos, ypos));
   }

   // Set ship starting position
   ship.Reset();

   leveltext_timeout = LEVEL_TEXT_TIMEOUT;

   fuelmeter.Refuel(FUEL_BASE + FUEL_PER_LEVEL*level);

   // Start the game
   levelcomp_timeout = 0;
   state = gsFadeIn;
   fade_alpha = 1.0f;
   life_alpha = LIFE_ALPHA_BASE + 1.0f;
}

/* 
 * Destroys the ship after a collision.
 */
void Game::ExplodeShip()
{
   // Set the game state
   state = gsExplode;

   // Decrement lives
   life_alpha = LIFE_ALPHA_BASE - 1.0f;
}

void Game::Display()
{
   FreeType &ft = FreeType::GetInstance();
   OpenGL &opengl = OpenGL::GetInstance();
   
   // Draw the stars
   for (int i = 0; i < nStarCount; i++) {
      int x = stars[i].xpos - viewport.GetXAdjust();
      int y = stars[i].ypos - viewport.GetYAdjust();
      starImage.Draw(x, y, starrotate, stars[i].scale);
      starrotate += 0.005f;
   }

   surface.Display();

   // Draw the asteroids
   for (int i = 0; i < asteroidcount; i++) {
      if (asteroids[i].ObjectInScreen(&viewport))
         asteroids[i].Draw(viewport.GetXAdjust(), viewport.GetYAdjust());			
   }

   // Draw the keys
   for (KeyListIt it = keys.begin(); it != keys.end(); ++it)
      (*it).DrawKey(&viewport);   
      
   // Draw gateways
   for (ElectricGateListIt it = gateways.begin(); it != gateways.end(); ++it)
      (*it).Draw();
   
   // Draw mines
   for (MineListIt it = mines.begin(); it != mines.end(); ++it)
      (*it).Draw();

   if (bDebugMode) {
      // Draw red squares around no-go areas
      int x, y;
      opengl.DisableTexture();
      opengl.EnableBlending();
      opengl.Colour(1.0f, 0.0f, 0.0f, 0.4f);
      for (x = 0; x < objgrid.GetWidth(); x++) {
         for (y = 0; y < objgrid.GetHeight(); y++) {
            if (objgrid.IsFilled(x, y)) {
               glLoadIdentity();
               glBegin(GL_QUADS);
               glVertex2i(x*ObjectGrid::OBJ_GRID_SIZE - viewport.GetXAdjust(),
                          y*ObjectGrid::OBJ_GRID_SIZE - viewport.GetYAdjust()
                          + ObjectGrid::OBJ_GRID_TOP);
               glVertex2i((x+1)*ObjectGrid::OBJ_GRID_SIZE - viewport.GetXAdjust(),
                          y*ObjectGrid::OBJ_GRID_SIZE - viewport.GetYAdjust()
                          + ObjectGrid::OBJ_GRID_TOP);
               glVertex2i((x+1)*ObjectGrid::OBJ_GRID_SIZE - viewport.GetXAdjust(),
                          (y+1)*ObjectGrid::OBJ_GRID_SIZE - viewport.GetYAdjust()
                          + ObjectGrid::OBJ_GRID_TOP);
               glVertex2i(x*ObjectGrid::OBJ_GRID_SIZE - viewport.GetXAdjust(),
                          (y+1)*ObjectGrid::OBJ_GRID_SIZE - viewport.GetYAdjust()
                          + ObjectGrid::OBJ_GRID_TOP);
               glEnd();
            }
         }
      }
   }

   // Draw the landing pads
   for (LandingPadListIt it = pads.begin(); it != pads.end(); ++it)
      (*it).Draw(nKeysRemaining > 0);

   // Draw the exhaust
   ship.DrawExhaust(state == gsPaused);
   
   if (state != gsDeathWait && state != gsGameOver
       && state != gsFadeToDeath && state != gsFadeToRestart) {
      ship.Display();
   }
   
   // Draw the explosion if necessary
   if (state == gsExplode) {
      ship.DrawExplosion(true);
      opengl.Colour(0.0f, 1.0f, 0.0f);
      const char *sdeath = i18n("Press  SPACE  to  continue");
      ft.Print
         (ftNormal,
          (opengl.GetWidth() - ft.GetStringWidth(ftNormal, sdeath)) / 2,
          opengl.GetHeight() - 40,
          sdeath); 
   }
   else if (state == gsDeathWait || state == gsGameOver 
            || state == gsFadeToDeath || state == gsFadeToRestart) {
      ship.DrawExplosion(false);
   }
   
   // Draw the arrows
   for (KeyListIt it = keys.begin(); it != keys.end(); ++it)
      (*it).DrawArrow(&viewport);

   // Draw HUD
   opengl.Colour(0.0f, 0.9f, 0.0f);
   ft.Print(ftScore, 10, SCORE_Y, "%.7d", score);

   fuelmeter.Display();
   speedmeter.Display();

   // Draw life icons
   for (int i = 0; i < lives; i++) {
      smallship.x = 5 + i*30;
      smallship.y = 60;
      if (i == lives-1) {
         if (life_alpha > LIFE_ALPHA_BASE)
            opengl.Draw(&smallship);
         else if (life_alpha < 0.0f) {
            // Decrement lives
            lives--;
            life_alpha = LIFE_ALPHA_BASE + 1.0f;
         }
         else {
            opengl.DrawBlend(&smallship, life_alpha);
            life_alpha -= 0.03f;
         }
      }
      else
         opengl.Draw(&smallship);
   }
   
   // Draw key icons
   int offset = (opengl.GetWidth() - MAX_KEYS*32)/2; 
   if (nKeysRemaining > 0) {
      int i = 0;
      for (KeyListIt it = keys.begin(); it != keys.end(); ++it) {
         (*it).DrawIcon(offset + i, 0.3f);
         i += 32;
      }
   }
   else {
      int i = 0;
      for (KeyListIt it = keys.begin(); it != keys.end(); ++it) {
         (*it).DrawIcon(offset + i*32, 0.0f);
         i += 32;
      }
      opengl.Colour(0.0f, 1.0f, 0.0f);
      const char *sland = i18n("Land  now");
      ft.Print
         (ftNormal,
          (opengl.GetWidth() - ft.GetStringWidth(ftNormal, sland))/2,
          30, sland);
   }

   // Draw level complete messages
   const char *scoretxt = i18n("Score:  %d");
   if (state == gsLevelComplete) {
      opengl.Draw(&levcomp);
      opengl.Colour(0.0f, 0.5f, 0.9f);
      ft.Print
         (ftBig,
          (opengl.GetWidth() - ft.GetStringWidth(ftBig, scoretxt) - 40)/2,
          (opengl.GetHeight() - 30)/2 + 50,
          scoretxt,
          newscore > 0 ? newscore : 0);
   }
   
   // Draw level number text
   if (leveltext_timeout) {
      opengl.Colour(0.9f, 0.9f, 0.0f);
      const char *lvltxt = i18n("Level  %d");
      ft.Print
         (ftBig,
          (opengl.GetWidth() - ft.GetStringWidth(ftBig, lvltxt) - 20)/2,
          (opengl.GetHeight() - 30)/2,
          lvltxt, level);
   }

   // Draw the fade
   if (state == gsFadeIn || state == gsFadeToDeath || state == gsFadeToRestart)
      opengl.DrawBlend(&fade, fade_alpha);

   // Draw game over message
   if (lives == 0 || (lives == 1 && life_alpha < LIFE_ALPHA_BASE)) {
      opengl.Colour(0.9f, 0.0f, 0.0f);
      const char *sgo = i18n("Game Over");
      ft.Print
         (ftHollow,
          (opengl.GetWidth() - ft.GetStringWidth(ftHollow, sgo) - 20)/2,
          (opengl.GetHeight() - 150)/2,
          sgo);
   }

   // Draw paused message
   if (state == gsPaused) {
      const char *txtpaused = i18n("Paused");
      opengl.Colour(0.0f, 0.5f, 1.0f);
      ft.Print
         (ftBig,
          (opengl.GetWidth() - ft.GetStringWidth(ftBig, txtpaused) - 20)/2,
          (opengl.GetHeight() - 150)/2,
          txtpaused);
   }
}

Texture FuelMeter::uFuelMeterTexture, FuelMeter::uFuelBarTexture;

FuelMeter::FuelMeter()
   : fuel(0), maxfuel(1)
{
   LOAD_ONCE {
      OpenGL &opengl = OpenGL::GetInstance();
   
      uFuelMeterTexture = opengl.LoadTextureAlpha(g_pData, "FuelMeter.bmp");
      uFuelBarTexture = opengl.LoadTextureAlpha(g_pData, "FuelBar.bmp");
   }
}

void FuelMeter::Display()
{
   OpenGL &opengl = OpenGL::GetInstance();
   
   int fbsize = (int)(((float)fuel/(float)maxfuel)*(256-FUELBAR_OFFSET)); 
   float texsize = fbsize/(256.0f-FUELBAR_OFFSET);
   opengl.EnableTexture();
   opengl.DisableBlending();
   opengl.Colour(1.0f, 1.0f, 1.0f);
   opengl.SelectTexture(uFuelBarTexture);
   glLoadIdentity();
   glBegin(GL_QUADS);
     glTexCoord2f(1.0f-texsize, 1.0f);
     glVertex2i(opengl.GetWidth()-fbsize-10, FUELBAR_Y);
     glTexCoord2f(1.0f, 1.0f);
     glVertex2i(opengl.GetWidth()-10, FUELBAR_Y);
     glTexCoord2f(1.0f, 0.0f);
     glVertex2i(opengl.GetWidth()-10, FUELBAR_Y + 32);
     glTexCoord2f(1.0f-texsize, 0.0f);
     glVertex2i(opengl.GetWidth()-fbsize-10, FUELBAR_Y + 32);
   glEnd();

   border.x = opengl.GetWidth() - 266;
   border.y = FUELBAR_Y;
   border.width = 256;
   border.height = 32;
   border.uTexture = uFuelMeterTexture;
   opengl.Draw(&border);
}

void FuelMeter::Refuel(int howmuch)
{
   maxfuel = howmuch;
   fuel = maxfuel;
}

void FuelMeter::BurnFuel()
{
   assert(fuel > 0);
   fuel--;
}

bool FuelMeter::OutOfFuel() const
{
   return fuel <= 0;
}

Texture SpeedMeter::uSpeedTexture;

SpeedMeter::SpeedMeter(Ship *ship)
   : ship(ship)
{
   LOAD_ONCE {
      uSpeedTexture = OpenGL::GetInstance().LoadTextureAlpha(g_pData, "SpeedMeter.bmp");
   }
   
   border.x = 10;
   border.y = 40;
   border.width = 128;
   border.height = 16;

   speedbar.x = 12;
   speedbar.y = 40;
   speedbar.width = 124;
   speedbar.height = 16;
   speedbar.red = 1.0f;
   speedbar.green = 0.0f;
   speedbar.blue = 0.0f;
}

void SpeedMeter::Display()
{
   // Resize the speed bar
   float flSpeed1 = 30.0f / LAND_SPEED;
   int width = (int)((float)ship->GetYSpeed() * flSpeed1); 
   if (width < 0) 
      width = 0;
   if (width > 124) 
      width = 124;
   speedbar.blue = 0.0f;
   speedbar.red = (float)width/124.0f;
   speedbar.green = 1.0f - (float)width/124.0f;
   speedbar.width = width;

   OpenGL &opengl = OpenGL::GetInstance();
   opengl.Draw(&speedbar);
   border.uTexture = uSpeedTexture;
   opengl.Draw(&border);
}

bool SpeedMeter::SafeLandingSpeed()
{
   return ship->GetYSpeed() < LAND_SPEED;
}
