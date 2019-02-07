//  AnimatedImage.hpp -- Images with multiple frames.
//  Copyright (C) 2008-2019  Nick Gasson
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

#pragma once

#include "Image.hpp"

class AnimatedImage {
public:
   AnimatedImage(const string& fileName, int frameWidth,
                 int frameHeight, int frameCount=0);

   void Draw(int x, int y, float rotate=0.0, float scale=1.0,
             float alpha=1.0, float white=1.0) const;
   void DrawFrame(int frame, int x, int y, float rotate=0.0, float scale=1.0,
                  float alpha=1.0, float white=1.0) const;
   void NextFrame();
   void SetFrame(int f);

   int GetFrame() const;
   int GetFrameWidth() const { return frameWidth; }
   int GetFrameHeight() const { return frameHeight; }
private:
   int FramesPerRow() const;
   int FramesPerCol() const;

   int frameWidth, frameHeight, frameCount;
   int currFrame;
   Texture m_texture;
   VertexBuffer m_vbo;
};
