//
// Copyright (C) 2006-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INC_EMITTER_HPP
#define INC_EMITTER_HPP

#include "Platform.hpp"
#include "Texture.hpp"
#include "OpenGL.hpp"

#define MAX_PARTICLES 512


//
// A generic particle emitter.
//
class Emitter {
public:
   Emitter(int x, int y, float r, float g, float b, bool createnew=true,
           float deviation=0.0f, float xg=0.0f, float yg=0.0f,
           float life=1.0f, float max_speed=10.0f, float size=2.0f,
           float slowdown=2.0f);
   Emitter(Emitter&&) = default;
   virtual ~Emitter() { }

   void Draw(float adjust_x=0.0f, float adjust_y=0.0f) const;
   void NewCluster(int x, int y);
   void Reset();
   void Process(bool createnew, bool evolve = true);

   virtual void ProcessEffect(int particle) { }

   float partsize, r, g, b, deviation, xg, yg, life, maxspeed, flSize;
   float xpos, ypos, slowdown, createrate;

   float xi_bias, yi_bias;

protected:
   void NewParticle(int index);

   struct Particle {
      bool active;
      float life, fade;
      float r, g, b;
      float x, y;
      float xi, yi;
      float xg, yg;
   } particle[MAX_PARTICLES];

   Texture m_texture;
   VertexBuffer m_vbo;
};


//
// A smoke trail.
//
class SmokeTrail : public Emitter {
public:
   SmokeTrail(float r, float g, float b,
              float dr, float dg, float db);
   SmokeTrail(SmokeTrail&&) = default;
   virtual ~SmokeTrail() { }

   void ProcessEffect(int particle);
private:
   const float dr, dg, db;
};

class BlueSmokeTrail : public SmokeTrail {
public:
   BlueSmokeTrail();
};

class OrangeSmokeTrail : public SmokeTrail {
public:
   OrangeSmokeTrail();
};

//
// An explosion.
//
class Explosion : public Emitter {
public:
   Explosion();
   virtual ~Explosion() { }

   void ProcessEffect(int particle);
private:
};

#endif
