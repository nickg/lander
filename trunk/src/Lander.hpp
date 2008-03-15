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

// Different fonts to be loaded
enum FontType { ftNormal, ftBig, ftScore, ftHollow, ftScoreName, ftLarge };

/* Constants */
enum DIRECTIONS { UP, RIGHT, DOWN, LEFT, NODIR };

/* Macros */
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

/*
 * A single point in space.
 */
struct Point 
{
    int x, y;	
};


/* 
 * A line segment. Used for collision detection.
 */
class LineSegment
{
public:
    LineSegment(int x1, int y1, int x2, int y2)
    { p1.x=x1; p1.y=y1; p2.x=x2; p2.y=y2; }
    LineSegment()
    { p1.x = 0; p1.y = 0; p2.x = 0; p2.y = 0; }

    Point p1, p2;
};


/* An object that can move about the screen */
class ActiveObject
{
public:
    ActiveObject() : xpos(0), ypos(0), flSpeedX(0), flSpeedY(0), angle(0)
    { }
    float xpos, ypos;
    float flSpeedX, flSpeedY, angle;
    TextureQuad tq;
};


/*
 * Grid where objects may be placed.
 */
class ObjectGrid
{
public:
    ObjectGrid();
    ~ObjectGrid();
    
    void Reset(int width, int height);
    bool AllocFreeSpace(int &x, int &y);
    bool AllocFreeSpace(int &x, int &y, int width, int height);
    void UnlockSpace(int x, int y);
	
    bool IsFilled(int x, int y) const;
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
private:
    bool *grid;
    int width, height;
};


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


/*
 * An object on the object grid.
 */
class StaticObject
{
public:
    StaticObject() : xpos(0), ypos(0), width(0), height(0) {}
    
    int GetXPos() const { return xpos; }
    int GetYPos() const { return ypos; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

protected:
    int xpos, ypos, width, height;
};


/*
 * An asteroid floating in space.
 */
class Asteroid : public StaticObject
{
public:
    Asteroid() {}
    ~Asteroid() {}
	
    void ConstructAsteroid(int x, int y, int width, Texture texture);
    void Draw(int viewadjust_x, int viewadjust_y);
    LineSegment GetUpBoundary(int poly);
    LineSegment GetDownBoundary(int poly);

    static const int MAX_ASTEROID_WIDTH = 15;
	
private:
    Poly uppolys[MAX_ASTEROID_WIDTH], downpolys[MAX_ASTEROID_WIDTH];
};

/* Game class */
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
    bool CheckCollision(ActiveObject &a, LineSegment &l, float xpos=-1, float ypos=-1);
    bool HotSpotCollision(ActiveObject &a, LineSegment &l, Point *points, int nPoints, float dx=0, float dy=0);
    bool BoxCollision(ActiveObject &a, int x, int y, int w, int h, Point *points, int nPoints);
    void RotatePoints(const Point *pPoints, Point *pDest, int nCount, float angle, int adjustx=0, int adjusty=0);
    bool ObjectInScreen(int xpos, int ypos, int width, int height);
    bool PointInScreen(int xpos, int ypos, int width, int height);
    void BounceShip();
    void ExplodeShip();

    // Private variables
    ActiveObject ship;
    int death_timeout, level, fuel, maxfuel, lives;
    int nViewAdjustX, nViewAdjustY, levelwidth, levelheight;
    bool hasloaded, bThrusting, bDebugMode;
    float flGravity, starrotate, fade_alpha, life_alpha;
    Explosion explosion;
    SmokeTrail exhaust;
    Poly *surface;
    TextureQuad fade, levcomp, speedmeter, fuelmeter, smallship, gameover, paused;
    ColourQuad speedbar;
    int score, newscore, nextnewlife;
    int countdown_timeout, leveltext_timeout, levelcomp_timeout;

    // Game states
    enum GameState { gsNone, gsInGame, gsExplode, gsGameOver, gsDeathWait, 
                     gsFadeIn, gsFadeToDeath, gsFadeToRestart, gsLevelComplete, gsPaused };
    GameState state;

    ObjectGrid objgrid;
	
    // Number of available _surface textures
    static const int NUM_SURF_TEX = 5;

    // Textures
    GLuint uStarTexture, uShipTexture, uSurfaceTexture[NUM_SURF_TEX], uSurf2Texture[NUM_SURF_TEX], uFadeTexture;
    GLuint uLevComTexture, uSpeedTexture, uBlueKey[18], uRedKey[18], uGreenKey[18], uPinkKey[18], uYellowKey[18];
    GLuint uBlueArrow, uPinkArrow, uRedArrow, uYellowArrow, uGreenArrow;
    GLuint uGatewayTexture, uFuelMeterTexture, uFuelBarTexture, uMineTexture[36], uShipSmallTexture;
    GLuint uGameOver, uPausedTexture;
    
    static const int NUM_HOTSPOTS = 8;
    Point points[NUM_HOTSPOTS];
    static const Point hotspots[];
    
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
