//
//  ElectricGate.cpp -- Electric gateway thingys.
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

#include "ElectricGate.hpp"
#include "Ship.hpp"

ElectricGate::ElectricGate(Viewport* v, int length, bool vertical, int x, int y)
   : StaticObject(x, y), length(length), vertical(vertical), viewport(v),
     gateImage("images/gateway.png")
{
   lightning.Build(length * OBJ_GRID_SIZE, vertical);
   
   timer = rand() % 70 + 10;
}

bool ElectricGate::CheckCollision(Ship& ship)
{
   int dx = vertical ? 0 : length;
   int dy = vertical ? length : 0;
   if (timer > GATEWAY_ACTIVE) {
      bool collide1 = ship.BoxCollision
         (xpos*OBJ_GRID_SIZE,
          ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP,
          OBJ_GRID_SIZE,
          OBJ_GRID_SIZE);
			
      bool collide2 = ship.BoxCollision
         ((xpos + dx)*OBJ_GRID_SIZE,
          (ypos + dy)*OBJ_GRID_SIZE + OBJ_GRID_TOP,
          OBJ_GRID_SIZE,
          OBJ_GRID_SIZE);
		
      return collide1 || collide2;
   }
   else {
      return ship.BoxCollision
         (xpos*OBJ_GRID_SIZE, 
          ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP,
          (dx + 1)*OBJ_GRID_SIZE,
          (dy + 1)*OBJ_GRID_SIZE); 
   }
}

void ElectricGate::Draw()
{
   // Draw first sphere
   int draw_x = xpos*OBJ_GRID_SIZE - viewport->GetXAdjust();
   int draw_y = ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP - viewport->GetYAdjust();
   gateImage.Draw(draw_x, draw_y);

   // Draw second sphere
   if (vertical) {
      draw_x = xpos*OBJ_GRID_SIZE - viewport->GetXAdjust();
      draw_y = (ypos+length)*OBJ_GRID_SIZE + OBJ_GRID_TOP - viewport->GetYAdjust();
   }
   else {
      draw_x = (xpos+length)*OBJ_GRID_SIZE - viewport->GetXAdjust();
      draw_y = ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP - viewport->GetYAdjust();
   }
   gateImage.Draw(draw_x, draw_y);

   // Draw the electricity stuff
   if (--timer < GATEWAY_ACTIVE) { 
      double x = xpos*OBJ_GRID_SIZE + 16 - viewport->GetXAdjust();
      double y = ypos*OBJ_GRID_SIZE + OBJ_GRID_TOP + 16 - viewport->GetYAdjust();

      glLoadIdentity();
      glTranslated(x, y, 0.0);
      lightning.Draw();

      if (timer % 5 == 0)
         lightning.Build(length * OBJ_GRID_SIZE, vertical);
            
      // Reset timer 
      if (timer < 0)
         timer = 100;
   }
}

void Lightning::Build(int length, bool vertical)
{
   line.SwapXandY(vertical);
   line.Clear();

   const int POINT_STEP = 20;
   int npoints = (length / POINT_STEP) + 1;
   double delta = (double)length / (double)(npoints - 1);

   const double SWING_SIZE = 5;
   const double MAX_OUT = 25;
   double y = 0;
   for (int i = 0; i < npoints; i++) {
      if (i == npoints - 1)
         y = 0;
      
      line.AddPoint(i*delta, y);

      double swing = rand() % 2 == 0 ? -1 : 1;
      y += swing * SWING_SIZE * (double)(rand() % 4);
      if (y > MAX_OUT)
         y = MAX_OUT - swing * SWING_SIZE;
      else if (y < -MAX_OUT)
         y = -MAX_OUT + swing * SWING_SIZE;
   }
}

void Lightning::Draw() const
{
   glDisable(GL_TEXTURE_2D);
   glEnable(GL_BLEND);
   
   line.Draw();
}

void LightLineStrip::AddPoint(double x, double y)
{   
   if (swapXandY)
      points.push_back(Point_t(y, x));
   else
      points.push_back(Point_t(x, y));
}

void LightLineStrip::Draw() const
{
   DrawWithOffset(0, 1, 1, 1, 1);

   DrawWithOffset(1, 0.8, 0.8, 1, 0.8);
   DrawWithOffset(-1, 0.8, 0.8, 1, 0.8);
   
   DrawWithOffset(2, 0.6, 0.6, 1, 0.6);
   DrawWithOffset(-2, 0.6, 0.6, 1, 0.6);
   
   DrawWithOffset(3, 0.4, 0.4, 1, 0.4);
   DrawWithOffset(-3, 0.4, 0.4, 1, 0.4);
   
   DrawWithOffset(4, 0.2, 0.2, 1, 0.2);
   DrawWithOffset(-4, 0.2, 0.2, 1, 0.2);
}

void LightLineStrip::DrawWithOffset(double off, double r, double g, double b,
                                    double a) const
{
   double y_off = swapXandY ? 0 : off;
   double x_off = swapXandY ? off : 0;

   glColor4d(r, g, b, a);
   glBegin(GL_LINE_STRIP);
   
   list<Point_t>::const_iterator it;
   for (it = points.begin(); it != points.end(); ++it)
      glVertex2d((*it).first + x_off, (*it).second + y_off);
   
   glEnd();
}

