/*
  Font_TT.cpp - A C++ class that contains a pointer to a GFXfont struct
  instance, and which provides functions for using it. Some functions here were
  copied from Adafruit_GFX.h/.cpp, and the Adafruit Copyright is retained here.

  Created by Ted Toal, July 30, 2023 from Adafruit_GFX.cpp.


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
#include <Font_TT.h>
#include <pgmspace_GFX.h>
#include <glcdfont.c>

// Define the "min" macro.
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

// Define a macro to swap two int16_t variable values.
#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

/**************************************************************************/
void Font_TT::charBounds(unsigned char c, int16_t* x, int16_t* y, int16_t* minX,
    int16_t* minY, int16_t* maxX, int16_t* maxY) {

  if (gfxFont) {

    // Custom GFX font.
    if (c == '\n') { // Newline?
      *x = 0;        // Reset x to zero, advance y by one line
      *y += sizeY * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else if (c != '\r') { // Not a carriage return; is normal char
      uint8_t first = pgm_read_byte(&gfxFont->first),
              last = pgm_read_byte(&gfxFont->last);
      if ((c >= first) && (c <= last)) { // Char present in this font?
        GFXglyph* glyph = pgm_read_glyph_ptr(gfxFont, c - first);
        uint8_t gw = pgm_read_byte(&glyph->width),
                gh = pgm_read_byte(&glyph->height),
                xa = pgm_read_byte(&glyph->xAdvance);
        int8_t xo = pgm_read_byte(&glyph->xOffset),
               yo = pgm_read_byte(&glyph->yOffset);
        int16_t tsx = (int16_t)sizeX, tsy = (int16_t)sizeY,
                x1 = *x + xo * tsx, y1 = *y + yo * tsy, x2 = x1 + gw * tsx - 1,
                y2 = y1 + gh * tsy - 1;
        if (x1 < *minX)
          *minX = x1;
        if (y1 < *minY)
          *minY = y1;
        if (x2 > *maxX)
          *maxX = x2;
        if (y2 > *maxY)
          *maxY = y2;
        *x += xa * tsx;
      }
    }

  } else {

    // Default built-in font.
    if (c == '\n') {        // Newline?
      *x = 0;               // Reset x to zero,
      *y += sizeY * 8; // advance y one line
      // min/max x/y unchanged -- that waits for next 'normal' character
    } else if (c != '\r') { // Normal char; ignore carriage returns
      int x2 = *x + sizeX * 6 - 1, // Lower-right pixel of char
          y2 = *y + sizeY * 8 - 1;
      if (x2 > *maxX)
        *maxX = x2; // Track max x, y
      if (y2 > *maxY)
        *maxY = y2;
      if (*x < *minX)
        *minX = *x; // Track min x, y
      if (*y < *minY)
        *minY = *y;
      *x += sizeX * 6; // Advance x one char
    }
  }
}

/**************************************************************************/
void Font_TT::getTextBounds(const char* str, int16_t x, int16_t y, int16_t* xL,
    int16_t* yT, uint16_t* wt, uint16_t* ht, int16_t* xF, int16_t* yF) {

  uint8_t c; // Current character
  int16_t minX = 0x7FFF, minY = 0x7FFF, maxX = -0x7FFF, maxY = -0x7FFF;
  // Bound rect is intentionally initialized to extremes, so 1st char sets it

  *xL = x; // This initialization SHOULD be overwritten below after charBounds()
  *yT = y; //     is called. (Unless string is empty).
  *wt = *ht = 0; // Initial size is zero

  while ((c = *str++)) {
    // charBounds() modifies x/y to advance for each character,
    // and min/max x/y are updated to incrementally build bounding rect.
    charBounds(c, &x, &y, &minX, &minY, &maxX, &maxY);
  }

  if (maxX >= minX) { // If legit string bounds were found...
    *xL = minX;       // Update xL to least X coord,
    *wt = maxX - minX + 1; // And wt to bound rect width
  }

  if (maxY >= minY) { // Same for height as for width
    *yT = minY;
    *ht = maxY - minY + 1;
  }

  // If xF and yF arguments provided, return final cursor x and y position.
  if (xF != nullptr)
    *xF = x;
  if (yF != nullptr)
    *yF = y;
}

/**************************************************************************/
void Font_TT::getTextBounds(const String &str, int16_t x, int16_t y,
    int16_t* xL, int16_t* yT, uint16_t* wt, uint16_t* ht, int16_t* xF,
    int16_t* yF) {

  if (str.length() != 0) {
    getTextBounds(const_cast<char*>(str.c_str()), x, y, xL, yT, wt, ht, xF, yF);
  }
}

/**************************************************************************/
void Font_TT::getTextBounds(const __FlashStringHelper* str, int16_t x,
    int16_t y, int16_t* xL, int16_t* yT, uint16_t* wt, uint16_t* ht,
    int16_t* xF, int16_t* yF) {

  uint8_t* s = (uint8_t*)str, c;

  *xL = x;
  *yT = y;
  *wt = *ht = 0;

  int16_t minX = 0x7FFF, minY = 0x7FFF, maxX = -0x7FFF, maxY = -0x7FFF;

  while ((c = pgm_read_byte(s++)))
    charBounds(c, &x, &y, &minX, &minY, &maxX, &maxY);

  if (maxX >= minX) {
    *xL = minX;
    *wt = maxX - minX + 1;
  }
  if (maxY >= minY) {
    *yT = minY;
    *ht = maxY - minY + 1;
  }

  // If xF and yF arguments provided, return final cursor x and y position.
  if (xF != nullptr)
    *xF = x;
  if (yF != nullptr)
    *yF = y;
}

/**************************************************************************/
void Font_TT::getTextBoundsAndOffset(const char* str, int16_t &dX, int16_t &dY,
    uint16_t &wt, uint16_t &ht, int16_t &dXcF) {

  getTextBounds(str, 0, 0, &dX, &dY, &wt, &ht, &dXcF);

  // Add one pixel on each side of the text bounding box to account for
  // apparent inaccuracy creeping in somewhere.
  dX -= 1;
  dY -= 1;
  wt += 2;
  ht += 2;
  dXcF += 1;
}

/**************************************************************************/
void Font_TT::getTextBoundsAndOffset(const String &str, int16_t &dX,
    int16_t &dY, uint16_t &wt, uint16_t &ht, int16_t &dXcF) {

  if (str.length() != 0) {
    getTextBoundsAndOffset(const_cast<char*>(str.c_str()), dX, dY, wt, ht, dXcF);
  }
}

/**************************************************************************/
void Font_TT::getTextBoundsAndOffset(const __FlashStringHelper* str,
    int16_t &dX, int16_t &dY, uint16_t &wt, uint16_t &ht, int16_t &dXcF) {

  getTextBounds(str, 0, 0, &dX, &dY, &wt, &ht, &dXcF);

  // Add one pixel on each side of the text bounding box to account for
  // apparent inaccuracy creeping in somewhere.
  dX -= 1;
  dY -= 1;
  wt += 2;
  ht += 2;
  dXcF += 1;
}

/**************************************************************************/
void Font_TT::newTextBoundsAndOffset(int16_t dX, int16_t dY, uint16_t wt,
    uint16_t ht, int16_t dXcF, int16_t dX2, int16_t dY2, uint16_t wt2,
    uint16_t ht2, int16_t dXcF2, int16_t &dYnew, uint16_t &wt_new,
    uint16_t &ht_new, int16_t &dXcFnew) {

  // Compute wt_new, the total width of the text strings. See comments in
  // getTextBoundsAndOffset().
  wt_new = dXcF - dX + dX2 + wt2;

  // Compute ht_new, the total height of the text strings. The height is the
  // maximum string y-coord minus the minimum y-coord.
  ht_new = max(dY + ht, dY2 + ht2) - min(dY, dY2);

  // Compute dXcF_new, the cursor x-position after the end of the strings.
  dXcFnew = dXcF + dXcF2;

  // Compute dYnew, the offset from cursor y (baseline y coord) to top of text
  // bound box. This is done last in case dYnew is the same variable as dY,
  // since we use dY above.
  dYnew = min(dY, dY2);
}

/**************************************************************************/
void Font_TT::getTextAlignCursor(int16_t dX, int16_t dY, uint16_t wt,
    uint16_t ht, int16_t xL, int16_t yT, uint16_t w, uint16_t h, char alignH,
    char alignV, int16_t &xC, int16_t &yC) {

  xC = xL - dX; // Left-aligned
  if (alignH == 'R')
    xC += w - wt; // Shift right to be right-aligned
  else if (alignH == 'C')
    xC += w/2 - wt/2; // Shift right partially to be center-aligned

  yC = yT - dY; // Top-aligned
  if (alignV == 'B')
    yC += h - ht; // Shift down to be bottom-aligned
  else if (alignV == 'C')
    yC += h/2 - ht/2; // Shift down partially to be middle-aligned
}

/**************************************************************************/
void Font_TT::getTextAlignCursor(const char* str, int16_t xL, int16_t yT,
    uint16_t w, uint16_t h, char alignH, char alignV, int16_t &xC,
    int16_t &yC) {

  int16_t dX, dY, dXcF;
  uint16_t wt, ht;
  getTextBoundsAndOffset(str, dX, dY, wt, ht, dXcF);
  getTextAlignCursor(dX, dY, wt, ht, xL, yT, w, h, alignH, alignV, xC, yC);
}

/**********************************************************************/
void Font_TT::getTextAlignCursor(const String &str, int16_t xL, int16_t yT,
    uint16_t w, uint16_t h, char alignH, char alignV, int16_t &xC,
    int16_t &yC) {

  int16_t dX, dY, dXcF;
  uint16_t wt, ht;
  getTextBoundsAndOffset(str, dX, dY, wt, ht, dXcF);
  getTextAlignCursor(dX, dY, wt, ht, xL, yT, w, h, alignH, alignV, xC, yC);
}

/**********************************************************************/
void Font_TT::getTextAlignCursor(const __FlashStringHelper* str, int16_t xL,
    int16_t yT, uint16_t w, uint16_t h, char alignH, char alignV, int16_t &xC,
    int16_t &yC) {

  int16_t dX, dY, dXcF;
  uint16_t wt, ht;
  getTextBoundsAndOffset(str, dX, dY, wt, ht, dXcF);
  getTextAlignCursor(dX, dY, wt, ht, xL, yT, w, h, alignH, alignV, xC, yC);
}

// -------------------------------------------------------------------------
