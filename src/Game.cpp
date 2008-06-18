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

#include "Game.hpp"
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


Game::Game()
   : ship(&viewport),
     surface(&viewport),
     speedmeter(&ship),
     state(gsNone),
     fadeTexture("images/fade.png"),
     levelComp("images/levelcomp.png"),
     smallShip("images/shipsmall.png"),
     starImage("images/star.png"),
     gameOver("images/gameover.png"),
     normalFont(LocateResource("Default_Font.ttf"), 11),
     scoreFont(LocateResource("Default_Font.ttf"), 16),
     bigFont(LocateResource("Default_Font.ttf"), 20)
{

}

void Game::Load()
{
   // Create the fade
   fade.x = 0;
   fade.y = 0;
   fade.width = OpenGL::GetInstance().GetWidth();
   fade.height = OpenGL::GetInstance().GetHeight();
   fade.uTexture = fadeTexture.GetGLTexture();

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
   newscore_width = bigFont.GetStringWidth(i18n("Score:  %d"), newscore);
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
      asteroids[i].ConstructAsteroid(x, y, width, surftex);			
   }
   
   // Create gateways
   int gatewaycount = level/3 + rand()%level - 2;
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
   int minecount = level/2 + rand()%level - 2;
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
      glDisable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glColor4f(1.0f, 0.0f, 0.0f, 0.4f);
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
      glColor3f(0.0f, 1.0f, 0.0f);
      const char *sdeath = i18n("Press SPACE to continue");
      int x = (opengl.GetWidth() - normalFont.GetStringWidth(sdeath)) / 2;
      int y = opengl.GetHeight() - 40;
      normalFont.Print(x, y, sdeath);
   }
   else if (state == gsDeathWait || state == gsGameOver 
            || state == gsFadeToDeath || state == gsFadeToRestart) {
      ship.DrawExplosion(false);
   }
   
   // Draw the arrows
   for (KeyListIt it = keys.begin(); it != keys.end(); ++it)
      (*it).DrawArrow(&viewport);

   // Draw HUD
   glColor3f(0.0f, 0.9f, 0.0f);
   scoreFont.Print(10, SCORE_Y, "%.7d", score);

   fuelmeter.Display();
   speedmeter.Display();

   // Draw life icons
   for (int i = 0; i < lives; i++) {
      int draw_x = 5 + i*30;
      int draw_y = 60;
      if (i == lives-1) {
         if (life_alpha > LIFE_ALPHA_BASE)
            smallShip.Draw(draw_x, draw_y);
         else if (life_alpha < 0.0f) {
            // Decrement lives
            lives--;
            life_alpha = LIFE_ALPHA_BASE + 1.0f;
         }
         else {
            smallShip.Draw(draw_x, draw_y, 0.0, 1.0, life_alpha);
            life_alpha -= 0.03f;
         }
      }
      else
         smallShip.Draw(draw_x, draw_y);
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
      glColor3f(0.0f, 1.0f, 0.0f);
      const char *sland = i18n("Land  now");
      int x = (opengl.GetWidth() - normalFont.GetStringWidth(sland)) / 2;
      normalFont.Print(x, 30, sland);
   }

   // Draw level complete messages
   const char *scoretxt = i18n("Score:  %d");
   if (state == gsLevelComplete) {
      int lc_x = (opengl.GetWidth() - levelComp.GetWidth()) / 2;
      int lc_y = (opengl.GetHeight() - levelComp.GetHeight()) / 2 - 50;
      levelComp.Draw(lc_x, lc_y);
         
      glColor3f(0.0f, 0.5f, 0.9f);
      int printScore = newscore > 0 ? newscore : 0;
      int x = (opengl.GetWidth() - newscore_width) / 2;
      int y = (opengl.GetHeight() - 30)/2 + 50;
      bigFont.Print(x, y, scoretxt, printScore);
   }
   
   // Draw level number text
   if (leveltext_timeout) {
      glColor3f(0.9f, 0.9f, 0.0f);
      const char *lvltxt = i18n("Level  %d");
      int x = (opengl.GetWidth() - bigFont.GetStringWidth(lvltxt) - 20) / 2;
      int y = (opengl.GetHeight() - 30) / 2;
      bigFont.Print(x, y, lvltxt, level);
   }

   // Draw the fade
   if (state == gsFadeIn || state == gsFadeToDeath || state == gsFadeToRestart)
      opengl.DrawBlend(&fade, fade_alpha);

   // Draw game over message
   if (lives == 0 || (lives == 1 && life_alpha < LIFE_ALPHA_BASE)) {
      int draw_x = (opengl.GetWidth() - gameOver.GetWidth()) / 2;
      int draw_y = (opengl.GetHeight() - 150)/2;
      gameOver.Draw(draw_x, draw_y);
   }

   // Draw paused message
   if (state == gsPaused) {
      const char *txtpaused = i18n("Paused");
      glColor3f(0.0f, 0.5f, 1.0f);
      int x = (opengl.GetWidth() - bigFont.GetStringWidth(txtpaused) - 20) / 2;
      int y = (opengl.GetHeight() - 150) / 2;
      bigFont.Print(x, y, txtpaused);
   }
}

Image *FuelMeter::fuelMeterImage = NULL;
Texture *FuelMeter::fuelBarTexture = NULL;

FuelMeter::FuelMeter()
   : fuel(0), maxfuel(1)
{
   LOAD_ONCE {
      fuelMeterImage = new Image("images/fuelmeter.png");
      fuelBarTexture = new Texture("images/fuelbar.png");
   }
}

void FuelMeter::Display()
{
   OpenGL &opengl = OpenGL::GetInstance();
   
   int fbsize = (int)(((float)fuel/(float)maxfuel)*(256-FUELBAR_OFFSET)); 
   float texsize = fbsize/(256.0f-FUELBAR_OFFSET);
   glEnable(GL_BLEND);
   glDisable(GL_DEPTH_TEST);
   glColor3f(1.0f, 1.0f, 1.0f);
   glBindTexture(GL_TEXTURE_2D, fuelBarTexture->GetGLTexture());
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

   int draw_x = opengl.GetWidth() - fuelMeterImage->GetWidth() - 10;
   int draw_y = FUELBAR_Y;
   fuelMeterImage->Draw(draw_x, draw_y);
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

Image *SpeedMeter::speedMeterImage = NULL;

SpeedMeter::SpeedMeter(Ship *ship)
   : ship(ship)
{
   LOAD_ONCE {
      speedMeterImage = new Image("images/speedmeter.png");
   }

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

   OpenGL::GetInstance().Draw(&speedbar);

   speedMeterImage->Draw(10, 40);
}

bool SpeedMeter::SafeLandingSpeed() const
{
   return ship->GetYSpeed() < LAND_SPEED;
}

