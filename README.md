# Button_TT

## Draws buttons on a pixel display and detects button taps

The ***Button_TT*** library defines classes and functions to support drawing buttons on a pixel display. Support is included for various types of buttons, as well as button hit-testing. This lengthy document fully describes how to use the library.

## Required libraries

The *Button_TT* library uses the *Adafruit_GFX_Library* graphics library, which requires a display with a controller object class that is derived (directly or indirectly) from class *Adafruit_GFX*. Many low-cost displays use the ILI9341 controller, which can use library *Adafruit_ILI9341* for this purpose. *You must install the **Adafruit_GFX_Library** library on your system **and** the compatible display controller library such as **Adafruit_ILI9341***.

## Introductory code without touchscreen

Here is simple code showing how to use this library to create a single on-screen rectangular button with no text and with no user interaction.

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
// The button name can be anything and is unrelated to text on a button (It is
// useful at times for debugging).

// A plain button with no text, whose name is "simple".
Button_TT btn_Simple("simple");

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

The second argument, *align*, is a text string of one or two characters that tells how the button is to be aligned relative to the button position (x,y) specified by the x and y arguments. This is very handy for easily centering buttons or left- or right- justifying them. In the btn_Simple example above, the value "TC" is specified, meaning the button is to be justified against the top edge (i.e. the top of the button will be at the specified y coordinate of 5) and centered left-right (so the button center is at the specified x coordinate of 120, which is halfway across the 240-pixel-wide display). Note that justification of text within a labelled button is handled entirely separately by a different argument to the *Button_TT_label.initButton()* function, described later.

The next four arguments (x, y, w, and h) are straightforward. They give the button (x,y) position as (120, 5), and the button size (w,h) in pixels as (40, 20), w being the width and h being the height.

The next two arguments (outlineColor and fillColor) are simply ILI9341_ color constants (from Adafruit_GFX.h) specifying the color of the button outline and interior. If both are the same color, the button appears as a simple rectangle with no outline. Also, a special constant, *TRANSPARENT_COLOR*, is defined in Button_TT.h and it can be used anywhere a color value is needed. It causes the graphic element assigned that color to not be drawn. If outlineColor were specified to be TRANSPARENT_COLOR, the outline of the button would not be drawn, so the color of the background screen fill would show. Likewise, if fillColor were that color, the button interior would not be drawn and the background screen fill would show. For labelled buttons (described later) whose label text can change, do not use TRANSPARENT_COLOR for the fillColor because the button interior must be drawn whenever the label changes, to erase the previous label.

When reading the Button_TT.h file comments, pay attention to the actual function declaration and what the default values are for arguments. For this function you can see that the defaults for the last four "exp*" arguments are all 0:

```
              uint8_t expU = 0, uint8_t expD = 0, uint8_t expL = 0, uint8_t expR = 0
```

As the comment says, the purpose of these last four arguments is to expand the size of the button used by the *contain()* function. That function will be described later, but it tests a point (x,y) to see if it lies within the button, and these expansion parameters allow for testing a larger area than the actual button, which can help when a finger is used to touch a touchpad. They are useful when there are no other close-by buttons on the screen.

## Filling the blank screen and drawing the buttons

After initializing the display and button variables in setup(), you will fill the screen with a background color, draw the buttons, and draw any other graphical objects you want. This can be done in setup(), but in programs that display different screens at different times, you would typically define a function to draw each different screen. The operations are simple:

```
  // Fill screen with white.
  lcd->fillScreen(ILI9341_WHITE);

  // Draw btn_Simple button.
  btn_Simple.drawButton();
```

With nothing more than the code presented above, you can run a program whose loop() function does nothing, and you should see the display filled with white and with a rectangle representing the button at the specified position.

## Introduction to Adafruit_GFX fonts and Font_TT.h

A brief discussion of font support is in order. The *Adafruit_GFX_Library* library supports the writing of text to displays, and this includes support for a built-in mono-spaced font and additional proportionally-spaced fonts.

The *Adafruit_GFX_Library* library file *gfxfont.h* provides definitions for using the proportional fonts, including defining the C++ struct named *GFXfont* that contains all information for one proportional font. Adafruit_GFX.h automatically #includes gfxfont.h.

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

The *Button_TT* library provides files *Font_TT.h* and *Font_TT.cpp* that expand on the Adafruit_GFX_Library font support by providing functions to better compute text string sizes and positions to allow for accurate alignment of text on the display. They define the class *Font_TT*, which holds a pointer to the *GFXfont* struct for *one proportional font* or holds a nullptr value to indicate use of the built-in font. You will need to create one instance of that class for each different font you use, whether the built-in font or a proportional font, as shown below.

The *Font_TT* class doesn't have a *setFont()* function, but instead you pass a *GFXfont* instance pointer to the Font_TT constructor when you declare a variable of type Font_TT. Each Font_TT instance is for a single font. You pass *nullptr* to that constructor to indicate that the instance is for the built-in font. (The *Font_TT::getFont()** function returns that pointer, and it is used in the Button_TT library when *Adafruit_GFX_Library* text functions are called.)

Here is a modification of the above program that uses Font_TT functions to compute cursor position so that text strings will be aligned in the desired way, putting "Hello world!" at the top, centered, and "Goodbye world!" at the bottom right.

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

  // Turn on backlight.
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(LCD_BACKLIGHT_PIN, LCD_BACKLIGHT_ON);

  // Initialize the LCD display.
  lcd->begin();
  lcd->setRotation(2);   // portrait mode
  lcd->setTextSize(1);
  lcd->setTextWrap(false);

  // Fill screen with white.
  lcd->fillScreen(ILI9341_WHITE);

  // Write text to display, centered.
  lcd->setTextColor(ILI9341_BLUE);
  lcd->setFont(fontBuiltIn.getFont());
  str = "Hello world!";
  fontBuiltIn.getTextAlignCursor(str, 0, 0, lcd->width(), lcd->height(), 'C', 'T', xC, yC);
  lcd->setCursor(xC, yC);
  lcd->print(str);

  lcd->setTextColor(ILI9341_RED);
  lcd->setFont(fontSans12.getFont());
  str = "Goodbye world!";
  fontSans12.getTextAlignCursor(str, 0, 50, lcd->width(), lcd->height()-50, 'R', 'B', xC, yC);
  lcd->setCursor(xC, yC);
  lcd->print(str);
}

// Standard Arduino loop() function.
void loop() {
}
```

## Button_TT_label.h: buttons containing text

Usually buttons on the display have text inside them to indicate what the button does. Buttons with text are supported by the *Button_TT_label.h* and *Button_TT_label.cpp* files, which define a new C++ class named *Button_TT_label* that is derived from the basic button class *Button_TT*.

To create a button with text (a labelled button), start by including additional header files at the start of your .ino file:

```
// Include files when any button with text is used.
#include <Font_TT.h>
#include <Button_TT_label.h>

// Include proportional font files, if any. Built-in font is always available.
#include <Fonts/FreeSans12pt7b.h>
```

The *Font_TT.h* file was introduced in the previous section, as were the header files in the *Adafruit_GFX_Library* *Fonts* subfolder. Above the font file for the sans-serif 12-point proportionally-spaced font is included. For each included font file, a separate Font_TT object must be created, passing a pointer to the font to the Font_TT constructor:

```
// Create a Font_TT object for each font.
Font_TT font12(&FreeSans12pt7b);
```

Two create a labelled button, define a variable of type *Button_TT_label*:

```
// A labelled button whose name is "hello".
Button_TT_label btn_Hello("hello");
```

As with the simple button shown earlier, each labelled button must be initialized by calling the *initButton()* function from setup() (or from some other function called by setup()):

```
  // Initialize btn_Hello.
  btn_Hello.initButton(lcd, "TC", 120, 30, 200, 26, ILI9341_BLACK,
    ILI9341_LIGHTGREY, ILI9341_BLUE, "C", "Hello World!", false, &font12);
```

There is a different initButton() function for each different type of button, and each one has its own unique arguments. Therefore, you should load the *Button_TT_label.h* file into the Arduino IDE and locate and inspect the initButton() comments and default arguments. In most cases, new types of buttons are based on simpler buttons, and the initButton() function has most of the same arguments as the simpler button did, with additional arguments added. The arguments that are the same are *not necessarily in the same order or position*. Usually, the first 8 arguments are the same as the first 8 of Button_TT::initButton(), and the last four are the same as the last four of that function. New arguments appear after the first 8. This is true here.

In the above initButton() call, it is seen that the button is being aligned against the top edge of the specified position (120,30) and horizontally centered at that point. The button size is 200 pixels wide by 26 pixels tall, it has a black outline and a light grey interior.

After that come the new arguments:

```
textColor     Color of the button label (16-bit 5-6-5 standard).

textAlign     Like 'align' but gives alignment for label,
              1st char = up/down alignment, 2nd = left/right.

label         String of the text inside the button, should be the
              largest possible label if w or h is negative to ensure all
              possible labels will fit in the size that is automatically
              computed.

degreeSym     If true, a degree symbol is drawn at the end of the label.

f             Pointer to Font_TT object for the custom font to use
              for the label, nullptr for built-in font.

rCorner       Radius in pixels of rounded corners of the button
              outline rectangle, 0 for straight corners.
```

Thus, *textColor* specifies the text color, in this case blue, *textAlign* specifies center alignment of the text within the button rectangle, and *label* gives the button label as "Hello world!". The button and text colors, text alignment, and text itself can be changed at any time by calling other button functions, such as *Button_TT_label::setLabel()*.

Automatically setting button size based on the label text size will be discussed later.

The Adafruit_GFX_Library fonts do not include a symbol for the *degree character*. This can be drawn manually using circle-drawing functions in that library. However, the Button_TT library was used in a thermostat and it was desired to provide better support for that symbol, without actually modifying the font files to add the symbol. Instead, the *degreeSym* argument is provided. It is either true or false (the default value). When true, a degree symbol is automatically added after the label text and is included in the text alignment algorithm. In the code above, *degreeSym* is specified as the default value of false so that the following argument, *f* (the font for the text) can be specified.

The address of the *font12* font object created earlier is specified for *f*, so that font will be used to draw the button label. The font can also be changed later by calling *Button_TT_label::setFont()*.

The Button_TT simple buttons are drawn as rectangles with straight corners, but Button_TT_label buttons can optionally use rounded corners. The *rCorner* argument specifies the radius of the corners, with the default value of giving straight corners.

The last piece of code necessary for the labelled button is to draw it using a *drawButton()* call from setup() or a function called by it:

```
  btn_Hello.drawButton();
```

Buttons don't always need to be actual buttons to be tapped by a user. Label buttons in particular are useful simply as labels on the display. It may be more convenient to use buttons for text labels rather than simply writing the text string to the display, because then each screen element is created in the same manner. When buttons are used as text strings but are not tappable, often you will want to display the text string on the screen background without placing it inside a rectangle, which is done by setting the button outline and fill colors equal to the screen background color.

However, *usually* one wants to allow the user to tap a button, then detect the tap and perform an appropriate action. The next section describes how to do this.

## Adding touchscreen support and detection of button taps

The *Button_TT* library provides support to make detection of button taps and responses to them easier to code. For this, the library requires that your touchscreen uses an XPT2046 controller, which is common on low-cost touchscreens. The touchscreen must be controllable using the library *XPT2046_Touchscreen_TT*, which is an enhanced version of the *XPT2046_Touchscreen* library. As long as that latter library works with your touchscreen, you can install the *XPT2046_Touchscreen_TT* library to provide button tap support. Example programs in that library allow you to test that it works correctly by itself and with your display.

The *XPT2046_Touchscreen_TT* library has two pairs of files supporting the touchscreen. Files *XPT2046_Touchscreen_TT.h/.cpp* define class XPT2046_Touchscreen_TT that talks to the XPT2046 controller and has functions to test for taps and get their positions. Files *TS_Display.h/.cpp* define class TS_Display that provides several services for working with a combination of a touchscreen and a display, such as mapping touchscreen coordinates to display coordinates. To use the touchscreen with the *Button_TT* library, include both of those header files near the top of your .ino file:

```
// Include files to use touchscreen with display.
#include <XPT2046_Touchscreen_TT.h>
#include <TS_Display.h>
```

The file pair *Button_TT_collection.h/.cpp* in the *Button_TT* library defines a class *Button_TT_collection* whose purpose is to maintain a set of buttons that are currently displayed on the screen and can be tapped to initiate some action. Your code must have one instance of the class, and as tappable buttons are created, they must be registered with that instance, including a function to be called if the button is tapped. If the display is changed, for example by displaying a different screen with different buttons, the registered buttons must be cleared from the instance and then the buttons for the new screen must be registered with it. To use the touchscreen with the *Button_TT* library, include that header file:

```
// Include when button tap response is required.
#include <Button_TT_collection.h>
```

The touchscreen requires two signals from the microcomputer to access it, and these must be defined, usually near the top of your .ino file:

```
// Touchscreen pin definitions.
#define TOUCH_CS_PIN        A0
#define TOUCH_IRQ_PIN       A7
```

The above are examples, change the values to match the pins connected to the touchscreen on your system.

When the *XPT2046_Touchscreen_TT* library is used, a variable must be defined that will point to an instance of the *XPT2046_Touchscreen_TT* class. It is used to interact with the touchscreen, and if the touchscreen is used with a display, a second variable must be defined that points to an instance of the *TS_Display* class:

```
// Touchscreen object.
XPT2046_Touchscreen* touch;

// Touchscreen-display object.
TS_Display* ts_display;
```

A third variable must be defined that will point to an instance of the *Button_TT_collection* class, used to register buttons to be monitored for taps:

```
// Button collection object to manage taps of buttons.
Button_TT_collection* screenButtons;
```

All three of the above pointer variables must be initialized, which involves allocating and assigning an object instance to them, followed by one or more calls to member functions to perform required initialization. It is during the *touch* object initialization that the touchscreen pin numbers defined earlier are used. *Typically this is all done within the standard Arduino **setup()** function, **after** initializing the display object:*

```
  // Create and initialize touchscreen object, same rotation as lcd.
  touch = new XPT2046_Touchscreen(TOUCH_CS_PIN, TOUCH_IRQ_PIN);
  touch->begin();
  touch->setRotation(lcd->getRotation());

  // Create and initialize touchscreen-LCD object.
  ts_display = new(TS_Display);
  ts_display->begin(touch, lcd);

  // Initialize button collection object.
  screenButtons = new Button_TT_collection;
  screenButtons->clear();
```

A function must be defined for each button that must respond to a tap, and it must perform the desired action. Although in general each tappable button needs such a function, in some cases it makes sense to use the same function for multiple buttons, and this will be shown later. In our example case here, we have already defined two buttons that we want to respond to taps, the *Simple* and *Hello* buttons. We will make a tap of the Simple button toggle its fill color between two colors, and a tap of the Hello button will toggle its text label between two labels:

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

The above functions make use of the basic button functions *getFillColor()*, *setFillColor()*, *getLabel()*, and *setLabel()*, and they both call the *drawButton()* function to redraw the button on the screen, since its appearance has been changed.

The button tap functions must be associated with their actual button variables by registering each one with the *screenButtons* object. This may be done during initialization, either directly within *setup()* or in a function called by it. However, often there will be several different screens that are displayed at different times depending on user activity, and each time a new screen is displayed, its buttons must be re-registered. The complexities introduced by using multiple screens will be introduced later. Here, we will assume that the registration of buttons is done within *setup()*:
  
```
  // Register buttons with screenButtons object to handle button taps/releases.
  screenButtons->registerButton(btn_Simple, btnTap_Simple);
  screenButtons->registerButton(btn_Hello, btnTap_Hello);
```

The last thing to do to support button taps is to define another function that checks for touchscreen taps. It is called from the standard Arduino loop() function. It uses the *ts_display* instance to test for a touchscreen tap, and the *screenButtons* instance to find the button that was tapped and call its tap function to perform the desired button action:

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
    // Check for a button release. Only one button press at a time can be detected.
    screenButtons->release();
    break;
  }
}
```

This function could instead be placed directly within the Arduino *loop()* function, but instead it is cleaner to define it as a separate function and call it from *loop():*

```
// Standard Arduino loop() function.
void loop() {
  // Process button taps/releases for the current screen, which has registered
  // its buttons with the screenButtons object.
  processTapsAndReleases();
}
```

## Playing a beep sound when button is tapped

Usually it is desirable to provide some audible user feedback when he taps a button. Here (and in the *Button.ino* example program) we use the *SAMD_PWM* library, which requires the SAMD microprocessor architecture, to produce the sound. You can adapt this code to use your preferred sound library. The *SAMD_PWM* library defines class *SAMD_PWM*, which talks to the sound hardware to produce sounds. To use the library, you must include its header file, define the hardware pin that is attached to the sound device (BEEPER_PIN), define constants that determine the particular sound to be produced when a button is tapped, and define a pointer to a *SAMD_PWM* object:

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

The SAMD_PWM object must be initialized, usually within the *setup()* function. This is where the sound device pin number is used, and there may also be a need for sound constants, such as TS_TONE_FREQ here:

```
  // Initialize PWM object for sound from beeper (requires SAMD architecture).
  sound = new SAMD_PWM(BEEPER_PIN, TS_TONE_FREQ, 0);
```

To create a sound when a button is pressed, a function must be defined that will start or stop a sound when a button is pressed or released. Here, for clarity, we define one function to start or stop a sound, and a second one, called the *master button tap/release function*, that is called when a button is pressed or released, and it calls the first function to start or stop the sound:

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

Finally, the last thing to do is to register the master button tap/release function with the *screenButton* object, which will call the function as buttons are pressed and released. This is usually done within *setup():*

```
  // Register master tap/release function to turn tone on/off at screen taps.
  screenButtons->registerMasterProcessFunc(buttonTapRelease);
```

That is all that is necessary to implement audible button press/release feedback.

## Using buttons showing integer values as labels

Often there is a need to show integer values that the user is able to alter. The *Button_TT* library supports this with four classes, each derived from class *Button_TT_label*, one class for each of four different types of integers: signed and unsigned, 8-bit and 16-bit. The classes are:

> Button_TT_uint8: unsigned 8-bit integer
>
> Button_TT_uint16: unsigned 16-bit integer
>
> Button_TT_int8: signed 8-bit integer
> 
> Button_TT_int16: signed 16-bit integer

All four classes are used the same way. They example here uses the *Button_TT_int8* class.

Typically these are used as non-tappable buttons. That is, they are only buttons in the sense that they create numeric text strings on the display. They might not even show the number in a rectangle, but rather, as a number directly on the screen background (by setting the button outline and fill colors equal to the screen background color). There is nothing that prevents making them tappable, and perhaps a program might do that and have a tap call up a keypad where the user could enter a new value. However, the simplest implementation is to use these as non-tappable buttons, and add a pair of *arrow* buttons next to each integer-valued button. The arrow buttons can be tapped to increment and decrement the number. This section only shows how to create the integer-valued button. The next section will show how to create and use arrow buttons.

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

The button variable is then initialized by calling its *initButton()* function. Again, the *initButton()* function comments and default argument values should first be examined in the *Button_TT_int8.h* file. The additional arguments for this style of button, starting after the button rectangle corner radius argument *rCorner*, are:

```
value         The int8_t value for the button, used to create the label string for the button.
minValue      The minimum allowed value for value.
maxValue      The maximum allowed value for value.
degreeSym     If true, a degree symbol is drawn at the end of the label.
showPlus      If true, a leading + is used for positive values in the label.
checkValue    If not nullptr, a pointer to a function that checks a new button
              value and can return an adjusted value if it is out of range.
```

There are often many buttons defined in a program, and often many of the *initButton()* argument values are the same for many buttons. It is therefore useful to define, in a section near the start of the .ino file, constants for the common *initButton()* argument values. For example, if rounded buttons are used but they always have the same corner radius, the following definition would be helpful for improving program clarity. Note the short name, to keep *initButton()* call statements shorter, since they tend to be long anyway:

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

The button tap function must of course be registered with the *screenButtons* object, perhaps from *setup():*

```
screenButtons->registerButton(btn_int8Val, btnTap_int8Val);
```

Thus, making a button tappable, once the basic tap code shown earlier is in place, is as simple as defining a tap function and registering it whenever the button is displayed.

## Using triangular arrow buttons for increment/decrement

The *Button_TT* library includes another pair of files, *Button_TT_arrow.h/.cpp* that define class *Button_TT_arrow*, which implements another button style, triangular-shaped buttons. These are *unlabelled* buttons. The intended use of them is as *increment* and *decrement* buttons for changing numeric values. As usual, the first step to use this style of buttons is to include the header file:

```
// Include file for using triangle buttons, usually used as arrow buttons.
#include <Button_TT_arrow.h>
```

Then, a variable is defined for each arrow button. They are usually defined in pairs, one for increment and one for decrement, and coding clarity is improved by naming the button variables the same as the name of the button they will increment or decrement, with an appendix indicating which one it is. Each button can be oriented so that its tip lies in any of the four directions up, down, left, or right. It works well to place a button pair side-by-side, the left one with the tip to the left and the right with tip to right. This helps conserve limited screen space. The left-facing button would decrement the value and the right-facing button would increment the value:

```
// Define two arrow buttons (left and right, for decrement and increment) to go with btn_int8Val.
Button_TT_arrow btn_int8Val_left("int8Val_left");
Button_TT_arrow btn_int8Val_right("int8Val_right");

// The tap function is shared by both arrow buttons. It invokes valueIncDec() on the int8val button.
void btnTap_int8Val_delta(Button_TT& btn) {
  btn_int8Val.valueIncDec(1, &btn);
}
```

The *Button_TT_int8* function *valueIncDec()* handles all the work of incrementing or decrementing the button value, honoring its minValue and maxValue settings by doing nothing once the limit is reached. It even determines which of the two registered buttons for the function was the actual button that was pressed, by comparing the *btn* argument to the two button variables. The *amount* that the function adds to or subtracts from the button value is given by the first argument to the function, 1 above.

The *initButton()* function for arrow buttons has the same arguments as the basic *Button_TT* button style, with the addition of one argument *(orient)* just after the first argument and two arguments after the x and y arguments:

```
orient  Orientation of triangle: U=UP, D=DOWN, L=LEFT, R=RIGHT.
s1      Length of the base of the equilateral triangle.
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

And they are registered just after the *registerButton()* call for the associated *btn_int8Val* button:

```
  screenButtons->registerButton(btn_int8Val_left, btnTap_int8Val_delta);
  screenButtons->registerButton(btn_int8Val_right, btnTap_int8Val_delta);
```

With the above code, a pair of arrow buttons is implemented that will respectively decrement and increment the btn_int8Val value.

## Using a button with a signed integer value

The previous sections illustrated use of an signed integer button. Here the code is shown for using an unsigned integer button, of class *Button_TT_uint8*, with a pair of arrow buttons for increment and decrement. There are minor differences worthy of noting.

Start as usual by including the header file and define the button variables and tap function:

```
// Include file for using button with uint8_t value.
#include <Button_TT_uint8.h>

// A uint8_t button whose name is "uint8Val".
// There is no tap handler for this button, tapping it does nothing. It changes
// its value when one of the arrow buttons to its right is tapped.
Button_TT_uint8 btn_uint8Val("uint8Val");

// Define two arrow buttons (left/right, for decrement/increment)  to go with btn_uint8Val.
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

Above we see that "--" is used for the value of the *zeroString* argument, to show that string instead of "0" for the value 0. The *degreeSym* argument is true, which causes a small superscript circle to be displayed after the number, indicating *degrees*. This feature obviates the need for devising a font that contains a degree symbol.

The *drawButton()* function is called for each function to draw it:

```
  btn_uint8Val.drawButton();
  btn_uint8Val_left.drawButton();
  btn_uint8Val_right.drawButton();
```

Finally, the *screenButtons* *registerButton()* function is called for each arrow button to make it tappable:

```
  screenButtons->registerButton(btn_uint8Val_left, btnTap_uint8Val_delta);
  screenButtons->registerButton(btn_uint8Val_right, btnTap_uint8Val_delta);
```



## Creating new button styles with your own button classes

## Using button values

What if the initial button value is to be obtained from a program variable? The variable value can of course be used as the *value* argument to *initButton()*, but another coding design pattern might initialize button values *after initButton() calls*, using the *setValue()* button member function.

How is the button value *used* within the program? One way is that it can simply be extracted from the button using the *getValue()* function. A more complex option 


## Contact

If you find a problem, please submit an issue report [here](https://github.com/tedtoal/Button_TT/issues/new/choose).

You can contact me (the author) at [ted@tedtoal.net](ted@tedtoal.net)
