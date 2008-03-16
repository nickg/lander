/*
 * Game.cpp - Implementation of core game logic.
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

/*
 * Constants affecting level generation.
 */
#define MAX_SURFACE_HEIGHT  300
#define SHIP_SPEED		      0.15f
#define MAX_PAD_SIZE		    3
#define LAND_SPEED		      2.0f
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

extern DataFile *g_pData;


Game::Game()
   : hasloaded(false),
     state(gsNone),
     ship(&viewport),
     surface(&viewport)
{

}

void Game::Load()
{
   const int TEX_NAME_LEN = 128;
   char buf[TEX_NAME_LEN];
   int i;

   OpenGL &opengl = OpenGL::GetInstance();

   if (!hasloaded) {
      uStarTexture = opengl.LoadTextureAlpha(g_pData, "Star.bmp");
      uFadeTexture = opengl.LoadTexture(g_pData, "Fade.bmp");
      uLevComTexture = opengl.LoadTextureAlpha(g_pData, "LevelComplete.bmp");
      uSurf2Texture[0] = opengl.LoadTexture(g_pData, "GrassSurface2.bmp");
      uSurf2Texture[1] = opengl.LoadTexture(g_pData, "DirtSurface2.bmp");
      uSurf2Texture[2] = opengl.LoadTexture(g_pData, "SnowSurface2.bmp");
      uSurf2Texture[3] = opengl.LoadTexture(g_pData, "RedRockSurface2.bmp");
      uSurf2Texture[4] = opengl.LoadTexture(g_pData, "RockSurface2.bmp");
      uSpeedTexture = opengl.LoadTextureAlpha(g_pData, "SpeedMeter.bmp");
      uFuelMeterTexture = opengl.LoadTextureAlpha(g_pData, "FuelMeter.bmp");
      uFuelBarTexture = opengl.LoadTextureAlpha(g_pData, "FuelBar.bmp");
      uShipSmallTexture = opengl.LoadTextureAlpha(g_pData, "ShipSmall.bmp");
      uGameOver = opengl.LoadTextureAlpha(g_pData, "GameOver.bmp");
      uPausedTexture = opengl.LoadTextureAlpha(g_pData, "Paused.bmp");
      
      Ship::Load();
      LandingPad::Load();
      Surface::Load();
      Mine::Load();
      ElectricGate::Load();
      Key::Load();
      
      hasloaded = true;
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

   // Create the speed meter
   speedmeter.x = 10;
   speedmeter.y = 40;
   speedmeter.width = 128;
   speedmeter.height = 16;
   speedmeter.uTexture = uSpeedTexture;

   // Create the fuel meter
   fuelmeter.x = opengl.GetWidth() - 266;
   fuelmeter.y = 30;
   fuelmeter.width = 256;
   fuelmeter.height = 32;
   fuelmeter.uTexture = uFuelMeterTexture;

   // Create the speed bar
   speedbar.x = 12;
   speedbar.y = 40;
   speedbar.width = 124;
   speedbar.height = 16;
   speedbar.red = 1.0f;
   speedbar.green = 0.0f;
   speedbar.blue = 0.0f;

   // Create the small ship icon
   smallship.width = 32;
   smallship.height = 32;
   smallship.uTexture = uShipSmallTexture;

   // Create the game over thing
   gameover.x = (opengl.GetWidth()-512)/2;
   gameover.y = (opengl.GetHeight()-64)/2;
   gameover.width = 512;
   gameover.height = 64;
   gameover.uTexture = uGameOver;

   // Create the paused text
   paused.x = (opengl.GetWidth()-512)/2;
   paused.y = (opengl.GetHeight()-64)/2;
   paused.width = 512;
   paused.height = 64;
   paused.uTexture = uPausedTexture;

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
       && fuel > 0 && state == gsInGame) {
      // Thrusting
      ship.ThrustOn();
      ship.Thrust(SHIP_SPEED);
      fuel--;
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
                && ship.GetYSpeed() < LAND_SPEED && nKeysRemaining == 0) {
               // Landed safely
               state = gsLevelComplete;
               newscore = (level * 100) + 
                  (((MAX_PAD_SIZE+2) - pads[padIndex].GetLength())*10*level);
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
   for (int i = 0; i < nKeys; i++) {
      if(keys[i].CheckCollision(ship)) {
         nKeysRemaining--;
         keys[i].Collected();
         objgrid.UnlockSpace(keys[i].GetX(), keys[i].GetY());
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
      ship.Turn(5.0f);

   // Resize the speed bar
   float flSpeed1 = 30.0f / LAND_SPEED;
   int width = (int)((float)ship.GetYSpeed() * flSpeed1); 
   if (width < 0) 
      width = 0;
   if (width > 124) 
      width = 124;
   speedbar.blue = 0.0f;
   speedbar.red = (float)width/124.0f;
   speedbar.green = 1.0f - (float)width/124.0f;
   speedbar.width = width;
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
      stars[i].quad.uTexture = uStarTexture;
      stars[i].quad.width = stars[i].quad.height = rand()%15;
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
   for (int i = 0; i < MAX_KEYS; i++) {
      int xpos, ypos;
      objgrid.AllocFreeSpace(xpos, ypos, 1, 1);
      keys[i].Reset(i < nKeysRemaining, xpos, ypos, acols[i]);
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
   int gatewaycount = level/2 + rand()%(level);
   if (gatewaycount > MAX_GATEWAYS)
      gatewaycount = MAX_GATEWAYS;
   for (int i = 0; i < gatewaycount; i++) {
      // Allocate space for gateway
      int length = rand()%(MAX_GATEWAY_LENGTH-3) + 3;
      bool vertical;
      switch(rand() % 2) {
      case 0: vertical = true; break;
      case 1: vertical = false; break;
      }
		
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
   int minecount = level/2 + rand()%level;
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

   // Set fuel
   fuel = maxfuel = 750 + 50*level;

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
      stars[i].quad.x = stars[i].xpos - viewport.GetXAdjust();
      stars[i].quad.y = stars[i].ypos - viewport.GetYAdjust();
      opengl.DrawRotate(&stars[i].quad, starrotate);
      starrotate += 0.005f;
   }

   surface.Display();

   // Draw the asteroids
   for (int i = 0; i < asteroidcount; i++) {
      if (asteroids[i].ObjectInScreen(&viewport))
         asteroids[i].Draw(viewport.GetXAdjust(), viewport.GetYAdjust());			
   }

   // Draw the keys
   for (int i = 0; i < nKeys; i++)
      keys[i].DrawKey(&viewport);
      
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
      ft.Print
         (ftNormal,
          (opengl.GetWidth() - ft.GetStringWidth(ftNormal, S_DEATH)) / 2,
          opengl.GetHeight() - 40,
          S_DEATH); 
   }
   else if (state == gsDeathWait || state == gsGameOver 
            || state == gsFadeToDeath || state == gsFadeToRestart) {
      ship.DrawExplosion(false);
   }
   
   // Draw the arrows
   for (int i = 0; i < nKeys; i++)
      keys[i].DrawArrow(&viewport);

   // Draw fuel bar
   int fbsize = (int)(((float)fuel/(float)maxfuel)*(256-FUELBAR_OFFSET)); 
   float texsize = fbsize/(256.0f-FUELBAR_OFFSET);
   opengl.EnableTexture();
   opengl.DisableBlending();
   opengl.Colour(1.0f, 1.0f, 1.0f);
   opengl.SelectTexture(uFuelBarTexture);
   glLoadIdentity();
   glBegin(GL_QUADS);
   glTexCoord2f(1.0f-texsize, 1.0f); glVertex2i(opengl.GetWidth()-fbsize-10, 30);
   glTexCoord2f(1.0f, 1.0f); glVertex2i(opengl.GetWidth()-10, 30);
   glTexCoord2f(1.0f, 0.0f); glVertex2i(opengl.GetWidth()-10, 62);
   glTexCoord2f(1.0f-texsize, 0.0f); glVertex2i(opengl.GetWidth()-fbsize-10, 62);
   glEnd();

   // Draw HUD
   opengl.Colour(0.0f, 0.9f, 0.0f);
   ft.Print(ftScore, 10, 10, "%.7d", score);
   ft.Print(ftNormal, opengl.GetWidth()-70, 10, S_FUEL);
   opengl.Draw(&speedbar);
   opengl.Draw(&speedmeter);
   opengl.Draw(&fuelmeter);
   opengl.Colour(0.0f, 1.0f, 0.0f);

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
      for (int i = 0; i < MAX_KEYS; i++) {
         keys[i].DrawIcon(offset + i*32, 0.3f);
      }
   }
   else {
      
      for (int i = 0; i < MAX_KEYS; i++)
         keys[i].DrawIcon(offset + i*32, 0.0f);
      opengl.Colour(0.0f, 1.0f, 0.0f);
      ft.Print
         (ftNormal,
          (opengl.GetWidth() - ft.GetStringWidth(ftNormal, S_LAND))/2,
          30, S_LAND);
   }

   // Draw level complete messages
   if (state == gsLevelComplete) {
      opengl.Draw(&levcomp);
      opengl.Colour(0.0f, 0.5f, 0.9f);
      ft.Print
         (ftBig,
          (opengl.GetWidth() - ft.GetStringWidth(ftBig, S_SCORE) - 40)/2,
          (opengl.GetHeight() - 30)/2 + 50,
          S_SCORE,
          newscore > 0 ? newscore : 0);
   }
   
   // Draw level number text
   if (leveltext_timeout) {
      opengl.Colour(0.9f, 0.9f, 0.0f);
      ft.Print
         (ftBig,
          (opengl.GetWidth() - ft.GetStringWidth(ftBig, S_LEVEL) - 20)/2,
          (opengl.GetHeight() - 30)/2,
          S_LEVEL, level);
   }

   // Draw the fade
   if (state == gsFadeIn || state == gsFadeToDeath || state == gsFadeToRestart)
      opengl.DrawBlend(&fade, fade_alpha);

   // Draw game over message
   if (lives == 0 || (lives == 1 && life_alpha < LIFE_ALPHA_BASE))
      opengl.Draw(&gameover);

   // Draw paused message
   if (state == gsPaused)
      opengl.Draw(&paused);
}
