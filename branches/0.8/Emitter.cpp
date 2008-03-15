/*
 * Emitter.cpp - Implementation of particle emitter classes.
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

#include "Emitter.hpp"

extern DataFile *g_pData;

/*
 * Creates a new particle emitter.
 *	x, y -> Starting position.
 *	r, g, b -> Base colour components.
 *	createnew -> New particles will be created on initialisation.
 *	deviation -> Randomness in particle colours.
 *	xg, yg -> X and Y components of acceleration.
 *  life -> Average life of each particle.
 *	max_speed -> Maximum speed of all particles.
 *	size -> Size of particles.
 *	slowdown -> Rate of speed decrease.
 */
Emitter::Emitter(int x, int y, float r, float g, float b, bool createnew, float deviation, float xg, float yg,
		float life, float max_speed, float size, float slowdown)
: m_partsize(size), m_r(r), m_g(g), m_b(b), m_deviation(deviation), m_xg(xg), m_yg(yg), m_life(life), 
	m_maxspeed(max_speed), m_xpos((float)x), m_ypos((float)y), m_slowdown(slowdown), m_createrate(64.0f), 
	uTexture(-1)
{
	// Set up the particles
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		if (createnew)
			NewParticle(i);
		else
		{
			particle[i].active = false;
			particle[i].life = -1.0f;
		}
	}
}


/* 
 * Resets the emitter.
 */
void Emitter::Reset()
{
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		particle[i].active = false;
		particle[i].life = -1.0f;
	}	
}


/* 
 * Creates a new cluster of particles at (x, y).
 */
void Emitter::NewCluster(int x, int y)
{
	int i, created=0;
	float oldx, oldy;

	oldx = m_xpos;
	oldy = m_ypos;
	m_xpos = (float)x;
	m_ypos = (float)y;

	for (i = 0; i < MAX_PARTICLES; i++)
	{
		if ((particle[i].life < 0.0f || !particle[i].active) && created < MAX_PARTICLES/m_createrate)
		{
			NewParticle(i);
			created++;
		}
	}

	m_xpos = oldx;
	m_ypos = oldy;
}


/* 
 * Draws all the particles created by this emitter.
 */
void Emitter::Draw(float adjust_x, float adjust_y, bool createnew, bool evolve)
{
	int i, created = 0;
	OpenGL &opengl = OpenGL::GetInstance();

	if (uTexture == OpenGL::INVALID_TEXTURE)
		uTexture = opengl.LoadTextureAlpha(g_pData, "Particle.bmp");

	opengl.EnableTexture();
	opengl.EnableBlending();
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glLoadIdentity();

	for (i = 0; i < MAX_PARTICLES; i++)
	{
		if (particle[i].active)
		{
			float x = particle[i].x - adjust_x;
			float y = particle[i].y - adjust_y;

			glColor4f(particle[i].r, particle[i].g, particle[i].b, particle[i].life);
			glBindTexture(GL_TEXTURE_2D, uTexture);
			glBegin(GL_TRIANGLE_STRIP);	
				glTexCoord2d(1, 1); glVertex3f(x+m_partsize, y+m_partsize, 0);
				glTexCoord2d(0, 1); glVertex3f(x-m_partsize, y+m_partsize, 0);
				glTexCoord2d(1, 0); glVertex3f(x+m_partsize, y-m_partsize, 0);
				glTexCoord2d(0, 0); glVertex3f(x-m_partsize, y-m_partsize, 0); 
			glEnd();

			if (evolve)
			{
				// Move particle
				particle[i].x += particle[i].xi/(m_slowdown*1000);
				particle[i].y += particle[i].yi/(m_slowdown*1000);

				// Apply gravity
				particle[i].xi+=particle[i].xg;
				particle[i].yi+=particle[i].yg;

				// Fade particle
				particle[i].life -= particle[i].fade;

				// Apply special effect
				ProcessEffect(i);

				// See if particle died
				if (particle[i].life < 0.0f && createnew && created < MAX_PARTICLES/m_createrate)
				{
					NewParticle(i);
					created++;
				}
				else if (particle[i].life < 0.0f)
					particle[i].active = false;
			}
		}
		else if (createnew && created < MAX_PARTICLES/m_createrate)
		{
			NewParticle(i);
			created++;
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


/* 
 * Creates one new particle at the specified index.
 */
void Emitter::NewParticle(int index)
{
	float d[3];
	int i;

	for (i = 0; i < 3; i++)
	{
		d[i] = (float)(rand()%100) - 50.0f;
		d[i] /= 100.0f; 
		d[i] *= m_deviation;
	}

	particle[index].active = true;
	particle[index].life = m_life;
	particle[index].fade = (float)(rand()%100)/1000.0f+0.003f;	
	particle[index].r = m_r + d[0] >= 1.0f ? 1.0f : m_r + d[0];
	particle[index].g = m_g + d[1] >= 1.0f ? 1.0f : m_g + d[1];
	particle[index].b = m_b + d[2] >= 1.0f ? 1.0f : m_b + d[2];
	particle[index].x = m_xpos;
	particle[index].y = m_ypos;
	particle[index].xg = m_xg;
	particle[index].yg = m_yg;
	
	do
	{
		particle[index].xi = (float)((rand()%50)-26.0f)*m_maxspeed;
		particle[index].yi = (float)((rand()%50)-25.0f)*m_maxspeed;
	} while (pow(particle[index].yi, 2) + pow(particle[index].xi, 2) > pow(25.0f*m_maxspeed, 2));
}


/* 
 * Smoke trail constructor. Sets special Emitter constants.
 */
SmokeTrail::SmokeTrail()
: Emitter(0, 0, 0.9f, 0.9f, 0.0f, false, 0.2f, 0.0f, 0.5f, 0.5f, 15.0f, 2.0f, 2.0f)
{
	
}


/* 
 * Processes the smoke trail effect for particle p.
 */
void SmokeTrail::ProcessEffect(int p)
{
	if (particle[p].g > 0.5f)
		particle[p].g -= 0.025f;
	else
	{
		if (particle[p].r > 0.1f)
			particle[p].r -= 0.02f;
		if (particle[p].b < 0.1f)
			particle[p].b += 0.03f;
		if (particle[p].g > 0.1f)
			particle[p].g -= 0.03f;
	}
}


/* 
 * Explosion constructor. Sets Emitter constants to create a pretty explosion.
 */
Explosion::Explosion()
: Emitter(0, 0, 0.7f, 0.7f, 0.0f, false, 0.3f, 0.0f, 0.0f, 1.0f, 120.0f, 8.5f, 2.0f) 
{
	// Make a BIG explosion
	m_createrate = 20.0f;
}


/* 
 * Processes the explosion effect for particle p.
 */
void Explosion::ProcessEffect(int p)
{
	if (particle[p].g > 0.5f)
		particle[p].g -= 0.025f;
	else
	{
		if (particle[p].r > 0.1f)
			particle[p].r -= 0.025f;
		if (particle[p].b < 0.1f)
			particle[p].b += 0.025f;
		if (particle[p].g > 0.1f)
			particle[p].g -= 0.025f;
	}
}
