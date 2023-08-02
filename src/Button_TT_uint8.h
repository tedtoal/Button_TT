/*
  Button_TT_uint8.h - Defines C++ class Button_TT_uint8 that derives from
  class Button_TT_label to add a variable of type uint8_t to a labelled button.
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

  This class expands on class Button_TT_label by including a uint8_t class
  variable that holds the value that is displayed as the button text. The value
  can be queried and changed, and incremented and decremented, by calling member
  functions. The variable value can be limited to a specific range, and if
  desired a special alternative string can be displayed in the button when the
  value is 0. Also, a user-supplied function can do additional checking on the
  value for validity.
*/
#ifndef Button_TT_uint8_h
#define Button_TT_uint8_h

#include <Arduino.h>
#include <Button_TT_label.h>
#include <Button_TT_arrow.h>

/**************************************************************************/
/*!
  @brief  A class that enhances class Button_TT_label by supplying a uint8_t
          value that provides the button label.
*/
/**************************************************************************/
class Button_TT_uint8 : public Button_TT_label {

protected:

  // The current button value that is displayed, and its minimum and maximum.
  uint8_t _value, _minValue, _maxValue;

  // Pointer to string to display when value is 0, nullptr to just show "0".
  const char *_zeroString;

  // Function to call to check if a new button value is valid.
  uint8_t (*_checkValue)(Button_TT_uint8 &btn, uint8_t value);

public:
  /**************************************************************************/
  /*!
    @brief    Constructor.
    @param    name        String giving a name to the button, for debugging
                          purposes only!
    @param    (others)    Remaining (optional) arguments are the same as
                          initButton() below.
  */
  /**************************************************************************/
  Button_TT_uint8(
      const char *name, Adafruit_GFX *gfx = 0, const char *align = "C",
      int16_t x = 0, int16_t y = 0, int16_t w = 0, int16_t h = 0,
      uint16_t outlineColor = 0, uint16_t fillColor = 0, uint16_t textColor = 0,
      const char *textAlign = "C", Font_TT* f = nullptr, int16_t rCorner = 0,
      uint8_t value = 0, uint8_t minValue = 0, uint8_t maxValue = 0,
      const char *zeroString = nullptr, bool degreeSym = false,
      uint8_t (*checkValue)(Button_TT_uint8 &btn, uint8_t value) = nullptr,
      uint8_t expU = 0, uint8_t expD = 0, uint8_t expL = 0, uint8_t expR = 0)
      : Button_TT_label(name) {

    initButton(gfx, align, x, y, w, h, outlineColor, fillColor, textColor,
               textAlign, f, rCorner, value, minValue, maxValue, zeroString,
               degreeSym, checkValue, expU, expD, expL, expR);
  }

  /**************************************************************************/
  /*!
    @brief    Initialize button with our desired color/size/etc. settings.
    @param    gfx     See Button_TT::initButton()
    @param    align   See Button_TT_label::initButton()
    @param    x       See Button_TT_label::initButton()
    @param    y       See Button_TT_label::initButton()
    @param    w       See Button_TT_label::initButton()
    @param    h       See Button_TT_label::initButton()
    @param    outlineColor  See Button_TT::initButton()
    @param    fillColor     See Button_TT::initButton()
    @param    textColor     See Button_TT_label::initButton()
    @param    textAlign     See Button_TT_label::initButton()
    @param    f             See Button_TT_label::initButton()
    @param    rCorner       See Button_TT_label::initButton()
    @param    value         The uint8_t value for the button, used to create the
                            label string for the button.
    @param    minValue      The minimum allowed value for value.
    @param    maxValue      The maximum allowed value for value.
    @param    zeroString    If not nullptr, this points to a character string to
                            be used as the button label if value is 0.
    @param    degreeSym     If true, a degree symbol is drawn at the end of the
                            label.
    @param    checkValue    If not nullptr, a pointer to a function that checks
                            a new button value and can return an adjusted value
                            if it is out of range.
    @param    expU    See Button_TT::initButton()
    @param    expD    See Button_TT::initButton()
    @param    expL    See Button_TT::initButton()
    @param    expR    See Button_TT::initButton()
  */
  /**************************************************************************/
  void initButton(
      Adafruit_GFX *gfx = 0, const char *align = "C", int16_t x = 0,
      int16_t y = 0, int16_t w = 0, int16_t h = 0, uint16_t outlineColor = 0,
      uint16_t fillColor = 0, uint16_t textColor = 0,
      const char *textAlign = "C", Font_TT* f = nullptr, int16_t rCorner = 0,
      uint8_t value = 0, uint8_t minValue = 0, uint8_t maxValue = 0,
      const char *zeroString = nullptr, bool degreeSym = false,
      uint8_t (*checkValue)(Button_TT_uint8 &btn, uint8_t value) = nullptr,
      uint8_t expU = 0, uint8_t expD = 0, uint8_t expL = 0, uint8_t expR = 0);

  /**************************************************************************/
  /*!
    @brief    Get current button label uint8_t value.
    @returns  The current uint8_t value of the button.
  */
  /**************************************************************************/
  uint8_t getValue(void) { return (_value); }

  /**************************************************************************/
  /*!
    @brief    Set button label uint8_t value, limiting the result to lie between
              minVal and maxVal and checking its value with checkValue function
              if one was supplied, convert the value to a string, and set that
              as the new button label, but don't redraw the button.
    @param    value       The uint8_t value for the button.
    @param    dontCheck   If true, don't call the checkValue function if it is
                          supplied, which may be necessary during initialization
                          when checkValue tests for interaction of two different
                          values, one of which may not yet have been
                          initialized.
    @returns  true if the value is different than before.
  */
  /**************************************************************************/
  bool setValue(uint8_t value, bool dontCheck = false);

  /**************************************************************************/
  /*!
    @brief    Set button label uint8_t value, convert the value to a string and
              set that as the new button label, then draw the button if the
              label changed or if any visible button attribute changed since
              last drawn.
    @param    value       The uint8_t value for the button.
    @param    forceDraw   If true, the button is drawn even if attributes have
                          not changed.
    @returns  true if button was drawn.
  */
  /**************************************************************************/
  bool setValueAndDrawIfChanged(uint8_t value, bool forceDraw = false);

  /**************************************************************************/
  /*!
    @brief    Add N to the button's value, limiting the result to lie between
              minVal and maxVal and using setValue to set the value, ensuring
              the value is fully checked. If btn is not nullptr and btn->delta()
              is not 0, that value is used instead of N. Set the new value as
              the new label for the button and redraw it if the label or visible
              button attribute changed.

    @param    N     Value to add to this button's value, used if btn is nullptr
                    or btn->delta() is 0.
    @param    btn   Pointer to the base class of the button object that was
                    pressed in order to invoke this function, or nullptr if
                    none.
    @returns  true if button was drawn.
  */
  /**************************************************************************/
  bool valueIncDec(int8_t N = 1, Button_TT *btn = nullptr);
};

#endif // Button_TT_uint8_h
