/*
  Button_TT_Collection.cpp - Library for maintaining a collection of buttons.
  Created by Ted Toal, July 5, 2023.
  Released into the public domain.
*/
#include <Arduino.h>
#include <Button_TT_Collection.h>

/**************************************************************************/

void Button_TT_Collection::clear() {
  release();
  numRegisteredButtons = 0;
  for (int i = 0; i < MAX_BUTTONS_IN_COLLECTION; i++) {
    buttons[i] = NULL;
    processPressFuncs[i] = NULL;
  }
  pressedButton = MAX_BUTTONS_IN_COLLECTION;
}

/**************************************************************************/

bool Button_TT_Collection::registerButton(Button_TT_Base& button,
    void (*processPress)(Button_TT_Base& button)) {
  for (int i = 0; i < numRegisteredButtons; i++)
    if (buttons[i] == &button)
      return(true);
  if (numRegisteredButtons == MAX_BUTTONS_IN_COLLECTION)
    return(false);
  buttons[numRegisteredButtons] = &button;
  processPressFuncs[numRegisteredButtons] = processPress;
  numRegisteredButtons++;
  return(true);
}

/**************************************************************************/

bool Button_TT_Collection::unregisterButton(Button_TT_Base& button) {
  for (int i = 0; i < numRegisteredButtons; i++) {
    if (buttons[i] == &button) {
      numRegisteredButtons--;
      while (i < numRegisteredButtons) {
        buttons[i] = buttons[i+1];
        processPressFuncs[i] = processPressFuncs[i+1];
      }
      buttons[i] = NULL;
      processPressFuncs[i] = NULL;
      return(true);
    }
  }
  return(false);
}

/**************************************************************************/

bool Button_TT_Collection::press(int16_t x, int16_t y) {
  for (int i = 0; i < numRegisteredButtons; i++) {
    if (buttons[i]->contains(x, y)) {
      if (!buttons[i]->isPressed()) {
        if (masterPressRelease != NULL)
          masterPressRelease(true);
        pressedButton = i;
        buttons[pressedButton]->press();
        (*processPressFuncs[pressedButton]) (*buttons[pressedButton]);
        return(true);
      }
      return(false);
    }
  }
  return(false);
}

/**************************************************************************/

bool Button_TT_Collection::release() {
  if (masterPressRelease != NULL)
    masterPressRelease(false);
  if (pressedButton == MAX_BUTTONS_IN_COLLECTION)
    return(false);
  buttons[pressedButton]->release();
  pressedButton = MAX_BUTTONS_IN_COLLECTION;
  return(true);
}

// -------------------------------------------------------------------------
