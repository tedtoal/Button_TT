/*
  Button_TT.h - Defines C++ class Button_TT that maintains size, position, and
  press-state information for a button object displayed on a pixel device, and
  is a base class from which to derive other button classes.
  This class originated in file Adafruit_GFX.h as class Adafruit_GFX_Button.
  It was copied from there and modified, and hence this file retains the
  original Adafruit copyright.

  Created by Ted Toal, July 5, 2023 from Adafruit_GFX.h


  Copyright (c) 2013 Adafruit Industries.  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.


  Details:

  This code is a modification of class Adafruit_GFX_Button in the file
  Adafruit_GFX.cpp. That code didn't center the text properly in the button when
  the text is made using a custom font. The class Adafruit_GFX_Button has been
  split into two classes, Button_TT defined here (from which other types of
  button classes can be derived) and Button_TT_label (which fixes the centering
  problem and adds other features).

  This class can be used to display a rectangular button containing no text.
  Typical usage would be to define a variable of this type, giving the _name
  argument to the constructor, then in an initialization function call the
  initButton() function to assign it to a display object and provide its size,
  position, color, and other information. Call drawButton() to draw it. To
  handle user button clicks, use class Button_TT_collection.
*/
#ifndef Button_TT_h
#define Button_TT_h

#include <Arduino.h>
#include <Adafruit_GFX.h>

// Set BUTTON_TT_DBG to 1 to enable debug output via monitor.printf(), 0 if not.
#define BUTTON_TT_DBG 0

// Use this color to avoid having outline, button background, or label drawn.
// Note: The ILI9341 controller actually uses all 16 bits as color info, 5 bits
// for red and blue, and 6 bits for green. It internally maps these to 6 bits
// for each color. Therefore, every 16-bit combination is a valid color. We
// would like to use an invalid value to represent a transparent color, but
// there is no invalid value. So, what we will do is arbitrarily pick a color
// value that is unlikely to be used anywhere. We will choose with the least
// significant bit of the R, G, and B values being 1 and all other bits 0.
const uint16_t TRANSPARENT_COLOR = 0x0841;

/**************************************************************************/
/*!
  @brief  A class that maintains size, position, and press-state information
          for a button object displayed on a pixel device.
*/
/**************************************************************************/
class Button_TT {

protected:

  // Arbitrary button name, to assist with debugging.
  const char* _name;

  // The pixel-based display object used to draw on the display.
  Adafruit_GFX* _gfx;

  // Coordinates of top-left corner of button.
  int16_t _xL, _yT;

  // Width and height of button in pixels.
  uint16_t _w, _h;

  // Number of pixels to expand button bounding box to get coordinates of hit
  // box used to test for user hits on the button using a pointing device or
  // touchscreen.
  uint16_t _expU, _expD, _expL, _expR;

  // Button outline and fill colors.
  uint16_t _outlineColor, _fillColor;

  // Reserved for derived classes to use to change something such as a value
  // displayed inside the button.
  int16_t _delta;

  // true while button colors are inverted.
  bool _inverted;

  // true if a visible attribute changes, cleared when button drawn.
  bool _changedSinceLastDrawn;

  // true if button is pressed.
  bool _isPressed;

  // true if new value of _isPressed has been returned to caller.
  bool _returnedLastAction;

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
  Button_TT(const char* name, Adafruit_GFX* gfx = 0, const char* align = "C",
      int16_t x = 0, int16_t y = 0, uint16_t w = 0, uint16_t h = 0,
      uint16_t outlineColor = 0, uint16_t fillColor = 0, uint8_t expU = 0,
      uint8_t expD = 0, uint8_t expL = 0, uint8_t expR = 0) : _name(name) {

    initButton(gfx, align, x, y, w, h, outlineColor, fillColor, expU, expD,
      expL, expR);
  }

  /**************************************************************************/
  /*!
    @brief    Initialize button.
    @param    gfx       Pointer to display object so we can draw button on it.
    @param    align     (x,y) alignment: TL, TC, TR, CL, CC, CR, BL, BC, BR, C
                        where T=top, B=bottom, L=left, R=right, C=center, C=CC.
    @param    x         The X coordinate of the button, relative to 'align'.
    @param    y         The X coordinate of the button, relative to 'align'.
    @param    w         Width of the button in pixels.
    @param    h         Height of the button in pixels.
    @param    outlineColor Color of the outline (16-bit 5-6-5 standard).
    @param    fillColor    Color of the button fill (16-bit 5-6-5 standard).
    @param    expU      Expand button up by this when contains() tests a point.
    @param    expD      Expand button down by this when contains() tests a point.
    @param    expL      Expand button left by this when contains() tests a point.
    @param    expR      Expand button right by this when contains() tests a point.
  */
  /**************************************************************************/
  void initButton(Adafruit_GFX* gfx = 0, const char* align = "C", int16_t x = 0,
      int16_t y = 0, uint16_t w = 0, uint16_t h = 0, uint16_t outlineColor = 0,
      uint16_t fillColor = 0, uint8_t expU = 0, uint8_t expD = 0,
      uint8_t expL = 0, uint8_t expR = 0);

  /**************************************************************************/
  /*!
    @brief    Get current outline color for button.
    @returns  The current outline color.
  */
  /**************************************************************************/
  uint16_t getOutlineColor(void) { return (_outlineColor); }

  /**************************************************************************/
  /*!
    @brief    Set new outline color for button.
    @param    outlineColor    The new outline color.
    @returns  true if new color is different from old.
  */
  /**************************************************************************/
  bool setOutlineColor(uint16_t outlineColor);

  /**************************************************************************/
  /*!
    @brief    Get current fill color for button.
    @returns  The current fill color.
  */
  /**************************************************************************/
  uint16_t getFillColor(void) { return (_fillColor); }

  /**************************************************************************/
  /*!
    @brief    Set new fill color for button.
    @param    fillColor   The new fill color.
    @returns  true if new color is different from old.
  */
  /**************************************************************************/
  bool setFillColor(uint16_t fillColor);

  /**************************************************************************/
  /*!
    @brief    Get inversion flag for last draw.
    @returns  The last value of "inverted" used to draw the button.
  */
  /**************************************************************************/
  uint16_t getInverted(void) { return (_inverted); }

  /**************************************************************************/
  /*!
    @brief    Draw the button on the screen.
    @param    inverted    Whether to draw with fill/text colors swapped to
                          indicate 'pressed'.
  */
  /**************************************************************************/
  virtual void drawButton(bool inverted);

  /**************************************************************************/
  /*!
    @brief    Draw the button on the screen, using value of "inverted" from last
              call to drawButton().
  */
  /**************************************************************************/
  virtual void drawButton() { drawButton(_inverted); }

  /**************************************************************************/
  /*!
    @brief    If any button attribute has changed since the button was last
              drawn, redraw the button.
    @param    forceDraw   If true, the button is drawn even if attributes have
                          not changed.
    @returns  true if button was drawn.
  */
  /**************************************************************************/
  virtual bool drawIfChanged(bool forceDraw = false);

  /**********************************************************************/
  /*!
    @brief    Sets button to the pressed state and draws it inverted.
  */
  /**********************************************************************/
  void press() {
    if (!_isPressed) {
      _isPressed = true;
      _returnedLastAction = false;
      drawButton(_isPressed);
    }
  }

  /**********************************************************************/
  /*!
    @brief    Sets button to the released state and draws it non-inverted.
  */
  /**********************************************************************/
  void release() {
    if (_isPressed) {
      _isPressed = false;
      _returnedLastAction = false;
      drawButton(_isPressed);
    }
  }

  /**********************************************************************/
  /*!
    @brief    Query whether the button is currently pressed.
    @returns  true if pressed.
  */
  /**********************************************************************/
  bool isPressed(void) { return _isPressed; };

  /**************************************************************************/
  /*!
    @brief    Query whether the button was pressed since we last checked state.
    @returns  true if was not-pressed before, now is.
  */
  /**************************************************************************/
  bool justPressed() {
    if (!_isPressed || _returnedLastAction)
      return (false);
    _returnedLastAction = true;
    return (true);
  }

  /**************************************************************************/
  /*!
    @brief    Query whether the button was released since we last checked state.
    @returns  true if was pressed before, now is not.
  */
  /**************************************************************************/
  bool justReleased() {
    if (_isPressed || _returnedLastAction)
      return (false);
    _returnedLastAction = true;
    return (true);
  }

  /**************************************************************************/
  /*!
    @brief    Test if a coordinate is within the bounds of the button.
    @param    x       The X coordinate to check.
    @param    y       The Y coordinate to check.
    @returns  true if within button graphics outline.
  */
  /**************************************************************************/
  bool contains(int16_t x, int16_t y);

  /**************************************************************************/
  /*!
    @brief    Return a value that is the amount by which to change some other
              value, used for derived classes that act as "increment" or
              "decrement" buttons.
    @returns  Value by which to change another value.
  */
  /**************************************************************************/
  virtual int16_t delta(void) { return (_delta); }
};

#endif // Button_TT_h
