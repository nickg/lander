//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "AnimatedImage.hpp"

#include <iostream>
#include <cassert>
#include <stdexcept>

AnimatedImage::AnimatedImage(const string& fileName, int frameWidth,
                             int frameHeight, int frameCount)
   : frameWidth(frameWidth),
     frameHeight(frameHeight),
     frameCount(frameCount),
     currFrame(0),
     m_texture(Texture::Load(fileName))
{
   if (frameCount == 0) {
      if (m_texture.GetWidth() % frameWidth != 0) {
         cerr << "Warning: " << fileName << " with frame width " << frameWidth
              << " does not have whole number of frames" << endl;
      }
      if (m_texture.GetHeight() % frameHeight != 0) {
         cerr << "Warning: " << fileName << " with frame height " << frameHeight
              << " does not have whole number of frames" << endl;
      }
      this->frameCount = FramesPerRow() * FramesPerCol();
   }

   VertexI *vertexBuf = new VertexI[4 * frameCount];

   const int texWidth = m_texture.GetWidth();
   const int texHeight = m_texture.GetHeight();

   for (int i = 0; i < frameCount; i++) {
      int frameX = i % FramesPerRow();
      int frameY = i / FramesPerRow();

      float tex_l = ((float)(frameX * frameWidth))/(float)texWidth;
      float tex_r = tex_l + (float)frameWidth/(float)texWidth;

      float tex_t = ((float)(frameY * frameHeight))/(float)texHeight;
      float tex_b = tex_t + (float)frameHeight/(float)texHeight;

      const VertexI vertices[4] = {
         { -(frameWidth/2), -(frameHeight/2), tex_l, tex_t },
         { -(frameWidth/2), frameHeight/2, tex_l, tex_b },
         { frameWidth/2, frameHeight/2, tex_r, tex_b },
         { frameWidth/2, -frameHeight/2, tex_r, tex_t }
      };

      copy(vertices, vertices + 4, vertexBuf + i*4);
   }

   m_vbo = VertexBuffer::Make(vertexBuf, 4 * frameCount);

   delete[] vertexBuf;
}

// Draw a particular frame
void AnimatedImage::DrawFrame(int frame, int x, int y, float rotate,
                              float scale, float alpha, float white) const
{
   assert(frame >= 0 && frame < frameCount);

   OpenGL& opengl = OpenGL::GetInstance();

   opengl.SetTexture(m_texture);
   opengl.SetScale(scale);
   opengl.SetColour(white, white, white, alpha);
   opengl.SetRotation(rotate);
   opengl.SetTranslation(x + frameWidth/2, y + frameHeight/2);

   opengl.Draw(m_vbo, frame * 4, 4);
}

// Draw the current frame
void AnimatedImage::Draw(int x, int y, float rotate, float scale,
                         float alpha, float white) const
{
   DrawFrame(currFrame, x, y, rotate, scale, alpha, white);
}

int AnimatedImage::FramesPerRow() const
{
   return m_texture.GetWidth() / frameWidth;
}

int AnimatedImage::FramesPerCol() const
{
   return m_texture.GetHeight() / frameHeight;
}

void AnimatedImage::NextFrame()
{
   currFrame = (currFrame + 1) % frameCount;
}

void AnimatedImage::SetFrame(int f)
{
   if (f < 0 || f >= frameCount)
      Die("SetFrame frame out of range");
   else
      currFrame = f;
}

int AnimatedImage::GetFrame() const
{
   return currFrame;
}
