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

AnimatedImage::AnimatedImage(const char* file, int frameWidth, int frameHeight,
                             int frameCount)
   : Image(file), frameWidth(frameWidth), frameHeight(frameHeight),
     frameCount(frameCount)
{
   if (frameCount == 0) {
      if (Image::GetWidth() % frameWidth != 0) {
         cerr << "Warning: " << file << " with frame width " << frameWidth
              << " does not have whole number of frames" << endl;
      }
      if (Image::GetHeight() % frameHeight != 0) {
         cerr << "Warning: " << file << " with frame height " << frameHeight
              << " does not have whole number of frames" << endl;
      }
      this->frameCount = FramesPerRow() * FramesPerCol();
   }
   currFrameX = 0;
   currFrameY = 0;
}

void AnimatedImage::Draw(int x, int y, double rotate, double scale,
                         double alpha, double white) const
{
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

   double tex_l = ((double)(currFrameX * frameWidth))/(double)width;
   double tex_r = tex_l + (double)frameWidth/(double)width;

   double tex_t = ((double)(currFrameY * frameHeight))/(double)height;
   double tex_b = tex_t + (double)frameHeight/(double)height;
   
   glBegin(GL_QUADS);
   glTexCoord2d(tex_l, tex_t); glVertex2i(-(frameWidth/2), -(frameHeight/2));
   glTexCoord2d(tex_l, tex_b); glVertex2i(-(frameWidth/2), frameHeight/2);
   glTexCoord2d(tex_r, tex_b); glVertex2i(frameWidth/2, frameHeight/2);
   glTexCoord2d(tex_r, tex_t); glVertex2i(frameWidth/2, -(frameHeight/2));
   glEnd();
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
   currFrameX = (currFrameX + 1) % FramesPerRow();
   if (currFrameX == 0)
      currFrameY = (currFrameY + 1) % FramesPerCol();
   if (GetFrame() >= frameCount) {
      currFrameX = 0;
      currFrameY = 0;
   }
}

void AnimatedImage::SetFrame(int f)
{   
   if (f >= frameCount)
      throw runtime_error("SetFrame frame out of range");
   else {
      currFrameX = f % FramesPerRow();
      currFrameY = f / FramesPerRow();
   }
}

int AnimatedImage::GetFrame() const
{
   return (currFrameY * FramesPerRow()) + currFrameX;
}

