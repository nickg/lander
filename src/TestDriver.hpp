//
// Copyright (C) 2019  Nick Gasson
// SPDX-License-Identifier: GPL-3.0-or-later
//

#pragma once

//
// Allows automation of user actions
//
class TestDriver {
public:
   virtual ~TestDriver() {}

   void Poll();

protected:
   TestDriver();

   void WaitFor(float seconds);
   void AssertScreen(const char *name);

   virtual void Process() {}

private:
   float m_sleep = 0;
};

TestDriver *makeSanityTestDriver();
