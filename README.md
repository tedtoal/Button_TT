# Button_TT

## Draws various styles of buttons on a pixel display

This library defines classes and functions to support drawing buttons on a pixel display. Support is included for various types of buttons, as well as button hit-testing. These classes use the *Adafruit_GFX_Library* graphics library, and they require a display with a controller object class that is derived from class *Adafruit_GFX*. Many low-cost displays use the ILI9341 controller, which has compatible library *Adafruit_ILI9341* which defines a class of the same name. *You must install the **Adafruit_GFX_Library** library on your system, and the controller library such as **Adafruit_ILI9341***.

## Introductory code

Here is simple code showing how to use this library to create a single on-screen rectangular button with no text and with no user interaction.

```
#include <Arduino.h>
#include <Button_TT.h>

// LCD display pin definitions.
#define LCD_DC_PIN          2
#define LCD_CS_PIN          10
#define LCD_BACKLIGHT_PIN   A2

// LCD display backlight definitions.
#define LCD_BACKLIGHT_OFF   HIGH
#define LCD_BACKLIGHT_ON    LOW

// LCD object.
Adafruit_ILI9341* lcd;

// To create a button, create a variable of type Button_TT or a derived class.
// The button name can be anything and is unrelated to text on a button (It is
// useful at times for debugging).

// A plain button with no text, whose name is "simple".
Button_TT btn_Simple("simple");

// Standard Arduino setup() function.
void setup() {

  // Create LCD object.
  lcd = new Adafruit_ILI9341(LCD_CS_PIN, LCD_DC_PIN);

  // Turn on backlight.
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);

  // Initialize the LCD display.
  lcd->begin();
  lcd->setRotation(2);   // portrait mode
  lcd->setTextColor(ILI9341_BLUE);
  lcd->setTextSize(1);
  lcd->setTextWrap(false);

  // Initialize btn_Simple.
  btn_Simple.initButton(lcd, "TC", 120, 5, 40, 20, ILI9341_BLACK, ILI9341_BLUE);

  // Fill screen with white.
  lcd->fillScreen(ILI9341_WHITE);

  // Draw btn_Simple button.
  btn_Simple.drawButton();
}

// Standard Arduino loop() function.
void loop() {
}
```

The following sections introduce the Button_TT library functionality one piece at a time.

## Basic include file

Usually you start your #include statements in any Arduino program by including *Arduino.h*. Then, to use the basic functionality of the Button_TT library, include *Button_TT.h*:

```
#include <Arduino.h>
#include <Button_TT.h>
```

There is a constant named BUTTON_TT_DBG defined as 0 in Button_TT.h. The file can be edited and the constant set to 1, causing some Button_TT library functions to print debug info to the serial monitor. In the unlikely event you want to use this functionality, include *monitor_printf.h* **before** including *Button_TT.h*:

```
#include <Arduino.h>
#include <monitor_printf.h>
#include <Button_TT.h>
```

The *monitor_printf* library is recommended as an easy way to print numbers to the Arduino IDE serial monitor.

## Defining pin numbers and constants for the LCD display controller

Early in your .ino file you should define constants for the three key pins that connect to the ILI9341 display controller. Here is an example with the pins defined for my particular system (a Nano 33 IoT processor in an MKR A-Z Touch case). You will need to know which pins on your system are connected and set these constants accordingly.

```
// LCD display pin definitions.
#define LCD_DC_PIN          2
#define LCD_CS_PIN          10
#define LCD_BACKLIGHT_PIN   A2
```

You should also define constants for controlling the LCD backlight:

```
// LCD display backlight definitions.
#define LCD_BACKLIGHT_OFF   HIGH
#define LCD_BACKLIGHT_ON    LOW
```

## Defining a pointer to the LCD display object

In order for your program to use the LCD display, you must create a pointer to an Adafruit_ILI9341 object, which knows how to talk to the ILI9341 controller to control the display. The Adafruit_ILI9341 class is a derivation of the Adafruit_GFX class, which has many functions to write various kinds of graphics to the display. Here is how you define the pointer variable in your .ino file:

```
// LCD display object.
Adafruit_ILI9341* lcd;
```

## Defining button variables

To create a button on the LCD display, you must define a variable that represents the button. The variable type is *Button_TT* or a class that derives from that, such as *Button_TT_label*. The various types of buttons are described later. The basic button type, *Button_TT*, is nothing more than a rectangle on the screen, containing no text.

Later, design patterns will be presented for keeping your buttons organized, but for now, just know that you can define a variable for a simple button like this:

```
// A plain button with no text, whose name is "simple".
Button_TT btn_Simple("simple");
```

Take note of the button name, *simple*. This can be any name you want (including an empty string). It is *not* shown as text inside the button. Instead, this name is used to display debug information about the button when debugging the Button_TT library. You may never need to use it, but it is advisable to define each of your buttons with a *unique name*.

A useful design pattern is to name the button variable in the format *btn_<Name>* as is done above.

## Initializing the LCD display

The standard Arduino function named *setup()* is used for initialization in Arduino programs. Before you can use the LCD display or create any buttons, you must initialize it. This is done in *setup()* as follows:

```
// Standard Arduino setup() function.
void setup() {

  // Create LCD object.
  lcd = new Adafruit_ILI9341(LCD_CS_PIN, LCD_DC_PIN);

  // Turn on backlight.
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);

  // Initialize the LCD display.
  lcd->begin();
  lcd->setRotation(2);   // portrait mode
  lcd->setTextColor(ILI9341_BLUE);
  lcd->setTextSize(1);
  lcd->setTextWrap(false);
}
```

The first statement creates a new object of type Adafruit_ILI9341 and assigns its pointer to the *lcd* variable defined earlier. This is where two of the pin number constants for the LCD display are used.

The next two statements above turn on the LCD backlight. They use the third pin number constant, which is the pin that connects to the LCD backlight. Your code might choose to turn the backlight off, and only turn it on when the touchscreen is touched.

The last statements above call functions in the LCD Adafruit_ILI9341 object to initialize the display:

> *begin()* is called first to do basic initialization of the ILI9341 controller.

> *setRotation()* is called next to set the basic rotation of the display, a value between 0 and 3. The value of 2 above is the orientation I use in my project, but you should determine the orientation you need for yours.

> *setTextColor()* sets the color of text drawn on the screen, but this is not really necessary here, as normally the color is set just prior to drawing text, each time it is drawn. The constant *ILI9341_BLUE* is defined in Adafruit_ILI9341.h, see that file for a full list of color constants.

> *setTextSize()* sets a magnification factor for text in the x- and y- directions. Usually I use a value of 1 in order to achieve the highest possible resolution of the text.

> *setTextWrap()* enables and disables automatic text wrapping. Normally this is not used and set false.

## Initializing buttons

  // Initialize btn_Simple.
  btn_Simple.initButton(lcd, "TC", 120, 5, 40, 20, ILI9341_BLACK, ILI9341_BLUE);

## Drawing the screen and buttons

  // Fill screen with white.
  lcd->fillScreen(ILI9341_WHITE);

  // Draw btn_Simple button.
  btn_Simple.drawButton();

## loop() function not needed in simplest case

## Contact

If you find a problem, please submit an issue report [here](https://github.com/tedtoal/Button_TT/issues/new/choose).

You can contact me (the author) at [ted@tedtoal.net](ted@tedtoal.net)
