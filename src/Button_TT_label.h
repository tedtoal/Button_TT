/*
  Button_TT_label.h - Defines C++ class Button_TT_label that derives from class
  Button_TT to add a label to the button.
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
  split into two classes, Button_TT_label defined here, which fixes problems and
  adds features, and class Button_TT, split out as a separate base class for all
  button types including this one.

  This class and the base class have many changes over the Adafruit class,
  including:
    - text centered properly.
    - functions added for get/set outline, fill, text color, text size, label.
    - added setLabelAndDrawIfChanged() function.
    - constructors changed to a single one with a new first argument "align".
    - font functionality factored out into class Font_TT, GFXfont* arguments to
      class functions changed to Font_TT* arguments.
    - custom font can be set for the button.
    - added textAlign parameter and made button drawing code work properly for
        text alignment within the button.
    - added drawDegreeSymbol() function and degreeSym argument to init().
    - special TRANSPARENT_COLOR added to allow a color that causes no element
      to be drawn, the background color remains, applied to outline, fill,
      label.
    - w and h (width and height of button) changed from unsigned to signed,
      and if they are specified non-positive AND IF gfx is not 0, then to get
      actual button width and height, LABEL WIDTH/HEIGHT is ADDED TO ABSOLUTE
      width/height.
    - made initButton() compute width and height and lower-left coords of the
        button label and store it in class variables for use by drawButton()
        etc.
    - maximum button label length changed from 9 to 20.

  This class can be used to display a rectangular button (optionally with
  rounded corners) containing a text label inside it. The label can optionally
  end with a special symbol not present in the font: a degree symbol.
  Typical usage is similar to Button_TT class usage, with the difference that
  this class has additional initializer function arguments to support the
  additional features, such as the string giving the text for the label.
*/
#ifndef Button_TT_label_h
#define Button_TT_label_h

#include <Arduino.h>
#include <Button_TT.h>
#include <Font_TT.h>

/**************************************************************************/
/*!
  @brief  A class that enhances class Button_TT by adding a text label inside
          the button rectangle, and allowing the rectangle to have rounded
          corners.
*/
/**************************************************************************/
class Button_TT_label : public Button_TT {

protected:

  // Text color.
  uint16_t _textColor;

  // Text alignment, two characters, first is alignment in y direction (T=top,
  // B=bottom, C=center) and second is alignment in x direction (L=left,
  // R=right, C=center).
  const char* _textAlign;

  // Pointer to the font object for the font to use for the label.
  Font_TT* _f;

  // The label text, a dynamically allocated buffer into which the
  // user-specified label is copied.
  char* _label;

  // true if label is to have a degree symbol appended to the end of it.
  bool _degreeSym;

  // Radius of rectangle corner in pixels, 0 = pure rectangle.
  int16_t _rCorner;

  // Width and height of the label bounding box inside the button rectangle.
  uint16_t _w_label, _h_label;

  // Degree symbol data:
  //  _dx_degree: distance from degree initial cursor to left of degree bound
  //              box.
  //  _dy_degree: distance from degree initial cursor to top of degree bound
  //              box.
  //  _xa_degree: amount to advance y-coordinate of cursor for degree symbol.
  //  _d_degree: diameter of degree symbol (its width AND height).
  //  _rO_degree: outer radius of degree symbol.
  //  _rI_degree: inner radius of degree symbol.
  int8_t _dx_degree, _dy_degree, _xa_degree;
  uint8_t _d_degree, _rO_degree, _rI_degree;

  // A Font_TT object whose gfx pointer is nullptr and whose textSize values are 1.
  // The _f variable above is pointed to this when the user specifies a nullptr
  // value for the font object to be used.
  static Font_TT builtInFont;

  // Helper variables used by getWidestValue().
  static uint16_t digitWidths[10];
  static bool haveDigitWidths;
  static uint8_t widestDigit;

  /**************************************************************************/
  // protected functions follow.
  /**************************************************************************/

  /**************************************************************************/
  // Given a range of integer values, determine the value in that range that,
  // when converted to a string, has the widest pixel width using the current
  // font/size.
  //
  // Arguments:
  //  minValue: minimum value of the range.
  //  maxValue: maximum value of the range.
  //  S: on return this contains the string that is the largest pixed width.
  //  wt: on return this contains the width in pixels of the widest value.
  //  ht: on return this contains the height in pixels of the widest value.
  //  showPlus: if true, positive values have a leading "+" sign in the
  //    character string.
  //  zeroString: if not nullptr, the width of this string is also considered
  //    when looking for the widest string.
  //
  // Returns: no return value. The widest string found is returned in S, and the
  // string width and height in pixels are returned in wt and ht.
  //
  // Note: the font that will be used to draw the value must be set in this
  // class before calling this function.
  //
  // Note: In order to keep the code simple, a shortcut is taken that results
  // in the possibility that the returned value (and its string in S) might not
  // be within the given range. It will be an approximation of the widest value,
  // but its width may be wider than the actual widest value. For example, if
  // minValue were -10 and maxValue were +120 and showPlus were true, the
  // returned value might be 133 and returned string "+133".
  /**************************************************************************/
  void getWidestValue(int32_t minValue, int32_t maxValue, char S[12],
      uint16_t &wt, uint16_t &ht, bool showPlus = false,
      const char* zeroString = nullptr);

  /**************************************************************************/
  // Compute degree symbol delta x/y from cursor position to upper-left corner,
  // cursor x-position advance amount, diameter, and outer and inner radius.
  /**************************************************************************/
  void getDegreeSymSize(int8_t &dx, int8_t &dy, int8_t &xa, uint8_t &d,
      uint8_t &rO, uint8_t &rI);

  /**************************************************************************/
  // Update the arguments that are references by adding in the effect of putting
  // the degree symbol after the label. On call, the arguments reflect the size
  // and relative cursor position of the label without the degree symbol. This
  // requires that _dx_degree, _dy_degree, _xa_degree, and _d_degree have been
  // set correctly for the degree symbol.
  /**************************************************************************/
  void updateLabelSizeForDegreeSymbol(int16_t dX, int16_t &dY, int16_t &dXcF,
      uint16_t &wt, uint16_t &ht);

  /**************************************************************************/
  // public functions follow.
  /**************************************************************************/

public:

  /**************************************************************************/
  /*!
    @brief    Constructor.
    @param    name          String giving a name to the button, for debugging
                            purposes only!
    @param    (others)      Remaining (optional) arguments are the same as
                            initButton() below.
  */
  /**************************************************************************/
  Button_TT_label(const char* name, Adafruit_GFX* gfx = 0,
      const char* align = "C", int16_t x = 0, int16_t y = 0, int16_t w = 0,
      int16_t h = 0, uint16_t outlineColor = 0, uint16_t fillColor = 0,
      uint16_t textColor = 0, const char* textAlign = "C", char* label = 0,
      bool degreeSym = false, Font_TT* f = nullptr, int16_t rCorner = 0,
      uint8_t expU = 0, uint8_t expD = 0, uint8_t expL = 0, uint8_t expR = 0)
      : Button_TT(name) {

    initButton(gfx, align, x, y, w, h, outlineColor, fillColor, textColor,
      textAlign, label, degreeSym, f, rCorner, expU, expD, expL, expR);
  }

  /**************************************************************************/
  /*!
    @brief    Destructor.   Release memory used by _label.
  */
  /**************************************************************************/
  ~Button_TT_label() {

    if (_label != nullptr) {
      free(_label);
      _label = nullptr;
    }
  }

  /**************************************************************************/
  /*!
    @brief    Initialize button with our desired color/size/etc. settings
    @param    gfx     See Button_TT::initButton()
    @param    align   (x,y) alignment: TL, TC, TR, CL, CC, CR, BL, BC, BR, C
                      where T=top, B=bottom, L=left, R=right, C=center, C=CC.
    @param    x       The X coordinate of the button, relative to 'align'.
    @param    y       The X coordinate of the button, relative to 'align'.
    @param    w       Width of the button, non-positive to compute width from
                      label and add abs(w).
    @param    h       Height of the button, non-positive to compute height from
                      label and add abs(h).
    @param    outlineColor  See Button_TT::initButton()
    @param    fillColor     See Button_TT::initButton()
    @param    textColor     Color of the button label (16-bit 5-6-5 standard).
    @param    textAlign     Like 'align' but gives alignment for label, 1st char
                            = up/down alignment, 2nd = left/right.
    @param    label   String of the text inside the button, should be the
                      largest possible label if w or h is negative to ensure all
                      possible labels will fit in the size that is automatically
                      computed.
    @param    degreeSym     If true, a degree symbol is drawn at the end of the
                            label.
    @param    f             Pointer to Font_TT object for the custom font to use
                            for the label, nullptr for built-in font.
    @param    rCorner       Radius of rounded corners of button outline
                            rectangle, 0 for straight corners.
    @param    expU    See Button_TT::initButton()
    @param    expD    See Button_TT::initButton()
    @param    expL    See Button_TT::initButton()
    @param    expR    See Button_TT::initButton()
  */
  /**************************************************************************/
  void initButton(Adafruit_GFX* gfx = 0, const char* align = "C", int16_t x = 0,
      int16_t y = 0, int16_t w = 0, int16_t h = 0, uint16_t outlineColor = 0,
      uint16_t fillColor = 0, uint16_t textColor = 0,
      const char* textAlign = "C", char* label = 0, bool degreeSym = false,
      Font_TT* f = nullptr, int16_t rCorner = 0, uint8_t expU = 0,
      uint8_t expD = 0, uint8_t expL = 0, uint8_t expR = 0);

  /**************************************************************************/
  /*!
    @brief    Get current text color for button label.
    @returns  The current text color.
  */
  /**************************************************************************/
  uint16_t getTextColor(void) { return (_textColor); }

  /**************************************************************************/
  /*!
    @brief    Set new color for button label.
    @param    textColor   The new text color.
    @returns  true if new color is different from old.
  */
  /**************************************************************************/
  bool setTextColor(uint16_t textColor);

  /**************************************************************************/
  /*!
    @brief    Get current alignment for button label.
    @returns  The label alignment setting.
  */
  /**************************************************************************/
  const char* getTextAlign(void) { return (_textAlign); }

  /**************************************************************************/
  /*!
    @brief    Set new alignment for button label.
    @param    textAlign   The new text alignment.
    @returns  true if new alignment is different from old.
  */
  /**************************************************************************/
  bool setTextAlign(const char* textAlign);

  /**************************************************************************/
  /*!
    @brief    Get current button label font object.
    @returns  The current font object for the button label.
    @note     If the built-in font is in use, the returned Font_TT object will
              have a GFXfont member that is nullptr.
  */
  /**************************************************************************/
  const Font_TT* getFont(void) { return (_f); }

  /**************************************************************************/
  /*!
    @brief    Set new font object for button label.
    @param    f   The new font object.
    @returns  true if new font is different from old (font object may be
              different, but if actual font or its magnification is different,
              true is returned).
  */
  /**************************************************************************/
  bool setFont(Font_TT* f = nullptr);

  /**************************************************************************/
  /*!
    @brief    Get current button label.
    @returns  The current label text.
  */
  /**************************************************************************/
  const char* getLabel(void) { return (_label); }

  /**************************************************************************/
  /*!
    @brief    Set new label for button.
    @param    label   The new label.
    @returns  true if new label is different from old label.
    @note     The new label is copied to a buffer allocated in memory for it,
              after freeing memory used by any previous label, but if the new
              label is exactly the same length as the old one, the existing
              memory is simply reused.
  */
  /**************************************************************************/
  bool setLabel(const char* label);

  /**************************************************************************/
  /*!
    @brief    Get flag indicating whether a degree symbol is drawn after the
              label.
    @returns  true if degree symbol is drawn, else false.
  */
  /**************************************************************************/
  bool getDegreeSymbol(void) { return (_degreeSym); }

  /**************************************************************************/
  /*!
    @brief    Draw the button on the screen.
    @param    inverted  Whether to draw with fill/text swapped to indicate
                        'pressed'.
  */
  /**************************************************************************/
  using Button_TT::drawButton;
  virtual void drawButton(bool inverted) override;

  /**************************************************************************/
  /*!
    @brief    Set new label for button and draw the button if the label changed
              or if any visible button attribute changed since last drawn.
    @param    label       The new label.
    @param    forceDraw  If true, the button is drawn even if attributes have
              not changed.
    @returns  true if button was drawn.
  */
  /**************************************************************************/
  bool setLabelAndDrawIfChanged(const char* label, bool forceDraw = false);
};

#endif // Button_TT_label_h
