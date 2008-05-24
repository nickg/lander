/*  AnimatedImage.cpp -- Images with multiple frames.
 *  Copyright (C) 2008  Nick Gasson
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "AnimatedImage.hpp"

AnimatedImage::AnimatedImage(const char *file, int frameWidth, int frameCount)
   : Image(file), frameWidth(frameWidth), frameCount(frameCount)
{
   if (frameCount == 0) {
      if (width % frameWidth != 0) {
         cerr << "Warning: " << file << " with frame width " << frameWidth
              << " does not have whole number of frames" << endl;
      }
      this->frameCount = width / frameWidth;
   }
   currFrame = 0;
}

void AnimatedImage::Draw(int x, int y, double rotate, double scale,
                         double alpha, double white) const
{
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_BLEND);
   glBindTexture(GL_TEXTURE_2D, texture);
   glLoadIdentity();
   glTranslated((double)(x + frameWidth/2), (double)(y + height/2), 0.0);
   glScaled(scale, scale, 0);
   glRotated(rotate, 0.0, 0.0, 1.0);
   glColor4d(white, white, white, alpha);

   double tex_l = ((double)(currFrame * frameWidth))/(double)width;
   double tex_r = tex_l + (double)frameWidth/(double)width;
   
   glBegin(GL_QUADS);
   glTexCoord2d(tex_l, 0.0); glVertex2i(-(frameWidth/2), -(height/2));
   glTexCoord2d(tex_l, 1.0); glVertex2i(-(frameWidth/2), height/2);
   glTexCoord2d(tex_r, 1.0); glVertex2i(frameWidth/2, height/2);
   glTexCoord2d(tex_r, 0.0); glVertex2i(frameWidth/2, -(height/2));
   glEnd();
}

void AnimatedImage::NextFrame()
{
   currFrame = (currFrame + 1) % frameCount;
}

void AnimatedImage::SetFrame(int f)
{
   if (f >= frameCount)
      throw new runtime_error("SetFrame frame out of range");
   else
      currFrame = f;
}
