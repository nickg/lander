//
// Copyright (C) 2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include "Platform.hpp"
#include "TestDriver.hpp"
#include "OpenGL.hpp"
#include "Input.hpp"
#include "ScreenManager.hpp"

#include <iostream>

TestDriver::TestDriver()
   : m_sleep(1.0f)
{

}

void TestDriver::Poll()
{
   const float timeScale = OpenGL::GetInstance().GetTimeScale();
   const float delta = timeScale / OpenGL::VIRTUAL_FRAME_RATE;

   if (delta > m_sleep) {
      m_sleep = 0.0f;
      Process();
   }
   else
      m_sleep -= delta;
}

void TestDriver::AssertScreen(const string& id)
{
   Screen *s = ScreenManager::GetInstance().GetActiveScreen();
   if (id != s->GetName())
      Die("[TEST] expected active screen to be %s but is %s",
          id.c_str(), s->GetName());
}

void TestDriver::WaitFor(float seconds)
{
   m_sleep = seconds;
}

////////////////////////////////////////////////////////////////////////////////
// Quick sanity test

class SanityTestDriver : public TestDriver {
protected:
   void Process() override;

private:
   enum State {
      INIT, MENU, THRUST_UP, ESCAPE, HIGH_SCORE, RETURN_MENU,
      DOWN1, DOWN2, DOWN3, QUIT, BAD
   };

   State m_state = INIT;
   int m_thrustCount = 0;
};

void SanityTestDriver::Process()
{
   switch (m_state) {
   case INIT:
      cout << "[TEST] startup" << endl;
      AssertScreen("MAIN MENU");
      m_state = MENU;
      WaitFor(2.0f);
      break;

   case MENU:
      cout << "[TEST] menu" << endl;
      AssertScreen("MAIN MENU");
      Input::GetInstance().FakeAction(Input::FIRE);
      m_state = THRUST_UP;
      WaitFor(1.0f);
      break;

   case THRUST_UP:
      AssertScreen("GAME");
      Input::GetInstance().FakeAction(Input::THRUST);
      if (m_thrustCount++ >= 100) {
         m_state = ESCAPE;
         WaitFor(0.5f);
      }
      break;

   case ESCAPE:
      cout << "[TEST] escape" << endl;
      AssertScreen("GAME");
      Input::GetInstance().FakeAction(Input::ABORT);
      m_state = HIGH_SCORE;
      WaitFor(1.0f);
      break;

   case HIGH_SCORE:
      cout << "[TEST] go to high score" << endl;
      AssertScreen("GAME");
      Input::GetInstance().FakeAction(Input::SKIP);
      m_state = RETURN_MENU;
      WaitFor(2.5f);
      break;

   case RETURN_MENU:
      cout << "[TEST] return to menu" << endl;
      AssertScreen("HIGH SCORES");
      Input::GetInstance().FakeAction(Input::FIRE);
      m_state = DOWN1;
      WaitFor(2.0f);
      break;

   case DOWN1:
      cout << "[TEST] down 1" << endl;
      AssertScreen("MAIN MENU");
      Input::GetInstance().FakeAction(Input::DOWN);
      m_state = DOWN2;
      WaitFor(0.5f);
      break;

   case DOWN2:
      cout << "[TEST] down 2" << endl;
      AssertScreen("MAIN MENU");
      Input::GetInstance().FakeAction(Input::DOWN);
      m_state = DOWN3;
      WaitFor(0.5f);
      break;

   case DOWN3:
      cout << "[TEST] down 3" << endl;
      AssertScreen("MAIN MENU");
      Input::GetInstance().FakeAction(Input::DOWN);
      m_state = QUIT;
      WaitFor(0.5f);
      break;

   case QUIT:
      cout << "[TEST] quit" << endl;
      AssertScreen("MAIN MENU");
      Input::GetInstance().FakeAction(Input::FIRE);
      WaitFor(5.0f);
      m_state = BAD;
      break;

   case BAD:
      Die("Unexpected test state");
   }
}

TestDriver *makeSanityTestDriver()
{
   return new SanityTestDriver;
}
