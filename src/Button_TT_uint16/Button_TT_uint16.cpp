/*
  Button_TT_uint16.cpp - Library for adding a variable of type int16_t to a
  button. Created by Ted Toal, July 5, 2023. Released into the public domain.
*/
#include <Arduino.h>
#include <Button_TT_uint16.h>

/**************************************************************************/

void Button_TT_uint16::initButton(
    Adafruit_GFX *gfx, const char *align, int16_t x, int16_t y, int16_t w,
    int16_t h, uint16_t outlineColor, uint16_t fillColor, uint16_t textColor,
    const char *textAlign, uint8_t textSize_x, uint8_t textSize_y,
    const GFXfont *f, int16_t rCorner, uint16_t value, uint16_t minValue,
    uint16_t maxValue, const char *zeroString, bool degreeSym,
    uint16_t (*checkValue)(Button_TT_uint16 &btn, uint16_t value), uint8_t expU,
    uint8_t expD, uint8_t expL, uint8_t expR) {
  _value = value;
  _minValue = minValue;
  _maxValue = maxValue;
  _zeroString = zeroString;
  _checkValue = checkValue;

  Button_TT::initButton(
      gfx); // Pre-initialize base class with null values except gfx.

  if (gfx == 0)
    return;

  // For the initial value of the label, find the longest value in the range
  // minValue..maxValue and use that. The label might actually be out of that
  // range a bit, but it doesn't matter. We set 'value' as the actual button
  // value (including its label) AFTER calling Button_TT::initButton below.
  char widestValStr[12];
  uint16_t W, H;
  gfx->setTextSize(textSize_x, textSize_y);
  gfx->setFont(f);
  Button_TT::getWidestValue(minValue, maxValue, widestValStr, W, H);
  Button_TT::initButton(gfx, align, x, y, w, h, outlineColor, fillColor,
                        textColor, textAlign, widestValStr, degreeSym,
                        textSize_x, textSize_y, f, rCorner, expU, expD, expL,
                        expR);

  // Now set the correct value, applying limits.
  setValue(value);
}

/**************************************************************************/

bool Button_TT_uint16::setValue(uint16_t value, bool dontCheck) {
  if (value < _minValue)
    value = _minValue;
  if (value > _maxValue)
    value = _maxValue;
  if (!dontCheck && _checkValue != NULL)
    value = _checkValue(*this, value);
  if (value == _value)
    return (false);
  _value = value;
  if (_value == 0 && _zeroString != NULL)
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

bool Button_TT_uint16::setValueAndDrawIfChanged(uint16_t value,
                                                bool forceDraw) {
  setValue(value);
  if (_changedSinceLastDrawn || forceDraw) {
    drawButton();
    return (true);
  }
  return (false);
}

/**************************************************************************/

bool Button_TT_uint16::valueIncDec(int16_t N, Button_TT_Base *btn) {
  if (btn != NULL && btn->delta() != 0)
    N = btn->delta();
  uint16_t newValue;
  uint16_t Nt;
  if (N >= 0) {
    Nt = (uint16_t)N;
    newValue = (_value > _maxValue - Nt) ? _maxValue : _value + Nt;
  } else {
    Nt = (uint16_t)-N;
    newValue = (_value < _minValue + Nt) ? _minValue : _value - Nt;
  }
  return (setValueAndDrawIfChanged(newValue));
}

// -------------------------------------------------------------------------