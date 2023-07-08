/*
  Button_TT_arrow.h - Library for displaying a triangular button.
  Created by Ted Toal, July 5, 2023.
  Released into the public domain.

  Class Button_TT_arrow is a new button class derived from class Button_TT_Base,
  for creating graphical user interface buttons in the shape of equilateral
  triangles, to be used as arrow buttons, typically to signify increase or
  decrease in a numeric value. The virtual delta() function returns -1 for left
  or up-pointing buttons and +1 for right or down-pointing buttons.
*/
#ifndef Button_TT_arrow_h
#define Button_TT_arrow_h

#include <Arduino.h>
#include <Button_TT_Base.h>

class Button_TT_arrow :  public Button_TT_Base {

protected:
  char _orient;
  int16_t _x0, _y0, _x1, _y1, _x2, _y2; // Triangle vertices, 0=tip, CW for 1 and 2
  uint16_t _s1, _s2; // Lengths of triangle sides, one side is _s1, 2 are _s2.

public:
  /**************************************************************************/
  /*!
   @brief    Constructor.
   @param    name         String giving a name to the button, for debugging purposes only!
   @param    (others)     Remaining (optional) arguments are the same as initButton() below
  */
  /**************************************************************************/
  Button_TT_arrow(const char* name, Adafruit_GFX* gfx=0,
                  char orient='U', const char* align="C",
                  int16_t x=0, int16_t y=0, uint16_t s1=0, uint16_t s2=0,
                  uint16_t outlineColor=0, uint16_t fillColor=0,
                  uint8_t expU=0, uint8_t expD=0, uint8_t expL=0, uint8_t expR=0) :
            Button_TT_Base(name) {
    initButton(gfx, orient, align, x, y, s1, s2, outlineColor, fillColor,
      expU, expD, expL, expR);
  }

  /**************************************************************************/
  /*!
   @brief    Initialize button with our desired color/size/etc. settings
   @param    gfx     Pointer to our display so we can draw to it!
   @param    orient  Orientation of triangle: U=UP, D=DOWN, L=LEFT, R=RIGHT
   @param    align   See Button_TT::initButton()
   @param    x       See Button_TT::initButton()
   @param    y       See Button_TT::initButton()
   @param    s1      Length of the triangle that is opposite vertex at end of arrow
   @param    s2      Length of other two triangle sides
   @param    outlineColor See Button_TT_Base::initButton()
   @param    fillColor    See Button_TT_Base::initButton()
   @param    expU    See Button_TT_Base::initButton()
   @param    expD    See Button_TT_Base::initButton()
   @param    expL    See Button_TT_Base::initButton()
   @param    expR    See Button_TT_Base::initButton()
  */
  /**************************************************************************/
  void initButton(Adafruit_GFX* gfx=0,
                  char orient='U', const char* align="C",
                  int16_t x=0, int16_t y=0, uint16_t s1=0, uint16_t s2=0,
                  uint16_t outlineColor=0, uint16_t fillColor=0,
                  uint8_t expU=0, uint8_t expD=0, uint8_t expL=0, uint8_t expR=0);

  /**************************************************************************/
  /*!
   @brief    Return the button's orientation setting (orient argument to init)
   @returns  Single character giving orientation, U, D, L, or R
  */
  /**************************************************************************/
  char getOrientation(void) { return(_orient); }

  /**************************************************************************/
  /*!
   @brief    Draw the button on the screen
   @param    inverted   Whether to draw with fill/outline swapped to indicate
                        'pressed'
  */
  /**************************************************************************/
  using Button_TT_Base::drawButton;
  virtual void drawButton(bool inverted) override;

};

#endif // Button_TT_arrow_h
