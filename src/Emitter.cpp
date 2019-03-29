//
// Emitter.cpp - Implementation of particle emitter classes.
// Copyright (C) 2006  Nick Gasson
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

#include "Emitter.hpp"
#include "OpenGL.hpp"

#include <cmath>
#include <string>

//
// Creates a new particle emitter.
//	x, y -> Starting position.
//	r, g, b -> Base colour components.
//	createnew -> New particles will be created on initialisation.
//	deviation -> Randomness in particle colours.
//	xg, yg -> X and Y components of acceleration.
//  life -> Average life of each particle.
//	max_speed -> Maximum speed of all particles.
//	size -> Size of particles.
//	slowdown -> Rate of speed decrease.
//
Emitter::Emitter(int x, int y, float r, float g, float b, bool createnew,
                 float deviation, float xg, float yg, float life,
                 float max_speed, float size, float slowdown)
  : partsize(size), r(r), g(g), b(b), deviation(deviation), xg(xg), yg(yg),
    life(life), maxspeed(max_speed), xpos((float)x), ypos((float)y),
    slowdown(slowdown), createrate(128.0f), xi_bias(0.0f), yi_bias(0.0f),
    m_texture(Texture::Load("images/particle.png")),
    m_vbo(VertexBuffer::MakeQuad(partsize, partsize))
{
   // Set up the particles
   for (int i = 0; i < MAX_PARTICLES; i++) {
      if (createnew)
         NewParticle(i);
      else {
         particle[i].active = false;
         particle[i].life = -1.0f;
      }
   }
}


//
// Resets the emitter.
//
void Emitter::Reset()
{
   for (int i = 0; i < MAX_PARTICLES; i++)	{
      particle[i].active = false;
      particle[i].life = -1.0f;
   }
}


//
// Creates a new cluster of particles at (x, y).
//
void Emitter::NewCluster(int x, int y)
{
   int i, created=0;
   float oldx, oldy;

   const OpenGL::TimeScale timeScale = OpenGL::GetInstance().GetTimeScale();

   oldx = xpos;
   oldy = ypos;
   xpos = (float)x;
   ypos = (float)y;

   for (i = 0; i < MAX_PARTICLES; i++)	{
      if ((particle[i].life < 0.0f || !particle[i].active)
          && created < MAX_PARTICLES/(createrate * timeScale)) {
         NewParticle(i);
         created++;
      }
   }

   xpos = oldx;
   ypos = oldy;
}


//
// Draws all the particles created by this emitter.
//
void Emitter::Draw(float adjust_x, float adjust_y) const
{
   OpenGL& opengl = OpenGL::GetInstance();
   opengl.Reset();
   opengl.SetTexture(m_texture);
   opengl.SetBlendFunc(GL_SRC_ALPHA, GL_ONE);

   for (int i = 0; i < MAX_PARTICLES; i++)	{
      if (particle[i].active)	{
         float x = particle[i].x - adjust_x - partsize/2;
         float y = particle[i].y - adjust_y - partsize/2;

         opengl.SetTranslation(x, y);
         opengl.SetColour(particle[i].r, particle[i].g, particle[i].b,
                          particle[i].life);
         opengl.Draw(m_vbo);
      }
   }
}

void Emitter::Process(bool createnew, bool evolve)
{
   const OpenGL::TimeScale timeScale = OpenGL::GetInstance().GetTimeScale();

   int created = 0;
   for (int i = 0; i < MAX_PARTICLES; i++) {
      if (particle[i].active) {
         if (evolve)	{
            // Move particle
            particle[i].x += (particle[i].xi/(slowdown*1000)) * timeScale;
            particle[i].y += (particle[i].yi/(slowdown*1000)) * timeScale;

            // Apply gravity
            particle[i].xi += particle[i].xg * timeScale;
            particle[i].yi += particle[i].yg * timeScale;

            // Fade particle
            particle[i].life -= particle[i].fade * timeScale;

            // Apply special effect
            ProcessEffect(i);

            // See if particle died
            if (particle[i].life < 0.0f
                && createnew
                && created < MAX_PARTICLES/(createrate * timeScale)) {
               NewParticle(i);
               created++;
            }
            else if (particle[i].life < 0.0f)
               particle[i].active = false;
         }
      }
      else if (createnew && created < MAX_PARTICLES/(createrate * timeScale)) {
         NewParticle(i);
         created++;
      }
   }
}

//
// Creates one new particle at the specified index.
//
void Emitter::NewParticle(int index)
{
   float d[3];
   int i;

   for (i = 0; i < 3; i++)	{
      d[i] = (float)(rand()%100) - 50.0f;
      d[i] /= 100.0f;
      d[i] *= deviation;
   }

   particle[index].active = true;
   particle[index].life = life;
   particle[index].fade = (float)(rand()%100)/1000.0f+0.003f;
   particle[index].r = r + d[0] >= 1.0f ? 1.0f : r + d[0];
   particle[index].g = g + d[1] >= 1.0f ? 1.0f : g + d[1];
   particle[index].b = b + d[2] >= 1.0f ? 1.0f : b + d[2];
   particle[index].x = xpos;
   particle[index].y = ypos;
   particle[index].xg = xg;
   particle[index].yg = yg;

   do {
      particle[index].xi = (float)((rand()%50)-26.0f)*maxspeed;
      particle[index].yi = (float)((rand()%50)-25.0f)*maxspeed;
   } while (pow(particle[index].yi, 2) + pow(particle[index].xi, 2) > pow(25.0f*maxspeed, 2));

   const OpenGL::TimeScale timeScale = OpenGL::GetInstance().GetTimeScale();

   particle[index].xi += xi_bias * timeScale;
   particle[index].yi += yi_bias * timeScale;
}


//
// Smoke trail constructor. Sets special Emitter constants.
//
SmokeTrail::SmokeTrail(float r, float g, float b,
                       float dr, float dg, float db)
  : Emitter(0, 0, r, g, b,
            false, 0.2f,
            0.0f, 0.0f,
            0.3f, 0.0f, 4.0f, 0.001f),
    dr(dr), dg(dg), db(db)
{
   createrate = 64.0f;
}

BlueSmokeTrail::BlueSmokeTrail()
   : SmokeTrail(0.6f, 0.6f, 0.9f, 0.03f, 0.03f, 0.02f)
{

}

OrangeSmokeTrail::OrangeSmokeTrail()
   : SmokeTrail(0.9f, 0.6f, 0.6f, 0.02f, 0.03f, 0.03f)
{

}

//
// Processes the smoke trail effect for particle p.
//
void SmokeTrail::ProcessEffect(int p)
{
   const OpenGL::TimeScale timeScale = OpenGL::GetInstance().GetTimeScale();

   if (particle[p].g > 0.5f)
      particle[p].g -= 0.025f * timeScale;
   else {
      if (particle[p].r > 0.1f)
         particle[p].r -= dr * timeScale;
      if (particle[p].b > 0.1f)
         particle[p].b -= db * timeScale;
      if (particle[p].g > 0.1f)
         particle[p].g -= dg * timeScale;
   }
}


//
// Explosion constructor. Sets Emitter constants to create a pretty explosion.
//
Explosion::Explosion()
  : Emitter(0, 0, 0.7f, 0.7f, 0.0f, false, 0.3f, 0.0f, 0.0f, 1.0f, 120.0f, 8.5f, 2.0f)
{
   // Make a BIG explosion
   createrate = 20.0f;
}


//
// Processes the explosion effect for particle p.
//
void Explosion::ProcessEffect(int p)
{
   const OpenGL::TimeScale timeScale = OpenGL::GetInstance().GetTimeScale();

   if (particle[p].g > 0.5f)
      particle[p].g -= 0.025f * timeScale;
   else {
      if (particle[p].r > 0.1f)
         particle[p].r -= 0.025f * timeScale;
      if (particle[p].b < 0.1f)
         particle[p].b += 0.025f * timeScale;
      if (particle[p].g > 0.1f)
         particle[p].g -= 0.025f * timeScale;
   }
}
