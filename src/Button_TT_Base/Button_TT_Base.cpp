/*
  Button_TT_Base.cpp - Library for displaying a labelled button on a pixel
  device, and provides a base class from which more sophisticated button classes
  can be derived.

  Created by Adafruit Industries, 2013.
  Updated and enhanced by Ted Toal, July 5, 2023.

  This was created by edits to the Adafruit_GFX_Button class in the library
  Adafruit_GFX_Library.

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
*/

#include <Arduino.h>
#include <Button_TT_Base.h>

/**************************************************************************/

void Button_TT_Base::initButton(Adafruit_GFX *gfx,
                int16_t xL, int16_t yT, uint16_t w, uint16_t h,
                uint16_t outlineColor, uint16_t fillColor,
                uint8_t expU, uint8_t expD, uint8_t expL, uint8_t expR) {
  _gfx = gfx;
  _xL = xL;
  _yT = yT;
  _w = w;
  _h = h;
  _expU = expU;
  _expD = expD;
  _expL = expL;
  _expR = expR;
  _outlinecolor = outlineColor;
  _fillcolor = fillColor;
  _inverted = false;
  _changedSinceLastDrawn = true;
  _isPressed = false;
  _returnedLastAction = true;
  _delta = 0;
}

/**************************************************************************/

bool Button_TT_Base::setOutlineColor(uint16_t outlineColor) {
  if (_outlinecolor != outlineColor) {
    _outlinecolor = outlineColor;
    _changedSinceLastDrawn = true;
    return(true);
  }
  return(false);
}

/**************************************************************************/

bool Button_TT_Base::setFillColor(uint16_t fillColor) {
  if (_fillcolor != fillColor) {
    _fillcolor = fillColor;
    _changedSinceLastDrawn = true;
    return(true);
  }
  return(false);
}

/**************************************************************************/

void Button_TT_Base::drawButton(bool inverted) {
  _inverted = inverted;

  uint16_t fill, outline;
  if (!_inverted) {
    fill = _fillcolor;
    outline = _outlinecolor;
  } else {
    fill = _outlinecolor;
    outline = _fillcolor;
  }

  if (fill != TRANSPARENT_COLOR)
    _gfx->fillRect(_xL, _yT, _w, _h, fill);
  if (outline != TRANSPARENT_COLOR)
    _gfx->drawRect(_xL, _yT, _w, _h, outline);

  _changedSinceLastDrawn = false;
}

/**************************************************************************/

bool Button_TT_Base::drawIfChanged(bool forceDraw) {
  if (_changedSinceLastDrawn || forceDraw) {
    drawButton(_inverted);
    return(true);
  }
  return(false);
}

/**************************************************************************/

bool Button_TT_Base::contains(int16_t x, int16_t y) {
  return ((x >= _xL-_expL) && (x < (int16_t)(_xL+_w+_expR)) &&
          (y >= _yT-_expU) && (y < (int16_t)(_yT+_h+_expD)));
}

// -------------------------------------------------------------------------
