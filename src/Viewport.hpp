//
// Copyright (C) 2008-2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INC_VIEWPORT_HPP
#define INC_VIEWPORT_HPP

class Viewport {
public:
   Viewport();

   int GetXAdjust() const { return adjustX; }
   int GetYAdjust() const { return adjustY; }
   void SetXAdjust(int x);
   void SetYAdjust(int y);

   int GetLevelWidth() const { return levelWidth; }
   int GetLevelHeight() const { return levelHeight; }
   void SetLevelWidth(int w) { levelWidth = w; }
   void SetLevelHeight(int h) { levelHeight = h; }

   bool ObjectInScreen(int xpos, int ypos, int width, int height);
   bool PointInScreen(int xpos, int ypos, int width, int height);

private:
   int adjustX, adjustY;
   int levelWidth, levelHeight;
};

#endif
