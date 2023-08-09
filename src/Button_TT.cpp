/*
  Button_TT.cpp - Defines functions of class Button_TT, which maintains size,
  position, and press-state information for a button object displayed on a pixel
  device, and is a base class from which to derive other button classes.
  This code originated in file Adafruit_GFX.cpp from class Adafruit_GFX_Button.
  It was copied from there and modified, and hence this file retains the
  original Adafruit copyright.

  Created by Ted Toal, July 5, 2023 from Adafruit_GFX.cpp


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
#include <Button_TT.h>

// If debug enabled, include monitor_printf.h for printf to serial monitor.
#if BUTTON_TT_DBG
#include <monitor_printf.h>
#endif

/**************************************************************************/

void Button_TT::initButton(Adafruit_GFX* gfx, const char* align, int16_t x,
    int16_t y, uint16_t w, uint16_t h, uint16_t outlineColor,
    uint16_t fillColor, uint8_t expU, uint8_t expD, uint8_t expL,
    uint8_t expR) {

  _gfx = gfx;
  _w = w;
  _h = h;
  _expU = expU;
  _expD = expD;
  _expL = expL;
  _expR = expR;
  _outlineColor = outlineColor;
  _fillColor = fillColor;
  _inverted = false;
  _changedSinceLastDrawn = true;
  _isPressed = false;
  _returnedLastAction = true;
  _delta = 0;

  // Compute the upper-left coords of the entire button rectangle, (xL, yT),
  // using _align, x, y, w and h.
  if (align[0] == 'C' && align[1] == 0)
    align = "CC";

  _xL = x;
  if (align[1] == 'R')
    _xL += 1 - w;
  else if (align[1] == 'C')
    _xL += 1 - w / 2;

  _yT = y;
  if (align[0] == 'B')
    _yT += 1 - h;
  else if (align[0] == 'C')
    _yT += 1 - h / 2;

  #if BUTTON_TT_DBG
  monitor.printf("  xL: %d  yT: %d\n", _xL, _yT);
  #endif
}

/**************************************************************************/

bool Button_TT::setOutlineColor(uint16_t outlineColor) {
  if (_outlineColor != outlineColor) {
    _outlineColor = outlineColor;
    _changedSinceLastDrawn = true;
    return (true);
  }
  return (false);
}

/**************************************************************************/

bool Button_TT::setFillColor(uint16_t fillColor) {
  if (_fillColor != fillColor) {
    _fillColor = fillColor;
    _changedSinceLastDrawn = true;
    return (true);
  }
  return (false);
}

/**************************************************************************/

void Button_TT::drawButton(bool inverted) {
  _inverted = inverted;

  uint16_t fill, outline;
  if (!_inverted) {
    fill = _fillColor;
    outline = _outlineColor;
  } else {
    fill = _outlineColor;
    outline = _fillColor;
  }

  if (fill != TRANSPARENT_COLOR)
    _gfx->fillRect(_xL, _yT, _w, _h, fill);
  if (outline != TRANSPARENT_COLOR)
    _gfx->drawRect(_xL, _yT, _w, _h, outline);

  _changedSinceLastDrawn = false;
}

/**************************************************************************/

bool Button_TT::drawIfChanged(bool forceDraw) {
  if (_changedSinceLastDrawn || forceDraw) {
    drawButton(_inverted);
    return (true);
  }
  return (false);
}

/**************************************************************************/

bool Button_TT::contains(int16_t x, int16_t y) {
  return ((x >= _xL - _expL) && (x < (int16_t)(_xL + _w + _expR)) &&
          (y >= _yT - _expU) && (y < (int16_t)(_yT + _h + _expD)));
}

// -------------------------------------------------------------------------
