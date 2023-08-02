/*
  Button_TT_Collection.h - Defines a C++ class for maintaining a collection of
  buttons and providing button hit-testing to test for user button click.
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


  Details:

  Class Button_TT_Collection holds an array of pointers to Button_TT objects.
  It can search them to find which one contains a given point, and it can call a
  function registered along with the button to process the contained point (such
  as when the user clicks or touches the button).
*/
#ifndef Button_TT_Collection_h
#define Button_TT_Collection_h

#include <Arduino.h>
#include <Button_TT.h>

#define MAX_BUTTONS_IN_COLLECTION 20

/**************************************************************************/
/*!
  @brief  A class that holds a collection of pointers to buttons, and scans
          them when requested to find out if one of them contains a given point.
*/
/**************************************************************************/
class Button_TT_Collection {

protected:

  // Number of buttons currently registered in this class.
  uint8_t numRegisteredButtons;

  // Pointers to the registered buttons. When one is deleted, the others are
  // moved down, so 0..numRegisteredButtons-1 contains the button pointers.
  Button_TT *buttons[MAX_BUTTONS_IN_COLLECTION];

  // Pointers to processing functions, same index as buttons[] above.
  void (*processPressFuncs[MAX_BUTTONS_IN_COLLECTION])(Button_TT &button);

  // Index into above arrays when a button is pressed, MAX_BUTTONS_IN_COLLECTION
  // if none. Only one button at a time can be pressed.
  uint8_t pressedButton;

  // Pointer to master button press/release function to call on press or release.
  void (*masterPressRelease)(bool press);

public:
  /**************************************************************************/
  /*!
    @brief    Constructor.
  */
  /**************************************************************************/
  Button_TT_Collection(void (*masterPressReleaseFunc)(bool press) = NULL) {
    numRegisteredButtons = MAX_BUTTONS_IN_COLLECTION;
    pressedButton = MAX_BUTTONS_IN_COLLECTION;
    masterPressRelease = NULL;
    clear();
  }

  /**************************************************************************/
  /*!
    @brief    Clear all currently registered buttons from the collection.
  */
  /**************************************************************************/
  void clear(void);

  /**************************************************************************/
  /*!
    @brief    Register a master button press/release processing function.
    @param    masterPressReleaseFunc  Function to call at each press or release
                                      of a button with argument indicating a
                                      press (true) or release (false), e.g.
                                      this function might make a sound upon
                                      button press, use NULL for none.
  */
  /**************************************************************************/
  void
  registerMasterProcessFunc(void (*masterPressReleaseFunc)(bool press) = NULL) {
    masterPressRelease = masterPressReleaseFunc;
  }

  /**************************************************************************/
  /*!
    @brief    Register a button and its "press processing function".
    @param    button        The button to be registered.
    @param    processPress  The function to call when the button is pressed,
                            with its single argument being a reference to the
                            button object.
    @returns  true if successful or button is already registered, false if
              collection is full.
  */
  /**************************************************************************/
  bool registerButton(Button_TT &button,
                      void (*processPress)(Button_TT &button));

  /**************************************************************************/
  /*!
    @brief    Unregister a previously-registered button.
    @param    button        The button to be unregistered.
    @returns  true if successful, false if button was not previously registered.
  */
  /**************************************************************************/
  bool unregisterButton(Button_TT &button);

  /**************************************************************************/
  /*!
    @brief    Search registered buttons for one containing the point (x,y), and
              if found, call masterPressRelease() if not NULL, call the button's
              press() function, and then call the registered processPress()
              function for that button.
    @param    x             The X coordinate to check.
    @param    y             The Y coordinate to check.
    @returns  false if no button contains (x,y), else true.
  */
  bool press(int16_t x, int16_t y);

  /**************************************************************************/
  /*!
    @brief    Call masterPressRelease() if not NULL, then call the release()
              function for the last button pressed if any.
    @returns  false if no button was previously pressed, else true.
  */
  bool release();
};

#endif // Button_TT_Collection_h
