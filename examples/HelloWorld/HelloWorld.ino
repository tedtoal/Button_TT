/*
  HelloWorld.ino - A program to show a button on a TFT display with touchscreen,
  and with button text label "Hello World" that changes when it is tapped.
  Created by Ted Toal, August 7, 2023.
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

  To use this, set the _PIN #defines below for your system, compile, load, run,
  and view the resulting button. Tap it to change its text.

  If the SAMD architecture is being used (ARDUINO_ARCH_SAMD), this uses the
  SAMD_PWM library to produce a beep sound when a button is tapped.
*/
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Font_TT.h>
#include <Button_TT.h>
#include <Button_TT_label.h>
#include <Button_TT_collection.h>
#include <XPT2046_Touchscreen_TT.h>
#include <TS_ILI9341.h>
#include <monitor_printf.h>
// SOUND/BEEPER support currently only if SAMD architecture.
#ifdef ARDUINO_ARCH_SAMD
#define _PWM_LOGLEVEL_ 0
#include <SAMD_PWM.h>
#endif

// Pin definitions for the LCD display.
#define LCD_DC_PIN          2
#define LCD_CS_PIN          10
#define LCD_BACKLIGHT_PIN   A2
#define LCD_BACKLIGHT_OFF   HIGH
#define LCD_BACKLIGHT_ON    LOW
#define LCD_BACKLIGHT_AUTO_OFF_MS (45*1000)

// Touchscreen pin definitions.
#define TOUCH_CS_PIN        A0
#define TOUCH_IRQ_PIN       A7

// Beeper pin.
#define BEEPER_PIN          A3
// Frequency to play when user taps the touch screen.
#define TS_TONE_FREQ        3000
// Duty cycle of "tone" (a square wave) in percent.  0 turns it off.
#define TS_TONE_DUTY        50

// LCD object.
static Adafruit_ILI9341* lcd;

// Button collection object to manage taps of buttons.
static Button_TT_collection* screenButtons;

// PWM object for sound from beeper.
#ifdef ARDUINO_ARCH_SAMD
static SAMD_PWM* sound;
#endif

// Touchscreen object.
XPT2046_Touchscreen* touch;

// Touchscreen-display object.
TS_ILI9341* ts_display;

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

// The font to use.
Font_TT font12(&FreeSans12pt7b);

// Each time the example button is tapped, isHello is toggled and the button
// text is switched between two strings.
bool isHello = true;

// A labelled button on the screen whose name (not text label) is "hello".
Button_TT_label btn_Hello("hello");

// Declare button handler function.
void btnTap_Hello(Button_TT& btn);

// Set the Hello button label according to isHello, draw the button, and
// register it with screenButtons so we can monitor for touches. (Reregistering
// when already registered does nothing).
void showHelloButton(bool forceDraw = false) {
  btn_Hello.setLabelAndDrawIfChanged(isHello ? "Hello World!" : "Goodbye World!", forceDraw);
  screenButtons->registerButton(btn_Hello, btnTap_Hello);
}

// Handle tap of Hello button.
void btnTap_Hello(Button_TT& btn) {
  // Toggle button text.
  isHello = !isHello;
  // Note: we already KNOW that btn is the hello button because it is the only
  // button we have registered with this function. So, we can ignore btn.
  // If this function were registered to handle two or more buttons, THEN we
  // can use 'btn' to know which button was tapped.
  showHelloButton();
}

// Master button tap/release function. Tap: start tone. Release: end tone.
void buttonTapRelease(bool tap) {
  #ifdef ARDUINO_ARCH_SAMD
  sound->setPWM(BEEPER_PIN, TS_TONE_FREQ, tap ? TS_TONE_DUTY : 0);
  #endif
}

// Check for touch screen button tap or release. If so, check to see if it is
// one of the current screen registered buttons, and if so, process that tap
// or release.
void processTapsAndReleases() {
  int16_t x, y, pres;

  // Check for a button tap or release event.
  switch (ts_display->getTouchEvent(x, y, pres)) {

  // When screen is not being touched or uncertain about touch, update the
  // backlight timer.
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

  // Touch events turn on the backlight if off, else are processed as possible
  // screen button taps.
  case TS_TOUCH_EVENT:
    if (digitalRead(LCD_BACKLIGHT_PIN) == LCD_BACKLIGHT_OFF)
      digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);
    else
      screenButtons->press(x, y);
    break;

  // Release events reset the backlight timer and are also tested for possible
  // screen button release.
  case TS_RELEASE_EVENT:
    MSsinceLastTouchBeforeBacklight = 0;
    MSatLastBacklightTimerUpdate = millis();
    screenButtons->release();
    break;
  }
}

// Standard Arduino setup() function.
void setup() {
  // Initialize for printfs to serial monitor.
  monitor.begin(&Serial, 115200);
  monitor.printf("Initializing\n");

  // Initialize PWM object for sound from beeper.
  #ifdef ARDUINO_ARCH_SAMD
  monitor.printf("SAMD_PWM\n");
  sound = new SAMD_PWM(BEEPER_PIN, TS_TONE_FREQ, 0);
  #endif

  // Create LCD object, turn on backlight, initialize backlight timer, and
  // initialize the LCD display.
  monitor.printf("LCD\n");
  lcd = new Adafruit_ILI9341(LCD_CS_PIN, LCD_DC_PIN);
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);
  MSsinceLastTouchBeforeBacklight = LCD_BACKLIGHT_AUTO_OFF_MS * 1000;
  MSatLastBacklightTimerUpdate = millis();
  lcd->begin();
  lcd->setRotation(2);   // portrait mode
  lcd->setTextColor(ILI9341_BLUE);
  lcd->setTextSize(1);
  lcd->setTextWrap(false);

  // Create and initialize touchscreen object, same rotation as lcd.
  monitor.printf("Touch\n");
  touch = new XPT2046_Touchscreen(TOUCH_CS_PIN, TOUCH_IRQ_PIN);
  touch->begin();
  touch->setRotation(lcd->getRotation());

  // Create and initialize touchscreen-LCD object.
  monitor.printf("ts_display\n");
  ts_display = new(TS_ILI9341);
  ts_display->begin(touch, lcd);

  // Initialize button collection object and register master tap/release function.
  monitor.printf("Button collection\n");
  screenButtons = new Button_TT_collection;
  screenButtons->clear();
  screenButtons->registerMasterProcessFunc(buttonTapRelease);

  // Initialize all buttons.
  monitor.printf("Buttons\n");

  // Initialize the "Hello" button. Just set the button text to the empty string
  // for now, showHelloButton() will set it correctly.
  btn_Hello.initButton(lcd, "TC", 120, 10, 200, 50, ILI9341_BLACK,
    ILI9341_LIGHTGREY, ILI9341_BLUE, "C", "", false, &font12);

  // Paint the screen.
  digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);
  lcd->fillScreen(ILI9341_WHITE);
  showHelloButton(true);
}

// Standard Arduino loop() function.
void loop() {
  // Process button taps/releases.
  processTapsAndReleases();
}
