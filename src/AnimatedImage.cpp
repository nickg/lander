//  AnimatedImage.cpp -- Images with multiple frames.
//  Copyright (C) 2008  Nick Gasson
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "AnimatedImage.hpp"

#include <iostream>
#include <cassert>
#include <stdexcept>

AnimatedImage::AnimatedImage(const string& fileName, int frameWidth,
                             int frameHeight, int frameCount)
   : Image(fileName), frameWidth(frameWidth), frameHeight(frameHeight),
     frameCount(frameCount), currFrame(0)
{
   if (frameCount == 0) {
      if (Image::GetWidth() % frameWidth != 0) {
         cerr << "Warning: " << fileName << " with frame width " << frameWidth
              << " does not have whole number of frames" << endl;
      }
      if (Image::GetHeight() % frameHeight != 0) {
         cerr << "Warning: " << fileName << " with frame height " << frameHeight
              << " does not have whole number of frames" << endl;
      }
      this->frameCount = FramesPerRow() * FramesPerCol();
   }
}

// Draw a particular frame
void AnimatedImage::DrawFrame(int frame, int x, int y, double rotate, double scale,
                              double alpha, double white) const
{
   assert(frame >= 0 && frame < frameCount);

   int frameX = frame % FramesPerRow();
   int frameY = frame / FramesPerRow();

   int width = Image::GetWidth();
   int height = Image::GetHeight();

   glEnable(GL_TEXTURE_2D);
   glEnable(GL_BLEND);
   glBindTexture(GL_TEXTURE_2D, GetTexture()->GetGLTexture());
   glLoadIdentity();
   glTranslated((double)(x + frameWidth/2), (double)(y + frameHeight/2), 0.0);
   glScaled(scale, scale, 0);
   glRotated(rotate, 0.0, 0.0, 1.0);
   glColor4d(white, white, white, alpha);

   double tex_l = ((double)(frameX * frameWidth))/(double)width;
   double tex_r = tex_l + (double)frameWidth/(double)width;

   double tex_t = ((double)(frameY * frameHeight))/(double)height;
   double tex_b = tex_t + (double)frameHeight/(double)height;

   glBegin(GL_QUADS);
   glTexCoord2d(tex_l, tex_t); glVertex2i(-(frameWidth/2), -(frameHeight/2));
   glTexCoord2d(tex_l, tex_b); glVertex2i(-(frameWidth/2), frameHeight/2);
   glTexCoord2d(tex_r, tex_b); glVertex2i(frameWidth/2, frameHeight/2);
   glTexCoord2d(tex_r, tex_t); glVertex2i(frameWidth/2, -(frameHeight/2));
   glEnd();
}

// Draw the current frame
void AnimatedImage::Draw(int x, int y, double rotate, double scale,
                         double alpha, double white) const
{
   DrawFrame(currFrame, x, y, rotate, scale, alpha, white);
}

int AnimatedImage::FramesPerRow() const
{
   return Image::GetWidth() / frameWidth;
}

int AnimatedImage::FramesPerCol() const
{
   return Image::GetHeight() / frameHeight;
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
