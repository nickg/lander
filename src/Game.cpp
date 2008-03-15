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
#define SURFACE_SIZE		    20
#define VARIANCE		        50
//#define AS_VARIANCE		      64
#define MAX_PAD_SIZE		    3
#define LAND_SPEED		      2.0f
//#define OBJ_GRID_SIZE		    32
#define SHIP_START_Y		    100
#define KEY_ROTATION_SPEED  2
#define MINE_ROTATION_SPEED 5
#define GATEWAY_ACTIVE		  30
#define MINE_MOVE_SPEED		  1
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

/* Globals */
extern DataFile *g_pData;


/*
 * Defines a simplified polygon representing the ship.
 */
const Point Game::hotspots[] = {{1, 31}, {1, 26}, {3, 14}, {15, 0}, 
                                  {28, 14}, {30, 26}, {30, 31}, {16, 31}};


/*
 * Sets the inital state of the Game object.
 */
Game::Game()
  : hasloaded(false), bThrusting(false), surface(NULL), state(gsNone)
{

}


/* Loads game data */
void Game::Load()
{
   const int TEX_NAME_LEN = 128;
   char buf[TEX_NAME_LEN];
   int i;

   OpenGL &opengl = OpenGL::GetInstance();

   // Load textures
   if (!hasloaded) {
      // Load textures
      uShipTexture = opengl.LoadTextureAlpha(g_pData, "Ship.bmp");
      uStarTexture = opengl.LoadTextureAlpha(g_pData, "Star.bmp");
      uFadeTexture = opengl.LoadTexture(g_pData, "Fade.bmp");
      uLevComTexture = opengl.LoadTextureAlpha(g_pData, "LevelComplete.bmp");
      uSurfaceTexture[0] = opengl.LoadTexture(g_pData, "GrassSurface.bmp");
      uSurfaceTexture[1] = opengl.LoadTexture(g_pData, "DirtSurface.bmp");
      uSurfaceTexture[2] = opengl.LoadTexture(g_pData, "SnowSurface.bmp");
      uSurfaceTexture[3] = opengl.LoadTexture(g_pData, "RedRockSurface.bmp");
      uSurfaceTexture[4] = opengl.LoadTexture(g_pData, "RockSurface.bmp");
      uSurf2Texture[0] = opengl.LoadTexture(g_pData, "GrassSurface2.bmp");
      uSurf2Texture[1] = opengl.LoadTexture(g_pData, "DirtSurface2.bmp");
      uSurf2Texture[2] = opengl.LoadTexture(g_pData, "SnowSurface2.bmp");
      uSurf2Texture[3] = opengl.LoadTexture(g_pData, "RedRockSurface2.bmp");
      uSurf2Texture[4] = opengl.LoadTexture(g_pData, "RockSurface2.bmp");
      uSpeedTexture = opengl.LoadTextureAlpha(g_pData, "SpeedMeter.bmp");
      uBlueArrow = opengl.LoadTextureAlpha(g_pData, "BlueArrow.bmp");
      uRedArrow = opengl.LoadTextureAlpha(g_pData, "RedArrow.bmp");
      uGreenArrow = opengl.LoadTextureAlpha(g_pData, "GreenArrow.bmp");
      uYellowArrow = opengl.LoadTextureAlpha(g_pData, "YellowArrow.bmp");
      uPinkArrow = opengl.LoadTextureAlpha(g_pData, "PinkArrow.bmp");
      uGatewayTexture = opengl.LoadTextureAlpha(g_pData, "Gateway.bmp");
      uFuelMeterTexture = opengl.LoadTextureAlpha(g_pData, "FuelMeter.bmp");
      uFuelBarTexture = opengl.LoadTextureAlpha(g_pData, "FuelBar.bmp");
      uShipSmallTexture = opengl.LoadTextureAlpha(g_pData, "ShipSmall.bmp");
      uGameOver = opengl.LoadTextureAlpha(g_pData, "GameOver.bmp");
      uPausedTexture = opengl.LoadTextureAlpha(g_pData, "Paused.bmp");
      
      // Load key and mine textures
      for (i = 0; i < 18; i++) {
         snprintf(buf, TEX_NAME_LEN, "keyblue%.2d.bmp", i);
         uBlueKey[i] = opengl.LoadTextureAlpha(g_pData, buf);
         
         snprintf(buf, TEX_NAME_LEN, "keygreen%.2d.bmp", i);
         uGreenKey[i] = opengl.LoadTextureAlpha(g_pData, buf);
         
         snprintf(buf, TEX_NAME_LEN, "keyred%.2d.bmp", i);
         uRedKey[i] = opengl.LoadTextureAlpha(g_pData, buf);
         
         snprintf(buf, TEX_NAME_LEN, "keyyellow%.2d.bmp", i);
         uYellowKey[i] = opengl.LoadTextureAlpha(g_pData, buf);
         
         snprintf(buf, TEX_NAME_LEN, "keypink%.2d.bmp", i);
         uPinkKey[i] = opengl.LoadTextureAlpha(g_pData, buf);
               
         snprintf(buf, TEX_NAME_LEN, "mine%d.bmp", i*5);
         uMineTexture[i] = opengl.LoadTextureAlpha(g_pData, buf);
      }
      
      LandingPad::Load();
      
      // Toggle loaded flag
      hasloaded = true;
   }

   // Create the ship
   ship.tq.width = 32;
   ship.tq.height = 32;
   ship.tq.uTexture = uShipTexture;

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

   // Set default values
   nViewAdjustX = 0;
   nViewAdjustY = 0;
   starrotate = 0.0f;
   death_timeout = 0;
   state = gsNone;

   bDebugMode = false;
}

/* Destructor */
Game::~Game()
{
   if (surface)
      delete[] surface;
}

/* Starts a new game */
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

/* Processes events */
void Game::Process()
{
   Input &input = Input::GetInstance();
   OpenGL &opengl = OpenGL::GetInstance();
   int i, k, m;

   // Rotate ship hotspots
   RotatePoints(hotspots, points, NUM_HOTSPOTS, ship.angle*PI/180, -16, 16);

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
         bThrusting = false;
      }
   }

   // Check for paused state
   if (state == gsPaused)
      return;

   if ((input.GetKeyState(SDLK_UP) || input.QueryJoystickButton(1))
       && fuel > 0 && state == gsInGame) {
      // Thrusting
      ship.flSpeedX += SHIP_SPEED * (float)sin(ship.angle*(PI/180));
      ship.flSpeedY -= SHIP_SPEED * (float)cos(ship.angle*(PI/180));
      bThrusting = true;
      fuel--;
   }
   else
      bThrusting = false;
   
   if ((input.GetKeyState(SDLK_RIGHT) || input.QueryJoystickAxis(0) > 0)
       && state == gsInGame) {
      // Turn clockwise
      ship.angle += 3.0f;
   }
   else if ((input.GetKeyState(SDLK_LEFT) || input.QueryJoystickAxis(0) < 0)
            && state == gsInGame) {
      // Turn anti-clockwise
      ship.angle -= 3.0f;
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

   // Move only if not in game over (prevent bugs)
   if (state == gsInGame || state == gsExplode) {
      // Apply gravity
      ship.flSpeedY += flGravity;

      // Move the ship (and exhaust and explosion)
      ship.xpos += ship.flSpeedX;
      ship.ypos += ship.flSpeedY;
      exhaust.xpos = ship.xpos + ship.tq.width/2
         - (ship.tq.width/2)*(float)sin(ship.angle*(PI/180));
      exhaust.ypos = ship.ypos + ship.tq.height/2
         + (ship.tq.height/2)*(float)cos(ship.angle*(PI/180));
      exhaust.yg = ship.flSpeedY + (flGravity * 10);
      exhaust.xg = ship.flSpeedX;
      explosion.xpos = ship.xpos + ship.tq.width/2;
      explosion.ypos = ship.ypos + ship.tq.height/2;
   }

   // Move mines
   for (i = 0; i < minecount; i++) {
      if (mines[i].displace_x%ObjectGrid::OBJ_GRID_SIZE == 0
          && mines[i].displace_y%ObjectGrid::OBJ_GRID_SIZE == 0) {
         switch (mines[i].dir) {
         case UP: mines[i].ypos-=1; break;
         case DOWN: mines[i].ypos+=1; break;
         case LEFT: mines[i].xpos-=1; break;
         case RIGHT: mines[i].xpos+=1; break;
         case NODIR: 
            break;	// Do nothing
         }
         mines[i].displace_x = 0;
         mines[i].displace_y = 0;
         mines[i].movedelay = 1;
         
         // Change direction
         bool ok = false;
         int nextx=0, nexty=0, timeout = 5;
         do {
            if (timeout < 5 || mines[i].movetimeout == 0) {
               mines[i].dir = rand() % 4;
               mines[i].movetimeout = 5;
            }
            else {
               mines[i].movetimeout--;
            }
            
            switch (mines[i].dir) {
            case UP: 
               nexty = mines[i].ypos - 1; 
               nextx = mines[i].xpos;
               break;
            case DOWN: 
               nexty = mines[i].ypos + 1;
               nextx = mines[i].xpos;
               break;
            case LEFT: 
               nexty = mines[i].ypos;
               nextx = mines[i].xpos - 1;
               break;
            case RIGHT:
               nexty = mines[i].ypos;
               nextx = mines[i].xpos + 1; 
               break;
            default:
               nextx = mines[i].xpos;
               nexty = mines[i].ypos;
            }
            
            // Check if this is ok
            ok = nextx >= objgrid.GetWidth() || nextx < 0 
               || nexty > objgrid.GetHeight() || nexty < 0 
               || objgrid.IsFilled(nextx, nexty)
               || objgrid.IsFilled(nextx + 1, nexty)
               || objgrid.IsFilled(nextx + 1, nexty + 1)
               || objgrid.IsFilled(nextx, nexty + 1);
            ok = !ok;
            timeout--;
         } while (!ok && timeout > 0);
         
         if (timeout == 0)
            mines[i].dir = NODIR;				
      }
      
      if (--mines[i].movedelay == 0) {
         switch(mines[i].dir) {
         case UP: mines[i].displace_y--; break;
         case DOWN: mines[i].displace_y++; break;
         case LEFT: mines[i].displace_x--; break;
         case RIGHT: mines[i].displace_x++; break;
         }
         mines[i].movedelay = MINE_MOVE_SPEED;
      }
   }
   
   // Check bounds
   if (ship.xpos <= 0.0f) {
      ship.xpos = 0.0f;
      ship.flSpeedX *= -0.5f;
   }
   else if (ship.xpos + ship.tq.width > levelwidth) {
      ship.xpos = (float)(levelwidth - ship.tq.width);
      ship.flSpeedX *= -0.5f;
   }
   if (ship.ypos <= 0.0f) {
      ship.ypos = 0.0f;
      ship.flSpeedY *= -0.5f;
   }
   else if (ship.ypos + ship.tq.height > levelheight) {
      ship.ypos = (float)(levelheight - ship.tq.height);
      ship.flSpeedY *= -0.5f;
      
      // Bug fix
      if (state == gsExplode) {
         state = gsDeathWait; 
         death_timeout = DEATH_TIMEOUT;
      }
   }
   
   // Calculate view adjusts
   int centrex = (int)ship.xpos + (ship.tq.width/2);
   int centrey = (int)ship.ypos + (ship.tq.height/2);
   nViewAdjustX = centrex - (opengl.GetWidth()/2);
   nViewAdjustY = centrey - (opengl.GetHeight()/2);
   if (nViewAdjustX < 0)
      nViewAdjustX = 0;
   else if (nViewAdjustX > levelwidth - opengl.GetWidth())
      nViewAdjustX = levelwidth - opengl.GetWidth();
   if (nViewAdjustY < 0)
      nViewAdjustY = 0;
   else if (nViewAdjustY > levelheight - opengl.GetHeight())
      nViewAdjustY = levelheight - opengl.GetHeight();

   // Check for collisions with surface
   LineSegment l;
   int lookmin = (int)(ship.xpos/SURFACE_SIZE) - 2;
   int lookmax = (int)(ship.xpos/SURFACE_SIZE) + 2;
   if (lookmin < 0)	lookmin = 0;
   if (lookmax >= levelwidth/SURFACE_SIZE) lookmax = (levelwidth / SURFACE_SIZE) - 1;
   for (i = lookmin; i <= lookmax; i++) {
      l.p1.x = i*SURFACE_SIZE;
      l.p1.y = levelheight - MAX_SURFACE_HEIGHT + surface[i].points[1].y;
      l.p2.x = (i+1)*SURFACE_SIZE;
      l.p2.y = levelheight - MAX_SURFACE_HEIGHT + surface[i].points[2].y;
      
      // Look through each hot spot and check for collisions
      if (HotSpotCollision(ship, l, points, NUM_HOTSPOTS, ship.xpos, ship.ypos)) {
         // Collided - see which game state we're in
         if (state == gsInGame) {
            bool bLanded = false;
            int nPadOn = -1;

            // See if this is a landing pad
            for (k = 0; k < nLandingPads; k++) {
               for (m = 0; m < pads[k].GetLength(); m++) {
                  if (pads[k].GetIndex() + m == i) {
                     // We landed
                     int nDAngle = ((int)ship.angle) % 360;
                     if ((nDAngle >= 350 || nDAngle <= 30) 
                         && ship.flSpeedY < LAND_SPEED && !nKeysRemaining) {
                           // Landed safely
                        bLanded = true;
                        nPadOn = k;
                        break;
                     }
                     else {
                        // Crash landed
                        bLanded = false;
                        nPadOn = -1;
                        break;
                     }
                  }
               }
            }
            
            if (bLanded) {
               // Landed - go to next level
               state = gsLevelComplete;
               newscore = (level * 100) + 
                  (((MAX_PAD_SIZE+2)-pads[nPadOn].GetLength())*10*level);
               countdown_timeout = 70;
            }
            else {
               // Crashed - destroy the ship
               ExplodeShip();
               BounceShip();
            }
         }
         else if (state == gsExplode) {
            BounceShip();
            
            // See if we need to stop the madness
            if (state == gsExplode && ship.flSpeedY*-1 < 0.05f) {
               state = gsDeathWait; 
               death_timeout = DEATH_TIMEOUT;
            }
         }
      }
   }
   
   // Check for collisions with asteroids
   LineSegment l1, l2;
   for (i = 0; i < asteroidcount; i++) {
      if (ObjectInScreen(asteroids[i].GetXPos(), 
                         asteroids[i].GetYPos() + SHIP_START_Y / ObjectGrid::OBJ_GRID_SIZE,
                         asteroids[i].GetWidth(), 4)) {
         // Look at polys
         for (k = 0; k < asteroids[i].GetWidth(); k++) {
            l1 = asteroids[i].GetUpBoundary(k);
            l2 = asteroids[i].GetDownBoundary(k);
            
            if (HotSpotCollision(ship, l1, points, NUM_HOTSPOTS, ship.xpos, ship.ypos) 
                || HotSpotCollision(ship, l2, points, NUM_HOTSPOTS, ship.xpos, ship.ypos)) {
               // Crashed
               if (state == gsInGame) {
                  // Destroy the ship
                  ExplodeShip();
                  BounceShip();
               }
               else if (state == gsExplode) {
                     BounceShip();
                     
                                 // See if we need to stop the madness
                                 if (state == gsExplode && ship.flSpeedY*-1 < 0.05f)
                                    {
                                       state = gsDeathWait; 
                                       death_timeout = DEATH_TIMEOUT;
                                    }
                              }
                        }
                  }
            }
      }

   // Check for collision with gateways
   for (i = 0; i < gatewaycount; i++)
      {
         int dx = gateways[i].vertical ? 0 : gateways[i].length;
         int dy = gateways[i].vertical ? gateways[i].length : 0;
         if (gateways[i].timer > GATEWAY_ACTIVE)
            {
               bool collide1 = BoxCollision
                  (
                   ship,
                   gateways[i].xpos*ObjectGrid::OBJ_GRID_SIZE,
                   gateways[i].ypos*ObjectGrid::OBJ_GRID_SIZE + SHIP_START_Y,
                   ObjectGrid::OBJ_GRID_SIZE,
                   ObjectGrid::OBJ_GRID_SIZE,
                   points,
                   NUM_HOTSPOTS
                   );
			
               bool collide2 = BoxCollision
                  (
                   ship,
                   (gateways[i].xpos + dx)*ObjectGrid::OBJ_GRID_SIZE,
                   (gateways[i].ypos + dy)*ObjectGrid::OBJ_GRID_SIZE + SHIP_START_Y,
                   ObjectGrid::OBJ_GRID_SIZE,
                   ObjectGrid::OBJ_GRID_SIZE,
                   points, 
                   NUM_HOTSPOTS
                   );
		
               if (collide1 || collide2)
                  {
                     if (state == gsInGame)
                        {
                           // Destroy the ship
                           ExplodeShip();
                           BounceShip();
                        }
                     else if (state == gsExplode)
                        {
                           BounceShip();

                           // See if we need to stop the madness
                           if (state == gsExplode && ship.flSpeedY*-1 < 0.05f)
                              {
                                 state = gsDeathWait; 
                                 death_timeout = DEATH_TIMEOUT;
                              }
                        }
                  }
            }
         else
            {
               bool collide = BoxCollision
                  (
                   ship,
                   gateways[i].xpos*ObjectGrid::OBJ_GRID_SIZE, 
                   gateways[i].ypos*ObjectGrid::OBJ_GRID_SIZE + SHIP_START_Y,
                   (dx + 1)*ObjectGrid::OBJ_GRID_SIZE,
                   (dy + 1)*ObjectGrid::OBJ_GRID_SIZE,
                   points,
                   NUM_HOTSPOTS
                   ); 
		
               if (collide)
                  {
                     if (state == gsInGame)
                        {
                           // Destroy the ship
                           ExplodeShip();
                           BounceShip();
                        }
                     else if (state == gsExplode)
                        {
                           // Destroy the ship anyway
                           state = gsDeathWait; 
                           death_timeout = DEATH_TIMEOUT;
                        }
                  }
            }
      }

   // Check for collisions with mines
   for (i = 0; i < minecount; i++)
      {
         bool collide = BoxCollision
            (
             ship,
             mines[i].xpos*ObjectGrid::OBJ_GRID_SIZE + 3 + mines[i].displace_x,
             mines[i].ypos*ObjectGrid::OBJ_GRID_SIZE + SHIP_START_Y + 6 + mines[i].displace_y,
             ObjectGrid::OBJ_GRID_SIZE*2 - 6,
             ObjectGrid::OBJ_GRID_SIZE*2 - 12,
             points,
             NUM_HOTSPOTS
             ); 
	
         if (collide)
            {
               if (state == gsInGame)
                  {
                     // Destroy the ship
                     ExplodeShip();
                     BounceShip();
                  }
               else if (state == gsExplode)
                  {
                     BounceShip();

                     // See if we need to stop the madness
                     if (state == gsExplode && -ship.flSpeedY < 0.05f)
                        {
                           state = gsDeathWait; 
                           death_timeout = DEATH_TIMEOUT;
                        }
                  }
            }
      }

   // See if the player collected a key
   for (i = 0; i < nKeys; i++)
      {
         bool collide = BoxCollision
            (
             ship,
             keys[i].xpos*ObjectGrid::OBJ_GRID_SIZE + 3,
             keys[i].ypos*ObjectGrid::OBJ_GRID_SIZE + SHIP_START_Y + 3,
             ObjectGrid::OBJ_GRID_SIZE - 6,
             ObjectGrid::OBJ_GRID_SIZE - 6,
             points, 
             NUM_HOTSPOTS
             );	
	
         if (keys[i].active && collide)
            {
               nKeysRemaining--;
               keys[i].active = false;
               objgrid.UnlockSpace(keys[i].xpos, keys[i].ypos);
            }
      }

   // Entry / exit states
   if (state == gsDeathWait)
      {
         if (--death_timeout == 0)
            {
               // Fade out
               if (lives == 0  || (lives == 1 && life_alpha < LIFE_ALPHA_BASE))
                  {
                     state = gsFadeToDeath;
                     fade_alpha = LIFE_ALPHA_BASE + 1.0f;
                  }
               else if (lives > 0)
                  {
                     if (life_alpha < LIFE_ALPHA_BASE)
                        {
                           life_alpha = LIFE_ALPHA_BASE + 1.0f;
                           lives--;
                        }
				
                     state = gsFadeToRestart;
                     fade_alpha = 0.0f;
                  }
            }
      }
   else if (state == gsGameOver)
      {
         if (--death_timeout == 0)
            {
               // Fade out
               state = gsFadeToDeath;
               fade_alpha = 0.0f;
            }
      }
   else if (state == gsFadeIn)
      {
         // Fade in
         fade_alpha -= GAME_FADE_IN_SPEED;
         if (fade_alpha < 0.0f)
            state = gsInGame;
      }
   else if (state == gsFadeToRestart)
      {
         // Fade out
         fade_alpha += GAME_FADE_OUT_SPEED;
         if (fade_alpha > 1.0f)
            {
               // Restart the level
               StartLevel(level);
               opengl.SkipDisplay();
            }
      }
   else if (state == gsFadeToDeath)
      {
         fade_alpha += GAME_FADE_OUT_SPEED;
         if (fade_alpha > 1.0f)
            {
               // Return to main menu
               ScreenManager &sm = ScreenManager::GetInstance();
               HighScores *hs = static_cast<HighScores*>(sm.GetScreenById("HIGH SCORES"));
               hs->CheckScore(score);
            }
      }
   else if (state == gsLevelComplete)
      {
         // Decrease the displayed score
         if (countdown_timeout > 0)
            countdown_timeout--;
         else if (levelcomp_timeout > 0)
            {
               if (--levelcomp_timeout == 0)
                  {
                     level++;
                     state = gsFadeToRestart;
                  }
            }
         else
            {
               int dec = level * 2;

               // Decrease the score
               newscore -= dec;
               score += dec;

               if (score > nextnewlife)
                  {
                     lives++;
                     nextnewlife *= 2;
                  }

               if (newscore < 0)
                  {
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
      ship.angle += 5.0f;

   // Resize the speed bar
   float flSpeed1 = 30.0f / LAND_SPEED;
   int width = (int)((float)ship.flSpeedY * flSpeed1); 
   if (width < 0) 
      width = 0;
   if (width > 124) 
      width = 124;
   speedbar.blue = 0.0f;
   speedbar.red = (float)width/124.0f;
   speedbar.green = 1.0f - (float)width/124.0f;
   speedbar.width = width;
}

/* Starts a level */
void Game::StartLevel(int level)
{
   int i, change, texloop=0, j, k, nPadHere;

   // Set level size
   levelwidth = 2000 + 2*SURFACE_SIZE*level;
   levelheight = 1500 + 2*SURFACE_SIZE*level;
   flGravity = GRAVITY;

   // Create the object grid
   int grid_w = levelwidth / ObjectGrid::OBJ_GRID_SIZE;
   int grid_h = (levelheight - SHIP_START_Y - MAX_SURFACE_HEIGHT - 100) / ObjectGrid::OBJ_GRID_SIZE;
   objgrid.Reset(grid_w, grid_h);

   // Create background stars
   nStarCount = (levelwidth * levelheight) / 10000;
   if (nStarCount > MAX_GAME_STARS)
      nStarCount = MAX_GAME_STARS;
   for (i = 0; i < nStarCount; i++)
      {
         stars[i].xpos = (int)(rand()%(levelwidth/20))*20;
         stars[i].ypos = (int)(rand()%(levelheight/20))*20;
         stars[i].quad.uTexture = uStarTexture;
         stars[i].quad.width = stars[i].quad.height = rand()%15;
      }

   // Create the planet surface 
   if (surface)
      delete[] surface;
   surface = new Poly[levelwidth/SURFACE_SIZE];
	
   // Generate landing pads
   nLandingPads = rand()%MAX_PADS + 1;
   for (int i = 0; i < nLandingPads; i++)
      {
         int index, length;
         bool overlap;
         do
            {
               index = rand() % (levelwidth / SURFACE_SIZE);
               length = rand() % MAX_PAD_SIZE + 3;

               // Check for overlap
               overlap = false;
               if (index + length > (levelwidth / SURFACE_SIZE))
                  overlap = true;
               for (int j = 0; j < i; j++)
                  {
                     if (pads[j].GetIndex() == index) 
                        overlap = true;
                     else if (pads[j].GetIndex() < index && pads[j].GetIndex() + pads[j].GetLength() >= index)
                        overlap = true;
                     else if (index < pads[j].GetIndex() && index + length >= pads[j].GetIndex())
                        overlap = true;
                  }
            } while (overlap);
		
         pads[i].Reset(index, length);
      }

   // Generate surface
   int surftex = rand()%NUM_SURF_TEX;
   for (i = 0; i < levelwidth/SURFACE_SIZE; i++)
      {
         surface[i].pointcount = 4;
         surface[i].xpos = i * SURFACE_SIZE;
         surface[i].ypos = levelheight - MAX_SURFACE_HEIGHT;
         surface[i].uTexture = uSurfaceTexture[surftex];
         surface[i].texX = ((float)texloop)/10;
         if (texloop++ == 10)
            texloop = 0;
         surface[i].texwidth = 0.1f;

         surface[i].points[0].x = 0;
         surface[i].points[0].y = MAX_SURFACE_HEIGHT;
		
         // See if we want to place a landing pad here
         nPadHere = -1;
         for (j = 0; j < nLandingPads; j++)
            {
               for (k = 0; k < pads[j].GetLength(); k++)
                  {
                     if (pads[j].GetIndex() + k == i)
                        {
                           nPadHere = j;
                           goto out;
                        }
                  }
            }

      out:

         if (nPadHere == -1)
            {
               // Genereate height randomly
               if (i != 0)
                  change = surface[i-1].points[2].y;
               else
                  change = rand()%MAX_SURFACE_HEIGHT;
               surface[i].points[1].x = 0;
               surface[i].points[1].y = change;
			
               do
                  change = surface[i].points[1].y + (rand()%VARIANCE-(VARIANCE/2));
               while (change > MAX_SURFACE_HEIGHT || change < 0);
               surface[i].points[2].x = SURFACE_SIZE;
               surface[i].points[2].y = change;
            }
         else
            {
               // Make flat terrain for landing pad
               if (i != 0)
                  change = surface[i-1].points[2].y;
               else
                  change = rand()%MAX_SURFACE_HEIGHT;
               surface[i].points[1].x = 0;
               surface[i].points[1].y = change;
               surface[i].points[2].x = SURFACE_SIZE;
               surface[i].points[2].y = change;

               pads[nPadHere].SetYPos(change);
            }

         surface[i].points[3].x = SURFACE_SIZE;
         surface[i].points[3].y = MAX_SURFACE_HEIGHT;
      }

   // Create the keys (must be created first because no success check is made on AllocFreeSpace call)
   nKeys = (level / 2) + (level % 2);
   if (nKeys > MAX_KEYS)
      nKeys = MAX_KEYS;
   nKeysRemaining = nKeys;
   for (i = 0; i < MAX_KEYS; i++)
      {
         if (i < nKeys)
            {		
               objgrid.AllocFreeSpace(keys[i].xpos, keys[i].ypos);
               keys[i].active = true;
               keys[i].alpha = 1.0f;
            } 
         else
            {
               keys[i].active = false;	
               keys[i].alpha = -1.0f;
            }
		
         keys[i].current = 0;
         keys[i].rotcount = KEY_ROTATION_SPEED;

         // Allocate arrow image
         keys[i].arrow.width = 32;
         keys[i].arrow.height = 32;
         switch(i)
            {
            case 0: keys[i].arrow.uTexture = uBlueArrow; break;
            case 1: keys[i].arrow.uTexture = uRedArrow; break;
            case 2: keys[i].arrow.uTexture = uYellowArrow; break;
            case 3: keys[i].arrow.uTexture = uPinkArrow; break;
            case 4: keys[i].arrow.uTexture = uGreenArrow; break;
            }
		
         // Allocate frame images
         for (j = 0; j < 18; j++)
            {
               keys[i].frame[j].width = ObjectGrid::OBJ_GRID_SIZE;
               keys[i].frame[j].height = ObjectGrid::OBJ_GRID_SIZE;

               // Allocate key color
               switch(i)
                  {
                  case 0:
                     keys[i].frame[j].uTexture = uBlueKey[j];
                     break;
                  case 1:
                     keys[i].frame[j].uTexture = uRedKey[j];
                     break;
                  case 2:
                     keys[i].frame[j].uTexture = uYellowKey[j];
                     break;
                  case 3:
                     keys[i].frame[j].uTexture = uPinkKey[j];
                     break;
                  case 4:
                     keys[i].frame[j].uTexture = uGreenKey[j];
                     break;
                  }
            }
      }

   // Create the asteroids
   asteroidcount = level*2 + rand()%(level+3);
   if (asteroidcount > MAX_ASTEROIDS)
      asteroidcount = MAX_ASTEROIDS;
   for (i = 0; i < asteroidcount; i++)
      {
         // Allocate space, check for timeout
         int x, y, width = rand() % (Asteroid::MAX_ASTEROID_WIDTH - 4) + 4;
         if (!objgrid.AllocFreeSpace(x, y, width, 4))
            {
               // Failed to allocate space so don't make any more asteroids
               asteroidcount = i + 1;
               break;
            }

         // Generate the asteroid
         asteroids[i].ConstructAsteroid(x, y, width, uSurf2Texture[surftex]);			
      }

   // Create gateways
   gatewaycount = level/2 + rand()%(level);
   if (gatewaycount > MAX_GATEWAYS)
      gatewaycount = MAX_GATEWAYS;
   for (i = 0; i < gatewaycount; i++)
      {
         // Allocate space for gateway
         gateways[i].length = rand()%(MAX_GATEWAY_LENGTH-3) + 3;
         switch(rand() % 2)
            {
            case 0: gateways[i].vertical = true; break;
            case 1: gateways[i].vertical = false; break;
            }
		
         bool result;
         if (gateways[i].vertical)
            result = objgrid.AllocFreeSpace(gateways[i].xpos, gateways[i].ypos, 1, gateways[i].length+1);
         else
            result = objgrid.AllocFreeSpace(gateways[i].xpos, gateways[i].ypos, gateways[i].length+1, 1);
         if (!result)
            {
               // Failed to allocate space so don't make any more gateways
               gatewaycount = i + 1;
               break;
            }

         // Set texture, etc.
         gateways[i].icon.width = ObjectGrid::OBJ_GRID_SIZE;
         gateways[i].icon.height = ObjectGrid::OBJ_GRID_SIZE;
         gateways[i].icon.uTexture = uGatewayTexture;
         gateways[i].timer = rand() % 70 + 10;
      }

   // Create mines (MUST BE CREATED LAST)
   minecount = level/2 + rand()%level;
   if (minecount > MAX_MINES)
      minecount = MAX_MINES;
   for (i = 0; i < minecount; i++)
      {
         // Allocate space for mine
         if (!objgrid.AllocFreeSpace(mines[i].xpos, mines[i].ypos, 2, 2))
            {
               // Failed to allocate space
               minecount = i + 1;
               break;
            }

         mines[i].current = 0;
         mines[i].rotcount = MINE_ROTATION_SPEED;
         mines[i].displace_x = 0;
         mines[i].displace_y = 0;
         mines[i].movedelay = MINE_MOVE_SPEED;
         mines[i].dir = NODIR;
         mines[i].movetimeout = 1;

         // Allocate frame images
         for (j = 0; j < 36; j++)
            {
               mines[i].frame[j].width = ObjectGrid::OBJ_GRID_SIZE*2;
               mines[i].frame[j].height = ObjectGrid::OBJ_GRID_SIZE*2;
               mines[i].frame[j].uTexture = uMineTexture[j];
            }

         // Free space on object grid
         objgrid.UnlockSpace(mines[i].xpos, mines[i].ypos);
         objgrid.UnlockSpace(mines[i].xpos + 1, mines[i].ypos);
         objgrid.UnlockSpace(mines[i].xpos + 1, mines[i].ypos + 1);
         objgrid.UnlockSpace(mines[i].xpos, mines[i].ypos + 1);
      }

   // Set ship starting position
   ship.xpos = (float)levelwidth/2;
   ship.ypos = SHIP_START_Y - 40;

   // Reset data
   ship.angle = 0.0f;
   ship.flSpeedX = 0.0f;
   ship.flSpeedY = 0.0f;
   leveltext_timeout = LEVEL_TEXT_TIMEOUT;

   // Reset emitters
   exhaust.Reset();
   explosion.Reset();

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


/*
 * Bounces a ship after an impact with a surface.
 */
void Game::BounceShip()
{
   ship.flSpeedY *= -1;
   ship.flSpeedX *= -1;
   ship.flSpeedX /= 2;
   ship.flSpeedY /= 2;
}


/* Rotates and array of Points */
void Game::RotatePoints(const Point *pPoints, Point *pDest, int nCount, float angle, int adjustx, int adjusty)
{
   for (int i = 0; i < nCount; i++)
      {
         int x = pPoints[i].x + adjustx;
         int y = pPoints[i].y*-1 + adjusty;
         pDest[i].x = (int)(x*cos(angle)) + (int)(y*sin(angle));
         pDest[i].y = (int)(y*cos(angle)) - (int)(x*sin(angle));
         pDest[i].y -= adjusty;
         pDest[i].x -= adjustx;
         pDest[i].y*=-1;
      }
}

/* Does a collision based on hot spots */
bool Game::HotSpotCollision(ActiveObject &a, LineSegment &l, Point *points, int nPoints, float dx, float dy)
{
   for (int i = 0; i < nPoints; i++)
      {
         if (CheckCollision(a, l, dx + points[i].x, dy + points[i].y))
            return true;
      }

   return false;
}

/* 
 * Works out whether or not an object is visible.
 *	xpos, ypos -> Grid co-ordinates.
 *	width, height -> Size of object in grid squares.
 */
bool Game::ObjectInScreen(int xpos, int ypos, int width, int height)
{
   return PointInScreen(xpos * ObjectGrid::OBJ_GRID_SIZE, ypos * ObjectGrid::OBJ_GRID_SIZE,
                        width * ObjectGrid::OBJ_GRID_SIZE, height * ObjectGrid::OBJ_GRID_SIZE);
}


/* 
 * Works out whether or not a point is visible.
 *	xpos, ypos -> Absolute co-ordinates.
 *	width, height -> Size of object.
 */
bool Game::PointInScreen(int xpos, int ypos, int width, int height)
{
   OpenGL &opengl = OpenGL::GetInstance();

   int sw = opengl.GetWidth();
   int sh = opengl.GetHeight();

   if (xpos + width > nViewAdjustX && xpos - nViewAdjustX < sw &&
       ypos + height > nViewAdjustY && ypos - nViewAdjustY < sh)
      return true;
   else
      return false;
}


/* Does simple box collision */
bool Game::BoxCollision(ActiveObject &a, int x, int y, int w, int h, Point *points, int nPoints)
{
   if (!PointInScreen(x, y, w, h))
      return false;

   LineSegment l1(x, y, x + w, y);
   LineSegment l2(x + w, y, x + w, y + h);
   LineSegment l3(x + w, y + h, x, y + h);
   LineSegment l4(x, y + h, x, y);

   return HotSpotCollision(a, l1, points, nPoints, a.xpos, a.ypos) ||
      HotSpotCollision(a, l2, points, nPoints, a.xpos, a.ypos) ||
      HotSpotCollision(a, l3, points, nPoints, a.xpos, a.ypos) ||
      HotSpotCollision(a, l4, points, nPoints, a.xpos, a.ypos);
}

/* Checks for collision between a vector and a line segment */
bool Game::CheckCollision(ActiveObject &a, LineSegment &l, float xpos, float ypos)
{
   if (xpos == -1 || ypos == -1)
      {
         xpos = a.xpos;
         ypos = a.ypos;
      }

   // Get position after next move
   float cX = xpos + a.flSpeedX;
   float cY = ypos + a.flSpeedY;

   // Get displacement
   float vecX = cX - xpos;
   float vecY = cY - ypos;

   // Get line position
   float wallX = (float)(l.p2.x - l.p1.x);
   float wallY = (float)(l.p2.y - l.p1.y);

   // Work out numerator and denominator (used parametric equations)
   float numT = wallX * (ypos - l.p1.y) - wallY * (xpos - l.p1.x);
   float numU = vecX * (ypos - l.p1.y) - vecY * (xpos - l.p1.x);

   // Work out denominator
   float denom = wallY * (cX - xpos) - wallX * (cY - ypos);

   // Work out u and t
   float u = numU / denom;
   float t = numT / denom;

   // Collision occured if (0 < t < 1) and (0 < u < 1)
   return (t > 0.0f) && (t < 1.0f) && (u > 0.0f) && (u < 1.0f);
}

/* Displays frame to user */
void Game::Display()
{
   int i, j, k, offset;
   static float xlast, ylast;

   FreeType &ft = FreeType::GetInstance();
   OpenGL &opengl = OpenGL::GetInstance();

   // Draw the stars
   for (i = 0; i < nStarCount; i++)
      {
         stars[i].quad.x = stars[i].xpos - nViewAdjustX;
         stars[i].quad.y = stars[i].ypos - nViewAdjustY;
         opengl.DrawRotate(&stars[i].quad, starrotate);
         starrotate += 0.005f;
      }

   // Draw the planet surface
   for (i = 0; i < levelwidth/SURFACE_SIZE; i++)
      {
         surface[i].xpos = i*SURFACE_SIZE - nViewAdjustX;
         surface[i].ypos = levelheight - nViewAdjustY - MAX_SURFACE_HEIGHT;
         opengl.Draw(&surface[i]);
      }

   // Draw the asteroids
   for (i = 0; i < asteroidcount; i++)
      {
         if (ObjectInScreen(asteroids[i].GetXPos(), asteroids[i].GetYPos() + SHIP_START_Y / ObjectGrid::OBJ_GRID_SIZE, 
                            asteroids[i].GetWidth(), asteroids[i].GetHeight()))
            {
               asteroids[i].Draw(nViewAdjustX, nViewAdjustY);			
            }
      }

   // Draw the keys
   for (i = 0; i < nKeys; i++)
      {
         if (keys[i].active)
            {
               keys[i].frame[keys[i].current].x = keys[i].xpos*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustX;
               keys[i].frame[keys[i].current].y = keys[i].ypos*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustY + SHIP_START_Y;		
               opengl.Draw(&keys[i].frame[keys[i].current]);
               if (--keys[i].rotcount == 0)
                  {
                     if (++keys[i].current == 18)
                        keys[i].current = 0;
                     keys[i].rotcount = KEY_ROTATION_SPEED;
                  }
            }
         else
            {
               if (keys[i].alpha > 0.0f)
                  {
                     keys[i].frame[keys[i].current].x = keys[i].xpos*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustX;
                     keys[i].frame[keys[i].current].y = keys[i].ypos*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustY + SHIP_START_Y;	
                     opengl.DrawBlend(&keys[i].frame[keys[i].current], keys[i].alpha);
                     keys[i].alpha -= 0.02f;
                     if (--keys[i].rotcount == 0)
                        {
                           if (++keys[i].current == 18)
                              keys[i].current = 0;
                           keys[i].rotcount = KEY_ROTATION_SPEED;
                        }
                  }
            }
      }

   // Draw gateways
   for (i = 0; i < gatewaycount; i++)
      {
         // Draw first sphere
         gateways[i].icon.x = gateways[i].xpos*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustX;
         gateways[i].icon.y = gateways[i].ypos*ObjectGrid::OBJ_GRID_SIZE + SHIP_START_Y - nViewAdjustY;
         opengl.Draw(&gateways[i].icon);

         // Draw second sphere
         if (gateways[i].vertical)
            {
               gateways[i].icon.x = gateways[i].xpos*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustX;
               gateways[i].icon.y = (gateways[i].ypos+gateways[i].length)*ObjectGrid::OBJ_GRID_SIZE + SHIP_START_Y - nViewAdjustY;
            }
         else
            {
               gateways[i].icon.x = (gateways[i].xpos+gateways[i].length)*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustX;
               gateways[i].icon.y = gateways[i].ypos*ObjectGrid::OBJ_GRID_SIZE + SHIP_START_Y - nViewAdjustY;
            }
         opengl.Draw(&gateways[i].icon);

         // Draw the electricity stuff
         if (--gateways[i].timer < GATEWAY_ACTIVE)
            {
               float r, g, b; 
               int x, y, deviation;

               opengl.DisableBlending();
               opengl.DisableTexture();

               for (j = 0; j < 10; j++)
                  {
                     deviation = 0;
                     for (k = 0; k < gateways[i].length; k++)
                        {
                           glLoadIdentity();
                           glBegin(GL_LINE_STRIP);
                           r = 0.0f + (float)(rand()%5)/10.0f;
                           g = 0.0f + (float)(rand()%5)/10.0f;
                           b = 1.0f - (float)(rand()%5)/10.0f;

                           glColor3f(r, g, b);
                           if (gateways[i].vertical)
                              {
                                 x = gateways[i].xpos*ObjectGrid::OBJ_GRID_SIZE + 16 + deviation - nViewAdjustX;
                                 y = (gateways[i].ypos+k)*ObjectGrid::OBJ_GRID_SIZE + SHIP_START_Y + 16 - nViewAdjustY;
                                 glVertex2i(x, y);
                                 if (k == gateways[i].length-1)
                                    deviation = 0;
                                 else
                                    deviation += rand()%20 - 10;
                                 x = gateways[i].xpos*ObjectGrid::OBJ_GRID_SIZE + 16 + deviation - nViewAdjustX;
                                 y += ObjectGrid::OBJ_GRID_SIZE;
                                 glVertex2i(x, y);
                              }
                           else
                              {
                                 x = (gateways[i].xpos+k)*ObjectGrid::OBJ_GRID_SIZE + 16 - nViewAdjustX;
                                 y = gateways[i].ypos*ObjectGrid::OBJ_GRID_SIZE + SHIP_START_Y + 16 + deviation - nViewAdjustY;
                                 glVertex2i(x, y);
                                 if (k == gateways[i].length-1)
                                    deviation = 0;
                                 else
                                    deviation += rand()%20 - 10;
                                 y = gateways[i].ypos*ObjectGrid::OBJ_GRID_SIZE + SHIP_START_Y + 16 + deviation - nViewAdjustY;
                                 x += ObjectGrid::OBJ_GRID_SIZE;
                                 glVertex2i(x, y);
                              }
                           glEnd();
                        }
                  }

               // Reset timer 
               if (gateways[i].timer < 0)
                  gateways[i].timer = 100;
            }
      }

   // Draw mines
   for (i = 0; i < minecount; i++)
      {
         mines[i].frame[mines[i].current].x = mines[i].xpos*ObjectGrid::OBJ_GRID_SIZE + mines[i].displace_x - nViewAdjustX;
         mines[i].frame[mines[i].current].y = mines[i].ypos*ObjectGrid::OBJ_GRID_SIZE + mines[i].displace_y - nViewAdjustY + SHIP_START_Y;		
         opengl.Draw(&mines[i].frame[mines[i].current]);
         if (--mines[i].rotcount == 0)
            {
               if (++mines[i].current == 18)
                  mines[i].current = 0;
               mines[i].rotcount = MINE_ROTATION_SPEED;
            }
      }

   // ***DEBUG***
   if (bDebugMode)
      {
         // Draw red squares around no-go areas
         int x, y;
         opengl.DisableTexture();
         opengl.EnableBlending();
         opengl.Colour(1.0f, 0.0f, 0.0f, 0.4f);
         for (x = 0; x < objgrid.GetWidth(); x++)
            {
               for (y = 0; y < objgrid.GetHeight(); y++)
                  {
                     if (objgrid.IsFilled(x, y))
                        {
                           glLoadIdentity();
                           glBegin(GL_QUADS);
                           glVertex2i(x*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustX, y*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustY + SHIP_START_Y);
                           glVertex2i((x+1)*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustX, y*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustY + SHIP_START_Y);
                           glVertex2i((x+1)*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustX, (y+1)*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustY + SHIP_START_Y);
                           glVertex2i(x*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustX, (y+1)*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustY + SHIP_START_Y);
                           glEnd();
                        }
                  }
            }
      }

   // Draw the landing pads
   for (i = 0; i < nLandingPads; i++)
      {
         pads[i].Draw(nViewAdjustX, nViewAdjustY, levelheight, nKeysRemaining > 0);
      }

   // Draw the ship
   ship.tq.x = (int)ship.xpos - nViewAdjustX;
   ship.tq.y = (int)ship.ypos - nViewAdjustY;
   if (bThrusting)
      {
         if (sqrt(ship.flSpeedX*ship.flSpeedX + ship.flSpeedY*ship.flSpeedY) > 2.0f)
            {
               exhaust.NewCluster
                  (
                   (int)(exhaust.xpos + (exhaust.xpos - xlast)/2), 
                   (int)(exhaust.ypos + (exhaust.ypos - ylast)/2)
                   );
            }
         exhaust.Draw((float)nViewAdjustX, (float)nViewAdjustY, true);
      }
   else if (state == gsPaused)
      exhaust.Draw((float)nViewAdjustX, (float)nViewAdjustY, false, false);
   else
      exhaust.Draw((float)nViewAdjustX, (float)nViewAdjustY, false);
	
   if (state != gsDeathWait && state != gsGameOver
       && state != gsFadeToDeath && state != gsFadeToRestart)
      {
         opengl.DrawRotate(&ship.tq, ship.angle);
      }
	
   xlast = exhaust.xpos;
   ylast = exhaust.ypos;

   // Draw the explosion if necessary
   if (state == gsExplode)
      {
         explosion.Draw((float)nViewAdjustX, (float)nViewAdjustY, true);
         opengl.Colour(0.0f, 1.0f, 0.0f);
         ft.Print
            (
             ftNormal,
             (opengl.GetWidth() - ft.GetStringWidth(ftNormal, S_DEATH)) / 2,
             opengl.GetHeight() - 40,
             S_DEATH
             ); 
      }
   else if (state == gsDeathWait || state == gsGameOver 
            || state == gsFadeToDeath || state == gsFadeToRestart)
      {
         explosion.Draw((float)nViewAdjustX, (float)nViewAdjustY, false);
      }
	
   // Draw the arrows
   for (i = 0; i < nKeys; i++)	{
      if (keys[i].active && !ObjectInScreen(keys[i].xpos, keys[i].ypos + SHIP_START_Y / ObjectGrid::OBJ_GRID_SIZE, 1, 1))	{
         int ax = keys[i].xpos*ObjectGrid::OBJ_GRID_SIZE - nViewAdjustX;
         int ay = keys[i].ypos*ObjectGrid::OBJ_GRID_SIZE + SHIP_START_Y - nViewAdjustY;
         double angle = 0.0;

         if (ax < 0) { 
            ax = 0; 
            angle = 90;
         }
         else if (ax + 32 > opengl.GetWidth()) { 
            ax = opengl.GetWidth() - 32;
            angle = 270;
         }
         if (ay < 0) { 
            ay = 0;
            angle = 180;
         }
         else if (ay + 32 > opengl.GetHeight()) { 
            ay = opengl.GetHeight() - 32;
            angle = 0;
         }

         keys[i].arrow.x = ax;
         keys[i].arrow.y = ay;
			
         opengl.DrawRotate(&keys[i].arrow, (float)angle);
      }
   }

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
   for (i = 0; i < lives; i++)
      {
         smallship.x = 5 + i*30;
         smallship.y = 60;
         if (i == lives-1)
            {
               if (life_alpha > LIFE_ALPHA_BASE)
                  opengl.Draw(&smallship);
               else if (life_alpha < 0.0f)
                  {
                     // Decrement lives
                     lives--;
                     life_alpha = LIFE_ALPHA_BASE + 1.0f;
                  }
               else
                  {
                     opengl.DrawBlend(&smallship, life_alpha);
                     life_alpha -= 0.03f;
                  }
            }
         else
            opengl.Draw(&smallship);
      }

   // Draw key icons
   if (nKeysRemaining)
      {
         offset = (opengl.GetWidth() - MAX_KEYS*32)/2; 
         for (i = 0; i < MAX_KEYS; i++)
            {
               keys[i].frame[5].x = offset + i*32;
               keys[i].frame[5].y = 10;
               if (keys[i].active)
                  opengl.Draw(&keys[i].frame[5]);
               else
                  opengl.DrawBlend(&keys[i].frame[5], keys[i].alpha > 0.3f ? keys[i].alpha : 0.3f);
            }
      }
   else
      {
         offset = (opengl.GetWidth() - MAX_KEYS*32)/2; 
         for (i = 0; i < MAX_KEYS; i++)
            {
               keys[i].frame[5].x = offset + i*32;
               keys[i].frame[5].y = 10;
               if (keys[i].active)
                  opengl.Draw(&keys[i].frame[5]);
               else
                  opengl.DrawBlend(&keys[i].frame[5], keys[i].alpha > 0.0f ? keys[i].alpha : 0.0f);
            }
         opengl.Colour(0.0f, 1.0f, 0.0f);
         ft.Print
            (
             ftNormal,
             (opengl.GetWidth() - ft.GetStringWidth(ftNormal, S_LAND))/2,
             30,
             S_LAND
             );
      }

   // Draw level complete messages
   if (state == gsLevelComplete)
      {
         opengl.Draw(&levcomp);
         opengl.Colour(0.0f, 0.5f, 0.9f);
         ft.Print
            (
             ftBig,
             (opengl.GetWidth() - ft.GetStringWidth(ftBig, S_SCORE) - 40)/2,
             (opengl.GetHeight() - 30)/2 + 50,
             S_SCORE,
             newscore > 0 ? newscore : 0
             );
      }

   // Draw level number text
   if (leveltext_timeout)
      {
         opengl.Colour(0.9f, 0.9f, 0.0f);
         ft.Print
            (
             ftBig,
             (opengl.GetWidth() - ft.GetStringWidth(ftBig, S_LEVEL) - 20)/2,
             (opengl.GetHeight() - 30)/2,
             S_LEVEL,
             level
             );
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


/*
 * Static members of LandingPad.
 */
Texture LandingPad::s_landtex, LandingPad::s_nolandtex;


/*
 * Loads landing pad graphics.
 */
void LandingPad::Load()
{
   OpenGL &opengl = OpenGL::GetInstance();

   s_landtex = opengl.LoadTextureAlpha(g_pData, "LandingPad.bmp");
   s_nolandtex = opengl.LoadTextureAlpha(g_pData, "LandingPadRed.bmp");
} 


/*
 * Draws the landing pad in the current frame.
 *	locked -> If true, pads a drawn with the red texture.
 */
void LandingPad::Draw(int viewadjust_x, int viewadjust_y, int levelheight, bool locked)
{
   quad.uTexture = locked ? s_nolandtex : s_landtex;
   quad.x = index * SURFACE_SIZE - viewadjust_x;
   quad.y = levelheight - viewadjust_y - MAX_SURFACE_HEIGHT + ypos;
   OpenGL::GetInstance().Draw(&quad);
}
 

/*
 * Resets the state of a landing pad.
 */
void LandingPad::Reset(int index, int length)
{
   this->index = index;
   this->length = length;
	
   quad.x = index * SURFACE_SIZE;
   quad.width = length * SURFACE_SIZE;
   quad.height = 16;
   quad.uTexture = s_landtex;
}
