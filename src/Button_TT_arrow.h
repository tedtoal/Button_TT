/*
  Button_TT_arrow.h - Class for displaying an unlabelled triangular button, that
  could be used as an arrow button indicating "increment" or "decrement".
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

  This class can be used to display a triangular button containing no text, and
  it could be used as an arrow button whose meaning when clicked would be to
  "increment" or "decrement" a value.

  Typical usage would be to define a variable of this type, giving the _name
  argument to the constructor, then in an initialization function call the
  initButton() function to assign it to a display object and provide its size,
  position, orientation, color, and other information. Call drawButton() to draw
  it. To handle user button clicks, use class Button_TT_collection. A click
  handler can, for example, call the valueIncDec() function of a Button_TT_int8
  type of button.
*/
#ifndef Button_TT_arrow_h
#define Button_TT_arrow_h

#include <Arduino.h>
#include <Button_TT.h>

/**************************************************************************/
/*!
  @brief  A class that enhances class Button_TT by changing the button shape
          to a triangle, which can be used as an arrow, e.g. up/down for
          incrementing and decrementing a value.
*/
/**************************************************************************/
class Button_TT_arrow : public Button_TT {

protected:

  // Orientation of triangle: U=UP, D=DOWN, L=LEFT, R=RIGHT.
  char _orient;

  // Lengths of triangle sides, base side is _s1, lateral sides are _s2.
  uint16_t _s1, _s2;

public:
  /**************************************************************************/
  /*!
    @brief    Constructor.
    @param    name      String giving a name to the button, for debugging
                        purposes only!
    @param    (others)  Remaining (optional) arguments are the same as
                        initButton() below.
  */
  /**************************************************************************/
  Button_TT_arrow(const char* name, Adafruit_GFX* gfx = 0, char orient = 'U',
      const char* align = "C", int16_t x = 0, int16_t y = 0, uint16_t s1 = 0,
      uint16_t s2 = 0, uint16_t outlineColor = 0, uint16_t fillColor = 0,
      uint8_t expU = 0, uint8_t expD = 0, uint8_t expL = 0, uint8_t expR = 0)
      : Button_TT(name) {

    initButton(gfx, orient, align, x, y, s1, s2, outlineColor, fillColor, expU,
      expD, expL, expR);
  }

  /**************************************************************************/
  /*!
    @brief    Initialize button with our desired color/size/etc. settings.
    @param    gfx     Pointer to our display so we can draw to it!
    @param    orient  Orientation of triangle: U=UP, D=DOWN, L=LEFT, R=RIGHT.
    @param    align   See Button_TT_label::initButton()
    @param    x       See Button_TT_label::initButton()
    @param    y       See Button_TT_label::initButton()
    @param    s1      Length of the base of the isosceles triangle.
    @param    s2      Length of the other two triangle sides.
    @param    outlineColor See Button_TT::initButton()
    @param    fillColor    See Button_TT::initButton()
    @param    expU    See Button_TT::initButton()
    @param    expD    See Button_TT::initButton()
    @param    expL    See Button_TT::initButton()
    @param    expR    See Button_TT::initButton()
  */
  /**************************************************************************/
  void initButton(Adafruit_GFX* gfx = 0, char orient = 'U',
      const char* align = "C", int16_t x = 0, int16_t y = 0, uint16_t s1 = 0,
      uint16_t s2 = 0, uint16_t outlineColor = 0, uint16_t fillColor = 0,
      uint8_t expU = 0, uint8_t expD = 0, uint8_t expL = 0, uint8_t expR = 0);

  /**************************************************************************/
  /*!
    @brief    Return the button's orientation setting (orient argument to init).
    @returns  Single character giving orientation, U, D, L, or R.
  */
  /**************************************************************************/
  char getOrientation(void) { return (_orient); }

  /**************************************************************************/
  /*!
    @brief    Draw the button on the screen.
    @param    inverted    Whether to draw with fill/outline swapped to indicate
                          'pressed'.
  */
  /**************************************************************************/
  using Button_TT::drawButton;
  virtual void drawButton(bool inverted) override;
};

#endif // Button_TT_arrow_h
