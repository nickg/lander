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
#define MAX_SURFACE_HEIGHT	300
#define SHIP_SPEED		0.1f
#define SURFACE_SIZE		20
#define VARIANCE		50
#define AS_VARIANCE		64
#define MAX_PAD_SIZE		3
#define LAND_SPEED		2.0f
#define OBJ_GRID_SIZE		32
#define SHIP_START_Y		100
#define KEY_ROTATION_SPEED	2
#define MINE_ROTATION_SPEED     5
#define GATEWAY_ACTIVE		30
#define MINE_MOVE_SPEED		1
#define FUELBAR_OFFSET		68

/*
 * Constants affecting state transitions.
 */
#define GAME_FADE_IN_SPEED	0.1f	// Rate of alpha change at level start
#define GAME_FADE_OUT_SPEED	0.1f	// Rate of alpha change at level end
#define DEATH_TIMEOUT		50	// Frames to wait for ending level
#define LIFE_ALPHA_BASE		2.0f
#define LEVEL_TEXT_TIMEOUT	75

/* Globals */
extern DataFile *g_pData;


/*
 * Defines a simplified polygon representing the m_ship.
 */
const Point Game::m_hotspots[] = {{1, 31}, {1, 26}, {3, 14}, {15, 0}, 
                                  {28, 14}, {30, 26}, {30, 31}, {16, 31}};


/*
 * Sets the inital state of the Game object.
 */
Game::Game()
   : m_hasloaded(false), bThrusting(false), m_surface(NULL), m_state(gsNone)
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
   if (!m_hasloaded) {
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
      m_hasloaded = true;
   }

   // Create the ship
   m_ship.tq.width = 32;
   m_ship.tq.height = 32;
   m_ship.tq.uTexture = uShipTexture;

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
   speedbar.m_x = 12;
   speedbar.m_y = 40;
   speedbar.m_width = 124;
   speedbar.m_height = 16;
   speedbar.m_red = 1.0f;
   speedbar.m_green = 0.0f;
   speedbar.m_blue = 0.0f;

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
   m_starrotate = 0.0f;
   m_death_timeout = 0;
   m_state = gsNone;

   bDebugMode = false;
}

/* Destructor */
Game::~Game()
{
   if (m_surface)
      delete[] m_surface;
}

/* Starts a new game */
void Game::NewGame()
{
   // Reset score, lives, etc.
   m_score = 0;
   m_lives = 3;

   // Start the game
   m_level = 1;
   m_nextnewlife = 1000;
   StartLevel(m_level);
}

/* Processes events */
void Game::Process()
{
   Input &input = Input::GetInstance();
   OpenGL &opengl = OpenGL::GetInstance();
   int i, k, m;

   // Rotate ship hotspots
   RotatePoints(m_hotspots, m_points, NUM_HOTSPOTS, m_ship.angle*PI/180, -16, 16);

   // Check keys
   if (input.GetKeyState(SDLK_p)) {
      if (m_state == gsPaused) {
         // Unpause the game
         m_state = gsInGame;
         input.ResetKey(SDLK_p);
      }
      else if (m_state == gsInGame) {
         // Pause the game
         m_state = gsPaused;
         input.ResetKey(SDLK_p);
         bThrusting = false;
      }
   }

   // Check for paused state
   if (m_state == gsPaused)
      return;

   if ((input.GetKeyState(SDLK_UP) || input.QueryJoystickButton(1)) 
       && m_fuel > 0 && m_state == gsInGame) {
      // Thrusting
      m_ship.flSpeedX += SHIP_SPEED * (float)sin(m_ship.angle*(PI/180));
      m_ship.flSpeedY -= SHIP_SPEED * (float)cos(m_ship.angle*(PI/180));
      bThrusting = true;
      m_fuel--;
   }
   else
      bThrusting = false;

   if ((input.GetKeyState(SDLK_RIGHT) || input.QueryJoystickAxis(0) > 0) 
       && m_state == gsInGame) {
      // Turn clockwise
      m_ship.angle += 3.0f;
   }
   else if ((input.GetKeyState(SDLK_LEFT) || input.QueryJoystickAxis(0) < 0) 
            && m_state == gsInGame) {
      // Turn anti-clockwise
      m_ship.angle -= 3.0f;
   }

   if (input.GetKeyState(SDLK_SPACE) && m_state == gsExplode) {
      // Skip explosion
      m_state = gsDeathWait; 
      if (m_lives == 0)
         m_death_timeout = DEATH_TIMEOUT;
      else	
         m_death_timeout = 1;
   }

   if (input.GetKeyState(SDLK_ESCAPE) && m_state == gsInGame) {
      // Quit to main menu
      ExplodeShip();
      m_lives = 0;
   }

   if (input.GetKeyState(SDLK_d)) {
      // Toggle mode
      bDebugMode = !bDebugMode;
      input.ResetKey(SDLK_d);
   }

   // Move only if not in game over (prevent bugs)
   if (m_state == gsInGame || m_state == gsExplode) {
      // Apply gravity
      m_ship.flSpeedY += flGravity;
        
      // Move the ship (and exhaust and explosion)
      m_ship.m_xpos += m_ship.flSpeedX;
      m_ship.m_ypos += m_ship.flSpeedY;
      exhaust.m_xpos = m_ship.m_xpos + m_ship.tq.width/2 - (m_ship.tq.width/2)*(float)sin(m_ship.angle*(PI/180));
      exhaust.m_ypos = m_ship.m_ypos + m_ship.tq.height/2 + (m_ship.tq.height/2)*(float)cos(m_ship.angle*(PI/180));
      exhaust.m_yg = m_ship.flSpeedY + (flGravity * 10);
      exhaust.m_xg = m_ship.flSpeedX;
      explosion.m_xpos = m_ship.m_xpos + m_ship.tq.width/2;
      explosion.m_ypos = m_ship.m_ypos + m_ship.tq.height/2;
   }

   // Move mines
   for (i = 0; i < m_minecount; i++) {
      if (mines[i].displace_x%OBJ_GRID_SIZE == 0
          && mines[i].displace_y%OBJ_GRID_SIZE == 0) {
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
            if (timeout < 5 || mines[i].movetimeout == 0)
               {
                  mines[i].dir = rand() % 4;
                  mines[i].movetimeout = 5;
               }
            else
               {
                  mines[i].movetimeout--;
               }

            switch (mines[i].dir)
               {
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
            ok = nextx >= m_objgrid.GetWidth() || nextx < 0 
               || nexty > m_objgrid.GetHeight() || nexty < 0 
               || m_objgrid.IsFilled(nextx, nexty)
               || m_objgrid.IsFilled(nextx + 1, nexty)
               || m_objgrid.IsFilled(nextx + 1, nexty + 1)
               || m_objgrid.IsFilled(nextx, nexty + 1);
            ok = !ok;
            timeout--;
         } while (!ok && timeout > 0);
			
         if (timeout == 0)
            mines[i].dir = NODIR;				
      }

      if (--mines[i].movedelay == 0)
         {
            switch(mines[i].dir)
               {
               case UP: mines[i].displace_y--; break;
               case DOWN: mines[i].displace_y++; break;
               case LEFT: mines[i].displace_x--; break;
               case RIGHT: mines[i].displace_x++; break;
               }
            mines[i].movedelay = MINE_MOVE_SPEED;
         }
   }

   // Check bounds
   if (m_ship.m_xpos <= 0.0f)
      {
         m_ship.m_xpos = 0.0f;
         m_ship.flSpeedX *= -0.5f;
      }
   else if (m_ship.m_xpos + m_ship.tq.width > m_levelwidth)
      {
         m_ship.m_xpos = (float)(m_levelwidth - m_ship.tq.width);
         m_ship.flSpeedX *= -0.5f;
      }
   if (m_ship.m_ypos <= 0.0f)
      {
         m_ship.m_ypos = 0.0f;
         m_ship.flSpeedY *= -0.5f;
      }
   else if (m_ship.m_ypos + m_ship.tq.height > m_levelheight)
      {
         m_ship.m_ypos = (float)(m_levelheight - m_ship.tq.height);
         m_ship.flSpeedY *= -0.5f;

         // Bug fix
         if (m_state == gsExplode)
            {
               m_state = gsDeathWait; 
               m_death_timeout = DEATH_TIMEOUT;
            }
      }

   // Calculate view adjusts
   int centrex = (int)m_ship.m_xpos + (m_ship.tq.width/2);
   int centrey = (int)m_ship.m_ypos + (m_ship.tq.height/2);
   nViewAdjustX = centrex - (opengl.GetWidth()/2);
   nViewAdjustY = centrey - (opengl.GetHeight()/2);
   if (nViewAdjustX < 0)
      nViewAdjustX = 0;
   else if (nViewAdjustX > m_levelwidth - opengl.GetWidth())
      nViewAdjustX = m_levelwidth - opengl.GetWidth();
   if (nViewAdjustY < 0)
      nViewAdjustY = 0;
   else if (nViewAdjustY > m_levelheight - opengl.GetHeight())
      nViewAdjustY = m_levelheight - opengl.GetHeight();

   // Check for collisions with surface
   LineSegment l;
   int lookmin = (int)(m_ship.m_xpos/SURFACE_SIZE) - 2;
   int lookmax = (int)(m_ship.m_xpos/SURFACE_SIZE) + 2;
   if (lookmin < 0)	lookmin = 0;
   if (lookmax >= m_levelwidth/SURFACE_SIZE) lookmax = (m_levelwidth / SURFACE_SIZE) - 1;
   for (i = lookmin; i <= lookmax; i++)
      {
         l.p1.x = i*SURFACE_SIZE;
         l.p1.y = m_levelheight - MAX_SURFACE_HEIGHT + m_surface[i].points[1].y;
         l.p2.x = (i+1)*SURFACE_SIZE;
         l.p2.y = m_levelheight - MAX_SURFACE_HEIGHT + m_surface[i].points[2].y;

         // Look through each hot spot and check for collisions
         if (HotSpotCollision(m_ship, l, m_points, NUM_HOTSPOTS, m_ship.m_xpos, m_ship.m_ypos))
            {
               // Collided - see which game state we're in
               if (m_state == gsInGame)
                  {
                     bool bLanded = false;
                     int nPadOn = -1;

                     // See if this is a landing pad
                     for (k = 0; k < nLandingPads; k++)
                        {
                           for (m = 0; m < pads[k].GetLength(); m++)
                              {
                                 if (pads[k].GetIndex() + m == i)
                                    {
                                       // We landed
                                       int nDAngle = ((int)m_ship.angle) % 360;
                                       if ((nDAngle >= 350 || nDAngle <= 30) 
                                           && m_ship.flSpeedY < LAND_SPEED && !nKeysRemaining)
                                          {
                                             // Landed safely
                                             bLanded = true;
                                             nPadOn = k;
                                             break;
                                          }
                                       else
                                          {
                                             // Crash landed
                                             bLanded = false;
                                             nPadOn = -1;
                                             break;
                                          }
                                    }
                              }
                        }
				
                     if (bLanded)
                        {
                           // Landed - go to next level
                           m_state = gsLevelComplete;
                           m_newscore = (m_level * 100) + 
                              (((MAX_PAD_SIZE+2)-pads[nPadOn].GetLength())*10*m_level);
                           m_countdown_timeout = 70;
                        }
                     else
                        {
                           // Crashed - destroy the ship
                           ExplodeShip();
                           BounceShip();
                        }
                  }
               else if (m_state == gsExplode)
                  {
                     BounceShip();

                     // See if we need to stop the madness
                     if (m_state == gsExplode && m_ship.flSpeedY*-1 < 0.05f)
                        {
                           m_state = gsDeathWait; 
                           m_death_timeout = DEATH_TIMEOUT;
                        }
                  }
            }
      }

   // Check for collisions with asteroids
   LineSegment l1, l2;
   for (i = 0; i < m_asteroidcount; i++)
      {
         if (ObjectInScreen(asteroids[i].GetXPos(), 
                            asteroids[i].GetYPos() + SHIP_START_Y / OBJ_GRID_SIZE, asteroids[i].GetWidth(), 4))
            {
               // Look at polys
               for (k = 0; k < asteroids[i].GetWidth(); k++)
                  {
                     l1 = asteroids[i].GetUpBoundary(k);
                     l2 = asteroids[i].GetDownBoundary(k);

                     if (HotSpotCollision(m_ship, l1, m_points, NUM_HOTSPOTS, m_ship.m_xpos, m_ship.m_ypos) 
                         || HotSpotCollision(m_ship, l2, m_points, NUM_HOTSPOTS, m_ship.m_xpos, m_ship.m_ypos))
                        {
                           // Crashed
                           if (m_state == gsInGame)
                              {
                                 // Destroy the m_ship
                                 ExplodeShip();
                                 BounceShip();
                              }
                           else if (m_state == gsExplode)
                              {
                                 BounceShip();

                                 // See if we need to stop the madness
                                 if (m_state == gsExplode && m_ship.flSpeedY*-1 < 0.05f)
                                    {
                                       m_state = gsDeathWait; 
                                       m_death_timeout = DEATH_TIMEOUT;
                                    }
                              }
                        }
                  }
            }
      }

   // Check for collision with gateways
   for (i = 0; i < m_gatewaycount; i++)
      {
         int dx = gateways[i].vertical ? 0 : gateways[i].length;
         int dy = gateways[i].vertical ? gateways[i].length : 0;
         if (gateways[i].timer > GATEWAY_ACTIVE)
            {
               bool collide1 = BoxCollision
                  (
                   m_ship,
                   gateways[i].xpos*OBJ_GRID_SIZE,
                   gateways[i].ypos*OBJ_GRID_SIZE + SHIP_START_Y,
                   OBJ_GRID_SIZE,
                   OBJ_GRID_SIZE,
                   m_points,
                   NUM_HOTSPOTS
                   );
			
               bool collide2 = BoxCollision
                  (
                   m_ship,
                   (gateways[i].xpos + dx)*OBJ_GRID_SIZE,
                   (gateways[i].ypos + dy)*OBJ_GRID_SIZE + SHIP_START_Y,
                   OBJ_GRID_SIZE,
                   OBJ_GRID_SIZE,
                   m_points, 
                   NUM_HOTSPOTS
                   );
		
               if (collide1 || collide2)
                  {
                     if (m_state == gsInGame)
                        {
                           // Destroy the ship
                           ExplodeShip();
                           BounceShip();
                        }
                     else if (m_state == gsExplode)
                        {
                           BounceShip();

                           // See if we need to stop the madness
                           if (m_state == gsExplode && m_ship.flSpeedY*-1 < 0.05f)
                              {
                                 m_state = gsDeathWait; 
                                 m_death_timeout = DEATH_TIMEOUT;
                              }
                        }
                  }
            }
         else
            {
               bool collide = BoxCollision
                  (
                   m_ship,
                   gateways[i].xpos*OBJ_GRID_SIZE, 
                   gateways[i].ypos*OBJ_GRID_SIZE + SHIP_START_Y,
                   (dx + 1)*OBJ_GRID_SIZE,
                   (dy + 1)*OBJ_GRID_SIZE,
                   m_points,
                   NUM_HOTSPOTS
                   ); 
		
               if (collide)
                  {
                     if (m_state == gsInGame)
                        {
                           // Destroy the ship
                           ExplodeShip();
                           BounceShip();
                        }
                     else if (m_state == gsExplode)
                        {
                           // Destroy the ship anyway
                           m_state = gsDeathWait; 
                           m_death_timeout = DEATH_TIMEOUT;
                        }
                  }
            }
      }

   // Check for collisions with mines
   for (i = 0; i < m_minecount; i++)
      {
         bool collide = BoxCollision
            (
             m_ship,
             mines[i].xpos*OBJ_GRID_SIZE + 3 + mines[i].displace_x,
             mines[i].ypos*OBJ_GRID_SIZE + SHIP_START_Y + 6 + mines[i].displace_y,
             OBJ_GRID_SIZE*2 - 6,
             OBJ_GRID_SIZE*2 - 12,
             m_points,
             NUM_HOTSPOTS
             ); 
	
         if (collide)
            {
               if (m_state == gsInGame)
                  {
                     // Destroy the ship
                     ExplodeShip();
                     BounceShip();
                  }
               else if (m_state == gsExplode)
                  {
                     BounceShip();

                     // See if we need to stop the madness
                     if (m_state == gsExplode && -m_ship.flSpeedY < 0.05f)
                        {
                           m_state = gsDeathWait; 
                           m_death_timeout = DEATH_TIMEOUT;
                        }
                  }
            }
      }

   // See if the player collected a key
   for (i = 0; i < nKeys; i++)
      {
         bool collide = BoxCollision
            (
             m_ship,
             keys[i].xpos*OBJ_GRID_SIZE + 3,
             keys[i].ypos*OBJ_GRID_SIZE + SHIP_START_Y + 3,
             OBJ_GRID_SIZE - 6,
             OBJ_GRID_SIZE - 6,
             m_points, 
             NUM_HOTSPOTS
             );	
	
         if (keys[i].active && collide)
            {
               nKeysRemaining--;
               keys[i].active = false;
               m_objgrid.UnlockSpace(keys[i].xpos, keys[i].ypos);
            }
      }

   // Entry / exit states
   if (m_state == gsDeathWait)
      {
         if (--m_death_timeout == 0)
            {
               // Fade out
               if (m_lives == 0  || (m_lives == 1 && m_life_alpha < LIFE_ALPHA_BASE))
                  {
                     m_state = gsFadeToDeath;
                     m_fade_alpha = LIFE_ALPHA_BASE + 1.0f;
                  }
               else if (m_lives > 0)
                  {
                     if (m_life_alpha < LIFE_ALPHA_BASE)
                        {
                           m_life_alpha = LIFE_ALPHA_BASE + 1.0f;
                           m_lives--;
                        }
				
                     m_state = gsFadeToRestart;
                     m_fade_alpha = 0.0f;
                  }
            }
      }
   else if (m_state == gsGameOver)
      {
         if (--m_death_timeout == 0)
            {
               // Fade out
               m_state = gsFadeToDeath;
               m_fade_alpha = 0.0f;
            }
      }
   else if (m_state == gsFadeIn)
      {
         // Fade in
         m_fade_alpha -= GAME_FADE_IN_SPEED;
         if (m_fade_alpha < 0.0f)
            m_state = gsInGame;
      }
   else if (m_state == gsFadeToRestart)
      {
         // Fade out
         m_fade_alpha += GAME_FADE_OUT_SPEED;
         if (m_fade_alpha > 1.0f)
            {
               // Restart the level
               StartLevel(m_level);
               opengl.SkipDisplay();
            }
      }
   else if (m_state == gsFadeToDeath)
      {
         m_fade_alpha += GAME_FADE_OUT_SPEED;
         if (m_fade_alpha > 1.0f)
            {
               // Return to main menu
               ScreenManager &sm = ScreenManager::GetInstance();
               HighScores *hs = static_cast<HighScores*>(sm.GetScreenById("HIGH SCORES"));
               hs->CheckScore(m_score);
            }
      }
   else if (m_state == gsLevelComplete)
      {
         // Decrease the displayed score
         if (m_countdown_timeout > 0)
            m_countdown_timeout--;
         else if (m_levelcomp_timeout > 0)
            {
               if (--m_levelcomp_timeout == 0)
                  {
                     m_level++;
                     m_state = gsFadeToRestart;
                  }
            }
         else
            {
               int dec = m_level * 2;

               // Decrease the score
               m_newscore -= dec;
               m_score += dec;

               if (m_score > m_nextnewlife)
                  {
                     m_lives++;
                     m_nextnewlife *= 2;
                  }

               if (m_newscore < 0)
                  {
                     // Move to the next level (after a 1s pause)
                     m_score -= -m_newscore;
                     m_levelcomp_timeout = 40;
                  }
            }
      }
	
   // Decrease level text timeout
   if (m_leveltext_timeout > 0)
      m_leveltext_timeout--;

   // Spin the ship if we're exploding
   if (m_state == gsExplode)
      m_ship.angle += 5.0f;

   // Resize the speed bar
   float flSpeed1 = 30.0f / LAND_SPEED;
   int width = (int)((float)m_ship.flSpeedY * flSpeed1); 
   if (width < 0) 
      width = 0;
   if (width > 124) 
      width = 124;
   speedbar.m_blue = 0.0f;
   speedbar.m_red = (float)width/124.0f;
   speedbar.m_green = 1.0f - (float)width/124.0f;
   speedbar.m_width = width;
}

/* Starts a level */
void Game::StartLevel(int m_level)
{
   int i, change, texloop=0, j, k, nPadHere;

   // Set level size
   m_levelwidth = 2000 + 2*SURFACE_SIZE*m_level;
   m_levelheight = 1500 + 2*SURFACE_SIZE*m_level;
   flGravity = 0.045f;

   // Create the object grid
   int grid_w = m_levelwidth / OBJ_GRID_SIZE;
   int grid_h = (m_levelheight - SHIP_START_Y - MAX_SURFACE_HEIGHT - 100) / OBJ_GRID_SIZE;
   m_objgrid.Reset(grid_w, grid_h);

   // Create background stars
   nStarCount = (m_levelwidth * m_levelheight) / 10000;
   if (nStarCount > MAX_GAME_STARS)
      nStarCount = MAX_GAME_STARS;
   for (i = 0; i < nStarCount; i++)
      {
         stars[i].xpos = (int)(rand()%(m_levelwidth/20))*20;
         stars[i].ypos = (int)(rand()%(m_levelheight/20))*20;
         stars[i].quad.uTexture = uStarTexture;
         stars[i].quad.width = stars[i].quad.height = rand()%15;
      }

   // Create the planet surface 
   if (m_surface)
      delete[] m_surface;
   m_surface = new Poly[m_levelwidth/SURFACE_SIZE];
	
   // Generate landing pads
   nLandingPads = rand()%MAX_PADS + 1;
   for (int i = 0; i < nLandingPads; i++)
      {
         int index, length;
         bool overlap;
         do
            {
               index = rand() % (m_levelwidth / SURFACE_SIZE);
               length = rand() % MAX_PAD_SIZE + 3;

               // Check for overlap
               overlap = false;
               if (index + length > (m_levelwidth / SURFACE_SIZE))
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
   for (i = 0; i < m_levelwidth/SURFACE_SIZE; i++)
      {
         m_surface[i].pointcount = 4;
         m_surface[i].xpos = i * SURFACE_SIZE;
         m_surface[i].ypos = m_levelheight - MAX_SURFACE_HEIGHT;
         m_surface[i].uTexture = uSurfaceTexture[surftex];
         m_surface[i].texX = ((float)texloop)/10;
         if (texloop++ == 10)
            texloop = 0;
         m_surface[i].texwidth = 0.1f;

         m_surface[i].points[0].x = 0;
         m_surface[i].points[0].y = MAX_SURFACE_HEIGHT;
		
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
                  change = m_surface[i-1].points[2].y;
               else
                  change = rand()%MAX_SURFACE_HEIGHT;
               m_surface[i].points[1].x = 0;
               m_surface[i].points[1].y = change;
			
               do
                  change = m_surface[i].points[1].y + (rand()%VARIANCE-(VARIANCE/2));
               while (change > MAX_SURFACE_HEIGHT || change < 0);
               m_surface[i].points[2].x = SURFACE_SIZE;
               m_surface[i].points[2].y = change;
            }
         else
            {
               // Make flat terrain for landing pad
               if (i != 0)
                  change = m_surface[i-1].points[2].y;
               else
                  change = rand()%MAX_SURFACE_HEIGHT;
               m_surface[i].points[1].x = 0;
               m_surface[i].points[1].y = change;
               m_surface[i].points[2].x = SURFACE_SIZE;
               m_surface[i].points[2].y = change;

               pads[nPadHere].SetYPos(change);
            }

         m_surface[i].points[3].x = SURFACE_SIZE;
         m_surface[i].points[3].y = MAX_SURFACE_HEIGHT;
      }

   // Create the keys (must be created first because no success check is made on AllocFreeSpace call)
   nKeys = (m_level / 2) + (m_level % 2);
   if (nKeys > MAX_KEYS)
      nKeys = MAX_KEYS;
   nKeysRemaining = nKeys;
   for (i = 0; i < MAX_KEYS; i++)
      {
         if (i < nKeys)
            {		
               m_objgrid.AllocFreeSpace(keys[i].xpos, keys[i].ypos);
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
               keys[i].frame[j].width = OBJ_GRID_SIZE;
               keys[i].frame[j].height = OBJ_GRID_SIZE;

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
   m_asteroidcount = m_level*2 + rand()%(m_level+3);
   if (m_asteroidcount > MAX_ASTEROIDS)
      m_asteroidcount = MAX_ASTEROIDS;
   for (i = 0; i < m_asteroidcount; i++)
      {
         // Allocate space, check for timeout
         int x, y, width = rand() % (Asteroid::MAX_ASTEROID_WIDTH - 4) + 4;
         if (!m_objgrid.AllocFreeSpace(x, y, width, 4))
            {
               // Failed to allocate space so don't make any more asteroids
               m_asteroidcount = i + 1;
               break;
            }

         // Generate the asteroid
         asteroids[i].ConstructAsteroid(x, y, width, uSurf2Texture[surftex]);			
      }

   // Create gateways
   m_gatewaycount = m_level/2 + rand()%(m_level);
   if (m_gatewaycount > MAX_GATEWAYS)
      m_gatewaycount = MAX_GATEWAYS;
   for (i = 0; i < m_gatewaycount; i++)
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
            result = m_objgrid.AllocFreeSpace(gateways[i].xpos, gateways[i].ypos, 1, gateways[i].length+1);
         else
            result = m_objgrid.AllocFreeSpace(gateways[i].xpos, gateways[i].ypos, gateways[i].length+1, 1);
         if (!result)
            {
               // Failed to allocate space so don't make any more gateways
               m_gatewaycount = i + 1;
               break;
            }

         // Set texture, etc.
         gateways[i].icon.width = OBJ_GRID_SIZE;
         gateways[i].icon.height = OBJ_GRID_SIZE;
         gateways[i].icon.uTexture = uGatewayTexture;
         gateways[i].timer = rand() % 70 + 10;
      }

   // Create mines (MUST BE CREATED LAST)
   m_minecount = m_level/2 + rand()%m_level;
   if (m_minecount > MAX_MINES)
      m_minecount = MAX_MINES;
   for (i = 0; i < m_minecount; i++)
      {
         // Allocate space for mine
         if (!m_objgrid.AllocFreeSpace(mines[i].xpos, mines[i].ypos, 2, 2))
            {
               // Failed to allocate space
               m_minecount = i + 1;
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
               mines[i].frame[j].width = OBJ_GRID_SIZE*2;
               mines[i].frame[j].height = OBJ_GRID_SIZE*2;
               mines[i].frame[j].uTexture = uMineTexture[j];
            }

         // Free space on object grid
         m_objgrid.UnlockSpace(mines[i].xpos, mines[i].ypos);
         m_objgrid.UnlockSpace(mines[i].xpos + 1, mines[i].ypos);
         m_objgrid.UnlockSpace(mines[i].xpos + 1, mines[i].ypos + 1);
         m_objgrid.UnlockSpace(mines[i].xpos, mines[i].ypos + 1);
      }

   // Set ship starting position
   m_ship.m_xpos = (float)m_levelwidth/2;
   m_ship.m_ypos = SHIP_START_Y - 40;

   // Reset data
   m_ship.angle = 0.0f;
   m_ship.flSpeedX = 0.0f;
   m_ship.flSpeedY = 0.0f;
   m_leveltext_timeout = LEVEL_TEXT_TIMEOUT;

   // Reset emitters
   exhaust.Reset();
   explosion.Reset();

   // Set fuel
   m_fuel = m_maxfuel = 750 + 50*m_level;

   // Start the game
   m_levelcomp_timeout = 0;
   m_state = gsFadeIn;
   m_fade_alpha = 1.0f;
   m_life_alpha = LIFE_ALPHA_BASE + 1.0f;
}

/* 
 * Destroys the ship after a collision.
 */
void Game::ExplodeShip()
{
   // Set the game state
   m_state = gsExplode;

   // Decrement lives
   m_life_alpha = LIFE_ALPHA_BASE - 1.0f;
}


/*
 * Bounces a ship after an impact with a m_surface.
 */
void Game::BounceShip()
{
   m_ship.flSpeedY *= -1;
   m_ship.flSpeedX *= -1;
   m_ship.flSpeedX /= 2;
   m_ship.flSpeedY /= 2;
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
   return PointInScreen(xpos * OBJ_GRID_SIZE, ypos * OBJ_GRID_SIZE,
                        width * OBJ_GRID_SIZE, height * OBJ_GRID_SIZE);
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

   return HotSpotCollision(a, l1, points, nPoints, a.m_xpos, a.m_ypos) ||
      HotSpotCollision(a, l2, points, nPoints, a.m_xpos, a.m_ypos) ||
      HotSpotCollision(a, l3, points, nPoints, a.m_xpos, a.m_ypos) ||
      HotSpotCollision(a, l4, points, nPoints, a.m_xpos, a.m_ypos);
}

/* Checks for collision between a vector and a line segment */
bool Game::CheckCollision(ActiveObject &a, LineSegment &l, float xpos, float ypos)
{
   if (xpos == -1 || ypos == -1)
      {
         xpos = a.m_xpos;
         ypos = a.m_ypos;
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
         opengl.DrawRotate(&stars[i].quad, m_starrotate);
         m_starrotate += 0.005f;
      }

   // Draw the planet surface
   for (i = 0; i < m_levelwidth/SURFACE_SIZE; i++)
      {
         m_surface[i].xpos = i*SURFACE_SIZE - nViewAdjustX;
         m_surface[i].ypos = m_levelheight - nViewAdjustY - MAX_SURFACE_HEIGHT;
         opengl.Draw(&m_surface[i]);
      }

   // Draw the asteroids
   for (i = 0; i < m_asteroidcount; i++)
      {
         if (ObjectInScreen(asteroids[i].GetXPos(), asteroids[i].GetYPos() + SHIP_START_Y / OBJ_GRID_SIZE, 
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
               keys[i].frame[keys[i].current].x = keys[i].xpos*OBJ_GRID_SIZE - nViewAdjustX;
               keys[i].frame[keys[i].current].y = keys[i].ypos*OBJ_GRID_SIZE - nViewAdjustY + SHIP_START_Y;		
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
                     keys[i].frame[keys[i].current].x = keys[i].xpos*OBJ_GRID_SIZE - nViewAdjustX;
                     keys[i].frame[keys[i].current].y = keys[i].ypos*OBJ_GRID_SIZE - nViewAdjustY + SHIP_START_Y;	
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
   for (i = 0; i < m_gatewaycount; i++)
      {
         // Draw first sphere
         gateways[i].icon.x = gateways[i].xpos*OBJ_GRID_SIZE - nViewAdjustX;
         gateways[i].icon.y = gateways[i].ypos*OBJ_GRID_SIZE + SHIP_START_Y - nViewAdjustY;
         opengl.Draw(&gateways[i].icon);

         // Draw second sphere
         if (gateways[i].vertical)
            {
               gateways[i].icon.x = gateways[i].xpos*OBJ_GRID_SIZE - nViewAdjustX;
               gateways[i].icon.y = (gateways[i].ypos+gateways[i].length)*OBJ_GRID_SIZE + SHIP_START_Y - nViewAdjustY;
            }
         else
            {
               gateways[i].icon.x = (gateways[i].xpos+gateways[i].length)*OBJ_GRID_SIZE - nViewAdjustX;
               gateways[i].icon.y = gateways[i].ypos*OBJ_GRID_SIZE + SHIP_START_Y - nViewAdjustY;
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
                                 x = gateways[i].xpos*OBJ_GRID_SIZE + 16 + deviation - nViewAdjustX;
                                 y = (gateways[i].ypos+k)*OBJ_GRID_SIZE + SHIP_START_Y + 16 - nViewAdjustY;
                                 glVertex2i(x, y);
                                 if (k == gateways[i].length-1)
                                    deviation = 0;
                                 else
                                    deviation += rand()%20 - 10;
                                 x = gateways[i].xpos*OBJ_GRID_SIZE + 16 + deviation - nViewAdjustX;
                                 y += OBJ_GRID_SIZE;
                                 glVertex2i(x, y);
                              }
                           else
                              {
                                 x = (gateways[i].xpos+k)*OBJ_GRID_SIZE + 16 - nViewAdjustX;
                                 y = gateways[i].ypos*OBJ_GRID_SIZE + SHIP_START_Y + 16 + deviation - nViewAdjustY;
                                 glVertex2i(x, y);
                                 if (k == gateways[i].length-1)
                                    deviation = 0;
                                 else
                                    deviation += rand()%20 - 10;
                                 y = gateways[i].ypos*OBJ_GRID_SIZE + SHIP_START_Y + 16 + deviation - nViewAdjustY;
                                 x += OBJ_GRID_SIZE;
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
   for (i = 0; i < m_minecount; i++)
      {
         mines[i].frame[mines[i].current].x = mines[i].xpos*OBJ_GRID_SIZE + mines[i].displace_x - nViewAdjustX;
         mines[i].frame[mines[i].current].y = mines[i].ypos*OBJ_GRID_SIZE + mines[i].displace_y - nViewAdjustY + SHIP_START_Y;		
         opengl.Draw(&mines[i].frame[mines[i].current]);
         if (--mines[i].rotcount == 0)
            {
               if (++mines[i].current == 18)
                  mines[i].current = 0;
               mines[i].rotcount = MINE_ROTATION_SPEED;
            }
      }

   // ***DEBUG***
   if (bDebugMode) {
      // Draw red squares around no-go areas
      int x, y;
      opengl.DisableTexture();
      opengl.EnableBlending();
      opengl.Colour(1.0f, 0.0f, 0.0f, 0.4f);
      for (x = 0; x < m_objgrid.GetWidth(); x++) {
         for (y = 0; y < m_objgrid.GetHeight(); y++) {
            if (m_objgrid.IsFilled(x, y)) {
               glLoadIdentity();
               glBegin(GL_QUADS);
               glVertex2i(x*OBJ_GRID_SIZE - nViewAdjustX, y*OBJ_GRID_SIZE - nViewAdjustY + SHIP_START_Y);
               glVertex2i((x+1)*OBJ_GRID_SIZE - nViewAdjustX, y*OBJ_GRID_SIZE - nViewAdjustY + SHIP_START_Y);
               glVertex2i((x+1)*OBJ_GRID_SIZE - nViewAdjustX, (y+1)*OBJ_GRID_SIZE - nViewAdjustY + SHIP_START_Y);
               glVertex2i(x*OBJ_GRID_SIZE - nViewAdjustX, (y+1)*OBJ_GRID_SIZE - nViewAdjustY + SHIP_START_Y);
               glEnd();
            }
         }
      }
   }

   // Draw the landing pads
   for (i = 0; i < nLandingPads; i++) {
      pads[i].Draw(nViewAdjustX, nViewAdjustY, m_levelheight, nKeysRemaining > 0);
   }

   // Draw the m_ship
   m_ship.tq.x = (int)m_ship.m_xpos - nViewAdjustX;
   m_ship.tq.y = (int)m_ship.m_ypos - nViewAdjustY;
   if (bThrusting) {
      if (sqrt(m_ship.flSpeedX*m_ship.flSpeedX + m_ship.flSpeedY*m_ship.flSpeedY) > 2.0f) {
         exhaust.NewCluster
            ((int)(exhaust.m_xpos + (exhaust.m_xpos - xlast)/2), 
             (int)(exhaust.m_ypos + (exhaust.m_ypos - ylast)/2));
      }
      exhaust.Draw((float)nViewAdjustX, (float)nViewAdjustY, true);
   }
   else if (m_state == gsPaused)
      exhaust.Draw((float)nViewAdjustX, (float)nViewAdjustY, false, false);
   else
      exhaust.Draw((float)nViewAdjustX, (float)nViewAdjustY, false);
	
   if (m_state != gsDeathWait && m_state != gsGameOver
       && m_state != gsFadeToDeath && m_state != gsFadeToRestart) {
      opengl.DrawRotate(&m_ship.tq, m_ship.angle);
   }
	
   xlast = exhaust.m_xpos;
   ylast = exhaust.m_ypos;

   // Draw the explosion if necessary
   if (m_state == gsExplode) {
      explosion.Draw((float)nViewAdjustX, (float)nViewAdjustY, true);
      opengl.Colour(0.0f, 1.0f, 0.0f);
      ft.Print
         (ftNormal,
          (opengl.GetWidth() - ft.GetStringWidth(ftNormal, S_DEATH)) / 2,
          opengl.GetHeight() - 40,
          S_DEATH); 
   }
   else if (m_state == gsDeathWait || m_state == gsGameOver 
            || m_state == gsFadeToDeath || m_state == gsFadeToRestart) {
      explosion.Draw((float)nViewAdjustX, (float)nViewAdjustY, false);
   }
	
   // Draw the arrows
   for (i = 0; i < nKeys; i++) {
      if (keys[i].active && !ObjectInScreen(keys[i].xpos, keys[i].ypos + SHIP_START_Y / OBJ_GRID_SIZE, 1, 1)) {
         int ax = keys[i].xpos*OBJ_GRID_SIZE - nViewAdjustX;
         int ay = keys[i].ypos*OBJ_GRID_SIZE + SHIP_START_Y - nViewAdjustY;
         double angle;
         
         if (ax < 0)
            { ax = 0; angle = 90; }
         if (ax + 32 > opengl.GetWidth())
            { ax = opengl.GetWidth() - 32; angle = 270; }
         if (ay < 0)
            { ay = 0; angle = 180; }
         if (ay + 32 > opengl.GetHeight())
            { ay = opengl.GetHeight() - 32; angle = 0; }
         
         keys[i].arrow.x = ax;
         keys[i].arrow.y = ay;
         
         opengl.DrawRotate(&keys[i].arrow, (float)angle);
      }
   }

   // Draw fuel bar
   int fbsize = (int)(((float)m_fuel/(float)m_maxfuel)*(256-FUELBAR_OFFSET)); 
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
   ft.Print(ftScore, 10, 10, "%.7d", m_score);
   ft.Print(ftNormal, opengl.GetWidth()-70, 10, S_FUEL);
   opengl.Draw(&speedbar);
   opengl.Draw(&speedmeter);
   opengl.Draw(&fuelmeter);
   opengl.Colour(0.0f, 1.0f, 0.0f);

   // Draw life icons
   for (i = 0; i < m_lives; i++) {
      smallship.x = 5 + i*30;
      smallship.y = 60;
      if (i == m_lives - 1) {
         if (m_life_alpha > LIFE_ALPHA_BASE)
            opengl.Draw(&smallship);
         else if (m_life_alpha < 0.0f) {
            // Decrement lives
            m_lives--;
            m_life_alpha = LIFE_ALPHA_BASE + 1.0f;
         }
         else {
            opengl.DrawBlend(&smallship, m_life_alpha);
            m_life_alpha -= 0.03f;
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
   if (m_state == gsLevelComplete) {
      opengl.Draw(&levcomp);
      opengl.Colour(0.0f, 0.5f, 0.9f);
      ft.Print
         (
          ftBig,
          (opengl.GetWidth() - ft.GetStringWidth(ftBig, S_SCORE) - 40)/2,
          (opengl.GetHeight() - 30)/2 + 50,
          S_SCORE,
          m_newscore > 0 ? m_newscore : 0
          );
   }

   // Draw level number text
   if (m_leveltext_timeout) {
      opengl.Colour(0.9f, 0.9f, 0.0f);
      ft.Print(ftBig,
               (opengl.GetWidth() - ft.GetStringWidth(ftBig, S_LEVEL) - 20)/2,
               (opengl.GetHeight() - 30)/2,
               S_LEVEL,
               m_level);
   }

   // Draw the fade
   if (m_state == gsFadeIn || m_state == gsFadeToDeath || m_state == gsFadeToRestart)
      opengl.DrawBlend(&fade, m_fade_alpha);

   // Draw game over message
   if (m_lives == 0 || (m_lives == 1 && m_life_alpha < LIFE_ALPHA_BASE))
      opengl.Draw(&gameover);

   // Draw paused message
   if (m_state == gsPaused)
      opengl.Draw(&paused);
}


/*
 * Creates a new object grid.
 */
ObjectGrid::ObjectGrid()
   : m_grid(NULL), m_width(0), m_height(0)
{
	
}


/*
 * Frees memory used by the object grid.
 */
ObjectGrid::~ObjectGrid()
{
   if (m_grid)
      delete[] m_grid;
}


/* 
 * Allocates a free space in the object grid.
 *	x, y -> Output x, y, co-ordinates.
 * Returns falce if area could not be allocated.
 */
bool ObjectGrid::AllocFreeSpace(int &x, int &y)
{
   int timeout = 10000;
	
   // Keep generating points until we find a free space
   do {
      if (--timeout == 0)
         return false;
        
      x = rand() % m_width;
      y = rand() % m_height;
   } while (m_grid[x + (y * m_width)]);
	
   m_grid[x + (y * m_width)] = true;
	
   return true;
}


/* 
 * Allocates a free space in the object grid.
 *	x, y -> Output x, y co-ordinates.
 *	width, height -> Size of desired space.
 * Returns false if area could not be allocated.
 */
bool ObjectGrid::AllocFreeSpace(int &x, int &y, int width, int height)
{
   bool isOk;
   int counter_x, counter_y;
   int timeout = 10000;
	
   // Keep generating points until we find a free space
   do {
      if (--timeout == 0)
         return false;
        
      x = rand() % (m_width - width);
      y = rand() % (m_height - height);
        
      // Check this position
      isOk = true;
      for (counter_x = x; counter_x < x + width; counter_x++) {
         for (counter_y = y; counter_y < y + height; counter_y++) {
            if (m_grid[counter_x + (counter_y * m_width)])
               isOk = false;
         }
      }
   } while (!isOk);
	
   for (counter_x = x; counter_x < x + width; counter_x++) {
      for (counter_y = y; counter_y < y + height; counter_y++)
         m_grid[counter_x + (counter_y * m_width)] = true;
   }
	
   return true;
}


/*
 * Marks the square at (x, y) as no longer in use.
 */
void ObjectGrid::UnlockSpace(int x, int y)
{
   m_grid[x + (y * m_width)] = false;
}


/*
 * Creates a new blank object grid.
 */
void ObjectGrid::Reset(int width, int height)
{
   assert(width > 0);
   assert(height > 0);

   if (m_grid)
      delete[] m_grid;
		
   m_width = width;
   m_height = height;
	
   m_grid = new bool[m_width * m_height];
	
   memset(m_grid, 0, m_width * m_height * sizeof(bool));
}


/*
 * Returns true if there is an object at the specified co-ordinates.
 */
bool ObjectGrid::IsFilled(int x, int y) const
{
   return m_grid[x + (m_width * y)];
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
   m_quad.uTexture = locked ? s_nolandtex : s_landtex;
   m_quad.x = m_index * SURFACE_SIZE - viewadjust_x;
   m_quad.y = levelheight - viewadjust_y - MAX_SURFACE_HEIGHT + m_ypos;
   OpenGL::GetInstance().Draw(&m_quad);
}
 

/*
 * Resets the state of a landing pad.
 */
void LandingPad::Reset(int index, int length)
{
   m_index = index;
   m_length = length;
	
   m_quad.x = m_index * SURFACE_SIZE;
   m_quad.width = m_length * SURFACE_SIZE;
   m_quad.height = 16;
   m_quad.uTexture = s_landtex;
}
 

/*
 * Generates a random asteroid.
 */
void Asteroid::ConstructAsteroid(int x, int y, int width, Texture texture)
{
   m_xpos = x;
   m_ypos = y;
   m_width = width;
   m_height = 4;

   int change, texloop=0;

   // Build up Polys
   for (int i = 0; i < m_width; i++) {
      // Set Poly parameters
      m_uppolys[i].texwidth = 0.1f;
      m_uppolys[i].texX = ((float)texloop)/10;
      if (texloop++ == 10)
         texloop = 0;
      m_uppolys[i].pointcount = 4;
      m_uppolys[i].uTexture = texture;

      // Lower left vertex
      m_uppolys[i].points[0].x = i * OBJ_GRID_SIZE;
      m_uppolys[i].points[0].y = 2 * OBJ_GRID_SIZE;

      // Upper left vertex
      m_uppolys[i].points[1].x = i * OBJ_GRID_SIZE;
      if (i == 0)
         m_uppolys[i].points[1].y = rand() % (2 * OBJ_GRID_SIZE);
      else
         m_uppolys[i].points[1].y = m_uppolys[i - 1].points[2].y;

      // Upper right vertex
      m_uppolys[i].points[2].x = (i + 1) * OBJ_GRID_SIZE;
      do
         change = m_uppolys[i].points[1].y + (rand() % AS_VARIANCE) - (AS_VARIANCE / 2);
      while (change < 0 || change > 2 * OBJ_GRID_SIZE);
      m_uppolys[i].points[2].y = change;

      // Lower right vertex
      m_uppolys[i].points[3].x = (i + 1) * OBJ_GRID_SIZE;
      m_uppolys[i].points[3].y = 2 * OBJ_GRID_SIZE;
   }

   // Taper last poly
   m_uppolys[m_width - 1].points[2].y = 2 * OBJ_GRID_SIZE;
   m_uppolys[0].points[1].y = 2 * OBJ_GRID_SIZE;

   // Build down Polys
   texloop = 0;
   for (int i = 0; i < m_width; i++) {
      // Set Poly parameters
      m_downpolys[i].texwidth = 0.1f;
      m_downpolys[i].texX = ((float)texloop) / 10;
      if (texloop++ == 10)
         texloop = 0;
      m_downpolys[i].pointcount = 4;
      m_downpolys[i].uTexture = texture;

      // Upper left vertex
      m_downpolys[i].points[0].x = i * OBJ_GRID_SIZE;
      m_downpolys[i].points[0].y = 0;
        
      // Lower left vertex
      m_downpolys[i].points[1].x = i * OBJ_GRID_SIZE;
      if (i == 0)
         m_downpolys[i].points[1].y = rand() % (2 * OBJ_GRID_SIZE);
      else
         m_downpolys[i].points[1].y = m_downpolys[i - 1].points[2].y;
        
      // Lower right vertex
      m_downpolys[i].points[2].x = (i + 1) * OBJ_GRID_SIZE;
      do {
         change = m_downpolys[i].points[1].y 
            + (rand() % AS_VARIANCE) 
            - (AS_VARIANCE / 2);
      } while (change < 0 || change > 2 * OBJ_GRID_SIZE);
      m_downpolys[i].points[2].y = change;
        
      // Upper right vertex
      m_downpolys[i].points[3].x = (i + 1) * OBJ_GRID_SIZE;
      m_downpolys[i].points[3].y = 0;
   }

   // Taper last poly
   m_downpolys[m_width-1].points[2].y = 0;
   m_downpolys[0].points[1].y = 0;
}


/*
 * Returns the line segment which defines the top of the given Poly.
 */
LineSegment Asteroid::GetUpBoundary(int poly)
{
   return LineSegment
      (
       m_xpos*OBJ_GRID_SIZE + m_uppolys[poly].points[1].x,
       m_ypos*OBJ_GRID_SIZE + m_uppolys[poly].points[1].y + SHIP_START_Y,
       m_xpos*OBJ_GRID_SIZE + m_uppolys[poly].points[2].x,
       m_ypos*OBJ_GRID_SIZE + m_uppolys[poly].points[2].y + SHIP_START_Y
       );
}


/*
 * Returns the line segment which defines the bottom of the given Poly.
 */
LineSegment Asteroid::GetDownBoundary(int poly)
{
   return LineSegment
      (
       m_xpos*OBJ_GRID_SIZE + m_downpolys[poly].points[1].x,
       (m_ypos+2)*OBJ_GRID_SIZE + m_downpolys[poly].points[1].y + SHIP_START_Y,
       m_xpos*OBJ_GRID_SIZE + m_downpolys[poly].points[2].x,
       (m_ypos+2)*OBJ_GRID_SIZE + m_downpolys[poly].points[2].y + SHIP_START_Y
       );
}


/*
 * Draws an asteroid on the display.
 */
void Asteroid::Draw(int viewadjust_x, int viewadjust_y)
{
   OpenGL &opengl = OpenGL::GetInstance();

   for (int i = 0; i < m_width; i++)
      {
         // Up
         m_uppolys[i].xpos = m_xpos*OBJ_GRID_SIZE - viewadjust_x;
         m_uppolys[i].ypos = m_ypos*OBJ_GRID_SIZE - viewadjust_y + SHIP_START_Y;
         opengl.Draw(&m_uppolys[i]);

         // Down
         m_downpolys[i].xpos = m_xpos*OBJ_GRID_SIZE - viewadjust_x;
         m_downpolys[i].ypos = (m_ypos+2)*OBJ_GRID_SIZE - viewadjust_y + SHIP_START_Y;
         opengl.Draw(&m_downpolys[i]);
      }
} 

 
