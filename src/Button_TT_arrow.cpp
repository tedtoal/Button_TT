/*
  Button_TT_arrow.cpp - Defines functions of class Button_TT_arrow.
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
#include <Button_TT_arrow.h>

/**************************************************************************/

void Button_TT_arrow::initButton(Adafruit_GFX* gfx, char orient,
    const char* align, int16_t x, int16_t y, uint16_t s1, uint16_t s2,
    uint16_t outlineColor, uint16_t fillColor, uint8_t expU, uint8_t expD,
    uint8_t expL, uint8_t expR) {

  // Initialize class variables from some arguments.
  _orient = orient;
  _s1 = s1;
  _s2 = s2;

  // Pre-initialize base class with null values except gfx.
  Button_TT::initButton(gfx);

  // No more to be done if gfx not specified.
  if (gfx == 0)
    return;

  // Align 'C' is actually 'CC'.
  if (align[0] == 'C' && align[1] == 0)
    align = "CC";

  // Use orientation and b and d to determine bounding box size (w, h).
  uint16_t w, h;
  // If s1 == s2 then they are equilateral triangles and its easy, but that
  // does not have to be the case. If U or D, the other two sides of length s2
  // are the hypotenuses of right triangles whose base is s1/2, giving the
  // height of the triangle as sqrt(s2^2 - s1^2/4).  If L or R, reverse the
  // role of s1 and s2.
  w = s1;
  h = (uint16_t)(1.0 + sqrt(s2 * s2 - s1 * s1 / 4));
  if (orient == 'L' || orient == 'R') {
    w = h;
    h = s1;
  }

  // Compute xL and yT using align, x, y, w and h.
  int16_t xL, yT;
  if (align[1] == 'L')
    xL = x;
  else if (align[1] == 'R')
    xL = x - w + 1;
  else
    xL = x - w / 2 + 1;

  if (align[0] == 'T')
    yT = y;
  else if (align[0] == 'B')
    yT = y - h + 1;
  else
    yT = y - h / 2 + 1;

  // Compute coords of vertices using orient, xL, yT, w, and h.
  _x0 = _x1 = _x2 = xL;
  _y0 = _y1 = _y2 = yT;
  if (orient == 'U') {
    _x0 += w / 2;
    _x1 += w;
    _y1 += h;
    _y2 += h;
  } else if (orient == 'D') {
    _x0 += w / 2;
    _y0 += h;
    _x2 += w;
  } else if (orient == 'L') {
    _y0 += h / 2;
    _x1 += w;
    _x2 += w;
    _y2 += h;
  } else { // Assume 'R'
    _x0 += w;
    _y0 += h / 2;
    _y1 += h;
  }

  // Invoke base class initialize function to finish initialization.
  Button_TT::initButton(gfx, xL, yT, w, h, outlineColor, fillColor, expU, expD,
    expL, expR);

  // Initialize _delta according to button orientation.
  _delta = (orient == 'L' || orient == 'U') ? -1 : +1;
}

/**************************************************************************/

void Button_TT_arrow::drawButton(bool inverted) {

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
    _gfx->fillTriangle(_x0, _y0, _x1, _y1, _x2, _y2, fill);
  if (outline != TRANSPARENT_COLOR)
    _gfx->drawTriangle(_x0, _y0, _x1, _y1, _x2, _y2, outline);

  _changedSinceLastDrawn = false;
}

// -------------------------------------------------------------------------
