/*
  Button_TT_uint8.cpp - Defines functions of class Button_TT_uint8.
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
#include <Button_TT_uint8.h>

/**************************************************************************/

void Button_TT_uint8::initButton(
    Adafruit_GFX *gfx, const char *align, int16_t x, int16_t y, int16_t w,
    int16_t h, uint16_t outlineColor, uint16_t fillColor, uint16_t textColor,
    const char *textAlign, Font_TT* f, int16_t rCorner, uint8_t value,
    uint8_t minValue, uint8_t maxValue, bool degreeSym, const char *zeroString,
    uint8_t (*checkValue)(Button_TT_uint8 &btn, uint8_t value), uint8_t expU,
    uint8_t expD, uint8_t expL, uint8_t expR) {

  _value = maxValue+1; // Force initial invalid value so setValue() below will change the value AND label string.
  _minValue = minValue;
  _maxValue = maxValue;
  _zeroString = zeroString;
  _checkValue = checkValue;

  // Pre-initialize base class with null values except gfx.
  Button_TT_label::initButton(gfx);
  if (gfx == 0)
    return;
  gfx->setFont(f->getFont()); // Must also set base class font value to use getWidestValue.

  // For the initial value of the label, find the longest value in the range
  // minValue..maxValue and use that. The label might actually be out of that
  // range a bit, but it doesn't matter. We set 'value' as the actual button
  // value (including its label) AFTER calling Button_TT_label::initButton below.
  char widestValStr[12];
  uint16_t wt, ht;
  Button_TT_label::getWidestValue(minValue, maxValue, widestValStr, wt, ht,
    zeroString = _zeroString);
  Button_TT_label::initButton(gfx, align, x, y, w, h, outlineColor, fillColor,
    textColor, textAlign, widestValStr, degreeSym, f, rCorner, expU, expD, expL,
    expR);

  // Now set the correct value, applying limits.
  setValue(value);
}

/**************************************************************************/

bool Button_TT_uint8::setValue(uint8_t value, bool dontCheck) {
  if (value < _minValue)
    value = _minValue;
  if (value > _maxValue)
    value = _maxValue;
  if (!dontCheck && _checkValue != nullptr)
    value = _checkValue(*this, value);
  if (value == _value)
    return (false);
  _value = value;
  if (_value == 0 && _zeroString != nullptr)
    setLabel(_zeroString);
  else {
    char S[6];
    itoa(_value, S, 10);
    setLabel(S);
  }
  _changedSinceLastDrawn = true;
  return (true);
}

/**************************************************************************/

bool Button_TT_uint8::setValueAndDrawIfChanged(uint8_t value, bool forceDraw) {
  setValue(value);
  if (_changedSinceLastDrawn || forceDraw) {
    drawButton();
    return (true);
  }
  return (false);
}

/**************************************************************************/

bool Button_TT_uint8::valueIncDec(int8_t N, Button_TT *btn) {
  if (btn != nullptr && (uint8_t)btn->delta() != 0)
    N = (uint8_t)btn->delta();
  uint8_t newValue;
  uint8_t Nt;
  if (N >= 0) {
    Nt = (uint8_t)N;
    newValue = (_value > _maxValue - Nt) ? _maxValue : _value + Nt;
  } else {
    Nt = (uint8_t)-N;
    newValue = (_value < _minValue + Nt) ? _minValue : _value - Nt;
  }
  return (setValueAndDrawIfChanged(newValue));
}

// -------------------------------------------------------------------------
