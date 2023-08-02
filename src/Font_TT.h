/*
  Font_TT.h - A C++ class that contains a pointer to a GFXfont struct instance,
  and which provides functions for using it. Some functions here were copied
  from Adafruit_GFX.h/.cpp, and the Adafruit Copyright is retained here.

  Created by Ted Toal, July 30, 2023 from Adafruit_GFX.h.


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

  This file and its attendant .cpp file define class Font_TT, which has a
  pointer to a GFXfont struct instance and whose member functions include those
  from Adafruit_GFX.h/.cpp that work with a GFXfont object but do not require a
  display, such as string size functions.

  Font_TT also has some new member functions to enhance the capabilities of
  working with fonts.

  This class also supports the Adafruit classic built-in fixed-space font.

  Text wrapping is not supported.

  In an ideal world this class would have been used by class Adafruit_GFX, since
  this class is essentially a factoring-out from that class of font-related
  functionality.
*/
#ifndef Font_TT_h
#define Font_TT_h

#include <Arduino.h>
#include <gfxfont.h>

/**************************************************************************/
/*!
  @brief  A font services class providing services related to fonts that
          use a GFXfont struct to define them.
*/
/**************************************************************************/
class Font_TT {

protected:
  GFXfont* gfxFont; // Pointer to font struct to attach to the class instance.
  uint8_t sizeX;    // Desired magnification in X-axis of font characters.
  uint8_t sizeY;    // Desired magnification in Y-axis of font characters.

public:

  /**********************************************************************/
  /*!
    @brief  Constructor.
    @param  font  Pointer to GFXfont font struct to attach to the class
                  instance. nullptr means use the built-in fixed-space font.
    @param  sx    Desired text width magnification in X direction.
    @param  sy    Desired text width magnification in Y direction.
  */
  /**********************************************************************/
  Font_TT(GFXfont* font, uint8_t sx, uint8_t sy) {
    gfxFont = font;
    sizeX = sx;
    sizeY = sy;
  }

  /**********************************************************************/
  /*!
    @brief  Constructor.
    @param  font  Pointer to GFXfont font struct to attach to the class
                  instance. nullptr means use the built-in fixed-space font.
  */
  /**********************************************************************/
  Font_TT(GFXfont* font) : Font_TT(font, 1, 1) {}

  /**********************************************************************/
  /*!
    @brief  Default constructor. The built-in fixed-space font is used.
  */
  /**********************************************************************/
  Font_TT() : Font_TT(nullptr, 1, 1) {}

  /**********************************************************************/
  /*!
    @brief  == operator for Font_TT objects.
    @param  other   Reference to the other Font_TT object to compare to this.
    @returns  true if Font_TT objects refer to same font with same attributes.
  */
  /**********************************************************************/
  bool operator == (const Font_TT& other) const {
    return(gfxFont == other.gfxFont &&
      sizeX == other.sizeX && sizeY == other.sizeY);
    }

   /**********************************************************************/
  /*!
    @brief    Get the font attached to this Font_TT class instance.
    @returns  The GFXfont pointer of the font attached to this class instance,
              or nullptr if this class instance uses the built-in fixed-space
              font.
    @note   The font attached to the class instance is fixed by the constructor
            and can't be changed on the fly. Create a different class instance
            to use a different font.
  */
  /**********************************************************************/
  const GFXfont* getFont() { return(gfxFont); }

  /**********************************************************************/
  /*!
    @brief    Get text 'magnification' size in X direction.
    @returns  Text size in X direction.
    @note     If character size at a setting of 1 is 6x8 pixels, then a setting
              of 2 in both directions would give a size of 12x14, 3 would give
              18x24, etc.
  */
  /**********************************************************************/
  uint8_t getTextSizeX( { return(sizeX); }

  /**********************************************************************/
  /*!
    @brief    Get text 'magnification' size in Y direction.
    @returns  Text size in Y direction.
    @note     If character size at a setting of 1 is 6x8 pixels, then a setting
              of 2 in both directions would give a size of 12x14, 3 would give
              18x24, etc.
  */
  /**********************************************************************/
  uint8_t getTextSizeY( { return(sizeY); }

  /**********************************************************************/
  /*!
    @brief  Set text 'magnification' size in x and y directions.
    @param  sx    Desired text width magnification in X direction, default 1.
    @param  sy    Desired text height magnification in Y direction, default 1.
  */
  /**********************************************************************/
  void setTextSize(uint8_t sx, uint8_t sy) {
    sizeX = (sx > 0) ? sx : 1;
    sizeY = (sy > 0) ? sy : 1;
  }

  /**********************************************************************/
  /*!
    @brief  Set text 'magnification' size in both X and Y directions.
    @param  s   Desired text size in both X and Y directions.
  */
  /**********************************************************************/
  void setTextSize(uint8_t s) { setTextSize(s, s); }

  /**********************************************************************/
  /*!
    @brief  Compute size (in pixels) of a character using the current text size
            setting and of course using the font attached to the class instance,
            and then update the values pointed at by the remaining arguments.
            The arguments are designed so that this may be called repeatedly for
            each character of a string in order to compute the string bounding
            box (minX, minY, maxX, maxY). That box tells you the string size in
            pixels and its offset from a starting drawing position of (x,y).
            The actual first pixel is written at (x+minX, y+minY) and the last
            pixel is written at (x+maxX, y+maxY).
    @param  c     The ASCII character in question, or a newline ('\n').
    @param  x     Pointer to helper variable for this function, that accumulates
                  size in the X-direction. On call it contains an initial X
                  size, and on return it has been adjusted by adding to it the X
                  size of character 'c'. Use 0 as the initial value on call, and
                  if you are calling this repeatedly for each character of a
                  string, just re-use the last returned value as the value on
                  the next call. Do not use this as the final character or
                  string width, but instead, use maxX-minX+1.
    @param  y     Pointer to helper variable that accumulates size in the Y
                  direction. Same comments apply here as for 'x' argument.
                  Do not use this as the final character or string height, but
                  instead, use maxY-minY+1.
    @param  minX  Pointer to variable that accumulates a minimum character or
                  string bounding box X-coordinate. Initialize this to 0x7FFF
                  before the first call to this function, and if you are calling
                  this repeatedly for each character of a string, just re-use
                  the last returned value as the value on the next call. After
                  the final call, you can compute the character or string width
                  and height using minX, maxX, minY, and maxY as described
                  above. The actual values of minX and minY are also important,
                  especially for the purpose of aligning text. For example, say
                  the final values of minX and maxX are -3 and 10. This means
                  the character or string width is 10-(-3)+1 = 14 pixels, and
                  it also means that if you were to start drawing the character
                  at x-coordinate 100, the ACTUAL x-coordinates occupied by the
                  string are 100-3 through 100+10, i.e. from x=97 to x=110. The
                  same applies in the Y direction. Notably, character descenders
                  impact minX and can cause it to be negative.
    @param  minY  Pointer to variable for accumulating minimum Y coordinate.
                  Same comments apply here as for 'minX' argument.
    @param  maxX  Pointer to variable for accumulating maximum X coordinate.
                  Same comments apply here as for 'minX' argument, except
                  initialize this to -0x7FFF.
    @param  maxY  Pointer to variable for accumulating maximum Y coordinate.
                  Same comments apply here as for 'minX' argument, except
                  initialize this to -0x7FFF.
  */
  /**********************************************************************/
  void charBounds(unsigned char c, int16_t* x, int16_t* y, int16_t* minX,
                  int16_t* minY, int16_t* maxX, int16_t* maxY);

  /**********************************************************************/
  /*!
    @brief        Compute the bounding box of a string using the current text
                  size and the font attached to the class instance.
    @param  str   The ASCII string to measure.
    @param  x     Boundary box offset in x-direction, usually 0.
    @param  y     Boundary box offset in y-direction, usually 0.
    @param  xL    Pointer to variable to receive LEFT boundary x-coordinate.
    @param  yT    Pointer to variable to receive TOP boundary y-coordinate.
    @param  wt    Pointer to variable to receive string WIDTH in pixels.
    @param  ht    Pointer to variable to receive string HEIGHT in pixels.
    @param  xF    nullptr if not needed, else a pointer to a variable to receive
                  the ending cursor x-coordinate, if start was 'x'.
    @param  yF    nullptr if not needed, else a pointer to a variable to receive
                  the ending cursor y-coordinate, if start was 'y'.
    @note   If called with (x,y) = (0,0), it is not true that upon return,
            (xL, yT) will be (0,0). xL might be ZERO OR NEGATIVE OR POSITIVE
            but will be a small number representing where the string's LEFTMOST
            pixel will be written relative to 'x'. Likewise, yT will be a small
            number, most likely POSITIVE, representing where the  string's
            TOPMOST pixel will be written relative to 'y'.
    @note   See getTextBoundsAndOffset() and getTextAlignCursor() for more
            understandable functions that use this function for their work.
  */
  /**********************************************************************/
  void getTextBounds(const char* str, int16_t x, int16_t y, int16_t* xL,
    int16_t* yT, uint16_t* wt, uint16_t* ht, int16_t* xF = nullptr,
    int16_t* yF = nullptr);

  /**********************************************************************/
  /*!
    @brief        Like getTextBounds() above except first argument is a String.
    @param  str   See getTextBounds(const char* str, ...)
    @param  x     See getTextBounds(const char* str, ...)
    @param  y     See getTextBounds(const char* str, ...)
    @param  xL    See getTextBounds(const char* str, ...)
    @param  yT    See getTextBounds(const char* str, ...)
    @param  wt    See getTextBounds(const char* str, ...)
    @param  ht    See getTextBounds(const char* str, ...)
    @param  xF    See getTextBounds(const char* str, ...)
    @param  yF    See getTextBounds(const char* str, ...)
    @note         See getTextBounds(const char* str, ...)
  */
  /**********************************************************************/
  void getTextBounds(const String &str, int16_t x, int16_t y, int16_t* xL,
    int16_t* yT, uint16_t* wt, uint16_t* ht, int16_t* xF = nullptr,
    int16_t* yF = nullptr);

  /**********************************************************************/
  /*!
    @brief        Like getTextBounds() above except for type of first argument.
    @param  str   See getTextBounds(const char* str, ...)
    @param  x     See getTextBounds(const char* str, ...)
    @param  y     See getTextBounds(const char* str, ...)
    @param  xL    See getTextBounds(const char* str, ...)
    @param  yT    See getTextBounds(const char* str, ...)
    @param  wt    See getTextBounds(const char* str, ...)
    @param  ht    See getTextBounds(const char* str, ...)
    @param  xF    See getTextBounds(const char* str, ...)
    @param  yF    See getTextBounds(const char* str, ...)
    @note         See getTextBounds(const char* str, ...)
  */
  /**********************************************************************/
  void getTextBounds(const __FlashStringHelper* str, int16_t x, int16_t y,
    int16_t* xL, int16_t* yT, uint16_t* wt, uint16_t* ht, int16_t* xF = nullptr,
    int16_t* yF = nullptr);

  /**********************************************************************/
  /*!
    @brief        Compute a string's bounding box size in pixels and the cursor
                  position offset, using current text size and the font attached
                  to the class instance.
    @param  str   The ASCII string whose size and cursor offset is computed.
    @param  dX    Reference to variable whose value is set by this function to
                  the delta x-coordinate from the left side of the string to the
                  first string character's starting cursor x-position. Subtract
                  this from the left-side x-coordinate to get the starting
                  cursor position x-coordinate.
    @param  dY    Like dX but for y-coordinate.
    @param  wt    Reference to variable whose value is set by this function to
                  the string's bounding rectangle width in pixels.
    @param  ht    Like wt but for the bounding rectangle's height in pixels.
    @param  dXcF  Reference to variable whose value is set by this function to
                  the delta x-coordinate from the starting to ending cursor
                  position. Add this to the starting cursor x-coordinate to get
                  the ending cursor x-coordinate, useful if something more is to
                  be printed after the string.
    @note         The correct cursor position to use when printing str if its
                  upper-left corner is to be at (X1,Y1), is (X1-dX, Y1-dY).
    @note         The position of the lower-right corner of the str bounding
                  box if its upper-left corner is at (X1,Y1) is (X1+wt, Y1+ht).
    @note         If the upper-left corner of str is at (X1, Y1), the final
                  cursor position is (X1-dX+dXcF, Y1-dY), which is NOT always
                  the same as (dX+wt, dY+ht).
    @note         The cursor position to use in order to print additional text
                  after str if its upper-left corner is at (X1, Y1) is
                  (X1-dX+dXcF, Y1-dY).
    @note         dX might be 0 or a small negative or positive value, and dY
                  will almost always be a negative value that is the distance
                  from the text baseline to the top of the text. In particular,
                  the text baseline is NOT y_top+ht if any str characters have
                  descenders that extend below the baseline.
    @note         If additional text is printed after str and that text is used
                  in a call to this function to obtain the  top-left bounding
                  box-to-cursor offset (DX, DY) and width W and height H, then
                  the new lower-right corner of the bounding box for ALL of the
                  text is (X1-dX+dXcF+DX+W, Y1-dY+max(0, DY+H)) and the total
                  width and height is w = dXcF-dX+DX+W and h = -dY+max(0, DY+H).
    @note         The returned values for wt and ht have a 2-pixel increase, dX
                  and dY have a 1-pixel decrease, and dXcF has a 1-pixel
                  increase over getTextBounds() values, to compensate for an
                  apparent inaccuracy creeping in somewhere.
  */
  /**********************************************************************/
  void getTextBoundsAndOffset(const char* str, int16_t &dX, int16_t &dY,
                     uint16_t &wt, uint16_t &ht, int16_t &dXcF);

  /**********************************************************************/
  /*!
    @brief        Like getTextBoundsAndOffset() above except first argument is
                  a String.
    @param  str   See getTextBoundsAndOffset(const char* str, ...)
    @param   dX   See getTextBoundsAndOffset(const char* str, ...)
    @param   dY   See getTextBoundsAndOffset(const char* str, ...)
    @param   wt   See getTextBoundsAndOffset(const char* str, ...)
    @param   ht   See getTextBoundsAndOffset(const char* str, ...)
    @param   dXcF See getTextBoundsAndOffset(const char* str, ...)
    @note         See getTextBoundsAndOffset(const char* str, ...)
  */
  /**********************************************************************/
  void getTextBoundsAndOffset(const String &str, int16_t &dX, int16_t &dY,
                     uint16_t &wt, uint16_t &ht, int16_t &dXcF);

  /**********************************************************************/
  /*!
    @brief        Like getTextBoundsAndOffset() above except for the type of the
                  first argument.
    @param  str   See getTextBoundsAndOffset(const char* str, ...)
    @param   dX   See getTextBoundsAndOffset(const char* str, ...)
    @param   dY   See getTextBoundsAndOffset(const char* str, ...)
    @param   wt   See getTextBoundsAndOffset(const char* str, ...)
    @param   ht   See getTextBoundsAndOffset(const char* str, ...)
    @param   dXcF See getTextBoundsAndOffset(const char* str, ...)
    @note         See getTextBoundsAndOffset(const char* str, ...)
  */
  /**********************************************************************/
  void getTextBoundsAndOffset(const __FlashStringHelper* s, int16_t &dX,
                     int16_t &dY, uint16_t &wt, uint16_t &ht, int16_t &dXcF);

  /**********************************************************************/
  /*!
    @brief          Compute new values dYnew, wt_new, ht_new, and dXcFnew that
                    give the delta Y, width, height, and delta cursor of the
                    string formed by printing two strings, first one, then the
                    second immediately following. The parameters for those two
                    strings are (dY, wt, ht, dXcF) and (dY2, wt2, ht2, dXcF2)
                    that were returned by two calls to getTextBoundsAndOffset().
    @param  dX      First string delta X from getTextBoundsAndOffset().
    @param  dY      First string delta Y from getTextBoundsAndOffset().
    @param  wt      First string width from getTextBoundsAndOffset().
    @param  ht      First string height from getTextBoundsAndOffset().
    @param  dXcF    First string cursor delta X from getTextBoundsAndOffset().
    @param  dX2     Second string delta X from getTextBoundsAndOffset().
    @param  dY2     Second string delta Y from getTextBoundsAndOffset().
    @param  wt2     Second string width from getTextBoundsAndOffset().
    @param  ht2     Second string height from getTextBoundsAndOffset().
    @param  dXcF2   Second string cursor delta X from getTextBoundsAndOffset().
    @param  dXnew   Total delta X of the two strings is returned here.
    @param  dYnew   Total delta Y of the two strings is returned here.
    @param  wt_new  Total width of the two strings is returned here.
    @param  ht_new  Total height of the two strings is returned here.
    @param  dXcFnew Total cursor delta X of the two strings is returned here.
    @note           The dX value does not change when second string is printed.
    @note           dXnew, dYnew, wt_new, ht_new, and dXcFnew can be the SAME
                    VARIABLES AS dX, dY, wt, ht, and dXcF (which you might want
                    to do if you are merely updating those variables to account
                    for an additional string).
  */
  /**********************************************************************/
  void newTextBoundsAndOffset(int16_t dX, int16_t dY, uint16_t wt, uint16_t ht,
                              int16_t dXcF, int16_t dX2, int16_t dY2,
                              uint16_t wt2, uint16_t ht2, int16_t dXcF2,
                              int16_t &dYnew, uint16_t &wt_new,
                              uint16_t &ht_new, int16_t &dXcFnew);

  /**********************************************************************/
  /*!
    @brief          Compute cursor start position to print a text string in the
                    requested horizontal/vertical alignment within a rectangle.
    @param  xL      Rectangle left boundary X coordinate.
    @param  yT      Rectangle top boundary Y coordinate.
    @param  w       Rectangle width in pixels.
    @param  h       Rectangle height in pixels.
    @param  dX      Delta x-coordinate from the left side of text string to the
                    first string character's starting cursor x-position,
                    returned by getTextBoundsAndOffset().
    @param  dY      Delta y-coordinate from the top side of text string to the
                    first string character's starting cursor y-position,
                    returned by getTextBoundsAndOffset().
    @param  wt      Text string bounding rectangle's width, returned by
                    getTextBoundsAndOffset().
    @param  ht      Text string bounding rectangle's height, returned by
                    getTextBoundsAndOffset().
    @param  alignH  Desired text string horizontal alignment: 'L'=left,
                    'C'=center, 'R'=right.
    @param  alignV  Desired text string vertical alignment: 'T'=top, 'C'=center,
                    'B'=bottom.
    @param  xC      Starting cursor x-coordinate is returned here.
    @param  yC      Starting cursor y-coordinate is returned here.
  */
  /**********************************************************************/
  void getTextAlignCursor(int16_t xL, int16_t yT, uint16_t w, uint16_t h,
                          int16_t dX, int16_t dY, uint16_t wt, uint16_t ht,
                          char alignH, char alignV, int16_t &xC, int16_t &yC);

}; // class Font_TT

#endif // Font_TT_h
