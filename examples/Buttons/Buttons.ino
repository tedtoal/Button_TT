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

  If the SAMD architecture is being used (ARDUINO_ARCH_SAMD), this uses the
  SAMD_PWM library to produce a beep sound when a button is tapped.

  Note: if you copy this code to use in your project, you probably want to
  delete all references to SELECT.
*/

// Include files needed for any button functionality.
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <monitor_printf.h>
#include <Button_TT.h>

// Set SELECT to one of the following values to enable that functionality and
// all lower-numbered functionality.
//  0   Just a rectangular button with no text and no response to a tap.
//  1   Respond to tap of button by changing its fill color.
//  2   Play a tone while button is tapped, if SAMD architecture is used.
//  3   Add a button with text in it. Tapping it changes the text.
//  4   Add a button with an integer value in it. Tapping it increments the value.
//  5   Add arrow buttons that increment/decrement the numeric value.
//  6   Add a button with an unsigned integer in it, with arrow buttons.
//  7   Add LCD backlight timer.
//  8   Add non-volatile memory storage of numeric button values if SAMD architecture.
//  9   Add touchscreen calibration, storing it non-volatilely if SAMD architecture.
#define SELECT  9

// LCD display pin definitions.
#define LCD_DC_PIN          2
#define LCD_CS_PIN          10
#define LCD_BACKLIGHT_PIN   A2

// Touchscreen pin definitions.
#define TOUCH_CS_PIN        A0
#define TOUCH_IRQ_PIN       A7

// Beeper pin definition.
#define BEEPER_PIN          A3

/******************************************************************************/

// LCD display backlight definitions.
#define LCD_BACKLIGHT_OFF   HIGH
#define LCD_BACKLIGHT_ON    LOW
#define LCD_BACKLIGHT_AUTO_OFF_MS (10*1000)

// Beeper tone definitions (requires SAMD architecture).
#define TS_TONE_FREQ        3000  // Frequency to play when touch screen is tapped.
#define TS_TONE_DUTY        50    // Duty cycle in % of square wave tone, 0 for off.

/******************************************************************************/

// Include files when button tap response is enabled.
#if SELECT >= 1
#include <Button_TT_collection.h>
#include <XPT2046_Touchscreen_TT.h>
#include <TS_ILI9341.h>
#endif

// Include file for SAMD beeper tones (requires SAMD architecture).
#if SELECT >= 2
#ifdef ARDUINO_ARCH_SAMD
#define _PWM_LOGLEVEL_ 0
#include <SAMD_PWM.h>
#elif SELECT == 2
#error SAMD architecture is required to play a beep!
#endif
#endif

// Include files when any button with text is used.
#if SELECT >= 3
#include <Font_TT.h>
#include <Button_TT_label.h>
#endif

// When any button with text is used, include one or more font files.
#if SELECT >= 3
#include <Fonts/FreeSans12pt7b.h>
#endif

// Include files for using triangle buttons, usually used as arrow buttons.
#if SELECT >= 5
#include <Button_TT_arrow.h>
#endif

// Include files for using button with int8_t value.
#if SELECT >= 4
#include <Button_TT_int8.h>
#endif

// Include files for using button with uint8_t value.
#if SELECT >= 6
#include <Button_TT_uint8.h>
#endif

// Include files and define constants for using EEPROM to store values (requires
// SAMD architecture).
#if SELECT >= 8
// EEPROM support currently only if SAMD architecture.
#ifdef ARDUINO_ARCH_SAMD

// It appears (page 29 of Atmel SAM D21E / SAM D21G / SAM D21J data sheet) that
// the EEPROM page size is 64, and 4 pages must be erased at one time, giving an
// effective page size of 4*64 = 256.  This seems to be confirmed by the fact
// that FlashStorage_SAMD.hpp aligns the PPCAT variable to 256 bytes.
#define EEPROM_EMULATION_SIZE     (4 * 64)

// Use 0-2. Larger for more debugging messages
#define FLASH_DEBUG       0

// Now include flash storage library header file.
// To be included only in one file to avoid `Multiple Definitions` Linker Error.
#include <FlashStorage_SAMD.h>

// Signature used at start of a flash memory block to mark the block as
// containing valid data written by the application.
const int WRITTEN_SIGNATURE = 0xBEEFDEED;

#endif
#endif

/******************************************************************************/

// LCD object.
static Adafruit_ILI9341* lcd;

// PWM object for sound from beeper.
#if SELECT >= 2
#ifdef ARDUINO_ARCH_SAMD
static SAMD_PWM* sound;
#endif
#endif

// Variables required to support touchscreen and button taps.
#if SELECT >= 1

// Button collection object to manage taps of buttons.
static Button_TT_collection* screenButtons;

// Touchscreen object.
XPT2046_Touchscreen* touch;

// Touchscreen-display object.
TS_ILI9341* ts_display;

#endif

// When any button with text is used, create a Font_TT object for each font.
#if SELECT >= 3
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

// It is convenient to define an enum that enumerates the different screens you
// might want to show.

// Screens.
typedef enum _eScreen {
  SCREEN_MAIN
  #if SELECT >= 9
  , SCREEN_CALIBRATION
  #endif
} eScreen;

// Currently displayed screen.
eScreen currentScreen;

// Declare function for drawing the current screen. It is called by button
// handlers that wish to switch screens.
void drawCurrentScreen();

/******************************************************************************/

#if SELECT >= 8
// Structure containing non-volatile data to be stored in flash memory (with
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
// MAIN SCREEN buttons and handler functions.
//
// The Main screen shows the buttons selected by SELECT.
/******************************************************************************/

// To create a button, create a variable of type Button_TT or a derived class.
// The button name can be anything and is unrelated to text on a button. It is
// useful at times for debugging.

// A plain button with no text, whose name is "simple".
Button_TT btn_Simple("simple");

// When a button responds to taps, it needs a "btnTap_" function. Although in
// general each button needs such a function, in some cases it makes sense to
// use the same function for multiple buttons, and in that case the "btn"
// argument indicates which button was actually tapped.
#if SELECT >= 1
// Handle tap of "btn_Simple". Toggle button fill color.
void btnTap_Simple(Button_TT& btn) {
  uint16_t color = btn_Simple.getFillColor();
  btn_Simple.setFillColor(color == ILI9341_BLUE ? ILI9341_RED : ILI9341_BLUE);
  btn_Simple.drawIfChanged();
}
#endif

/******************************************************************************/

// Another button, using variable name "btn_Hello", and this time named "hello".
// This #if encloses the functions and variables needed to support a new button
// that supports taps.

#if SELECT >= 3
// A labelled button whose name is "hello".
Button_TT_label btn_Hello("hello");

// Declare button tap handler function. Note that draw_btnHello() refers to this
// function, and this function calls draw_btnHello().
void btnTap_Hello(Button_TT& btn);

// Function to draw "btn_Hello". When a button's text changes as it or other
// buttons are pressed, it works best to have a "draw" function for each such
// button. The forceDraw argument allows the button drawing to be skipped if the
// only purpose of calling this function is to update the button when its text
// has changed and no change occurs. forceDraw=true always draws it (used at
// initialization), while forceDraw=false only draws it if the text (or some
// other button attribute such as fill color) has changed (used when button is
// tapped or something else changes the button text or attributes). Here we
// decide what text to put in the button each time this draw function is called.

// Draw "btn_Hello".
// If the Hello button label starts with "H", set its text to "Goodbye World!",
// else set it to "Hello World!". Then draw the button. If forceDraw, register
// it with screenButtons to monitor for touches.
void draw_btnHello(bool forceDraw = false) {
  const char* label = btn_Hello.getLabel();
  bool isHello = (label[0] == 'H');
  btn_Hello.setLabelAndDrawIfChanged(!isHello ? "Hello World!" : "Goodbye World!", forceDraw);
  if (forceDraw)
    screenButtons->registerButton(btn_Hello, btnTap_Hello);
}

// Handle tap of "btn_Hello". We only need to call the draw function for the
// button, which will toggle its text and draw it.
void btnTap_Hello(Button_TT& btn) {
  draw_btnHello();
}
#endif

/******************************************************************************/

#if SELECT >= 4
// An int8_t button whose name is "btn_int8Val".
Button_TT_int8 btn_int8Val("int8Val");

// Handle tap of "btn_int8Val". Increment the value (which also draws the button).
void btnTap_int8Val(Button_TT& btn) {
  btn_int8Val.valueIncDec(+1);
}

#if SELECT >= 5
// Also define two arrow buttons (left and right, for decrement and increment)
// to go with btn_int8Val.
Button_TT_arrow btn_int8Val_left("int8Val_left");
Button_TT_arrow btn_int8Val_right("int8Val_right");

// The tap function is shared by both arrow buttons. It invokes valueIncDec()
// on the int8val button.
void btnTap_int8Val_delta(Button_TT& btn) {
  btn_int8Val.valueIncDec(1, &btn);
}

#endif
#endif

/******************************************************************************/

#if SELECT >= 6
// A uint8_t button whose name is "btn_uint8Val".
// There is no tap handler for this button, tapping it does nothing. It changes
// its value when one of the arrow buttons to its right is tapped.
Button_TT_uint8 btn_uint8Val("uint8Val");

// Also define two arrow buttons (left and right, for decrement and increment)
// to go with btn_uint8Val.
Button_TT_arrow btn_uint8Val_left("uint8Val_left");
Button_TT_arrow btn_uint8Val_right("uint8Val_right");

// The tap function is shared by both arrow buttons. It invokes valueIncDec()
// on the uint8Val button.
void btnTap_uint8Val_delta(Button_TT& btn) {
  btn_uint8Val.valueIncDec(1, &btn);
}
#endif

/******************************************************************************/

#if SELECT >= 9
// A labelled button whose name is "btn_Calibrate".

Button_TT_label btn_Calibrate("calibrate");

// Handle tap of "btn_Calibrate". We switch to the calibration screen.
void btnTap_Calibrate(Button_TT& btn) {
  currentScreen = SCREEN_CALIBRATION;
  drawCurrentScreen();
}
#endif

/******************************************************************************/

// Support for button tones (requires SAMD architecture).
#if SELECT >= 2
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
#if SELECT >= 1
// Check for touch screen tap or release. If so, use ScreenButtons to process
// the tap or release.
void processTapsAndReleases() {
  int16_t x, y, pres;

  // Check for a button tap or release event.
  switch (ts_display->getTouchEvent(x, y, pres)) {

  // Handle LCD backlight auto on/off.
  #if SELECT >= 7
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
    // When backlight auto on/off is supported, touch events are ignored when
    // the backlight is off.
    #if SELECT >= 7
    // Touch events turn on the backlight if off, else are processed as possible
    // screen button taps.
    if (digitalRead(LCD_BACKLIGHT_PIN) == LCD_BACKLIGHT_OFF) {
      digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);
      break;
    }
    #endif
    // Process possible button tap.
    screenButtons->press(x, y);
    break;

  // Handle a release event.
  case TS_RELEASE_EVENT:
    #if SELECT >= 7
    // Release events reset the backlight timer.
    MSsinceLastTouchBeforeBacklight = 0;
    MSatLastBacklightTimerUpdate = millis();
    #endif
    // Process possible button release.
    screenButtons->release();
    break;
  }
}
#endif

/******************************************************************************/

// It is convenient to define a function to initialize the screen buttons,
// because typically you will have multiple different screens, and providing a
// separate initialization function for each one keeps things cleaner.

// Initialize the main screen.
void initButtons_MainScreen(void) {

  // Initialize btn_Simple.
  btn_Simple.initButton(lcd, "TC", 120, 5, 40, 20, ILI9341_BLACK, ILI9341_BLUE);

  #if SELECT >= 3
  // Initialize btn_Hello. Just set the button text to the empty string
  // for now, draw_btnHello() will set it correctly.
  btn_Hello.initButton(lcd, "TC", 120, 30, 200, 26, ILI9341_BLACK,
    ILI9341_LIGHTGREY, ILI9341_BLUE, "C", "", false, &font12);
  #endif

  #if SELECT >= 4
  // Initialize btn_int8Val. Give it rounded corners, and use value -5 just for
  // the heck of it, and put min and max values at -10 and +10.
  btn_int8Val.initButton(lcd, "TL", 35, 68, 50, 26, ILI9341_BLACK, ILI9341_LIGHTGREY,
    ILI9341_RED, "C", &font12, RAD, -5, -10, +10);

  #if SELECT >= 5
  // Initialize arrow buttons for int8val button.
  btn_int8Val_left.initButton(lcd, 'L', "TR", 120, 65, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);
  btn_int8Val_right.initButton(lcd, 'R', "TL", 130, 65, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);
  #endif
  #endif

  #if SELECT >= 6
  // Initialize btn_uint8Val. Give it rounded corners, and use value 10 just for
  // the heck of it, put min and max values at 0 and +10, and add a degree
  // symbol after it. Display "--" for 0.
  btn_uint8Val.initButton(lcd, "TL", 35, 113, 50, 26, ILI9341_BLACK, ILI9341_LIGHTGREY,
    ILI9341_RED, "C", &font12, RAD, +10, 0, +10, "--", true);

  // Initialize arrow buttons for uint8val button.
  btn_uint8Val_left.initButton(lcd, 'L', "TR", 120, 110, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);
  btn_uint8Val_right.initButton(lcd, 'R', "TL", 130, 110, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);
  #endif

  #if SELECT >= 9
  // Initialize btn_Calibrate. Give it rounded corners, and use value 10 just for
  btn_Calibrate.initButton(lcd, "CC", 120, 170, BTN_WIDTH, BTN_HEIGHT, ILI9341_BLACK,
    ILI9341_PINK, ILI9341_BLACK, "C", "Calibrate", false, &font12, RAD);
  #endif
}

// It is ALSO convenient to define a function to draw the screen, because
// typically you will have multiple different screens that you will want to be
// able to switch between.

// Draw the main screen.
void drawMainScreen() {

  #if SELECT >= 1
  // Clear all existing button registrations.
  screenButtons->clear();
  #endif

  #if SELECT >= 7
  // Turn on backlight.
  MSsinceLastTouchBeforeBacklight = 0;
  MSatLastBacklightTimerUpdate = millis();
  digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);
  #endif

  // Fill screen with white.
  lcd->fillScreen(ILI9341_WHITE);

  // Draw screen buttons.
  #if SELECT >= 1
  // Also, register buttons with screenButtons object to handle button taps/releases.
  #endif

  btn_Simple.drawButton();
  #if SELECT >= 1
  screenButtons->registerButton(btn_Simple, btnTap_Simple);
  #endif

  #if SELECT >= 3
  draw_btnHello(true);
  #endif

  #if SELECT >= 4
  btn_int8Val.drawButton();
  screenButtons->registerButton(btn_int8Val, btnTap_int8Val);
  #endif

  #if SELECT >= 5
  btn_int8Val_left.drawButton();
  screenButtons->registerButton(btn_int8Val_left, btnTap_int8Val_delta);
  btn_int8Val_right.drawButton();
  screenButtons->registerButton(btn_int8Val_right, btnTap_int8Val_delta);
  #endif

  #if SELECT >= 6
  btn_uint8Val.drawButton();
  btn_uint8Val_left.drawButton();
  screenButtons->registerButton(btn_uint8Val_left, btnTap_uint8Val_delta);
  btn_uint8Val_right.drawButton();
  screenButtons->registerButton(btn_uint8Val_right, btnTap_uint8Val_delta);
  #endif

  #if SELECT >= 9
  btn_Calibrate.drawButton();
  screenButtons->registerButton(btn_Calibrate, btnTap_Calibrate);
  #endif
}

// It is ALSO convenient to define a function to perform any operations required
// by the screen, because typically you will have multiple different screens and
// each will require its own unique processing.

// Handle loop processing for the main screen.
void loopMainScreen() {

  // Currently the only thing that needs to be done in the main screen is to
  // save current non-volatile values in non-volatile memory if they have
  // changed.

  #if SELECT >= 8
  #ifdef ARDUINO_ARCH_SAMD
  // Save current values from btn_int8Val and btn_uint8Val to non-volatile memory.
  // This will compare the values to the current stored values, and if unchanged,
  // no write is done to EEPROM. RESET the device to see the effect.
  // Perhaps a better alternative to this would be to have a "Save" button the
  // user must press to save settings to non-volatile memory.
  NVsettings.int8val = btn_int8Val.getValue();
  NVsettings.uint8val = btn_uint8Val.getValue();
  writeNonvolatileSettingsIfChanged(NVsettings);
  #endif
  #endif
}

/******************************************************************************/

#if SELECT >= 9

// CALIBRATION SCREEN buttons and handler functions.
//
// The Calibration screen initially shows a Cancel button, a message to touch
// the "+", and a single "+" in one corner. When Cancel is touched the screen
// exits back to the main screen without changing the calibration setting. If
// the "+" is touched, it is erased and a second "+" in the opposite corner is
// displayed. If that "+" is also touched, it is erased, calibration settings
// are recomputed and temporarily changed, a Save button is shown along with the
// Cancel button, a message is displayed to touch anywhere to test calibration,
// and subsequent touches cause a "+" of another color to be drawn at that
// position. Touching Cancel reverts to original calibration settings, while
// touching Save saves the new calibration settings to nonvolatile memory and
// it exits back to the main screen.

Button_TT_label label_Calibration("CalibrationScreen");
Button_TT_label btn_CalibrationCancel("CalibrationCancel");
Button_TT_label btn_CalibrationSave("CalibrationSave");

// Initialize the calibration screen.
void initButtons_CalibrationScreen(void) {

  label_Calibration.initButton(lcd, "TC", 120, 5, TEW, TEW, TRANSPARENT_COLOR,
    TRANSPARENT_COLOR, ILI9341_DARKGREEN, "C", "Calibrate", false, &font12);
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

// States during calibration and subsequent showing of tapped points.
typedef enum _eCalibState {
  STATE_WAIT_UL,            // Wait for user to tap + at upper-left
  STATE_WAIT_UL_RELEASE,    // Wait for him to release the tap
  STATE_WAIT_LR,            // Wait for user to tap + at lower-right
  STATE_WAIT_LR_RELEASE,    // Wait for him to release the tap
  STATE_WAIT_POINT_SHOW_IT, // Wait for user to tap anywhere, then draw "+" there
  STATE_WAIT_RELEASE        // Wait for him to release the tap
} eCalibState;

// Current state of calibration screen interaction with user.
static eCalibState calibState;

// Display UL and LR calibration positions and corresponding touchscreen
// calibration coordinates.
int16_t x_UL, y_UL, x_LR, y_LR;
int16_t TSx_UL, TSy_UL, TSx_LR, TSy_LR;

// Print string S to display at cursor position (x,y) in specified color.
void lcd_print(int16_t x, int16_t y, int16_t color, const char* S) {
  lcd->setCursor(x, y);
  lcd->setTextColor(color);
  lcd->print(S);
}

// Text for user instructions to tap "+".
#define TEXT_TAP_PLUS "Tap the +"

// Declare button handler functions for the Calibration screen.
void btnTap_CalibrationCancel(Button_TT& btn);
void btnTap_CalibrationSave(Button_TT& btn);

// Draw the Calibration screen and register its buttons with the screenButtons
// object. Save button is only drawn if drawSaveButton=true.
void drawCalibrationScreen(bool drawSaveButton=false) {

  // Clear all existing button registrations.
  screenButtons->clear();

  // Turn on backlight.
  MSsinceLastTouchBeforeBacklight = 0;
  MSatLastBacklightTimerUpdate = millis();
  digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);

  // Fill screen with white.
  lcd->fillScreen(ILI9341_WHITE);

  // Draw screen buttons.
  label_Calibration.drawButton();
  btn_CalibrationCancel.drawButton();
  screenButtons->registerButton(btn_CalibrationCancel, btnTap_CalibrationCancel);
  if (drawSaveButton) {
    btn_CalibrationSave.drawButton();
    screenButtons->registerButton(btn_CalibrationSave, btnTap_CalibrationSave);
  }

  // Get position of two display points at which to draw "+" signs and require
  // that the user tap them to calibrate the touchscreen.
  ts_display->GetCalibration_UL_LR(PLUS_ARM_LEN+2, &x_UL, &y_UL, &x_LR, &y_LR);

  // Paint first "+" and wait for user to tap that point.
  drawPlus(x_UL, y_UL, ILI9341_BLUE);
  lcd_print(20, 60, ILI9341_RED, TEXT_TAP_PLUS);
  calibState = STATE_WAIT_UL;
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

// Handle loop processing for the calibration screen.
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
      // Erase the first plus and instructions.
      drawPlus(x_UL, y_UL, ILI9341_WHITE);
      lcd_print(20, 60, ILI9341_WHITE, TEXT_TAP_PLUS);
      // Paint second + and wait for user to tap that point.
      drawPlus(x_LR, y_LR, ILI9341_BLUE);
      lcd_print(120, 260, ILI9341_RED, TEXT_TAP_PLUS);
      // Stop sound.
      playSound(false);
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
      // Map the two touchscreen points to the correct calibration values at the
      // extreme ends of the display. Set resulting calibration parameters as the
      // new calibration parameters in ts_display.
      int16_t TS_LR_X, TS_LR_Y, TS_UL_X, TS_UL_Y;
      ts_display->findTS_calibration(x_UL, y_UL, x_LR, y_LR, TSx_UL, TSy_UL, TSx_LR, TSy_LR,
        &TS_LR_X, &TS_LR_Y, &TS_UL_X, &TS_UL_Y);
      ts_display->setTS_calibration(TS_LR_X, TS_LR_Y, TS_UL_X, TS_UL_Y);
      // Redraw the screen with "Save" button and show new instructions.
      drawCalibrationScreen(true);
      lcd_print(10, 200, ILI9341_RED, "Tap to test calibration");
      // Stop sound.
      playSound(false);
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

  // Don't turn off backlight in calibration mode.
  MSsinceLastTouchBeforeBacklight = 0;
}

#endif

/******************************************************************************/

// Draw the current screen.
void drawCurrentScreen() {
  switch (currentScreen) {
  case SCREEN_MAIN:
    drawMainScreen();
    break;
  case SCREEN_CALIBRATION:
    drawCalibrationScreen();
    break;
  }
}

// Handle loop() processing in current screen.
void loopCurrentScreen() {
  switch (currentScreen) {
  case SCREEN_MAIN:
    loopMainScreen();
    break;
  case SCREEN_CALIBRATION:
    loopCalibrationScreen();
    break;
  }
}

/******************************************************************************/

// Standard Arduino setup() function.
void setup() {
  // Initialize for printfs to serial monitor.
  monitor.begin(&Serial, 115200);
  monitor.printf("Initializing\n");

  // Initialize PWM object for sound from beeper.
  #if SELECT >= 2
  #ifdef ARDUINO_ARCH_SAMD
  monitor.printf("SAMD_PWM\n");
  sound = new SAMD_PWM(BEEPER_PIN, TS_TONE_FREQ, 0);
  #endif
  #endif

  // Create LCD object.
  monitor.printf("lcd\n");
  lcd = new Adafruit_ILI9341(LCD_CS_PIN, LCD_DC_PIN);
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);
  #if SELECT >= 7
  // Initialize backlight timer.
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);
  MSsinceLastTouchBeforeBacklight = LCD_BACKLIGHT_AUTO_OFF_MS;
  MSatLastBacklightTimerUpdate = millis();
  #endif
  // Initialize the LCD display.
  lcd->begin();
  lcd->setRotation(2);   // portrait mode
  lcd->setTextColor(ILI9341_BLUE);
  lcd->setTextSize(1);
  lcd->setTextWrap(false);

  #if SELECT >= 1
  // Create and initialize touchscreen object, same rotation as lcd.
  monitor.printf("touch\n");
  touch = new XPT2046_Touchscreen(TOUCH_CS_PIN, TOUCH_IRQ_PIN);
  touch->begin();
  touch->setRotation(lcd->getRotation());

  // Create and initialize touchscreen-LCD object.
  monitor.printf("ts_display\n");
  ts_display = new(TS_ILI9341);
  ts_display->begin(touch, lcd);

  // Initialize button collection object.
  monitor.printf("screenButtons\n");
  screenButtons = new Button_TT_collection;
  screenButtons->clear();
  #if SELECT >= 2
  // Register master tap/release function to turn tone on/off at screen taps.
  screenButtons->registerMasterProcessFunc(buttonTapRelease);
  #endif
  #endif

  // Initialize the buttons on each screen.
  monitor.printf("main screen buttons\n");
  initButtons_MainScreen();
  #if SELECT >= 9
  initButtons_CalibrationScreen();
  #endif

  // Read non-volatile settings from flash memory (EEPROM), or initialize it if
  // it is uninitialized. Note: each time the Arduino IDE stores a program in
  // EEPROM, it erases these settings, so they will be reinitialized when this
  // runs.
  #if SELECT >= 8
  #ifdef ARDUINO_ARCH_SAMD
  // Read non-volatile settings from flash-based EEPROM into 'NVsettings', then
  // use them to set the program values for which they are intended.
  readNonvolatileSettings(NVsettings, defaults);
  btn_int8Val.setValue(NVsettings.int8val);
  btn_uint8Val.setValue(NVsettings.uint8val);
  #else
  monitor.printf("Unable to read calibration settings from EEPROM - currently this needs SAMD architecture to do that\n");
  #endif
  #endif

  // Draw the main screen.
  monitor.printf("draw main screen\n");
  currentScreen = SCREEN_MAIN;
  drawCurrentScreen();

  monitor.printf("exiting setup\n");
}

/******************************************************************************/

// Standard Arduino loop() function.
void loop() {

  #if SELECT >= 1
  // Process button taps/releases for the current screen, which has registered
  // its buttons with the screenButtons object, which is used to test for button
  // taps and call the button tap handler function.
  processTapsAndReleases();
  #endif

  // Handle loop() processing in current screen.
  loopCurrentScreen();

}
