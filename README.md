# Button_TT

## Draws buttons on a pixel display and detects button taps

The ***Button_TT*** library defines classes and functions to support drawing buttons on a pixel display. Support is included for various types of buttons, as well as button hit-testing.

## Required libraries

The *Button_TT* library uses the *Adafruit_GFX_Library* graphics library, which requires a display with a controller object class that is derived (directly or indirectly) from class *Adafruit_GFX*. Many low-cost displays use the ILI9341 controller, which can use library *Adafruit_ILI9341* for this purpose. *You must install the **Adafruit_GFX_Library** library on your system **and** the compatible display controller library such as **Adafruit_ILI9341***.

## Introductory code

Here is simple code showing how to use this library to create a single on-screen rectangular button with no text and with no user interaction.

```
#include <Arduino.h>
#include <Adafruit_ILI9341.h>
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

You can copy and paste the above code into a new Arduino sketch. Of course, to run this as a program you need to change the hardware pin numbers (_PIN constants) to the correct values for your system, as well as install the necessary libraries.

A more complete example is provided in the example program *HelloWorld.ino*, which adds a button containing text and touchscreen interaction, and it responds to touches of the button by changing its text.

## Quick start with Button.ino

If you'd prefer to skip the rest of this document and just quickly get started, the example program *Button.ino* shows fairly thorough use of the *Button_TT* library (although additional details are given in the sections below). That program is set up so that a **#define of *SELECT*** near the top of the file selects which features of the program are enabled. Start with *#define SELECT 0* and work up to *#define SELECT 9* to gradually add more and more features:
> 0 :  Just a rectangular button with no text and no response to a tap.

> 1 :  Respond to tap of button by changing its fill color.

> 2 :  Play a tone while button is tapped, if SAMD architecture is used.

> 3 :  Add a button with text in it. Tapping it changes the text.

> 4 :  Add a button with an integer value in it. Tapping it increments the value.

> 5 :  Add arrow buttons that increment/decrement the numeric value.

> 6 :  Add a button with an unsigned integer in it, with arrow buttons.

> 7 :  Add LCD backlight timer.

> 8 :  Add non-volatile memory storage of numeric button values if SAMD architecture.

> 9 :  Add touchscreen calibration, storing it non-volatilely if SAMD architecture.

The following sections introduce the Button_TT library functionality one piece at a time.

## Basic include file

Usually you start your #include statements in any Arduino program by including *Arduino.h*. Then, to use the basic functionality of the Button_TT library, include *Adafruit_GFX.h*, *Button_TT.h*, and your display controller library such as *Adafruit_ILI9341.h*:

```
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Button_TT.h>
#include <Adafruit_ILI9341.h>
```

## BUTTON_TT_DBG and *monitor_printf.h*

There is a constant named BUTTON_TT_DBG defined as 0 in Button_TT.h. The file can be edited and the constant set to 1, causing some Button_TT library functions to print debug info to the serial monitor. In the event you need to use this functionality, include *monitor_printf.h* **before** including *Button_TT.h*:

```
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <monitor_printf.h>
#include <Button_TT.h>
#include <Adafruit_ILI9341.h>
```

You can add your own debug code as necessary to Button_TT library source files if you are having problems and need to print out information to help you debug.

Besides its use with BUTTON_TT_DBG, the *monitor_printf* library is recommended *in general* as an easy way to print numbers to the Arduino IDE serial monitor. Another library that is useful with it is *floatToString* for converting floating point numbers to strings for printing when full floating point to string conversion support is not enabled in your Arduino IDE.

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

In order for your program to use the LCD display, you must create a pointer to a display controller object that knows how to talk to the display controller to write text and graphics to the display. If your display uses the ILI9341 controller, which many inexpensive displays do, the Adafruit_ILI9341 library provides a class of the same name for use as the display controller object. Here is how you define the pointer variable in your .ino file if you use that library:

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

Take note of the button name, *simple*, which is given as the argument to the button constructor. This can be any name you want (including an empty string). It is *not* shown as text inside the button. Instead, this name is used to display information about the button if you need to debug problems with the Button_TT library or its use. You may never need to use it, but it is advisable to define each of your buttons with a *unique name*.

A useful design pattern is to name the button variable in the format *btn_<Name>* as is done above.

## Initializing the LCD display

The standard Arduino function named *setup()* is used for initialization in Arduino programs. Before you can use the LCD display or create any buttons, you must initialize the display. This is done in *setup()* as follows:

```
// Standard Arduino setup() function.
void setup() {

  // Create LCD object.
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
}
```

The first statement creates a new object of type Adafruit_ILI9341 and assigns its pointer to the *lcd* variable defined earlier. This is where two of the pin number constants for the LCD display are used.

The next statements call functions in the LCD Adafruit_ILI9341 object to initialize the display:

> *begin()* is called first to do basic initialization of the ILI9341 controller.

> *setRotation()* is called next to set the desired rotation (orientation) of the display, a value between 0 and 3. The value of 2 above is the orientation I use in my project, but you should determine the orientation you need for yours. You will see the effect of rotation by changing its value and running a program that displays something.

> *setTextColor()* sets the color of text drawn on the screen, but this is not really necessary here, as normally the color is set just prior to drawing text, each time it is drawn. The constant *ILI9341_BLUE* is defined in Adafruit_ILI9341.h. That file defines several *ILI9341_* constants for colors, so see that file for a full list of color constants. (Right-click on the #include and choose *Go to Definition*).

> *setTextSize()* sets a magnification factor for text in the x- and y- directions. Usually I use a value of 1 in order to achieve the highest possible resolution of the text. Again, if used this is typically called each time text is drawn.

> *setTextWrap()* enables and disables automatic text wrapping. Normally this is not used and set false.

The last two statements turn on the LCD backlight. They use the third pin number constant, which is the pin that connects to the LCD backlight. Your code might choose to turn the backlight off, and only turn it on when the touchscreen is touched.

## Initializing buttons

Each button variable that you define must be initialized, either in *setup()* or in a function called from it. The initialization is very important, as it sets several attributes of the button, such as its position on the display, its size, and outline and fill colors. For other types of buttons initialization can set other important attributes.

To initialize a button, call its *initButton()* function:

```
  // Initialize btn_Simple.
  btn_Simple.initButton(lcd, "TC", 120, 5, 40, 20, ILI9341_BLACK, ILI9341_BLUE);
```

The first argument to *initButton()* is always the pointer to the display controller object (lcd here). The full set of arguments is something you will want to become familiar with. To do that, start by opening the *Button_TT.h* file in the Arduino IDE, by right-clicking on its #include and choosing "Go to Definition". Scroll down to (or search for) the initButton() function. Above each function declaration in all the Button_TT library header files you will find a set of comments, written in the *Doxygen* documentation style, that describe what the function does, what each of its arguments is, and what value if any it returns. There may also be important notes. Read through the initButton() arguments. They are repeated here:

```

```

## Drawing the screen and buttons

  // Fill screen with white.
  lcd->fillScreen(ILI9341_WHITE);

  // Draw btn_Simple button.
  btn_Simple.drawButton();

## loop() function not needed in simplest case

## Contact

If you find a problem, please submit an issue report [here](https://github.com/tedtoal/Button_TT/issues/new/choose).

You can contact me (the author) at [ted@tedtoal.net](ted@tedtoal.net)
