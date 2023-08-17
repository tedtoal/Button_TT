# Button_TT

## Draws buttons on a pixel display and detects button taps

The ***Button_TT*** library defines classes and functions for drawing buttons on a pixel display. Support is included for various types of buttons, as well as button hit-testing. This lengthy document describes in detail how to use the library.

-------------------------
### Table of Contents

* [Required additional libraries](https://github.com/tedtoal/Button_TT/edit/main/README.md#required-additional-libraries)
* [Introductory code without touchscreen](https://github.com/tedtoal/Button_TT/edit/main/README.md#introductory-code-without-touchscreen)
* [Quick start with Button.ino](https://github.com/tedtoal/Button_TT/edit/main/README.md#quick-start-with-buttonino)
* [Basic include file](https://github.com/tedtoal/Button_TT/edit/main/README.md#basic-include-file)
* [BUTTON_TT_DBG and *monitor_printf.h*](https://github.com/tedtoal/Button_TT/edit/main/README.md#button_tt_dbg-and-monitor_printfh)
* [Defining pin numbers and constants for the LCD display controller](https://github.com/tedtoal/Button_TT/edit/main/README.md#defining-pin-numbers-and-constants-for-the-lcd-display-controller)
* [Defining a pointer to the LCD display object](https://github.com/tedtoal/Button_TT/edit/main/README.md#defining-a-pointer-to-the-lcd-display-object)
* [Defining button variables](https://github.com/tedtoal/Button_TT/edit/main/README.md#defining-button-variables)
* [Initializing the LCD display](https://github.com/tedtoal/Button_TT/edit/main/README.md#initializing-the-lcd-display)
* [Initializing buttons](https://github.com/tedtoal/Button_TT/edit/main/README.md#initializing-buttons)
* [Filling the blank screen and drawing the buttons](https://github.com/tedtoal/Button_TT/edit/main/README.md#filling-the-blank-screen-and-drawing-the-buttons)
* [Introduction to Adafruit_GFX fonts and Font_TT.h](https://github.com/tedtoal/Button_TT/edit/main/README.md#introduction-to-adafruit_gfx-fonts-and-font_tth)
* [Button_TT_label.h: buttons containing text](https://github.com/tedtoal/Button_TT/edit/main/README.md#button_tt_labelh-buttons-containing-text)
* [Adding touchscreen support and detection of button taps](https://github.com/tedtoal/Button_TT/edit/main/README.md#adding-touchscreen-support-and-detection-of-button-taps)
* [Playing a beep sound when button is tapped](https://github.com/tedtoal/Button_TT/edit/main/README.md#playing-a-beep-sound-when-button-is-tapped)
* [Using buttons showing integer values as labels](https://github.com/tedtoal/Button_TT/edit/main/README.md#using-buttons-showing-integer-values-as-labels)
* [Using triangular arrow buttons for increment/decrement](https://github.com/tedtoal/Button_TT/edit/main/README.md#using-triangular-arrow-buttons-for-incrementdecrement)
* [Using a button with an unsigned integer value](https://github.com/tedtoal/Button_TT/edit/main/README.md#using-a-button-with-an-unsigned-integer-value)
* [Turning off display backlight after no touch activity](https://github.com/tedtoal/Button_TT/edit/main/README.md#turning-off-display-backlight-after-no-touch-activity)
* [Storing settings in non-volatile memory](https://github.com/tedtoal/Button_TT/edit/main/README.md#storing-settings-in-non-volatile-memory)
* [Adding more screens](https://github.com/tedtoal/Button_TT/edit/main/README.md#adding-more-screens)
* [Adding a touchscreen calibration screen](https://github.com/tedtoal/Button_TT/edit/main/README.md#adding-a-touchscreen-calibration-screen)
* [Accessing and using button values](https://github.com/tedtoal/Button_TT/edit/main/README.md#accessing-and-using-button-values)
* [Creating new button styles: a button class with a "+" in it](https://github.com/tedtoal/Button_TT/edit/main/README.md#creating-new-button-styles-a-button-class-with-a--in-it)
* [Contact](https://github.com/tedtoal/Button_TT/edit/main/README.md#contact)

-------------------------

## Required additional libraries

The *Button_TT* library uses the *Adafruit_GFX_Library* graphics library, which requires a display with a controller object class that is derived (directly or indirectly) from class *Adafruit_GFX*. Many low-cost displays use the ILI9341 controller, which can use library *Adafruit_ILI9341* for this purpose. *You must install the **Adafruit_GFX_Library** library on your system **and** a compatible display controller library such as **Adafruit_ILI9341***. Also, typically your display will include a touchscreen, and to make use of it an additional library is required, **XPT2046_Touchscreen_TT**, which requires that your touchscreen use the XPT2046 controller (many low-cost touchscreens do).

## Introductory code without touchscreen

Here is simple code showing how to use this library to create a single on-screen rectangular button with no text and no user interaction.

```
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Button_TT.h>
#include <Adafruit_ILI9341.h>

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
// The button name can be anything and is unrelated to text on a button (it is
// useful at times for debugging).

// A plain button with no text, whose name is "Simple".
Button_TT btn_Simple("Simple");

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

  // Initialize btn_Simple with Top Center at (120,5) and size 40 wide x 20 tall,
  // with black outline and blue interior.
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

A more complex example is provided in the example program *HelloWorld.ino*, which adds a button containing text and touchscreen interaction, and it responds to touches of the button by changing its text.

## Quick start with Button.ino

If you'd prefer to skip the rest of this document and just quickly get started, the example program *Button.ino* illustrates fairly thorough use of the *Button_TT* library (although additional details are given in the sections below). That program is set up so that a **#define of *SELECT*** near the top of the file selects which features of the program are enabled. Start with *#define SELECT 0* and work up to *#define SELECT 10* to gradually add more and more features:
> 0 :  Just a rectangular button with no text and no response to a tap.
>
> 1 :  Respond to tap of button by changing its fill color.
>
> 2 :  Play a tone while button is tapped, if SAMD architecture is used.
>
> 3 :  Add a button with text in it. Tapping it changes the text.
>
> 4 :  Add a button with an integer value in it. Tapping it increments the value.
>
> 5 :  Add arrow buttons that increment/decrement the numeric value.
>
> 6 :  Add a button with an unsigned integer in it, with arrow buttons.
>
> 7 :  Add LCD backlight timer.
>
> 8 :  Add non-volatile memory storage of numeric button values if SAMD architecture.
>
> 9 :  Add touchscreen calibration, storing it non-volatilely if SAMD architecture.
>
> 10 : Add a new button class for a square button that displays a "+" in it.

The following sections introduce the Button_TT library functionality one piece at a time, following the graduated steps listed above.

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

Besides its use with BUTTON_TT_DBG, the *monitor_printf* library is recommended *in general* as an easy way to print numbers to the Arduino IDE serial monitor. Another useful companion library is *floatToString* for converting floating point numbers to strings for printing when full floating-point-to-string conversion is not enabled in your Arduino IDE.

## Defining pin numbers and constants for the LCD display controller

Early in your .ino file you should define constants for the I/O pins that connect to the display controller. The ILI9341 controller requires three pins. Here is an example with the pins defined for my particular system (a Nano 33 IoT processor in an MKR A-Z Touch case with ILI9341 display controller and XPT2046 touchscreen controller). You will need to know which pins on your system are connected and set these constants accordingly.

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

In order for your program to use the LCD display, you must create a pointer to a display controller object that knows how to talk to the display controller for writing text and graphics to the display. If your display uses the ILI9341 controller, the Adafruit_ILI9341 library provides a class of the same name for use as the display controller object. Here is how you define the pointer variable in your .ino file if you use that library:

```
// LCD display object.
Adafruit_ILI9341* lcd;
```

## Defining button variables

To create a button on the LCD display, you must define a variable that represents the button. The variable type is *Button_TT* or a class that derives from that, such as *Button_TT_label*. The various types of buttons are described later. The basic button type, *Button_TT*, is nothing more than a rectangle on the screen, containing no text.

You can define a variable for a simple button like this:

```
// A plain button with no text, whose name is "Simple".
Button_TT btn_Simple("Simple");
```

Take note of the button name, *Simple*, which is given as the argument to the button constructor. This can be any name you want (including an empty string). It is *not* shown as text inside the button. Instead, this name is used to display information about the button if you need to debug problems with the Button_TT library or its use by setting BUTTON_TT_DBG to 1. You may never need to use it, but it is advisable to define each of your buttons with a *unique name*.

A useful design pattern is to name the button variable in the format *`btn_<Name>`* as above. Later we'll see different types of buttons used for different purposes. You might adopt different naming prefixes for the different cases, such as *label_* for text labels, *btn_* for actual buttons, and *field_* for a value (numeric or text) that can be changed.

## Initializing the LCD display

The standard Arduino function *setup()* is used for initialization in Arduino programs. Before you can use the LCD display or create any buttons, you must create and assign the display controller object and then initialize the display. This is done in *setup()* as follows:

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

The next statements call functions in the Adafruit_ILI9341 object to initialize the display. Most of the functions have brief descriptions and argument declarations in *Adafruit_GFX.cpp* or *Adafruit_GFX.h*, so view those files for more detail on these functions:

> *begin()* is called first to do basic initialization of the ILI9341 controller.

> *setRotation()* is called next to set the desired rotation (orientation) of the display, a value between 0 and 3. The value of 2 above is the orientation I use in my project, but you should determine the orientation you need for yours. You will see the effect of rotation by changing its value and running a program that displays something.

> *setTextColor()* sets the color of text drawn on the screen, but this is not really necessary here, as normally the color is set just prior to drawing text, each time it is drawn. The constant *ILI9341_BLUE* is defined in Adafruit_ILI9341.h. That file defines several *ILI9341_* constants for colors, so view it for a full list of color constants. To view a #include file, right-click on the #include and choose *Go to Definition*.

> *setTextSize()* sets a magnification factor for text in the x- and y- directions. Usually I use a value of 1 in order to achieve the highest possible resolution of the text. Again, if used this is typically called each time text is drawn.

> *setTextWrap()* enables and disables automatic text wrapping. Normally this is not used and set false.

The last two statements turn on the LCD backlight. They use the third pin number constant, which is the pin that connects to the LCD backlight. Your code might choose to turn the backlight off and only turn it on when the touchscreen is touched.

## Initializing buttons

Each button variable that you define must be initialized, either in *setup()* or in a function called from it. The initialization is very important, as it sets several attributes of the button, such as its position on the display, its size, and outline and fill colors. For other types of buttons initialization can set other important attributes.

To initialize a button, call its *initButton()* function:

```
  // Initialize btn_Simple with Top Center at (120,5) and size 40 wide x 20 tall,
  // with black outline and blue interior.
  btn_Simple.initButton(lcd, "TC", 120, 5, 40, 20, ILI9341_BLACK, ILI9341_BLUE);
```

The first argument to *initButton()* is always the pointer to the display controller object (lcd here). The full set of arguments is something you will want to become familiar with. To do that, start by opening the *Button_TT.h* file in the Arduino IDE by right-clicking on its #include and choosing "Go to Definition". Scroll down to (or search for) the initButton() function. Above each function declaration in all Button_TT library header files you will find a set of comments (written in the *Doxygen* documentation style) that describe what the function does, what each of its arguments is, and what value if any it returns. There may also be important notes. Read through the initButton() arguments. They are repeated here:

```
gfx           Pointer to display object so we can draw button on it.

align         (x,y) alignment: TL, TC, TR, CL, CC, CR, BL, BC, BR, C
              where T=top, B=bottom, L=left, R=right, C=center, C=CC.

x             The pixel X coordinate of the button, relative to 'align'.

y             The pixel Y coordinate of the button, relative to 'align'.

w             Width of the button in pixels.

h             Height of the button in pixels.

outlineColor  Color of the outline (16-bit 5-6-5 standard).

fillColor     Color of the button fill (16-bit 5-6-5 standard).

expU          Expand button up by this many pixels when contains() tests a point.

expD          Like expU but expansion downwards.

expL          Like expU but expansion to the left.

expR          Like expU but expansion to the right.
```

The second argument, *align*, is a text string of one or two characters that tells how the button is to be aligned relative to the button position (x,y) specified by the x and y arguments. This is very handy for easily centering buttons or left- or right- justifying them. In the btn_Simple example above, the value "TC" is specified, meaning the button is to be justified against the top edge (i.e. the top of the button will be at the specified y coordinate of 5) and centered left-right (so the button center is at the specified x coordinate of 120, which is halfway across the 240-pixel-wide display in this example). Note that justification of text within a labelled button is handled entirely separately by a different argument to the *Button_TT_label.initButton()* function, described later.

The next four arguments (x, y, w, and h) are straightforward. They give the button (x,y) position as (120, 5), and the button size (w,h) in pixels as (40, 20), w being the width and h being the height.

The next two arguments (outlineColor and fillColor) are simply ILI9341_ color constants (from Adafruit_GFX.h) specifying the color of the button outline and interior. If both are the same color, the button appears as a simple rectangle with no outline. Also, a special constant, *TRANSPARENT_COLOR*, is defined in Button_TT.h and it can be used anywhere within the *Button_TT* library where a color value is needed. It causes the graphic element assigned that color to not be drawn. If outlineColor were specified to be TRANSPARENT_COLOR, the outline of the button would not be drawn, so the color of the background screen fill would show. Likewise, if fillColor were that color, the button interior would not be drawn and the background screen fill would show.

When reading the Button_TT.h file comments, pay attention to the actual function declaration and what the default values are for arguments. For this function you can see that the defaults for the last four "exp*" arguments are all 0:

```
              uint8_t expU = 0, uint8_t expD = 0, uint8_t expL = 0, uint8_t expR = 0
```

As the comment says, the purpose of these last four arguments is to expand the size of the button used by the *contain()* function. That function will be described later, but it tests a point (x,y) to see if it lies within the button, and these expansion parameters allow for testing a larger area than the actual button, which can help when a finger is used to touch a touchpad. They are useful when there are no other nearby buttons on the screen.

## Filling the blank screen and drawing the buttons

After initializing the display and button variables in setup(), you will fill the screen with a background color, draw the buttons, and draw any other graphical objects you want. This can be done in setup(), but in programs that display different screens at different times, you would typically define a function to draw each different screen. The code for our example is simple:

```
  // Fill screen with white.
  lcd->fillScreen(ILI9341_WHITE);

  // Draw btn_Simple button.
  btn_Simple.drawButton();
```

With nothing more than the code presented above, you can run a program (whose loop() function does nothing) and you should see the display filled with white and the button appearing as a black rectangle with blue interior at the specified position.

## Introduction to Adafruit_GFX fonts and Font_TT.h

The *Adafruit_GFX_Library* library supports the writing of text to displays, and this includes support for a built-in mono-spaced font and additional proportionally-spaced fonts. Its file *gfxfont.h* provides definitions for using the proportional fonts, including defining a C++ struct named *GFXfont* that contains all information for one proportional font. Adafruit_GFX.h automatically #includes gfxfont.h.

The *Fonts* subfolder within the *Adafruit_GFX_Library* library folder contains header files, one for each proportional font that is supported. The file names indicate the font types and sizes and styles. For example, file *Fonts/FreeSansBoldOblique12pt7b.h* contains the font data for a sans-serif font in boldface italic (oblique) whose size is 12 points. A single instance of the *GFXfont* struct is defined by each header file, and the name of the instance is the same as the .h filename with the .h dropped. For example, the instance created by file *Fonts/FreeSans12pt7b.h* is named *FreeSans12pt7b*. That instance contains *the actual font data* specifying the form of each of the characters, and it is defined in a way such that the data will be placed in EEPROM memory rather than RAM memory, since the data is fixed and doesn't change. You must #include the font header file for each proportional font you wish to use. If you have limited EEPROM memory, you may find that you are limited to just a few fonts maximum. To see which fonts will work well in your project, load different fonts and use them to display text, as shown below.

When you are using a proportional font, the *Adafruit_GFX_Library* uses a pointer to the *GFXfont* class instance for the font, set using its *setFont()* function. For example, to use the *FreeSans12pt7b* font, you would `#include <Fonts/FreeSans12pt7b.h>` and you would call the display controller function `setFont(&FreeSans12pt7b)`, where the `&` creates a pointer to the *FreeSans12pt7b GFXfont* instance. If instead you are using the built-in font, you call `setFont(nullptr)` with a value of *nullptr* instead of an actual *GFXfont* pointer, to indicate use of the built-in font.

Here is a short program (aka *sketch*) that prints two text strings to the display, one using the built-in font and one using a proportional font:

```
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Adafruit_ILI9341.h>

// LCD display pin definitions.
#define LCD_DC_PIN          2
#define LCD_CS_PIN          10
#define LCD_BACKLIGHT_PIN   A2

// LCD display backlight definitions.
#define LCD_BACKLIGHT_OFF   HIGH
#define LCD_BACKLIGHT_ON    LOW

// LCD object.
Adafruit_ILI9341* lcd;

// Standard Arduino setup() function.
void setup() {

  // Create LCD object.
  lcd = new Adafruit_ILI9341(LCD_CS_PIN, LCD_DC_PIN);

  // Initialize the LCD display.
  lcd->begin();
  lcd->setRotation(2);   // portrait mode
  lcd->setTextSize(1);
  lcd->setTextWrap(false);

  // Turn on backlight.
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);

  // Fill screen with white.
  lcd->fillScreen(ILI9341_WHITE);

  // Write text to display.
  lcd->setTextColor(ILI9341_BLUE);
  lcd->setFont(nullptr);
  lcd->setCursor(10, 50);
  lcd->print("Hello world!");

  lcd->setTextColor(ILI9341_RED);
  lcd->setFont(&FreeSans12pt7b);
  lcd->setCursor(10, 100);
  lcd->print("Goodbye world!");
}

// Standard Arduino loop() function.
void loop() {
}
```

An important caveat is to make sure you *call setCursor() **after** setFont()*. This is because *setFont()* (in the current library version at any rate) modifies the current cursor position, which it should not do. The reasoning for doing that rather than handling the problem it is intended to address in a different way is faulty.

The *Button_TT* library provides files *Font_TT.h* and *Font_TT.cpp* that expand on the Adafruit_GFX_Library font support by providing functions to better compute text string sizes and positions to allow for accurate alignment of text on the display. They define the class *Font_TT*, which holds a pointer to the *GFXfont* struct for *one proportional font* or holds a nullptr value to indicate use of the built-in font. You will need to create one instance of that class for each different font you use, whether the built-in font or a proportional font, as shown below.

The *Font_TT* class doesn't have a *setFont()* function, but instead you pass a *GFXfont* instance pointer to the Font_TT constructor when you declare a variable of type Font_TT. Each Font_TT instance is for a single font. You pass *nullptr* to that constructor to indicate that the instance is for the built-in font. (The *Font_TT::getFont()** function returns that pointer, and it is used in the Button_TT library when *Adafruit_GFX_Library* text functions are called.)

Here is a modification of the above program that uses Font_TT function *getTextAlignCursor()* to compute the cursor position so that text strings will be aligned in the desired way, putting "Hello world!" top-center and "Goodbye world!" bottom-right.

```
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Adafruit_ILI9341.h>
#include <Font_TT.h>

// LCD display pin definitions.
#define LCD_DC_PIN          2
#define LCD_CS_PIN          10
#define LCD_BACKLIGHT_PIN   A2

// LCD display backlight definitions.
#define LCD_BACKLIGHT_OFF   HIGH
#define LCD_BACKLIGHT_ON    LOW

// LCD object.
Adafruit_ILI9341* lcd;

// Font_TT objects.
Font_TT fontBuiltIn(nullptr);
Font_TT fontSans12(&FreeSans12pt7b);

// Standard Arduino setup() function.
void setup() {
  const char* str;
  int16_t xC, yC;

  // Create LCD object.
  lcd = new Adafruit_ILI9341(LCD_CS_PIN, LCD_DC_PIN);

  // Initialize the LCD display.
  lcd->begin();
  lcd->setRotation(2);   // portrait mode
  lcd->setTextSize(1);
  lcd->setTextWrap(false);

  // Turn on backlight.
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);

  // Fill screen with white.
  lcd->fillScreen(ILI9341_WHITE);

  // Write "Hello world!" to display, centered (C) at the top (T) of the display
  // using the built-in font.
  lcd->setTextColor(ILI9341_BLUE);
  lcd->setFont(fontBuiltIn.getFont());
  str = "Hello world!";
  fontBuiltIn.getTextAlignCursor(str, 0, 0, lcd->width(), lcd->height(), 'C', 'T', xC, yC);
  lcd->setCursor(xC, yC);
  lcd->print(str);

  // Write "Goodbye world!" to display, right-justified (R) at the bottom (B) of
  // the display using the sans-serif 12-point font.
  lcd->setTextColor(ILI9341_RED);
  lcd->setFont(fontSans12.getFont());
  str = "Goodbye world!";
  fontSans12.getTextAlignCursor(str, 0, 0, lcd->width(), lcd->height(), 'R', 'B', xC, yC);
  lcd->setCursor(xC, yC);
  lcd->print(str);
}

// Standard Arduino loop() function.
void loop() {
}
```

## Button_TT_label.h: buttons containing text

Usually buttons on the display have text inside them to indicate what the button does. Buttons with text are supported by the *Button_TT_label.h* and *Button_TT_label.cpp* files, which define a new C++ class named *Button_TT_label* that is derived from the basic button class *Button_TT*.

To create a button with text (a *labelled* button), start by including additional header files at the start of your .ino file:

```
// Include files when any button with text is used.
#include <Font_TT.h>
#include <Button_TT_label.h>

// Include the proportional font files that are used. Built-in font is always available.
#include <Fonts/FreeSans12pt7b.h>
```

The *Font_TT.h* file was introduced in the previous section, as were the header files in the *Adafruit_GFX_Library* *Fonts* subfolder. Above, the font file for the sans-serif 12-point proportionally-spaced font is #included. For each included font file, a separate *Font_TT* object must be created, passing a pointer to the font to the *Font_TT* constructor:

```
// Create a Font_TT object for each proportional font that is used.
Font_TT font12(&FreeSans12pt7b);
```

If the built-in font is used, another separate *Font_TT* object must be created, using *nullptr* as the constructor argument:

```
// Create a Font_TT object for the built-in font if it is used.
Font_TT fontBuiltin(nullptr);
```

To create a labelled button, define a variable of type *Button_TT_label*:

```
// A labelled button whose name is "Hello".
Button_TT_label btn_Hello("Hello");
```

As with the simple button shown earlier, each labelled button must be initialized by calling the *initButton()* function from *setup()* (or from some other function called by *setup()*):

```
  // Initialize btn_Hello.
  btn_Hello.initButton(lcd, "TC", 120, 30, 200, 26, ILI9341_BLACK,
    ILI9341_LIGHTGREY, ILI9341_BLUE, "C", "Hello World!", false, &font12);
```

This button is being aligned against the top edge (T) of the specified position (120,30) and horizontally centered (C{ at that point. The button size is 200 pixels wide by 26 pixels tall, it has a black outline and a light grey interior.

Setting the appropriate values for the *initButton()* parameters in all the various buttons of a project is a tedious job. In particular, getting the button positions and sizes right to produce a pleasing screen layout is very difficult. Professional software development environments typically have *layout manager* software available at run time or design time to make the layout job easier. For Arduino, we keep it simple, and insist that the programmer go through the tedious work of setting parameter values, running the code to test the appearance, making adjustments to values, running again, etc. Your code should be set up in a way that makes it easy to enable a particular screen at startup, which you would do when adjusting the layout of that screen. A later section shows how to handle multiple screens in a clean manner and set up for drawing a particular screen at startup.

There is a different *initButton()* function for each different type of button (for each different button class), and each one has its own unique arguments. Therefore, you should load the *Button_TT_label.h* file into the Arduino IDE and locate and inspect the *initButton()* comments and default arguments. Typically, new types of buttons are based on simpler buttons, and the *initButton()* function has most of the same arguments as the simpler button did, with additional arguments added. The arguments that are the same are *not necessarily in the same order or position*. Usually, the first 8 arguments are the same as the first 8 of *Button_TT::initButton()*, and the last four are the same as the last four of that function, with new arguments appearing after the first 8. Also, sometimes arguments may acquire new or modified meaning, so pay close attention to the argument comments. In the case of *Button_TT_label*, the *w* and *h* (width and height) arguments acquire an additional meaning:

```
w       Width of the button, non-positive to compute width from label and add abs(w).
h       Height of the button, non-positive to compute height from label and add abs(h).
```

For labelled buttons, it is sometimes desirable to let the code compute the size of the button based on the size of the text string inside it. This is accomplished by using *negative or zero* values for the button width and/or height! The absolute value of the negative or zero number is taken as the number of pixels of *additional width or height* to add beyond the auto-computed size. The size computation is done in *initButton()* and never subsequently, so if the button's label string is one that can change, it is crucial that the largest possible string be used in the *initButton()* call.

*Button_TT_label* also has new *initButton()* arguments after the first 8:

```
textColor     Color of the button label (16-bit 5-6-5 standard).

textAlign     Like 'align' but gives alignment for label, 1st char = up/down alignment, 2nd = left/right.

label         String of the text inside the button, should be the largest possible label if w or h is
              negative to ensure all possible labels will fit in the size that is automatically computed.

degreeSym     If true, a degree symbol is drawn at the end of the label.

f             Pointer to Font_TT object for the custom font to use for the label, nullptr for built-in font.

rCorner       Radius in pixels of rounded corners of the button outline rectangle, 0 for straight corners.
```

*textColor* specifies the text color (blue in the example above), *textAlign* specifies the text alignment within the button rectangle (center alignment above), and *label* gives the button label ("Hello world!" above). The button and text colors, text alignment, and text itself can be changed at any time by calling other button functions (such as *Button_TT_label::setLabel()*), read through the class functions in the header file.

The Adafruit_GFX_Library fonts do not include a symbol for the *degree character*, e.g. 65°. This could be drawn manually using circle-drawing functions in that library. However, the Button_TT library was used in a thermostat and it was desired to provide better support for that symbol, without actually modifying the font files to add the symbol. Instead, the *initButton()* *degreeSym* argument is provided. It is either true or false (default). When true, a degree symbol is automatically drawn after the label text and is included in the text alignment algorithm. In the code above, *degreeSym* is specified as the default value of false so that the following argument, *f* (the font for the text), can be specified.

In the above example the *f* argument is specified as *&font12*, referring to the font object *font12* created earlier, so that will be the font used to draw the button label. The font can also be changed later by calling *Button_TT_label::setFont()*.

The Button_TT simple buttons are drawn as rectangles with straight corners, but Button_TT_label buttons can optionally use rounded corners. The *rCorner* argument specifies the radius of the corners in pixels, with the default value of 0 giving straight corners.

The last piece of code necessary for the labelled button is to draw it using a *drawButton()* call within setup() (or a function called by it):

```
  btn_Hello.drawButton();
```

Buttons don't always need to be actual buttons to be tapped by a user. Label buttons in particular are useful simply as labels on the display. It may be more convenient to use buttons for text labels rather than simply writing the text string to the display, because then each screen element is created in the same manner and the various features of buttons, such as adjustable color, can be easily used.

When buttons are used as text strings but are not tappable, you will often want to display the text string on the screen background without placing it inside a rectangle. There are two ways to do this, first by simply setting the button outline and fill colors equal to the screen background color. The other method is to set these colors to *TRANSPARENT_COLOR* so the outline and fill are not drawn and the background color shows. The advantage of the latter method is that it speeds up screen drawing. However, the method cannot be used when the button text is changed, unless each time it is changed the entire screen is redrawn. If only the button is redrawn and not the entire screen, the fill color must not be *TRANSPARENT_COLOR* so that when the button is filled it erases the old text string.

Often, though, one wants to use a button for more than just a label, allowing the user to tap it, detect the tap, and perform an appropriate action. The next section describes how to do this.

## Adding touchscreen support and detection of button taps

The *Button_TT* library provides support to make detection of and response to button taps easier to code. For this, the library requires that your touchscreen uses an XPT2046 controller, which is common on low-cost touchscreens. The touchscreen must be controllable using the library *XPT2046_Touchscreen_TT*, which is an enhanced version of the *XPT2046_Touchscreen* library. As long as the *XPT2046_Touchscreen* library works with your touchscreen, you can install the *XPT2046_Touchscreen_TT* library to provide button tap support. Example programs in that library allow you to test that it works correctly by itself and with your display.

The *XPT2046_Touchscreen_TT* library has two pairs of files supporting the touchscreen. Files *XPT2046_Touchscreen_TT.h/.cpp* define class XPT2046_Touchscreen_TT that talks to the XPT2046 controller and has functions to test for taps and get their positions. Files *TS_Display.h/.cpp* define class TS_Display that provides several services for working with a combination of a touchscreen and a display, such as mapping touchscreen coordinates to display coordinates. To use the touchscreen with the *Button_TT* library, start by including both of the header files near the top of your .ino file:

```
// Include files to use touchscreen with display.
#include <XPT2046_Touchscreen_TT.h>
#include <TS_Display.h>
```

The file pair *Button_TT_collection.h/.cpp* in the *Button_TT* library defines a class *Button_TT_collection* which manages a set of tappable buttons that are currently displayed on the screen. You must create one instance of the class and register tappable buttons with it, with a function to be called if the button is tapped. When a new screen is displayed with different buttons, the registered buttons must be cleared and the buttons for the new screen registered. To use this class, start by including the header file:

```
// Include to manage tappable buttons.
#include <Button_TT_collection.h>
```

The touchscreen requires two signals from the microcomputer to access it, and these must be defined, usually near the top of your .ino file:

```
// Touchscreen pin definitions.
#define TOUCH_CS_PIN        A0
#define TOUCH_IRQ_PIN       A7
```

The above are examples, change the values to match the pins connected to the touchscreen on your system.

Three variables must be defined that point to object instances. The first points to an *XPT2046_Touchscreen_TT* object and is used to interact with the touchscreen. The second variable points to a *TS_Display* object and is used for mapping between touchscreen and display coordinates. The third points to a *Button_TT_collection* object and used to register buttons to be monitored for taps. Declare these pointer variables near the top of your .ino file before function definitions start:

```
// Touchscreen object.
XPT2046_Touchscreen* touch;

// Touchscreen-display object.
TS_Display* ts_display;

// Button collection object to manage taps of buttons.
Button_TT_collection* screenButtons;
```

All three variables must be initialized, which involves allocating and assigning an object instance to them, followed by one or more calls to member functions to perform required initialization. It is during the *touch* object initialization that the touchscreen pin numbers defined earlier are used. *Typically this is all done within the standard Arduino **setup()** function, **after** initializing the display object*. For example:

```
  // Create and initialize touchscreen object, same rotation as lcd.
  touch = new XPT2046_Touchscreen(TOUCH_CS_PIN, TOUCH_IRQ_PIN);
  touch->begin();
  touch->setRotation(lcd->getRotation());

  // Create and initialize touchscreen-LCD object.
  ts_display = new(TS_Display);
  ts_display->begin(touch, lcd);

  // Create and initialize button collection object.
  screenButtons = new Button_TT_collection;
  screenButtons->clear();
```

A function must be defined for each button that must respond to a tap, to perform the desired action and then redraw the button if the action changed it. Although in general each tappable button has its own tap function, in some cases it makes sense to use the same function for multiple buttons, and this will be shown later. In this example we have already defined two buttons that we want to respond to taps, the *Simple* and *Hello* buttons. Let's make a tap of the *Simple* button toggle its fill between two colors, and a tap of the *Hello* button toggle its text between two labels:

```
// Handle tap of "btn_Simple". Toggle button fill color and draw the button.
void btnTap_Simple(Button_TT& btn) {
  uint16_t color = btn_Simple.getFillColor();
  btn_Simple.setFillColor(color == ILI9341_BLUE ? ILI9341_RED : ILI9341_BLUE);
  btn_Simple.drawButton();
}

// Handle tap of "btn_Hello". Toggle its text and draw it.
void btnTap_Hello(Button_TT& btn) {
  const char* label = btn_Hello.getLabel();
  bool isHello = (label[0] == 'H');
  btn_Hello.setLabel(!isHello ? "Hello World!" : "Goodbye World!");
  btn_Hello.drawButton();
}
```

The above functions make use of the button functions *getFillColor()*, *setFillColor()*, *getLabel()*, and *setLabel()*, and they both call the *drawButton()* function to redraw the button on the screen, since its appearance has been changed.

The button tap functions must be associated with their actual button variables by registering each one with the *screenButtons* object. This may be done during initialization, either directly within *setup()* or in a function called by it. Note that often there will be several different screens that are displayed at different times depending on user activity, and each time a new screen is displayed, its buttons must be re-registered. This implies that screen-drawing functions should be defined. This and other complexities introduced by using multiple screens will be introduced later. Here, we will assume that the registration of buttons is done within *setup()*:
  
```
  // Register buttons with screenButtons object to handle button taps/releases.
  screenButtons->registerButton(btn_Simple, btnTap_Simple);
  screenButtons->registerButton(btn_Hello, btnTap_Hello);
```

The last thing to do to support button taps is to define a function that checks for touchscreen taps. It is called from the standard Arduino loop() function. It uses the *ts_display* instance to test for a touchscreen tap, and the *screenButtons* instance to find the button that was tapped and call its tap function to perform the desired button action:

```
// Check for touch screen tap or release and use ScreenButtons to process it.
void processTapsAndReleases() {
  int16_t x, y, pres;

  // Check for a button tap or release event.
  switch (ts_display->getTouchEvent(x, y, pres)) {

  // Handle a touch event.
  case TS_TOUCH_EVENT:
    // Check for a button tap and if so, call its tap function.
    screenButtons->press(x, y);
    break;

  // Handle a release event.
  case TS_RELEASE_EVENT:
    // Check for a button release. Only one button tap at a time can be detected.
    screenButtons->release();
    break;
  }
}
```

The logic of the *processTapsAndReleases()* function is straightforward. The call to the *ts_display* object's *getTouchEvent()* function tests to see if the touchscreen has been tapped (or a tap has been released), and if so, returns the (x,y) display coordinates of the tap. The *screenButtons* object's *press()* function is called to test each registered button to see if it was the one that was tapped, and it does that by calling the *contains()* function of each registered button, passing it the coordinates (x,y) of the tapped point. The button *contains()* function (defined in the base *Button_TT* class and therefore available for all types of buttons) tests to see if (x,y) lies within the interior of the button boundary rectangle, returning *true* if so. A *true* return causes *press()* to call the registered button's tap function (such as *btnTap_Simple()*).

The body of the *processTapsAndReleases()* function could instead be placed directly within the Arduino *loop()* function, but it is cleaner to define it as a separate function and call it from *loop():*

```
// Standard Arduino loop() function.
void loop() {
  // Process button taps/releases for the current screen, which has registered
  // its buttons with the screenButtons object.
  processTapsAndReleases();
}
```

## Playing a beep sound when button is tapped

Usually it is desirable to provide some audible feedback when the user taps a button. Here (and in the *Button.ino* example program) we use the *SAMD_PWM* library, which requires that your microprocessor use the SAMD architecture, to produce the sound. (If you have a different architecture, you will have to adapt this code to use your preferred sound library). The *SAMD_PWM* library defines class *SAMD_PWM*, which talks to the sound hardware to produce sounds. To use the library, you must include its header file, define the hardware pin that is attached to the sound device (BEEPER_PIN), define constants that determine the particular sound to be produced when a button is tapped, and define a pointer to a *SAMD_PWM* object:

```
// Include file for SAMD beeper tones (requires SAMD architecture).
#ifdef ARDUINO_ARCH_SAMD
#define _PWM_LOGLEVEL_ 0
#include <SAMD_PWM.h>

// Beeper pin definition.
#define BEEPER_PIN          A3

// Beeper tone definitions.
#define TS_TONE_FREQ        3000  // Frequency to play when touch screen is tapped.
#define TS_TONE_DUTY        50    // Duty cycle in % of square wave tone, 0 for off.

// PWM object for sound from beeper.
SAMD_PWM* sound;

#else
#error SAMD architecture is required to play a beep!
#endif
```

The SAMD_PWM object must be created and assigned to the *sound* pointer, usually within the *setup()* function. This is where the sound device pin number is used, and there may also be a need for sound constants, such as TS_TONE_FREQ here:

```
  // Create PWM object for sound from beeper (requires SAMD architecture).
  sound = new SAMD_PWM(BEEPER_PIN, TS_TONE_FREQ, 0);
```

To create a sound when a button is pressed, a function must be defined that will start or stop a sound when a button is pressed or released. Here, for clarity, we define one function to start or stop a sound, and a second one, called the *master button tap/release function*, that is called when a button is pressed or released and it calls the first function to start or stop the sound:

```
// Play (true) or stop playing (false) a sound.
void playSound(bool play) {
  sound->setPWM(BEEPER_PIN, TS_TONE_FREQ, play ? TS_TONE_DUTY : 0);
}

// Master button tap/release function. Tap: start tone. Release: end tone.
void buttonTapRelease(bool tap) {
  playSound(tap);
}
```

Finally, the last thing to do is to register the master button tap/release function with the *screenButton* object so that it will call the function as buttons are pressed and released. This is usually done within *setup():*

```
  // Register master tap/release function to turn tone on/off at screen taps.
  screenButtons->registerMasterProcessFunc(buttonTapRelease);
```

That is all that is necessary to implement audible button press/release feedback.

## Using buttons showing integer values as labels

Often there is a need to show number values that the user is able to alter. The *Button_TT* library supports this with four classes, each derived from class *Button_TT_label*, one class for each of four different types of numeric values: signed and unsigned 8-bit and 16-bit integers. The classes are:

> Button_TT_uint8: unsigned 8-bit integer
>
> Button_TT_uint16: unsigned 16-bit integer
>
> Button_TT_int8: signed 8-bit integer
> 
> Button_TT_int16: signed 16-bit integer

All four classes are used the same way. The example here uses the *Button_TT_int8* class.

Typically these are used as non-tappable buttons. That is, they are only buttons in the sense that they create numeric text strings on the display. They might not even show the number in a rectangle, but rather, as a number directly on the screen background (by setting the button outline and fill colors equal to the screen background color). There is nothing that prevents making them tappable. For example, a program might have a tap call up a keypad where the user could enter a new value. However, the simplest implementation is to use these as non-tappable buttons and then add a pair of *arrow* buttons next to each "integer" button. The arrow buttons can be tapped to increment and decrement the number. This section only shows how to create an "integer" button. The next section will show how to create and use arrow buttons to change the integer button values.

As with other button styles, the first step is to include the header file for the button style:

```
// Include file for using button with int8_t value.
#include <Button_TT_int8.h>
```

Then, define a variable of the desired button class, giving it a unique button name:

```
// An int8_t button whose name is "int8Val".
Button_TT_int8 btn_int8Val("int8Val");
```

The button variable is then initialized by calling its *initButton()* function. Again, the *initButton()* function comments and default argument values should first be examined in the *Button_TT_int8.h* file. The new arguments for this style of button, starting after the button rectangle corner radius argument *rCorner* that we saw previously, are:

```
value         The int8_t value for the button, used to create the label string for the button.
minValue      The minimum allowed value for value.
maxValue      The maximum allowed value for value.
degreeSym     If true, a degree symbol is drawn at the end of the label.
showPlus      If true, a leading + is used for positive values in the label.
checkValue    If not nullptr, a pointer to a function that checks a new button
              value and can return an adjusted value if it is out of range.
```

There are usually a lot of buttons defined in a program, and often many of the *initButton()* argument values are *the same* for many of them. It is therefore useful to define, in a section near the start of the .ino file, constants for the common *initButton()* argument values. For example, if rounded buttons are used but they always have the same corner radius, the following definition would be helpful for improving program clarity. Note the short name, to keep *initButton()* call statements shorter, since they tend to be annoyingly long:

```
// Useful constants for initializing buttons. These can be used as arguments to
// the initButton() function.

// Corner radius of buttons with rounded corners, in pixels.
#define RAD 10
```

With that constant available, *initButton()* is called, typically in *setup()* or a function called by it:

```
  // Initialize btn_int8Val. Give it rounded corners, with initial value -5 and range -10 to +10.
  btn_int8Val.initButton(lcd, "TL", 35, 68, 50, 26, ILI9341_BLACK, ILI9341_LIGHTGREY,
    ILI9341_RED, "C", &font12, RAD, -5, -10, +10);
```

The most important initialization values are *value*, *minValue*, and *maxValue*, all *int8_t* types. The integer button classes each maintain three integer values as part of the class instance, one for the current button value and two for the minimum and maximum allowed values. One advantage of using these classes is that they automatically enforce the minimum and maximum values. Here, the initial value is -5 and the allowed range is from -10 (minValue) to +10 (maxValue).

Three *initButton()* arguments weren't used above, *degreeSym*, *showPlus*, and *checkValue*. The *degreeSym* argument will be shown in a later section. The *showPlus* argument simply causes a "+" to be shown for positive values when showPlus=true. The "+" is omitted if showPlus=false (the default value). Finally, the *checkValue* argument is for advanced checking of button values and is left for the programmer to explore when needed.

Finally, the button must be drawn, often in a screen draw function but perhaps simply within *setup():*

```
  btn_int8Val.drawButton();
```

Although typically a button like this does not have a button tap handler, we'll add one for illustration purposes. Tapping the button will *increment* the button value, wrapping it from its maximum value back to its minimum value:

```
// Handle tap of "btn_int8Val". Increment the value and wrap from max to min.
void btnTap_int8Val(Button_TT& btn) {
  int8 value = btn_int8Val.getValue() + 1;
  int8 minValue = btn_int8Val.getMinValue();
  int8 maxValue = btn_int8Val.getMaxValue();
  if (value > maxValue)
    value = minValue;
  btn_int8Val.setValueAndDrawIfChanged(value);
}
```

The button tap function uses the *Button_TT_int8* class functions *getValue()*, *getMinValue()*, *getMaxValue()*, and *setValueAndDrawIfChanged()*.

The tap function must of course be registered with the *screenButtons* object, perhaps from *setup():*

```
screenButtons->registerButton(btn_int8Val, btnTap_int8Val);
```

Thus, making a button tappable, once the basic tap code shown earlier is in place, is as simple as defining a tap function and registering it whenever the button is displayed.

## Using triangular arrow buttons for increment/decrement

The *Button_TT* library includes another pair of files, *Button_TT_arrow.h/.cpp* that define class *Button_TT_arrow*, which implements another button style: *unlabelled* triangular-shaped buttons. Their intended use is as *increment* and *decrement* buttons for changing numeric values. As usual, the first step to use this style of buttons is to include the header file:

```
// Include file for using triangle buttons, usually used as arrow buttons.
#include <Button_TT_arrow.h>
```

Then, a variable is defined for each arrow button. They are usually defined in pairs, one for increment and one for decrement, and coding clarity is improved by naming the button variables the same as the name of the button they will increment or decrement, with a suffix indicating which one it is. Each button can be oriented so that its tip lies in any of the four directions up, down, left, or right. It works well to place a button pair side-by-side, the left one with the tip to the left and the right with tip to right. This helps conserve limited screen space. The left-facing button would decrement the value and the right-facing button would increment the value:

```
// Define two arrow buttons (left and right, for decrement and increment) to go with btn_int8Val.
Button_TT_arrow btn_int8Val_left("int8Val_left");
Button_TT_arrow btn_int8Val_right("int8Val_right");

// The tap function is shared by both arrow buttons. It invokes valueIncDec() on the int8val button.
void btnTap_int8Val_delta(Button_TT& btn) {
  btn_int8Val.valueIncDec(1, &btn);
}
```

The *Button_TT_int8* function *valueIncDec()* handles all the work of incrementing or decrementing the button value, determining which of the two registered buttons for the function was the actual button that was tapped by comparing the *btn* argument to the two button variables. It increments or decrements the button value depending on which button was tapped, and it honors the minValue and maxValue settings by doing nothing once the limit is reached. The *amount* that the function adds to or subtracts from the button value is given by the first argument to the function, 1 above.

The *initButton()* function for arrow buttons has the same arguments as the basic *Button_TT* button style, with the addition of one argument *(orient)* just after the first argument and two arguments after the x and y arguments:

```
orient  Orientation of triangle: U=UP, D=DOWN, L=LEFT, R=RIGHT.
s1      Length of the base of the isosceles triangle.
s2      Length of the other two triangle sides.
```

Using the same length for all three triangle sides looks good:

```
  // Initialize arrow buttons for int8val button.
  btn_int8Val_left.initButton(lcd, 'L', "TR", 120, 65, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);
  btn_int8Val_right.initButton(lcd, 'R', "TL", 130, 65, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);
```

Notice that the left-pointing triangle uses an *orient* value of 'L' while the right-pointing one uses 'R', and the *align* value that follows uses the opposite letters for the x-direction alignment, so the left-pointing one aligns its right (R) side at position x=120 and the right-pointing one aligns its left (L) side at position x=130.

The buttons are drawn just after the *drawButton()* call for the associated *btn_int8Val* button:

```
  btn_int8Val_left.drawButton();
  btn_int8Val_right.drawButton();
```

And they are registered just after that:

```
  screenButtons->registerButton(btn_int8Val_left, btnTap_int8Val_delta);
  screenButtons->registerButton(btn_int8Val_right, btnTap_int8Val_delta);
```

With the above code, a pair of arrow buttons is implemented that will respectively decrement and increment the btn_int8Val value.

## Using a button with an unsigned integer value

The previous sections illustrated use of a signed integer button. Here code is shown for using an unsigned integer button, of class *Button_TT_uint8*, with a pair of arrow buttons for increment and decrement. There are a couple minor differences.

Start as usual by including the header file and defining the button variables and tap function:

```
// Include file for using button with uint8_t value.
#include <Button_TT_uint8.h>

// A uint8_t button whose name is "uint8Val".
// There is no tap handler for this button, tapping it does nothing. It changes
// its value when one of the arrow buttons to its right is tapped.
Button_TT_uint8 btn_uint8Val("uint8Val");

// Define two arrow buttons (left/right, for decrement/increment) to go with btn_uint8Val.
Button_TT_arrow btn_uint8Val_left("uint8Val_left");
Button_TT_arrow btn_uint8Val_right("uint8Val_right");

// The tap function is shared by both arrow buttons.
void btnTap_uint8Val_delta(Button_TT& btn) {
  btn_uint8Val.valueIncDec(1, &btn);
}
```

The *initButton()* function for unsigned integer buttons has one argument difference with that for signed integer buttons: instead of the *showPlus* argument (unsigned integer buttons never shown a sign since the value is unsigned) there is an argument named *zeroString*, which is a character string value. It provides a string to display instead of "0" when the unsigned integer value is 0. Its default value is *nullptr*, which means "0" is displayed for "0". It is provided for cases where the code uses a value of 0 to mean something different such as "OFF".

Call *initButton()* to initialize each button:

```
  // Initialize btn_uint8Val. Give it rounded corners, with initial value 10 and
  // range 0 to 10, and add a degree symbol after it. Display "--" for 0.
  btn_uint8Val.initButton(lcd, "TL", 35, 113, 50, 26, ILI9341_BLACK, ILI9341_LIGHTGREY,
    ILI9341_RED, "C", &font12, RAD, +10, 0, +10, true, "--");

  // Initialize arrow buttons for uint8val button.
  btn_uint8Val_left.initButton(lcd, 'L', "TR", 120, 110, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);
  btn_uint8Val_right.initButton(lcd, 'R', "TL", 130, 110, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);
```

Above we see that "--" is used for the value of the *zeroString* argument, to show that string instead of "0" for the value 0. The *degreeSym* argument is true, which causes a small superscript circle to be automatically displayed after the number, indicating *degrees*. This feature obviates the need for devising a font that contains a degree symbol.

The *drawButton()* function is called to draw each button:

```
  btn_uint8Val.drawButton();
  btn_uint8Val_left.drawButton();
  btn_uint8Val_right.drawButton();
```

Finally, the *screenButtons* *registerButton()* function is called to register each arrow button to make it tappable:

```
  screenButtons->registerButton(btn_uint8Val_left, btnTap_uint8Val_delta);
  screenButtons->registerButton(btn_uint8Val_right, btnTap_uint8Val_delta);
```

## Turning off display backlight after no touch activity

Often a desired feature of a system with a backlit display (such as an LCD display) is to turn off the backlight after sufficient time has elapsed with no activity. Although support for that feature is not specifically part of the *Button_TT* library, it can easily be implemented within the framework already presented above.

The first step is to define a constant that gives the number of milliseconds after the last screen touch before turning off the backlight. This definition is placed together with the backlight on/off values:

```
// Number of milliseconds after last screen touch before backlight is turned off.
#define LCD_BACKLIGHT_AUTO_OFF_MS (15*1000)
```

Here two variables are used for timing the backlight. One counts milliseconds since last screen touching activity, and the other is used to record the actual millisecond time (from the Arduino *millis()* function) at which the first variable was last updated. Although a single variable would work, instead two are used to simplify dealing with overflow of the millis() count. Define these variables near the top of the .ino file, they are global to the whole file:

```
// Variables for supporting backlight timer.

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
```

The variables are initialized in *setup()* just after initializing the backlight on:

```
  // Turn on backlight.
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);
  // Initialize backlight timer.
  MSsinceLastTouchBeforeBacklight = 0;
  MSatLastBacklightTimerUpdate = millis();
```

Finally, the *processTapsAndReleases()* function is modified to turn the backlight on and off as needed:

```
void processTapsAndReleases() {
  int16_t x, y, pres;

  // Check for a button tap or release event.
  switch (ts_display->getTouchEvent(x, y, pres)) {

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

  // Handle a touch event.
  case TS_TOUCH_EVENT:
    // Check for a button tap and if so, call its tap function. Touch events turn
    // on the backlight if off, else are processed as possible screen button taps.
    if (digitalRead(LCD_BACKLIGHT_PIN) == LCD_BACKLIGHT_OFF)
      digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);
    else
      screenButtons->press(x, y);
    break;

  // Handle a release event.
  case TS_RELEASE_EVENT:
    // Release events reset the backlight timer.
    MSsinceLastTouchBeforeBacklight = 0;
    MSatLastBacklightTimerUpdate = millis();
    // Check for a button release. Only one button tap at a time can be detected.
    screenButtons->release();
    break;
  }
}
```

## Storing settings in non-volatile memory

Another often-desired feature of a system is to store settings in some form of non-volatile memory, so they aren't reset when system power is lost. Again, although support for that feature is not specifically part of the *Button_TT* library, it can easily be implemented within the framework already presented above.

The Arduino microprocessor, whatever its type, has EEPROM memory, which is non-volatile memory that stores the program. In at least some microprocessor architectures, this memory is available for use by the running program, in addition to its basic use for storing the program itself. This is true in the SAMD architecture, and the *FlashStorage_SAMD* library provides this support. Here (and in the *Buttons.ino* example program), that library is used to store program settings in EEPROM. If you use a microprocessor with a different architecture, you will need to investigate whether a similar library is available for it, and make the necessary changes to this example.

The *FlashStorage_SAMD* library requires that two constants be defined, *EEPROM_EMULATION_SIZE* and *FLASH_DEBUG*, before including the library's main header file:

```
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
```

Another constant must be defined, *WRITTEN_SIGNATURE*, whose value is probably arbitrary:

```
// Signature used at start of a flash memory block to mark the block as
// containing valid data written by the application.
const int WRITTEN_SIGNATURE = 0xBEEFDEED;
```

All data that needs to be stored in non-volatile memory must be collected together into a single struct. In our example here we assume that the only settings to be stored in EEPROM are the integer values of the two buttons btn_int8val and btn_uint8val:

```
// Structure containing non-volatile data to be stored in flash memory (with a
// copy in regular memory). We can use this structure even if we don't have the
// SAMD architecture support for storing it in EEPROM.
struct nonvolatileSettings {
  int8_t int8val;       // btn_int8Val value
  uint8_t uint8val;     // btn_uint8Val value
};
```

Two instances of that struct are defined, one containing the current system settings and the other containing the default values that are used to initialize EEPROM the first time it is used:

```
// The current non-volatile settings initialized from flash-based EEPROM (and
// written back to EEPROM when the data changes).
nonvolatileSettings NVsettings;

// The default non-volatile settings when the settings need to be initialized.
nonvolatileSettings defaults = {
    0,      // int8val
    0       // uint8val
};
```

Two functions must be defined, one for reading the settings from EEPROM when the system starts up, and the other for storing new setting values in EEPROM when they change:

```
#ifdef ARDUINO_ARCH_SAMD

// Predeclare following function to work around Arduino IDE bug.
void readNonvolatileSettings(nonvolatileSettings& settings,
    const nonvolatileSettings& defaults);

// Read non-volatile settings from flash memory into 'settings'.  If flash
// memory has not yet been initialized, initialize it with 'defaults'.
void readNonvolatileSettings(nonvolatileSettings& settings, const nonvolatileSettings& defaults) {
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
```

During initialization, typically within the *setup()* function, the settings last stored in EEPROM are read to initialize the *NVsettings* variable:

```
  // Read non-volatile settings from flash memory (EEPROM) into NVsettings, or
  // initialize EEPROM and NVsettings if EEPROM is uninitialized or corrupted.
  // Note: each time the Arduino IDE stores a program in EEPROM, it erases these
  // settings, so they will be reinitialized when this runs.
  readNonvolatileSettings(NVsettings, defaults);
```

The above note is an important point to remember. Downloading your program into your microprocessor with the Arduino IDE wipes out any previously saved EEPROM data, so each time you start up with a fresh program download, expect that the EEPROM values will have been re-initialized.

Later during initialization, *NVsettings* may be used to initialize other variables, as needed. Here in *setup()* we use it to initialize the two integer button values:

```
  // Initialize integer button values.
  btn_int8Val.setValue(NVsettings.int8val);
  btn_uint8Val.setValue(NVsettings.uint8val);
```

When the settings values change, they must be stored again in EEPROM. The function *writeNonvolatileSettingsIfChanged()* defined above was made to only write settings if they have changed. This avoids time-consuming and potentially EEPROM-wearing writes when no change occurs, and it allows one to call that function often, even if no settings change is likely, without causing harm. One way to do this is to call the function *every time the Arduino standard *loop()* function is called.*

Sometimes the setting value *only resides in NVsettings*, but in other cases, such as our integer buttons here, the settings reside elsewhere (in the buttons themselves) and must be copied to NVsettings.

Here is the code that would be used in *loop()* to both copy current values into *NVsettings* and to save current non-volatile settings from *NVsettings* to *EEPROM* when the settings have changed:

```
  // Save current values from btn_int8Val and btn_uint8Val to NVsettings.
  NVsettings.int8val = btn_int8Val.getValue();
  NVsettings.uint8val = btn_uint8Val.getValue();
  // Save NVsettings to EEPROM, which only gets written if the settings actually changed.
  writeNonvolatileSettingsIfChanged(NVsettings);
```

An alternative to this would be to create a "Save" button the user must tap to save settings to non-volatile memory.

To see the effect of this code, the system must be RESET after changing settings, then examined to see if the changed settings persisted.

## Adding more screens

Most systems with a display have more than one screen layout that are displayed at different times. Some code reorganization is needed to support this. In this section a design pattern is presented to help organize multiple screens. Although this is not part of the *Button_TT* library, the pattern fits well with the *Button_TT* code already presented.

In addition to the screen already defined above (referred to as the *main screen*), a second screen will be added below, a touchscreen *calibration screen*. In this section, the elements of the multi-screen design pattern are presented, with the changes needed to the previously presented code above. The following section will show the code needed to implement the calibration screen.

First, define an enum that enumerates the different screens your system will have, and define a variable of that enum type to identify the currently displayed screen:

```
// Display screens.
typedef enum _eScreen {
  SCREEN_MAIN,
  SCREEN_CALIBRATION
} eScreen;

// Currently displayed screen.
eScreen currentScreen;
```

The code is cleaner when a separate function is used to initialize screen. Declare one function for initializing each screen, then define a function that initializes all screens:

```
// Declare functions for initializing each screen.
void initMainScreen(void);
void initCalibrationScreen(void);

// Initialize all screens.
void initScreens() {
  initMainScreen();
  initCalibrationScreen();
}
```

The code is also cleaner when a function is used to draw a screen. Declare one function for drawing each screen, then define a function that draws the current screen:

```
// Declare functions for drawing each screen.
void drawMainScreen();
void drawCalibrationScreen(int state=1);

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
```

Notice that *drawCalibrationScreen()* has an argument, unlike the typical screen draw function that has none. We use this in a later section because we need to redraw the calibration screen multiple times from different calibration states.

Finally, the code is cleaner when a function is used to handle a screen's processing tasks each time the Arduino *loop()* function is called. Declare one function for each screen to handle loop() processing tasks, then define a function that calls the function for the current screen:

```
// Declare functions for handling loop() processing tasks for each screen.
void loopMainScreen();
void loopCalibrationScreen();

// Handle loop() processing tasks for the current screen.
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
```

The main screen initialization code previously included in *setup()* is moved to the new function *initMainScreen()*:

```
// Initialize the main screen.
void initButtons_MainScreen(void) {

  // Initialize btn_Simple.
  btn_Simple.initButton(lcd, "TC", 120, 5, 40, 20, ILI9341_BLACK, ILI9341_BLUE);

  // Initialize btn_Hello.
  btn_Hello.initButton(lcd, "TC", 120, 30, 200, 26, ILI9341_BLACK,
    ILI9341_LIGHTGREY, ILI9341_BLUE, "C", "Hello World!", false, &font12);

  // Initialize btn_int8Val and its arrow buttons. Give it rounded corners, with initial
  // value -5 and range -10 to +10.
  btn_int8Val.initButton(lcd, "TL", 35, 68, 50, 26, ILI9341_BLACK, ILI9341_LIGHTGREY,
    ILI9341_RED, "C", &font12, RAD, -5, -10, +10);
  btn_int8Val.setValue(NVsettings.int8val);
  btn_int8Val_left.initButton(lcd, 'L', "TR", 120, 65, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);
  btn_int8Val_right.initButton(lcd, 'R', "TL", 130, 65, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);

  // Initialize btn_uint8Val and its arrow buttons. Give it rounded corners, with initial
  // value 10 and range 0 to 10, and add a degree symbol after it. Display "--" for 0.
  btn_uint8Val.initButton(lcd, "TL", 35, 113, 50, 26, ILI9341_BLACK, ILI9341_LIGHTGREY,
    ILI9341_RED, "C", &font12, RAD, +10, 0, +10, true, "--");
  btn_uint8Val.setValue(NVsettings.uint8val);
  btn_uint8Val_left.initButton(lcd, 'L', "TR", 120, 110, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);
  btn_uint8Val_right.initButton(lcd, 'R', "TL", 130, 110, 30, 30, ILI9341_BLACK, ILI9341_LIGHTGREY);
}
```

Similarly, the main screen drawing code previously included in *setup()* is moved to the new function *drawMainScreen()*:

```
// Draw the main screen.
void drawMainScreen() {

  // Clear all existing button registrations for tap detection.
  screenButtons->clear();

  // Fill screen with white.
  lcd->fillScreen(ILI9341_WHITE);

  // Draw screen buttons and register them with the screenButtons object.

  btn_Simple.drawButton();
  screenButtons->registerButton(btn_Simple, btnTap_Simple);

  btn_Hello.drawButton();
  screenButtons->registerButton(btn_Hello, btnTap_Hello);

  btn_int8Val.drawButton();
  screenButtons->registerButton(btn_int8Val, btnTap_int8Val);
  btn_int8Val_left.drawButton();
  btn_int8Val_right.drawButton();
  screenButtons->registerButton(btn_int8Val_left, btnTap_int8Val_delta);
  screenButtons->registerButton(btn_int8Val_right, btnTap_int8Val_delta);

  btn_uint8Val.drawButton();
  btn_uint8Val_left.drawButton();
  btn_uint8Val_right.drawButton();
  screenButtons->registerButton(btn_uint8Val_left, btnTap_uint8Val_delta);
  screenButtons->registerButton(btn_uint8Val_right, btnTap_uint8Val_delta);
}
```

Gather the code previously included in *loop()* that is specific to the main screen and move it to the new function *loopMainScreen()*. The only such code is the EEPROM writing code, as the other *loop()* code that calls *processTapsAndReleases()* is used for all screens.

```
// Handle loop() processing for the main screen.
void loopMainScreen() {

  // Save current values from btn_int8Val and btn_uint8Val to NVsettings.
  NVsettings.int8val = btn_int8Val.getValue();
  NVsettings.uint8val = btn_uint8Val.getValue();

  // Save NVsettings to EEPROM, which only gets written if the settings actually changed.
  writeNonvolatileSettingsIfChanged(NVsettings);
}
```

Within *setup()*, all screens must be initialized, the *currentScreen* variable must be initialized to the desired the initial screen, and the current (initially shown) screen must be drawn. Recall the earlier discussion of screen layout and the need to force immediate display of a particular screen on whose layout you are working. This can be accomplished by defining, near the top of the .ino file, a constant whose value is the desired initial screen:

```
// Initial screen to be displayed at startup. Normally SCREEN_MAIN, but set this to other
// values to display a different screen initially, when working on its layout.
#define INITIAL_SCREEN  SCREEN_MAIN
```

Then, add this code to *setup()* to initialize screens:

```
  // Initialize the display screens.
  initScreens();

  // Set the initial screen to be displayed and draw it.
  currentScreen = INITIAL_SCREEN;
  drawCurrentScreen();
```

The final element of the design pattern is to call *loopCurrentScreen()* from the main Arduino *loop()* function:

```
  // Handle loop() processing for the current screen.
  loopCurrentScreen();
```

The call to *processTapsAndReleases()* remains in *loop()*.

Another useful design pattern for organizing the code for multiple screens is to put each screen together in one section of the file, or perhaps in its own separate file. That section would contain (in roughly this order):

> 1. Definitions of screen button variables.
>
> 2. The *initXxxxScreen()* function to initialize the screen, including the button variables.
>
> 3. Button tap handler functions. Sometimes it makes more sense to declare these functions here and define them after the *drawXxxxScreen()* function.
>
> 4. Special *draw* functions for buttons requiring specialized or more complex screen drawing than a simple call to *drawButton()*.
>
> 5. The *drawXxxxScreen()* function to draw the screen and register screen buttons.
>
> 6. The *loopXxxxScreen()* function to handle *loop()* processing for the screen.

## Adding a touchscreen calibration screen

In this section we add code to implement a touchscreen calibration screen. The code is drawn from example touchscreen calibration program *TS_DisplayCalibrate.ino* in the XPT_2046_Touchscreen_TT library *examples* directory.

There are four int16_t values that are part of the *ts_display* touchscreen-display object that provide the calibration for mapping touchscreen coordinates to display coordinates. These values must be made part of the non-volatile settings structure, copying their names and types from *TS_Display.h* to this struct:

```
// Structure containing non-volatile data to be stored in flash memory.
struct nonvolatileSettings {
  int8_t int8val;       // btn_int8Val value
  uint8_t uint8val;     // btn_uint8Val value
  int16_t TS_LR_X;      // Touchscreen calibration parameters.
  int16_t TS_LR_Y;
  int16_t TS_UL_X;
  int16_t TS_UL_Y;
};
```

Recall that the *defaults* variable, of type *nonvolatileSettings*, holds the default settings to use when settings must be initialized in fresh EEPROM. These initial values can't be hard-coded in the definition of *defaults* as can many settings. Instead, in *setup()* just before it uses *defaults* to read the settings from EEPROM, the *current calibration variable settings* established by the *ts_display* object when it was created are read into *defaults* using the *getTS_calibration()* function:

```
  // Make 'defaults' hold the default non-volatile settings when the settings
  // are first initialized. Here, we load it with the initial calibration
  // settings from ts_display.
  ts_display->getTS_calibration(&defaults.TS_LR_X, &defaults.TS_LR_Y, &defaults.TS_UL_X,
    &defaults.TS_UL_Y);
  #endif
```

There must be a way for the user to get to the calibration screen from the main screen. We add a new *Calibrate* button to the main screen:

```
// A labelled button whose name is "Calibrate".
Button_TT_label btn_Calibrate("Calibrate");

// Handle tap of "btn_Calibrate" by switching to the calibration screen.
void btnTap_Calibrate(Button_TT& btn) {
  currentScreen = SCREEN_CALIBRATION;
  drawCurrentScreen();
}
```

That is all that's required to switch screens: change the value of *currentScreen* and call the screen *draw* function.

Next, add two more constants to the section of constants used in *initButton()* calls, to provide a "standard" button size that might be used for many screen buttons:

```
// Define a "standard" button width and height.
#define BTN_WIDTH  110
#define BTN_HEIGHT 40
```

An *initButton()* call for the new *Calibrate* button is added to *initMainScreen()*:

```
  // Initialize btn_Calibrate to be a standard-sized button with rounded corners.
  btn_Calibrate.initButton(lcd, "CC", 120, 230, BTN_WIDTH, BTN_HEIGHT, ILI9341_BLACK,
    ILI9341_PINK, ILI9341_BLACK, "C", "Calibrate", false, &font12, RAD);
```

Calls to *drawButton()* and *registerButton()* for the *Calibrate* button are added to *drawMainScreen()*:

```
  btn_Calibrate.drawButton();
  screenButtons->registerButton(btn_Calibrate, btnTap_Calibrate);
```

The calibration screen itself will have four buttons:

> 1. A label that says "Calibrate" at the top of the screen (a good design pattern is to put a label at the top of each screen that identifies which screen it is)
>
> 2. A label that says "Tap the +" when a "+" appears in the upper-left or lower-right of the screen, and says "Tap to test calibration" after the user has tapped both of those + signs.
>
> 3. A "Cancel" button, always visible on this screen
>
> 4. A "Save" button, only displayed once the user has tapped the "+" on two screen corners for calibration

The button variable definitions:

```
Button_TT_label label_Calibration("CalibrationScreen");
Button_TT_label label_CalibrationTouch("CalibrationTouch");
Button_TT_label btn_CalibrationCancel("CalibrationCancel");
Button_TT_label btn_CalibrationSave("CalibrationSave");
```

Note that "Calibration" is included as part of the button name, since it is quite possible other screens will have similarly-named buttons.

More constants are added to the section of constants to be used when calling *initButton()*:

```
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
```

The six constants whose name ends with "EW" (edge width) are used to define button sizes. The *initButton()* arguments *w* and *h* can be positive constants giving the button pixel width and height. Alternatively, they can be negative values to signal that the size of the button is to be computed automatically based on the maximum size of the text string that will be displayed within the button. The "EW" constants all have negative values for this purpose. The (absolute value of the) negative value is *added to the automatically computed button width or height. For example, if *w* is set to *SEW* (= -5), then if the button width is computed to be 87 pixels based on the text in the button, then the actual button size will be set to 87 + 5 = 92 pixels. For accurate automatic sizing, it is crucial that for Button_TT_label type buttons the largest expected text string is used as the initial label when calling *initButton()*. After *initButton()* returns, the actual label can be changed to the desired initial label by calling *setLabel()*. For integer-value buttons, the maximum text size is computed automatically using the known button *minValue* and *maxValue* settings.

The five constants whose name starts with "EXP_" (expansion) are used with the last four arguments to each *initButton()* function, *expU*, *expD*, *expL*, and *expR*. As described earlier, these allow the actual button size to be increased to obtain the button rectangle tested by the *screenButtons* object to see if it contains a tapped point.

The *initCalibrationScreen()* function is defined next. It calls *initButton()* for each of the four buttons:

```
// Initialize the calibration screen.
void initButtons_CalibrationScreen(void) {

  label_Calibration.initButton(lcd, "TC", 120, 5, TEW, TEW, TRANSPARENT_COLOR,
    TRANSPARENT_COLOR, ILI9341_DARKGREEN, "C", "Calibrate", false, &font12);
  label_Touch.initButton(lcd, "TL", 10, 30, 220, TEW, TRANSPARENT_COLOR,
    TRANSPARENT_COLOR, ILI9341_RED, "TL", "Tap the +", false, &font12);
  btn_CalibrationCancel.initButton(lcd, "BL", 5, 313, BTN_WIDTH, BTN_HEIGHT,
    ILI9341_BLACK, ILI9341_PINK, ILI9341_BLACK, "C", "Cancel", false, &font12, RAD);
  btn_CalibrationSave.initButton(lcd, "BR", 235, 313, BTN_WIDTH, BTN_HEIGHT,
    ILI9341_BLACK, ILI9341_PINK, ILI9341_BLACK, "C", "Save", false, &font12, RAD);
}
```

Notice that the *label_Calibration* and *label_Touch* button outlines and fill colors are set to *TRANSPARENT_COLOR* so the button text will appear on the screen background with no surrounding rectangle. If the text were not fixed and the screen were not being completely redrawn whenever the text was changed, it would be necessary to use the screen background color instead of *TRANSPARENT_COLOR*.

Calibration requires that a "+" sign be drawn one at a time in opposite screen corners, and the user must click carefully on them. Define a function to draw the "+" signs:

```
// Length of each arm of "+" sign.
#define PLUS_ARM_LEN 10

// Draw a plus sign at a specified display location.
void drawPlus(int16_t x, int16_t y, int16_t color, uint8_t len = PLUS_ARM_LEN) {
  lcd->drawFastVLine(x, y-len, 2*len+1, color);
  lcd->drawFastHLine(x-len, y, 2*len+1, color);
}
```

Define tap handlers for the Cancel and Save buttons:

```
// Handle tap of Cancel button in Calibration screen. Revert calibration
// parameters to the original and switch back to main screen.
void btnTap_CalibrationCancel(Button_TT& btn) {
  ts_display->setTS_calibration(NVsettings.TS_LR_X, NVsettings.TS_LR_Y,
    NVsettings.TS_UL_X, NVsettings.TS_UL_Y);
  currentScreen = SCREEN_MAIN;
  drawMainScreen();
}

// Handle tap of Save button in Calibration screen. Save current calibration
// parameters into "NVsettings" and switch back to the main screen.
void btnTap_CalibrationSave(Button_TT& btn) {
  ts_display->getTS_calibration(&NVsettings.TS_LR_X, &NVsettings.TS_LR_Y,
    &NVsettings.TS_UL_X, &NVsettings.TS_UL_Y);
  currentScreen = SCREEN_MAIN;
  drawMainScreen();
}
```

These two functions use the *ts_display* functions *setTS_calibration()* and *getTS_calibration()*, the former to restore the original calibration settings when the user taps *Cancel*, and the latter to retrieve the new calibration settings into *NVsettings* when the user taps *Save*.

The workhorse of the calibration screen is a state machine, operated in the *loopCalibrationScreen()* function, that waits for the user to tap on each "+" sign. After he taps them in opposing corners of the display, the calibration parameters are computed and become the new parameters for translating screen taps to display coordinates. The user can then tap the screen further to test the new calibration, with a "+" sign drawn at each tapped position. If satisfied he can tap "Save" to save the new calibration or "Cancel" to revert back to the previous one. This state machine code is taken from *TS_DisplayCalibrate.ino* in the *XPT_2046_Touchscreen_TT* library, the main difference being that here we implement the "Tap the +" message using a label button rather than writing screen text directly.

Define an enum of the states and a variable that holds the current state:

```
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
```

The state machine starts in the *STATE_WAIT_UL* state when the *Calibration* screen is first drawn, and proceeds sequentially through the listed states, alternating between the last two until either *Cancel* or *Save* is tapped.

Also define variables to hold upper-left and lower-right calibration tap positions and the corresponding touchscreen positions:

```
// Display UL and LR calibration positions and corresponding touchscreen
// calibration coordinates.
int16_t x_UL, y_UL, x_LR, y_LR;
int16_t TSx_UL, TSy_UL, TSx_LR, TSy_LR;
```

The *drawCalibrationScreen()* function is defined next. Unlike our previous screen draw functions, this one has an argument, *state*, that defaults to 1. The function is called from the "Calibrate" button handler of the main screen using the default argument value of 1, which draws the initial calibration screen including the first "+" to be tapped, and sets *calibState* to the *STATE_WAIT_UL* initial state. It is subsequently called *two more times* (resulting in a complete re-draw of the screen), once after the user taps the first "+", to redraw the calibration screen with the second "+", and then again to redraw it with no "+" and drawing the *Save* button for the first time. The *state* argument also influences the label and position of the *CalibrateTouch* button.

```
// Draw the Calibration screen and register its buttons with the screenButtons
// object. Argument "state" defaults to 1 and is either 1, 2, or 3:
//      1: initial display, draw first +, "tap +", no Save, set STATE_WAIT_UL
//      2: finished STATE_WAIT_UL_RELEASE, draw second +, "tap +", no Save
//      3: finished STATE_WAIT_LR_RELEASE, draw no +, "tap to test", draw Save
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
```

Besides the twist of adding an argument to *drawCalibrationScreen()* so the function can be used to redraw the screen multiple times during calibration, there are a couple other new features to notice here. The *label_Touch* button label varies depending on the *state* argument, and the button is moved around on the screen by calling its *setPosition()* function, the position also depending on the value of the *state* argument. To compute the position, the *getLeft()*, *getWidth()*, and *getHeight()* functions are used to get the left-side x-coordinate and width and height of a button. *getTop()* is also available to get the top-side y-coordinate. The left side of the *Save* button is used as a convenient reasonable place to put the "Tap the +" label when the "+" is in the lower-right corner. The *drawPlus()* function is also used, illustrating how a screen drawing function is not limited to drawing buttons! Finally, the *Save* button isn't drawn until state=3.

The final function to define for the Calibration screen is *loopCalibrationScreen()*, which runs the state machine. This function tests for touchscreen touches using the *touch* object's *touched()* function **independently of and in parallel with** *processTapsAndReleases()*. It uses the previously-defined *playSound()* function to play a sound when the "+" is touched.

```
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
```

Some function calls to note above are the *touch* object's *getPoint()* to retrieve the current screen (x,y) coordinates being touched, and three functions of the *ts_display* object: *findTS_calibration()* performs the actual calibration work by determining which calibration parameter values will map the touchscreen (x,y) coordinates into the corresponding display (x,y) coordinates, *setTS_calibration()* sets those new calibration parameter values as the ones currently in use, and *mapTStoDisplay()* maps a touchscreen (x,y) coordinate into a corresponding display (x,y) coordinate using the current calibration parameters.

## Accessing and using button values

How is a button *used* within the program? Some buttons perform actions immediately upon being tapped, such as the "Cancel" and "Save" buttons in the previous section. Others are simply labels that don't change. Some buttons may have values that *do change*, such as those that hold and display integer values. A simple text label button could also *change* its label. For example, maybe a thermostat mode button changes its label between "Off", "On", and "Auto" each time it is tapped.

How does a program *access* these button values when it needs them? For integer-valued buttons the code would call the button *getValue()* function to obtain the current integer value. For label buttons, it *could* call the *getLabel()* function to get the current text string value. However, for many label buttons, a better method might be to use an enum to represent the possible text string values the button can have, with a variable holding the current enum value, and each time the button is pressed, the enum value is advanced and the new value is translated into a string that is set as the new button text by calling *setLabel()*. Then, the button value is always available in the enum variable.

To change a button's value in response to some event (such as a tap of another button, or *any* event), the button *setLabel()* function is called for label buttons, or *setValue()* for integer buttons. The functions *setLabelAndDrawIfChanged()* and *setValueAndDrawIfChanged()* can also be used. These combine the setting of the new button value with a call to *drawButton()*, but the button is only drawn if the value actually changed, which avoids needless flickering of the display.

There is also a *drawIfChanged()* button. Each button uses a flag to track whether a change has been made to any aspect of its state that affects the way it is drawn, whether it be a change of value, color, or position for example. *drawIfChanged()* only draws the button if that flag has been set. Drawing the button at any time clears the flag. *drawIfChanged()* as well as *setLabelAndDrawIfChanged()* and *setValueAndDrawIfChanged()* all have a boolean argument named *forceDraw* that defaults to false, but if set true by the caller, forces the function to draw the button even if the "changed" flag is not set. This is useful when it is necessary to force draw a button when its screen is initially drawn, but subsequently to only draw it when the change flag has been set.

When buttons are initialized, they must be set to their initial values. Rather than coding a fixed value in the *initButton()* call, the value might instead come from a variable, such as for example the *NVsettings* variable used in a section above to illustrate use of EEPROM to store program settings. Another method is to initialize the button to a fixed value (such as for example to its longest possible string, if the button is being auto-sized), and after *initButton()* returns, call *setLabel()* to set the desired initial label, or call *setValue()* to set the desired initial value of integer-valued buttons.

## Creating new button styles: a button class with a "+" in it

You may find that you need button functionality that is not part of the *Button_TT* library. Because the buttons are implemented as C++ classes, you can fairly easily extend the functionality by deriving a new class from an existing button class and adding new functions. The possibilities are endless. We will illustrate one possibility.

The *Button_TT* class is the base class for all buttons. There are two classes that derive from it. First is *Button_TT_label*, which implements buttons with labels. The integer-valued buttons derive from that class, since the integer value becomes a text label for the button. Second is *Button_TT_arrow*, which implements triangular buttons with no labels. If you wish to create a new button class, one good starting point is to  copy either *Button_TT_uint8.h/.cpp* (to make a labelled button class) or *Button_TT_arrow.h/.cpp* (to make an unlabelled button type) to new file names and edit them, removing the code specific to that class and adding new code for your new button class.

In this example, we will create a button class that draws a "+" in the middle of the button. There is no strong need for this class, but it could be used to replace the manual drawing of the "+" sign in the calibrate code shown previously, for example. This example provides a convenient way to illustrate how to create a button that draws itself differently. Here is the code, deriving a new class named *Button_TT_plus* from *Button_TT*:

```
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

bool Button_TT_plus::setArmLen(uint8_t armWidth) {
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
```

It is left as an exercise to change the code presented above to use this new style of button, perhaps simply as an additional button on the *main screen* or perhaps in place of drawing the "+" with draw function calls on the *calibration screen*.

## Contact

If you find a problem, please submit an issue report [here](https://github.com/tedtoal/Button_TT/issues/new/choose).

You can contact me (the author) at [ted@tedtoal.net](ted@tedtoal.net)
