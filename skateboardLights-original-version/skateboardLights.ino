#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#include <EEPROM.h>

#define PIN 8
int numPixels = 28;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.
uint32_t OFF = strip.Color(0, 0, 0);
uint32_t RED = strip.Color(255, 0, 0);
uint32_t GREEN = strip.Color(0, 255, 0);
uint32_t BLUE = strip.Color(0, 0, 255);
uint32_t YELLOW = strip.Color(255,255,0);
uint32_t ORANGE = strip.Color(255,128,0);
uint32_t AQUA = strip.Color(0,255,255);
uint32_t MAGENTA = strip.Color(255,0,255);
uint32_t WHITE = strip.Color(255, 255, 255);

uint32_t NEON_PINK = strip.Color(255,0,146);
uint32_t NEON_ORANGE = strip.Color(255,95,0);
uint32_t NEON_GREEN = strip.Color(182,255,0);
uint32_t NEON_BLUE = strip.Color(34,141,255);
uint32_t NEON_PURPLE = strip.Color(186,1,255);
uint32_t NEON_YELLOW = strip.Color(204,255,0);

int switch1IN = 2;
int switch2IN = 3;
int switch3IN = 4;
int switch4IN = 5;
int pushButtonIN = 12;
int LED = 13;

int switch1state;
int switch2state;
int switch3state;
int switch4state;

boolean UBL = false;
int buttonPressedCount;
int pushButtonState;
int buttonIsPressed;
int lastbuttonIsPressed;
int colourSelection = EEPROM.read(0); //initialise colourSelection to last used colour
int lastcolourSelection;

int LEFTSTRIP_START = 0;
int LEFTSTRIP_END = 5;
int HEADLIGHT_START = 6;
int HEADLIGHT_END = 10;
int RIGHTSTRIP_START = 11;
int RIGHTSTRIP_END = 22;
int TAILLIGHT_START = 23;
int TAILLIGHT_END = 27; //is last pixel address 27 or 28?

void setup()
{
  strip.begin();
  strip.setBrightness(64);
  strip.setPixelColor(0, 255,0,0);
  strip.show(); // Initialize all pixels to 'off'
 
pinMode(switch1IN, INPUT_PULLUP);
pinMode(switch2IN, INPUT_PULLUP);
pinMode(switch3IN, INPUT_PULLUP);
pinMode(switch4IN, INPUT_PULLUP);
pinMode(pushButtonIN, INPUT_PULLUP);
pinMode(LED, OUTPUT);
}

void loop()
{

switch1state = digitalRead(switch1IN);
switch2state = digitalRead(switch2IN);
switch3state = digitalRead(switch3IN);
switch4state = digitalRead(switch4IN);

//this is not an if else as these statements are not exclusive
if (switch1state == LOW)
{
RangeStripColour(HEADLIGHT_START, HEADLIGHT_END, RED);
RangeStripColour(TAILLIGHT_START,TAILLIGHT_END, WHITE);
strip.show();
} else {
RangeStripColour(HEADLIGHT_START, HEADLIGHT_END, OFF);
RangeStripColour(TAILLIGHT_START,TAILLIGHT_END, OFF);
strip.show();
}

if (switch2state == LOW)
{
UBL = true;
} else {
UBL = false;
}

/*
If switch 2 on turn on ubls.

If switch 3 on, override others and do cycling rainbow mode

When ubls on and Button pressed, it'll cycle through colors. Colour value will be stored in eeprom (switch Val rather than colour val)
*/

if(UBL == true)
{
    pushButtonState = digitalRead(pushButtonIN);
    
    if (pushButtonState == LOW){// assuming active low if pull up resistors enabled
        buttonPressedCount++;
    } else{
        buttonPressedCount = 0;
    }
    
    //debounce Button, don't need to do this for switches as they are on or off, but maybe it would be a good idea?
    if( buttonPressedCount > 10)
    {
        buttonIsPressed = true;
    }
    else
    {
        buttonIsPressed = false;
    }


    if ((buttonIsPressed == true) && (lastbuttonIsPressed == false))
    {
        colourSelection++;
    }
    
    if (colourSelection > 4)
    {
        colourSelection = 1;
    }

    if (colourSelection != lastcolourSelection) //filter to stop going through the switch case every time through and writing to the eeprom to many times.
    {
            EEPROM.write(0,colourSelection);
            digitalWrite(LED,HIGH);
            delay(250);
            digitalWrite(LED,LOW);
    }
    
    switch (colourSelection)
    {
        case 1:
    RangeStripColour(LEFTSTRIP_START, LEFTSTRIP_END, RED);
    RangeStripColour(RIGHTSTRIP_START, RIGHTSTRIP_END, RED);
    break;
    
            case 2:
    RangeStripColour(LEFTSTRIP_START, LEFTSTRIP_END, GREEN);
    RangeStripColour(RIGHTSTRIP_START, RIGHTSTRIP_END, GREEN);
    break;
    
            case 3:
    RangeStripColour(LEFTSTRIP_START, LEFTSTRIP_END, BLUE);
    RangeStripColour(RIGHTSTRIP_START, RIGHTSTRIP_END, BLUE);
    break;
    
            case 4:
    RangeStripColour(LEFTSTRIP_START, LEFTSTRIP_END, NEON_ORANGE);
    RangeStripColour(RIGHTSTRIP_START, RIGHTSTRIP_END, NEON_ORANGE);
    break;

    }
} else {
    RangeStripColour(LEFTSTRIP_START, LEFTSTRIP_END, OFF);
    RangeStripColour(RIGHTSTRIP_START, RIGHTSTRIP_END, OFF);
}

//At end of loop
    strip.show();
    lastbuttonIsPressed = buttonIsPressed;
    lastcolourSelection = colourSelection;
    delay(10); //delay to slow down the main loop and writing to strip continually
}
    
void RangeStripColour(uint8_t start_pixel, uint8_t stop_pixel, uint32_t colour)
{
    uint8_t i;
    for(i = start_pixel; i < (stop_pixel+1); i++)
    {
    strip.setPixelColor(i,colour);
    }
}

void RangeStripRGB(uint8_t start_pixel, uint8_t stop_pixel, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t i;
    for(i = start_pixel; i < (stop_pixel+1); i++)
    {
    strip.setPixelColor(i,red,green, blue);
    }
}


void stripoff()
{
    uint8_t i;
    for(i = 0; i < numPixels; i++)
    {
    strip.setPixelColor(i,0,0,0);
    }
}

