/*
  Buttons.ino - A program to illustrate various buttons and button functionality
  using the Button_TT library. A #define lets you select how many features to
  enable.
  Created by Ted Toal, August 8, 2023.
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


  Usage:

  To use this, set the SELECT #define below to choose which features to enable
  and set the _PIN #defines below for your system, then compile, load, run, and
  view the result, using the touchpad for interaction with buttons.

  This requires that your display use an ILI9341 controller. If you have a
  different display controller and have a compatible controller class derived
  from Adafruit_GFX, you can simply change Adafruit_ILI9341 to that class name
  within this file to switch to that display.

  If the SAMD architecture is being used (ARDUINO_ARCH_SAMD), this uses the
  SAMD_PWM library to produce a beep sound when a button is tapped.

  Note: if you copy this code to use in your project, you probably want to
  delete all references to SELECT.
*/

// Include files needed for any button functionality. Include <monitor_printf.h>
// only if you wish to turn on Button_TT debugging (see BUTTON_TT_DBG in file
// Button_TT.h).
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <monitor_printf.h>
#include <Button_TT.h>
#include <Adafruit_ILI9341.h>

// Set SELECT to one of the following values to enable that functionality and
// all lower-numbered functionality. Start at 0 and work your way up.
//  0   Just a rectangular button with no text and no response to a tap.
//  1   Add a button with text in it, still no response to a tap.
//  2   Respond to tap by changing fill color or changing text.
//  3   Play a tone while button is tapped, if SAMD architecture is used.
//  4   Add a button with an integer value in it. Tapping it increments the value.
//  5   Add arrow buttons that increment/decrement the numeric value.
//  6   Add a button with an unsigned integer in it, with arrow buttons.
//  7   Add LCD backlight timer.
//  8   Add non-volatile memory storage of numeric button values if SAMD architecture.
//  9   Add touchscreen calibration, storing it non-volatilely if SAMD architecture.
//  10  Add a new button class for a square button that displays a "+" in it.
#define SELECT  0

/******************************************************************************/

// Include files when button tap response is enabled.
#if SELECT >= 2
#include <XPT2046_Touchscreen_TT.h>
#include <TS_Display.h>
#include <Button_TT_collection.h>
#endif

// Include files when any button with text is used.
#if SELECT >= 1
#include <Font_TT.h>
#include <Button_TT_label.h>
#endif

// When any button with text is used, include one or more font files.
#if SELECT >= 1
#include <Fonts/FreeSans12pt7b.h>
#endif

// Include file for SAMD beeper tones (requires SAMD architecture).
#if SELECT >= 3
#ifdef ARDUINO_ARCH_SAMD
#define _PWM_LOGLEVEL_ 0
#include <SAMD_PWM.h>
#elif SELECT == 3
#error SAMD architecture is required to play a beep!
#endif
#endif

// Include file for using triangle buttons, usually used as arrow buttons.
#if SELECT >= 5
#include <Button_TT_arrow.h>
#endif

// Include file for using button with int8_t value.
#if SELECT >= 4
#include <Button_TT_int8.h>
#endif

// Include file for using button with uint8_t value.
#if SELECT >= 6
#include <Button_TT_uint8.h>
#endif

// Include file for using EEPROM to store values (requires SAMD architecture).
#if SELECT >= 8
// EEPROM support currently only if SAMD architecture.
#ifdef ARDUINO_ARCH_SAMD

// It appears (page 29 of Atmel SAM D21E / SAM D21G / SAM D21J data sheet) that
// the EEPROM page size is 64, and 4 pages must be erased at one time, giving an
// effective page size of 4*64 = 256.  This seems to be confirmed by the fact
// that FlashStorage_SAMD.hpp aligns the PPCAT variable to 256 bytes.
// Use that minimum of four pages for our non-volatile storage.
#define EEPROM_EMULATION_SIZE     (4 * 64)

// Turn off debug messages.
#define FLASH_DEBUG       0   // Use 0-2. Larger for more debugging messages

// Now include flash storage library header file.
// To be included only in one file to avoid `Multiple Definitions` Linker Error.
#include <FlashStorage_SAMD.h>

#else
#error SAMD architecture is required to use EEPROM memory!
#endif
#endif

/******************************************************************************/

// LCD display pin definitions.
#define LCD_DC_PIN          2
#define LCD_CS_PIN          10
#define LCD_BACKLIGHT_PIN   A2

#if SELECT >= 2
// Touchscreen pin definitions.
#define TOUCH_CS_PIN        A0
#define TOUCH_IRQ_PIN       A7
#endif

#if SELECT >= 3
// Beeper pin definition.
#define BEEPER_PIN          A3
#endif

// LCD display backlight definitions.
#define LCD_BACKLIGHT_OFF   HIGH
#define LCD_BACKLIGHT_ON    LOW
#if SELECT >= 7
// Number of milliseconds after last screen touch before backlight is turned off.
#define LCD_BACKLIGHT_AUTO_OFF_MS (15*1000)
#endif

#if SELECT >= 3
// Beeper tone definitions (requires SAMD architecture).
#define TS_TONE_FREQ        3000  // Frequency to play when touch screen is tapped.
#define TS_TONE_DUTY        50    // Duty cycle in % of square wave tone, 0 for off.
#endif

// Define constants for using EEPROM to store values (requires SAMD architecture).
#if SELECT >= 8
#ifdef ARDUINO_ARCH_SAMD

// Signature used at start of a flash memory block to mark the block as
// containing valid data written by the application.
const int WRITTEN_SIGNATURE = 0xBEEFDEED;

#endif
#endif

/******************************************************************************/

// LCD display object.
Adafruit_ILI9341* lcd;

#if SELECT >= 3
// PWM object for sound from beeper (requires SAMD architecture).
#ifdef ARDUINO_ARCH_SAMD
SAMD_PWM* sound;
#endif
#endif

// Variables required to support touchscreen and button taps.
#if SELECT >= 2

// Touchscreen object.
XPT2046_Touchscreen* touch;

// Touchscreen-display object.
TS_Display* ts_display;

// Button collection object to manage taps of buttons.
Button_TT_collection* screenButtons;

#endif

// When any button with text is used, create a Font_TT object for each font used.
#if SELECT >= 1
Font_TT font12(&FreeSans12pt7b);
#endif

// Variables for supporting backlight timer.
#if SELECT >= 7
// This timer serves for turning off the LCD backlight a bit after the user
// finishes using the touchscreen. Each time the user does a screen touch, the
// LCD backlight is turned on if it was off. Each time the user releases his
// touch on the screen, this variable is zeroed. At regular updates when the
// screen is not being touched, this is counted upwards by milliseconds to
// LCD_BACKLIGHT_AUTO_OFF_MS, and when it reaches that, the backlight is turned
// off and this stops counting and is not reset.
uint32_t MSsinceLastTouchBeforeBacklight;

// In order to be able to update the value of MSsinceLastTouchBeforeBacklight
// periodically, the following variable records the milliseconds time (millis())
// at which the last update was made to it, so that when the next update time
// occurs, the number of elapsed milliseconds can be determined and used to do
// the update.
uint32_t MSatLastBacklightTimerUpdate;
#endif

/******************************************************************************/

// Define an enum that enumerates the different screens of the system.

// Display screens.
typedef enum _eScreen {
  SCREEN_MAIN
  #if SELECT >= 9
  , SCREEN_CALIBRATION
  #endif
} eScreen;

// Currently displayed screen.
eScreen currentScreen;

// Declare functions for initializing each screen.
void initMainScreen(void);
void initCalibrationScreen(void);

// Initialize all screens.
void initScreens() {
  initMainScreen();
  #if SELECT >= 9
  initCalibrationScreen();
  #endif
}

// Declare functions for drawing each screen.
void drawMainScreen();
void drawCalibrationScreen(int state=1);

// Draw the current screen.
void drawCurrentScreen() {
  switch (currentScreen) {
  case SCREEN_MAIN:
    drawMainScreen();
    break;
  #if SELECT >= 9
  case SCREEN_CALIBRATION:
    drawCalibrationScreen();
    break;
  #endif
  }
}

// Declare functions for handling loop() processing tasks for each screen.
void loopMainScreen();
void loopCalibrationScreen();

// Handle loop() processing tasks for the current screen.
void loopCurrentScreen() {
  switch (currentScreen) {
  case SCREEN_MAIN:
    loopMainScreen();
    break;
  #if SELECT >= 9
  case SCREEN_CALIBRATION:
    loopCalibrationScreen();
    break;
  #endif
  }
}

/******************************************************************************/

#if SELECT >= 8
// Structure containing non-volatile data to be stored in flash memory (with a
// copy in regular memory). We use this structure even if we don't have the SAMD
// architecture support for storing it in EEPROM.
struct nonvolatileSettings {
  int8_t int8val;       // btn_int8Val value
  uint8_t uint8val;     // btn_uint8Val value
  #if SELECT >= 9
  int16_t TS_LR_X;      // Touchscreen calibration parameters.
  int16_t TS_LR_Y;
  int16_t TS_UL_X;
  int16_t TS_UL_Y;
  #endif
};

// The current non-volatile settings initialized from flash-based EEPROM (and
// written back to EEPROM when the data changes).
nonvolatileSettings NVsettings;

// The default non-volatile settings when the settings need to be initialized.
nonvolatileSettings defaults = {
    0,      // int8val
    0       // uint8val
};

#ifdef ARDUINO_ARCH_SAMD

// Predeclare following function to work around Arduino IDE bug.
void readNonvolatileSettings(nonvolatileSettings& settings,
    const nonvolatileSettings& defaults);

// Read non-volatile settings from flash memory into 'settings'.  If flash
// memory has not yet been initialized, initialize it with 'defaults'.
void readNonvolatileSettings(nonvolatileSettings& settings,
    const nonvolatileSettings& defaults) {
  // Initialize to only commit data when we call the commit function.
  EEPROM.setCommitASAP(false);
  // Check signature at address 0.
  int signature;
  uint16_t storedAddress = 0;
  EEPROM.get(storedAddress, signature);
  // If flash-based EEPROM is empty, write WRITTEN_SIGNATURE and defaults to it.
  if (signature != WRITTEN_SIGNATURE) {
    monitor.printf("EEPROM is uninitialized, writing defaults\n");
    EEPROM.put(storedAddress, WRITTEN_SIGNATURE);
    EEPROM.put(storedAddress + sizeof(signature), defaults);
    EEPROM.commit();
  }
  // Read settings data from flash-based EEPROM.
  EEPROM.get(storedAddress + sizeof(signature), settings);
}

// Predeclare following function to work around Arduino IDE bug.
bool writeNonvolatileSettingsIfChanged(nonvolatileSettings& settings);

// Write 'settings' to flash memory IF IT HAS CHANGED. Return true if it changed
// and was written, else false.
bool writeNonvolatileSettingsIfChanged(nonvolatileSettings& settings) {
  int signature;
  uint16_t storedAddress = 0;
  nonvolatileSettings tmp;
  EEPROM.get(storedAddress + sizeof(signature), tmp);
  if (memcmp(&settings, &tmp, sizeof(nonvolatileSettings)) == 0)
    return(false);
  EEPROM.put(storedAddress + sizeof(signature), settings);
  EEPROM.commit();
  return(true);
}

#endif
#endif

/******************************************************************************/

// Useful constants for initializing buttons. These can be used as arguments to
// the initButton() function.

// Corner radius of buttons with rounded corners, in pixels.
#define RAD 10

// Define a "standard" button width and height.
#define BTN_WIDTH  110
#define BTN_HEIGHT 40

// Negative button width/height values to use when creating buttons for the LCD
// display. A negative width or height means that the button is sized for the
// initial text label supplied for it, with the absolute value of the negative
// width or height ADDED to the text size to get the full button size.
#define ZEW 0   // Zero edge width
#define TEW -1  // Tiny edge width
#define SEW -5  // Small edge width
#define MEW -10 // Medium edge width
#define LEW -20 // Large edge width
#define HEW -30 // Huge edge width

// Values for expU, expD, expL, expR for button constructors, to expand the
// touch zone beyond the button outline.
#define EXP_T 5   // Tiny expansion of button
#define EXP_S 10  // Small expansion of button
#define EXP_M 20  // Medium expansion of button
#define EXP_L 30  // Large expansion of button
#define EXP_H 50  // Huge expansion of button

/******************************************************************************/

#if SELECT >= 10
// A new class for a square button containing a "+" in the middle. The width and
// height of the button are equal (w). If the "+" arm length is 0, 90% of w/2 is
// used for length. If the "+" arm width is 0, 10% of w/2 is used for width.
class Button_TT_plus : public Button_TT {

protected:
  uint8_t _armLen;      // Length of each plus arm.
  uint8_t _armWidth;    // Width (thickness) of each plus arm.
  uint16_t _plusColor;  // Color of plus.

public:

  Button_TT_plus(const char* name, Adafruit_GFX* gfx = 0,
      const char* align = "C", int16_t x = 0, int16_t y = 0, uint16_t w = 0,
      uint8_t armLen = 0, uint8_t armWidth = 0, uint16_t plusColor = 0,
      uint16_t outlineColor = 0, uint16_t fillColor = 0, uint8_t expU = 0,
      uint8_t expD = 0, uint8_t expL = 0, uint8_t expR = 0) : Button_TT(name) {
    initButton(gfx, align, x, y, w, armLen, armWidth, plusColor,
      outlineColor, fillColor, expU, expD, expL, expR);
  }

  void initButton(Adafruit_GFX* gfx = 0, const char* align = "C", int16_t x = 0,
      int16_t y = 0, uint16_t w = 0, uint8_t armLen = 0, uint8_t armWidth = 0,
      uint16_t plusColor = 0, uint16_t outlineColor = 0, uint16_t fillColor = 0,
      uint8_t expU = 0, uint8_t expD = 0, uint8_t expL = 0, uint8_t expR = 0);

  uint8_t getArmLen(void) { return (_armLen); }
  bool setArmLen(uint8_t armLen);
  uint8_t getArmWidth(void) { return (_armWidth); }
  bool setArmWidth(uint8_t armWidth);
  uint16_t getPlusColor(void) { return (_plusColor); }
  bool setPlusColor(uint8_t plusColor);
  using Button_TT::drawButton;
  virtual void drawButton(bool inverted) override;
};

void Button_TT_plus::initButton(Adafruit_GFX* gfx, const char* align,
    int16_t x, int16_t y, uint16_t w, uint8_t armLen, uint8_t armWidth,
    uint16_t plusColor, uint16_t outlineColor, uint16_t fillColor, uint8_t expU,
    uint8_t expD, uint8_t expL, uint8_t expR) {

  _armLen = (armLen > 0) ? armLen : (uint8_t)(1+9*w/2/10);
  _armWidth = (armWidth > 0) ? armWidth : (uint8_t)(1+w/2/10);
  _plusColor = plusColor;
  Button_TT::initButton(gfx, align, x, y, w, w, outlineColor, fillColor, expU,
    expD, expL, expR);
}

bool Button_TT_plus::setArmLen(uint8_t armLen) {
  if (_armLen != armLen) {
    _armLen = armLen;
    _changedSinceLastDrawn = true;
    return (true);
  }
  return (false);
}

bool Button_TT_plus::setArmWidth(uint8_t armWidth) {
  if (_armWidth != armWidth) {
    _armWidth = armWidth;
    _changedSinceLastDrawn = true;
    return (true);
  }
  return (false);
}

bool Button_TT_plus::setPlusColor(uint8_t plusColor) {
  if (_plusColor != plusColor) {
    _plusColor = plusColor;
    _changedSinceLastDrawn = true;
    return (true);
  }
  return (false);
}

void Button_TT_plus::drawButton(bool inverted) {
  Button_TT::drawButton(inverted);
  if (_plusColor != TRANSPARENT_COLOR) {
    int16_t x, y, l, t;
    x = _xL + _w/2;
    y = _yT + _h/2;
    l = 2*_armLen+1;
    t = _armWidth/2;
    _gfx->fillRect(x-t, y-_armLen, _armWidth, l, _plusColor);
    _gfx->fillRect(x-_armLen, y-t, l, _armWidth, _plusColor);
  }
  _changedSinceLastDrawn = false;
}
#endif

/******************************************************************************/
// MAIN SCREEN buttons and handler functions.
//
// The Main screen shows the buttons selected by SELECT.
/******************************************************************************/

// To create a button, create a variable of type Button_TT or a derived class.
// The button name can be anything and is unrelated to text on a button (it is
// useful at times for debugging).

// A plain button with no text, whose name is "Simple".
Button_TT btn_Simple("Simple");

// When a button responds to taps, it needs a "btnTap_" function. Although in
// general each button needs such a function, in some cases it makes sense to
// use the same function for multiple buttons, and in that case the "btn"
// argument indicates which button was actually tapped.
#if SELECT >= 2
// Handle tap of "btn_Simple". Toggle button fill color and draw the button.
void btnTap_Simple(Button_TT& btn) {
  uint16_t color = btn_Simple.getFillColor();
  btn_Simple.setFillColor(color == ILI9341_BLUE ? ILI9341_RED : ILI9341_BLUE);
  btn_Simple.drawButton();
}
#endif

/******************************************************************************/

// Another button, using variable name "btn_Hello", and this time named "Hello".

#if SELECT >= 1
// A labelled button whose name is "Hello".
Button_TT_label btn_Hello("Hello");

#if SELECT >= 2
// Handle tap of "btn_Hello". Toggle its text and draw it.
void btnTap_Hello(Button_TT& btn) {
  const char* label = btn_Hello.getLabel();
  bool isHello = (label[0] == 'H');
  btn_Hello.setLabel(!isHello ? "Hello World!" : "Goodbye World!");
  btn_Hello.drawButton();
}
#endif
#endif

/******************************************************************************/

#if SELECT >= 4
// An int8_t button whose name is "int8Val".
Button_TT_int8 btn_int8Val("int8Val");

// Handle tap of "btn_int8Val". Increment the value and wrap from max to min.
void btnTap_int8Val(Button_TT& btn) {
  int8_t value = btn_int8Val.getValue() + 1;
  int8_t minValue = btn_int8Val.getMinValue();
  int8_t maxValue = btn_int8Val.getMaxValue();
  if (value > maxValue)
    value = minValue;
  btn_int8Val.setValueAndDrawIfChanged(value);
}

#if SELECT >= 5
// Also define two arrow buttons (left and right, for decrement and increment)
// to go with btn_int8Val.
Button_TT_arrow btn_int8Val_left("int8Val_left");
Button_TT_arrow btn_int8Val_right("int8Val_right");

// The tap function is shared by both arrow buttons. It invokes valueIncDec()
// on the int8val button.
void btnTap_int8Val(Button_TT& btn) {
  int8 value = btn_int8Val.getValue() + 1;
  int8 minValue = btn_int8Val.getMinValue();
  int8 maxValue = btn_int8Val.getMaxValue();
  if (value > maxValue)
    value = minValue;
  btn_int8Val.setValueAndDrawIfChanged(value);
}
#endif
#endif

/******************************************************************************/

#if SELECT >= 6
// A uint8_t button whose name is "uint8Val".
// There is no tap handler for this button, tapping it does nothing. It changes
// its value when one of the arrow buttons to its right is tapped.
Button_TT_uint8 btn_uint8Val("uint8Val");

// Also define two arrow buttons (left and right, for decrement and increment)
// to go with btn_uint8Val.
Button_TT_arrow btn_uint8Val_left("uint8Val_left");
Button_TT_arrow btn_uint8Val_right("uint8Val_right");

// The tap function is shared by both arrow buttons.
void btnTap_uint8Val_delta(Button_TT& btn) {
  btn_uint8Val.valueIncDec(1, &btn);
}
#endif

/******************************************************************************/

#if SELECT >= 9
// A labelled button whose name is "Calibrate".
Button_TT_label btn_Calibrate("Calibrate");

// Handle tap of "btn_Calibrate". We switch to the calibration screen.
void btnTap_Calibrate(Button_TT& btn) {
  currentScreen = SCREEN_CALIBRATION;
  drawCurrentScreen();
}
#endif

/******************************************************************************/

#if SELECT >= 10
// A "+" button created using the Button_TT_plus class defined above.
Button_TT_plus btn_Plus("Plus");
#endif

/******************************************************************************/

// Support for button tones (requires SAMD architecture).
#if SELECT >= 3
// Play (true) or stop playing (false) a sound.
void playSound(bool play) {
  #ifdef ARDUINO_ARCH_SAMD
  sound->setPWM(BEEPER_PIN, TS_TONE_FREQ, play ? TS_TONE_DUTY : 0);
  #endif
}

// Master button tap/release function. Tap: start tone. Release: end tone.
void buttonTapRelease(bool tap) {
  playSound(tap);
}
#endif

// Button tap and release testing. This uses the screenButtons object press()
// and release() functions to handle a tap or release. It finds the tapped or
// released button and calls the button's press() or release() function.
#if SELECT >= 2
// Check for touch screen tap or release. If so, use ScreenButtons to process
// the tap or release.
void processTapsAndReleases() {
  int16_t x, y, pres;

  // Check for a button tap or release event.
  switch (ts_display->getTouchEvent(x, y, pres)) {

  #if SELECT >= 7
  // Handle LCD backlight auto on/off.

  // When screen is not being touched or uncertain about touch, update the
  // backlight timer and if it times out, turn off the backlight.
  case TS_NO_TOUCH:
  case TS_UNCERTAIN:
    if (MSsinceLastTouchBeforeBacklight < LCD_BACKLIGHT_AUTO_OFF_MS) {
      uint32_t MS = millis();
      uint32_t elapsedMS = MS - MSatLastBacklightTimerUpdate;
      MSatLastBacklightTimerUpdate = MS;
      MSsinceLastTouchBeforeBacklight += elapsedMS;
      if (MSsinceLastTouchBeforeBacklight >= LCD_BACKLIGHT_AUTO_OFF_MS) {
        MSsinceLastTouchBeforeBacklight = LCD_BACKLIGHT_AUTO_OFF_MS;
        digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_OFF);
      }
    }
    break;

  // As long as a touch is present, reset backlight timer.
  case TS_TOUCH_PRESENT:
    MSsinceLastTouchBeforeBacklight = 0;
    MSatLastBacklightTimerUpdate = millis();
    break;
  #endif

  // Handle a touch event.
  case TS_TOUCH_EVENT:
    #if SELECT >= 7
    // When backlight auto on/off is supported, touch events are ignored when
    // the backlight is off.

    // Check for a button tap and if so, call its tap function. Touch events turn
    // on the backlight if off, else are processed as possible screen button taps.
    if (digitalRead(LCD_BACKLIGHT_PIN) == LCD_BACKLIGHT_OFF)
      digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);
    else
      screenButtons->press(x, y);
    #else
    // Check for a button tap and if so, call its tap function.
    screenButtons->press(x, y);
    #endif
    break;

  // Handle a release event.
  case TS_RELEASE_EVENT:
    #if SELECT >= 7
    // Release events reset the backlight timer.
    MSsinceLastTouchBeforeBacklight = 0;
    MSatLastBacklightTimerUpdate = millis();
    #endif
    // Check for a button release. Only one button press at a time can be detected.
    screenButtons->release();
    break;
  }
}
#endif

/******************************************************************************/

// It is convenient to define a function to initialize each screen, because
// typically you will have multiple different screens, and providing a separate
// initialization function for each one keeps things cleaner.

// Initialize the main screen.
void initMainScreen(void) {

  // Initialize btn_Simple with Top Center at (120,5) and size 40 wide x 20 tall,
  // with black outline and blue interior.
  btn_Simple.initButton(lcd, "TC", 120, 5, 40, 20, ILI9341_BLACK, ILI9341_BLUE);

  #if SELECT >= 1
  // Initialize btn_Hello.
  btn_Hello.initButton(lcd, "TC", 120, 30, 200, 26, ILI9341_BLACK,
    ILI9341_LIGHTGREY, ILI9341_BLUE, "C", "Hello World!", false, &font12);
  #endif

  #if SELECT >= 4
  // Initialize btn_int8Val. Give it rounded corners, with initial value -5 and range -10 to +10.
  btn_int8Val.initButton(lcd, "TL", 35, 68, 50, 26, ILI9341_BLACK, ILI9341_LIGHTGREY,
    ILI9341_RED, "C", &font12, RAD, -5, -10, +10);
  #if SELECT >= 8
  // Initialize integer button values.
  btn_int8Val.setValue(NVsettings.int8val);
  #endif
  #if SELECT >= 5
  // Initialize arrow buttons for int8val button.
  btn_int8Val_left.initButton(lcd, 'L', "TR", 120, 65, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);
  btn_int8Val_right.initButton(lcd, 'R', "TL", 130, 65, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);
  #endif
  #endif

  #if SELECT >= 6
  // Initialize btn_uint8Val. Give it rounded corners, with initial value 10 and
  // range 0 to 10, and add a degree symbol after it. Display "--" for 0.
  btn_uint8Val.initButton(lcd, "TL", 35, 113, 50, 26, ILI9341_BLACK, ILI9341_LIGHTGREY,
    ILI9341_RED, "C", &font12, RAD, +10, 0, +10, true, "--");
  #if SELECT >= 8
  // Initialize integer button values.
  btn_uint8Val.setValue(NVsettings.uint8val);
  #endif

  // Initialize arrow buttons for uint8val button.
  btn_uint8Val_left.initButton(lcd, 'L', "TR", 120, 110, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);
  btn_uint8Val_right.initButton(lcd, 'R', "TL", 130, 110, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);
  #endif

  #if SELECT >= 9
  // Initialize btn_Calibrate to be a standard-sized button with rounded corners.
  btn_Calibrate.initButton(lcd, "CC", 120, 230, BTN_WIDTH, BTN_HEIGHT, ILI9341_BLACK,
    ILI9341_PINK, ILI9341_BLACK, "C", "Calibrate", false, &font12, RAD);
  #endif

  #if SELECT >= 10
  // Initialize btn_Plus.
  btn_Plus.initButton(lcd, "CC", 120, 175, 35, 0, 0, ILI9341_RED, TRANSPARENT_COLOR, TRANSPARENT_COLOR);
  #endif
}

// It is ALSO convenient to define a function to draw the screen, because
// typically you will have multiple different screens that you will want to be
// able to switch between.

// Draw the main screen.
void drawMainScreen() {

  #if SELECT >= 9
  // Clear all existing button registrations for tap detection.
  screenButtons->clear();
  #endif

  // Fill screen with white.
  lcd->fillScreen(ILI9341_WHITE);

  // Draw screen buttons.
  #if SELECT >= 2
  // Also, register buttons with screenButtons object to handle button taps/releases.
  #endif

  btn_Simple.drawButton();
  #if SELECT >= 2
  screenButtons->registerButton(btn_Simple, btnTap_Simple);
  #endif

  #if SELECT >= 1
  btn_Hello.drawButton();
  #if SELECT >= 2
  screenButtons->registerButton(btn_Hello, btnTap_Hello);
  #endif
  #endif

  #if SELECT >= 4
  btn_int8Val.drawButton();
  screenButtons->registerButton(btn_int8Val, btnTap_int8Val);
  #endif
  #if SELECT >= 5
  btn_int8Val_left.drawButton();
  btn_int8Val_right.drawButton();
  screenButtons->registerButton(btn_int8Val_left, btnTap_int8Val_delta);
  screenButtons->registerButton(btn_int8Val_right, btnTap_int8Val_delta);
  #endif

  #if SELECT >= 6
  btn_uint8Val.drawButton();
  btn_uint8Val_left.drawButton();
  btn_uint8Val_right.drawButton();
  screenButtons->registerButton(btn_uint8Val_left, btnTap_uint8Val_delta);
  screenButtons->registerButton(btn_uint8Val_right, btnTap_uint8Val_delta);
  #endif

  #if SELECT >= 9
  btn_Calibrate.drawButton();
  screenButtons->registerButton(btn_Calibrate, btnTap_Calibrate);
  #endif

  #if SELECT >= 10
  btn_Plus.drawButton();
  #endif
}

// It is ALSO convenient to define a function to perform any operations required
// by the screen, because typically you will have multiple different screens and
// each will require its own unique processing.

// Handle loop() processing for the main screen.
void loopMainScreen() {

  // Currently the only thing that needs to be done in the main screen is to
  // save current non-volatile values in non-volatile memory if they have
  // changed.

  #if SELECT >= 8
  #ifdef ARDUINO_ARCH_SAMD
  // Save current values from btn_int8Val and btn_uint8Val to NVsettings.
  NVsettings.int8val = btn_int8Val.getValue();
  NVsettings.uint8val = btn_uint8Val.getValue();
  // Save NVsettings to EEPROM, which only gets written if the settings actually changed.
  writeNonvolatileSettingsIfChanged(NVsettings);
  #endif
  #endif
}

/******************************************************************************/

#if SELECT >= 9

// CALIBRATION SCREEN buttons and handler functions.
//
// The Calibration screen initially shows a "Calibrate" label at the top of the
// screen, a Cancel button, a button with text saying to touch the "+", and a
// single "+" in one corner. When Cancel is touched the screen exits back to the
// main screen without changing the calibration setting. If the "+" is touched,
// it is erased and a second "+" in the opposite corner is displayed. If that
// "+" is also touched, it is erased, calibration settings are recomputed and
// temporarily changed, a Save button is shown along with the Cancel button,
// button text is changed to say to touch anywhere to test calibration, and
// subsequent touches cause a "+" of another color to be drawn at that position.
// Touching Cancel reverts to original calibration settings, while touching Save
// saves the new calibration settings to nonvolatile memory and it exits back to
// the main screen.

Button_TT_label label_Calibration("CalibrationScreen");
Button_TT_label label_Touch("CalibrationTouch");
Button_TT_label btn_CalibrationCancel("CalibrationCancel");
Button_TT_label btn_CalibrationSave("CalibrationSave");

// Initialize the calibration screen.
void initCalibrationScreen(void) {

  label_Calibration.initButton(lcd, "TC", 120, 5, TEW, TEW, TRANSPARENT_COLOR,
    TRANSPARENT_COLOR, ILI9341_DARKGREEN, "C", "Calibrate", false, &font12);
  label_Touch.initButton(lcd, "TL", 10, 30, 220, TEW, TRANSPARENT_COLOR,
    TRANSPARENT_COLOR, ILI9341_RED, "TL", "Tap the +", false, &font12);
  btn_CalibrationCancel.initButton(lcd, "BL", 5, 313, BTN_WIDTH, BTN_HEIGHT,
    ILI9341_BLACK, ILI9341_PINK, ILI9341_BLACK, "C", "Cancel", false, &font12, RAD);
  btn_CalibrationSave.initButton(lcd, "BR", 235, 313, BTN_WIDTH, BTN_HEIGHT,
    ILI9341_BLACK, ILI9341_PINK, ILI9341_BLACK, "C", "Save", false, &font12, RAD);
}

// Length of each arm of "+" sign.
#define PLUS_ARM_LEN 10

// Draw a plus sign at a specified display location.
void drawPlus(int16_t x, int16_t y, int16_t color, uint8_t len = PLUS_ARM_LEN) {
  lcd->drawFastVLine(x, y-len, 2*len+1, color);
  lcd->drawFastHLine(x-len, y, 2*len+1, color);
}

// Button tap handlers for the Calibration screen.

// Handle tap of Cancel button in Calibration screen. We revert calibration
// parameters to the original and switch back to main screen.
void btnTap_CalibrationCancel(Button_TT& btn) {
  ts_display->setTS_calibration(NVsettings.TS_LR_X, NVsettings.TS_LR_Y,
    NVsettings.TS_UL_X, NVsettings.TS_UL_Y);
  currentScreen = SCREEN_MAIN;
  drawMainScreen();
}

// Handle tap of Save button in Calibration screen. We save current calibration
// parameters into "NVsettings" and switch back to the main screen.
void btnTap_CalibrationSave(Button_TT& btn) {
  ts_display->getTS_calibration(&NVsettings.TS_LR_X, &NVsettings.TS_LR_Y,
    &NVsettings.TS_UL_X, &NVsettings.TS_UL_Y);
  currentScreen = SCREEN_MAIN;
  drawMainScreen();
}

// States during calibration and subsequent showing of tapped points.
typedef enum _eCalibState {
  STATE_WAIT_UL,            // Wait for user to tap + at upper-left
  STATE_WAIT_UL_RELEASE,    // Wait for user to release the tap
  STATE_WAIT_LR,            // Wait for user to tap + at lower-right
  STATE_WAIT_LR_RELEASE,    // Wait for user to release the tap
  STATE_WAIT_POINT_SHOW_IT, // Wait for user to tap anywhere, then draw "+" there
  STATE_WAIT_RELEASE        // Wait for user to release the tap
} eCalibState;

// Current state of calibration screen interaction with user.
eCalibState calibState;

// Display UL and LR calibration positions and corresponding touchscreen
// calibration coordinates.
int16_t x_UL, y_UL, x_LR, y_LR;
int16_t TSx_UL, TSy_UL, TSx_LR, TSy_LR;

// Draw the Calibration screen and register its buttons with the screenButtons
// object. Argument "state" defaults to 1 and is either 1, 2, or 3:
//      1: initial display, draw first +, "tap +", no Save, set STATE_WAIT_UL
//      2: finished STATE_WAIT_UL_RELEASE, draw second +, "tap +", no Save
//      3: finished STATE_WAIT_LR_RELEASE, draw no +, draw Save, "tap to test"
void drawCalibrationScreen(int state) {

  // Clear all existing button registrations.
  screenButtons->clear();

  // Fill screen with white.
  lcd->fillScreen(ILI9341_WHITE);

  // Get position of the two corner display points at which to draw "+" signs
  // to be tapped.
  ts_display->GetCalibration_UL_LR(PLUS_ARM_LEN+2, &x_UL, &y_UL, &x_LR, &y_LR);

  // Draw and register screen buttons.

  // Calibrate label.
  label_Calibration.drawButton();

  // "Touch ..." instruction label
  label_Touch.setLabel(state == 3 ? "Tap to test calibration" : "Tap the +");
  int16_t xL, yT;
  if (state == 1) {
    xL = x_UL;
    yT = y_UL + 2*PLUS_ARM_LEN;
  } else if (state == 2) {
    xL = btn_CalibrationSave.getLeft();
    yT = y_LR - 2*PLUS_ARM_LEN - label_Touch.getHeight();
  } else if (state == 3) {
    xL = (lcd->width() - label_Touch.getWidth())/2;
    yT = (lcd->height() - label_Touch.getHeight())/2;
  }
  label_Touch.setPosition(xL, yT);
  label_Touch.drawButton();

  // Cancel button.
  btn_CalibrationCancel.drawButton();
  screenButtons->registerButton(btn_CalibrationCancel, btnTap_CalibrationCancel);

  // Save button.
  if (state == 3) {
    btn_CalibrationSave.drawButton();
    screenButtons->registerButton(btn_CalibrationSave, btnTap_CalibrationSave);
  }

  // Draw first or second + or none
  if (state == 1)
    drawPlus(x_UL, y_UL, ILI9341_BLUE);
  else if (state == 2)
    drawPlus(x_LR, y_LR, ILI9341_BLUE);

  // When state = 1, we must initialize calibState because state=1 means the
  // user tapped the "Calibrate" button in the main screen, and we need to
  // restart the calibration state machine.
  if (state == 1)
    calibState = STATE_WAIT_UL;
}

// Handle loop() processing for the calibration screen.
// Note that this functions completely independently and in parallel with the
// processTapsAndReleases() function, which also monitors touch screen actions.
void loopCalibrationScreen() {
  boolean isTouched = touch->touched();
  TS_Point p;
  if (isTouched)
    p = touch->getPoint();

  switch (calibState) {

  case STATE_WAIT_UL:
    if (isTouched) {
      // Record the touchscreen coordinates.
      TSx_UL = p.x;
      TSy_UL = p.y;
      // Play sound.
      playSound(true);
      calibState = STATE_WAIT_UL_RELEASE;
    }
    break;

  case STATE_WAIT_UL_RELEASE:
    if (!isTouched) {
      // Stop sound.
      playSound(false);
      // Redraw the screen with second +.
      drawCalibrationScreen(2);
      calibState = STATE_WAIT_LR;
    }
    break;

  case STATE_WAIT_LR:
    if (isTouched) {
      // Record the touchscreen coordinates.
      TSx_LR = p.x;
      TSy_LR = p.y;
      // Play sound.
      playSound(true);
      calibState = STATE_WAIT_LR_RELEASE;
    }
    break;

  case STATE_WAIT_LR_RELEASE:
    if (!isTouched) {
      // Stop sound.
      playSound(false);
      // Map the two tapped touchscreen points to the correct calibration values
      // at the extreme ends of the display.
      int16_t TS_LR_X, TS_LR_Y, TS_UL_X, TS_UL_Y;
      ts_display->findTS_calibration(x_UL, y_UL, x_LR, y_LR, TSx_UL, TSy_UL, TSx_LR, TSy_LR,
        &TS_LR_X, &TS_LR_Y, &TS_UL_X, &TS_UL_Y);
      // Set resulting calibration parameters as the new calibration parameters
      // in ts_display.
      ts_display->setTS_calibration(TS_LR_X, TS_LR_Y, TS_UL_X, TS_UL_Y);
      // Redraw the screen with no +.
      drawCalibrationScreen(3);
      calibState = STATE_WAIT_POINT_SHOW_IT;
    }
    break;

  case STATE_WAIT_POINT_SHOW_IT:
    if (isTouched) {
      // Map touched point to display and draw a green "+" at that point.
      int16_t x, y;
      ts_display->mapTStoDisplay(p.x, p.y, &x, &y);
      drawPlus(x, y, ILI9341_DARKGREEN);
      // Play sound.
      playSound(true);
      calibState = STATE_WAIT_RELEASE;
    }
    break;

  case STATE_WAIT_RELEASE:
    if (!isTouched) {
      // Stop sound.
      playSound(false);
      calibState = STATE_WAIT_POINT_SHOW_IT;
    }
    break;

  }

  // Don't turn off display backlight in calibration mode.
  MSsinceLastTouchBeforeBacklight = 0;
}

#endif

/******************************************************************************/

// Standard Arduino setup() function.
void setup() {
  // Initialize for printfs to serial monitor.
  monitor.begin(&Serial, 115200);
  monitor.printf("Initializing\n");

  #if SELECT >= 3
  // Create PWM object for sound from beeper (requires SAMD architecture).
  #ifdef ARDUINO_ARCH_SAMD
  monitor.printf("SAMD_PWM\n");
  sound = new SAMD_PWM(BEEPER_PIN, TS_TONE_FREQ, 0);
  #endif
  #endif

  // Create LCD object.
  monitor.printf("lcd\n");
  lcd = new Adafruit_ILI9341(LCD_CS_PIN, LCD_DC_PIN);
  // Initialize the LCD display.
  lcd->begin();
  lcd->setRotation(2);   // portrait mode
  lcd->setTextColor(ILI9341_BLUE);
  lcd->setTextSize(1);
  lcd->setTextWrap(false);
  // Turn on backlight.
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);
  #if SELECT >= 7
  // Initialize backlight timer.
  MSsinceLastTouchBeforeBacklight = 0;
  MSatLastBacklightTimerUpdate = millis();
  #endif

  #if SELECT >= 2
  // Create and initialize touchscreen object, same rotation as lcd.
  monitor.printf("touch\n");
  touch = new XPT2046_Touchscreen(TOUCH_CS_PIN, TOUCH_IRQ_PIN);
  touch->begin();
  touch->setRotation(lcd->getRotation());

  // Create and initialize touchscreen-LCD object.
  monitor.printf("ts_display\n");
  ts_display = new(TS_Display);
  ts_display->begin(touch, lcd);

  // Create and initialize button collection object.
  monitor.printf("screenButtons\n");
  screenButtons = new Button_TT_collection;
  #if SELECT >= 3
  // Register master tap/release function to turn tone on/off at screen taps.
  screenButtons->registerMasterProcessFunc(buttonTapRelease);
  #endif
  #endif

  #if SELECT >= 9
  // Make 'defaults' hold the default non-volatile settings when the settings
  // are first initialized. Here, we load it with the initial calibration
  // settings from ts_display.
  ts_display->getTS_calibration(&defaults.TS_LR_X, &defaults.TS_LR_Y, &defaults.TS_UL_X,
    &defaults.TS_UL_Y);
  #endif

  #if SELECT >= 8
  #ifdef ARDUINO_ARCH_SAMD
  // Read non-volatile settings from flash memory (EEPROM) into NVsettings, or
  // initialize EEPROM and NVsettings if EEPROM is uninitialized or corrupted.
  // Note: each time the Arduino IDE stores a program in EEPROM, it erases these
  // settings, so they will be reinitialized when this runs.
  monitor.printf("read EEPROM settings\n");
  readNonvolatileSettings(NVsettings, defaults);
  #endif
  #endif

  // Initialize the display screens.
  monitor.printf("initialize screens\n");
  initScreens();

  // Set the initial screen to be displayed and draw it.
  monitor.printf("draw initial screen\n");
  currentScreen = SCREEN_MAIN;
  drawCurrentScreen();

  monitor.printf("exiting setup\n");
}

/******************************************************************************/

// Standard Arduino loop() function.
void loop() {

  #if SELECT >= 2
  // Process button taps/releases for the current screen, which has registered
  // its buttons with the screenButtons object.
  processTapsAndReleases();
  #endif

  // Handle loop() processing for the current screen.
  loopCurrentScreen();

}
