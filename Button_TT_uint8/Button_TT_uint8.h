/*
  Button_TT_uint8.h - Library for adding a variable of type uint8_t to a button.
  Created by Ted Toal, July 5, 2023.
  Released into the public domain.

  Class Button_TT_uint8 is derived from class Button_TT, and attaches an uint8_t
  signed integer value to the button, its value being used as the button label.
*/
#ifndef Button_TT_uint8_h
#define Button_TT_uint8_h

#include <Arduino.h>
#include <Button_TT.h>
#include <Button_TT_arrow.h>

class Button_TT_uint8 : public Button_TT {

protected:
  uint8_t _value, _minValue, _maxValue;
  const char* _zeroString;
  uint8_t (*_checkValue)(Button_TT_uint8& btn, uint8_t value);

public:
  /**************************************************************************/
  /*!
   @brief    Constructor.
   @param    name         String giving a name to the button, for debugging purposes only!
   @param    (others)     Remaining (optional) arguments are the same as initButton() below
  */
  /**************************************************************************/
  Button_TT_uint8(const char* name, Adafruit_GFX* gfx=0, const char* align="C",
                  int16_t x=0, int16_t y=0, int16_t w=0, int16_t h=0,
                  uint16_t outlineColor=0, uint16_t fillColor=0,
                  uint16_t textColor=0, const char* textAlign="C",
                  uint8_t textSize_x=1, uint8_t textSize_y=1,
                  const GFXfont* f=NULL, int16_t rCorner=0,
                  uint8_t value=0, uint8_t minValue=0, uint8_t maxValue=0,
                  const char* zeroString=NULL, bool degreeSym=false,
                  uint8_t (*checkValue)(Button_TT_uint8& btn, uint8_t value) = NULL,
                  uint8_t expU=0, uint8_t expD=0, uint8_t expL=0, uint8_t expR=0) :
            Button_TT(name) {
    initButton(gfx, align, x, y, w, h, outlineColor, fillColor, textColor, textAlign,
      textSize_x, textSize_y, f, rCorner, value, minValue, maxValue, zeroString,
      degreeSym, checkValue, expU, expD, expL, expR);
  }

  /**************************************************************************/
  /*!
   @brief   Initialize button with our desired color/size/etc. settings
   @param   gfx     See Button_TT_Base::initButton()
   @param   align   See Button_TT::initButton()
   @param   x       See Button_TT::initButton()
   @param   y       See Button_TT::initButton()
   @param   w       See Button_TT::initButton()
   @param   h       See Button_TT::initButton()
   @param   outlineColor  See Button_TT_Base::initButton()
   @param   fillColor     See Button_TT_Base::initButton()
   @param   textColor     See Button_TT::initButton()
   @param   textAlign     See Button_TT::initButton()
   @param   textSize_x    See Button_TT::initButton()
   @param   textSize_y    See Button_TT::initButton()
   @param   f             See Button_TT::initButton()
   @param   rCorner       See Button_TT::initButton()
   @param   value         The uint8_t value for the button, used to create the
                          label string for the button
   @param   minValue      The minimum allowed value for value
   @param   maxValue      The maximum allowed value for value
   @param   zeroString    If not NULL, this points to a character string to be
                          used as the button label if value is 0.
   @param   degreeSym     If true, a degree symbol is drawn at the end of the label
   @param   checkValue    If not NULL, a pointer to a function that checks a new
                          button value and can return an adjusted value if it is
                          out of range
   @param   expU    See Button_TT_Base::initButton()
   @param   expD    See Button_TT_Base::initButton()
   @param   expL    See Button_TT_Base::initButton()
   @param   expR    See Button_TT_Base::initButton()
  */
  /**************************************************************************/
  void initButton(Adafruit_GFX* gfx=0, const char* align="C",
                  int16_t x=0, int16_t y=0, int16_t w=0, int16_t h=0,
                  uint16_t outlineColor=0, uint16_t fillColor=0,
                  uint16_t textColor=0, const char* textAlign="C",
                  uint8_t textSize_x=1, uint8_t textSize_y=1,
                  const GFXfont* f=NULL, int16_t rCorner=0,
                  uint8_t value=0, uint8_t minValue=0, uint8_t maxValue=0,
                  const char* zeroString=NULL, bool degreeSym=false,
                  uint8_t (*checkValue)(Button_TT_uint8& btn, uint8_t value) = NULL,
                  uint8_t expU=0, uint8_t expD=0, uint8_t expL=0, uint8_t expR=0);

  /**************************************************************************/
  /*!
   @brief   Get current button label uint8_t value
   @returns The current uint8_t value of the button
  */
  /**************************************************************************/
  uint8_t getValue(void) { return(_value); }

  /**************************************************************************/
  /*!
   @brief   Set button label uint8_t value, limiting the result to lie between
            minVal and maxVal and checking its value with checkValue function
            if one was supplied, convert the value to a string, and set that as
            the new button label, but don't redraw the button
   @param   value   The uint8_t value for the button
   @param   dontCheck   If true, don't call the checkValue function if it is
                        supplied, which may be necessary during initialization
                        when checkValue tests for interaction of two different
                        values, one of which may not yet have been initialized
   @returns true if the value is different than before
  */
  /**************************************************************************/
  bool setValue(uint8_t value, bool dontCheck = false);

  /**************************************************************************/
  /*!
   @brief   Set button label uint8_t value, convert the value to a string and
            set that as the new button label, then draw the button if the label
            changed or if any visible button attribute changed since last drawn.
   @param   value   The uint8_t value for the button
   @param   forceDraw   If true, the button is drawn even if attributes have
              not changed.
   @returns true if button was drawn
  */
  /**************************************************************************/
  bool setValueAndDrawIfChanged(uint8_t value, bool forceDraw = false);

  /**************************************************************************/
  /*!
   @brief   Add N to the button's value, limiting the result to lie between
            minVal and maxVal and using setValue to set the value, ensuring
            the value is fully checked. If btn is not NULL and btn->delta()
            is not 0, that value is used instead of N. Set the new value as the
            new label for the button and redraw it if the label or visible
            button attribute changed.

   @param   N     Value to add to this button's value, used if btn is NULL or
                  btn->delta() is 0
   @param   btn   Pointer to the base class of the button object that was
                  pressed in order to invoke this function, or NULL if none
   @returns true if button was drawn
  */
  /**************************************************************************/
  bool valueIncDec(int8_t N = 1, Button_TT_Base* btn = NULL);

  };

#endif // Button_TT_uint8_h
