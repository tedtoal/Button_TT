/*
  Button_TT_Collection.cpp - Defines functions of class Button_TT_Collection.
  Created by Ted Toal, July 5, 2023.
  Released into the public domain.


  Software License Agreement (BSD License)

  Copyright (c) 2023 Ted Toal
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
  3. Neither the name of the copyright holders nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <Arduino.h>
#include <Button_TT_Collection.h>

/**************************************************************************/

void Button_TT_Collection::clear() {
  release();
  numRegisteredButtons = 0;
  for (int i = 0; i < MAX_BUTTONS_IN_COLLECTION; i++) {
    buttons[i] = nullptr;
    processPressFuncs[i] = nullptr;
  }
  pressedButton = MAX_BUTTONS_IN_COLLECTION;
}

/**************************************************************************/

bool Button_TT_Collection::registerButton(
    Button_TT &button, void (*processPress)(Button_TT &button)) {
  for (int i = 0; i < numRegisteredButtons; i++)
    if (buttons[i] == &button)
      return (true);
  if (numRegisteredButtons == MAX_BUTTONS_IN_COLLECTION)
    return (false);
  buttons[numRegisteredButtons] = &button;
  processPressFuncs[numRegisteredButtons] = processPress;
  numRegisteredButtons++;
  return (true);
}

/**************************************************************************/

bool Button_TT_Collection::unregisterButton(Button_TT &button) {
  for (int i = 0; i < numRegisteredButtons; i++) {
    if (buttons[i] == &button) {
      numRegisteredButtons--;
      while (i < numRegisteredButtons) {
        buttons[i] = buttons[i + 1];
        processPressFuncs[i] = processPressFuncs[i + 1];
      }
      buttons[i] = nullptr;
      processPressFuncs[i] = nullptr;
      return (true);
    }
  }
  return (false);
}

/**************************************************************************/

bool Button_TT_Collection::press(int16_t x, int16_t y) {
  for (int i = 0; i < numRegisteredButtons; i++) {
    if (buttons[i]->contains(x, y)) {
      if (!buttons[i]->isPressed()) {
        if (masterPressRelease != nullptr)
          masterPressRelease(true);
        pressedButton = i;
        buttons[pressedButton]->press();
        (*processPressFuncs[pressedButton])(*buttons[pressedButton]);
        return (true);
      }
      return (false);
    }
  }
  return (false);
}

/**************************************************************************/

bool Button_TT_Collection::release() {
  if (masterPressRelease != nullptr)
    masterPressRelease(false);
  if (pressedButton == MAX_BUTTONS_IN_COLLECTION)
    return (false);
  buttons[pressedButton]->release();
  pressedButton = MAX_BUTTONS_IN_COLLECTION;
  return (true);
}

// -------------------------------------------------------------------------
