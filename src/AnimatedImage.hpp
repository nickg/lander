//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
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
