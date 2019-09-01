#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#include <EEPROM.h>
//#include <accelLights.h>

//note:it's a pro mini, you spanner!

//============= class declarations ==========
//accelLights lightLib_x accelLights();
//lightLib_y accelLights();
//lightLib_z accelLights();

//============= IO Declarations ============
const int anInPinX = A3;

//const int anInPinY = A2; //z and y swapped as board is mounted 90 deg out for convenient mounting
//const int anInPinZ = A1;
const int anInPinY = A1;
const int anInPinZ = A2;

#define RGB_PIN 8
int numPixels = 22;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, RGB_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

//======== Colour Declarations ========
uint32_t OFF = strip.Color(0, 0, 0);
uint32_t RED = strip.Color(255, 0, 0);
uint32_t GREEN = strip.Color(0, 255, 0);
uint32_t BLUE = strip.Color(0, 0, 255);
uint32_t YELLOW = strip.Color(255, 255, 0);
uint32_t ORANGE = strip.Color(255, 128, 0);
uint32_t AQUA = strip.Color(0, 255, 255);
uint32_t MAGENTA = strip.Color(255, 0, 255);
uint32_t WHITE = strip.Color(255, 255, 255);

uint32_t NEON_PINK = strip.Color(255, 0, 146);
uint32_t NEON_ORANGE = strip.Color(255, 95, 0);
uint32_t NEON_GREEN = strip.Color(182, 255, 0);
uint32_t NEON_BLUE = strip.Color(34, 141, 255);
uint32_t NEON_PURPLE = strip.Color(186, 1, 255);
uint32_t NEON_YELLOW = strip.Color(204, 255, 0);

uint32_t ELECTRIC_BLUE = strip.Color(125, 249, 255);

//======== Program Variables ========

//bool toggle = true;

//#define Z_INVERT
#define SERIAL_ON

byte counter = 0;

bool rightWayUp = 0;

int rawXadc = 0;
int rawYadc = 0;
int rawZadc = 0;

int pitch = 0;
int roll = 0;

float mapX = 0;
float mapY = 0;
float mapZ = 0;

const float xScale = 0.3;
const float yScale = 0.3;
const float zScale = 0.3;

int aveXadc = 0;
int aveYadc = 0;
int aveZadc = 0;

int filtXadc = 0;
int filtYadc = 0;
int filtZadc = 0;

int rollAveZadc = 0;
int LPFZadc = 0;

int echs;
int why;
int zed;

const int numSamples = 25;

unsigned long lightPeriod = 20; //50;
unsigned long accelPeriod = 20;
unsigned long transmitPeriod = 100;

int lightMode = 1;
/*
    PITCH_AND_ROLL = 1,
    KNIGHT_RIDER = 2,
    HI_G_FLASH = 3,
    POLICE_FLASHERS = 4,
    ACCEL_MAP = 5,
    SPIRIT_LEVEL = 6,
    RAINBOW = 7
*/

const int NO_OF_LIGHT_MODES = 7;

bool LightModeCycleEnabled = false;
byte lightModeCycleCounter = 0;
const int LIGHT_MODE_SELECTOR_DELAY_TIME = 1; //2secs

const int NEG_1G_ADC_VAL = 400;
const int POS_1G_ADC_VAL = 620;

const int ADC_MIN = 0;
const int ADC_MAX = 1023;

const int MAP_MAX = 127;
const int MAP_MIN = -127;

void setup()
{
  analogReference(EXTERNAL);
  strip.begin();
  //strip.setBrightness(50); //otherwise it's blinding!
  strip.setPixelColor(0, 0, 255, 0);
  strip.show(); // Initialize all pixels to 'off'

#ifdef SERIAL_ON
  Serial.begin(115200);
#endif
}

void loop()
{

  rawXadc = analogRead(anInPinX);
  rawYadc = analogRead(anInPinY);
  rawZadc = analogRead(anInPinZ);

//X average sample
  for (uint8_t i = 1; i < numSamples; i++)
  {
    aveXadc += analogRead(anInPinX);
    delay(1);
  }
  aveXadc /= numSamples;

//Y average sample
  for (uint8_t i = 1; i < numSamples; i++)
  {
    aveYadc += analogRead(anInPinY);
    delay(1);
  }
  aveYadc /= numSamples;

//Z average sample
  for (uint8_t i = 1; i < numSamples; i++)
  {
    aveZadc += analogRead(anInPinZ);
    delay(1);
  }
  aveZadc /= numSamples;

//low pass filters
  filtXadc = (xScale * aveXadc) + ((1 - xScale) * filtXadc);
  filtYadc = (yScale * aveYadc) + ((1 - yScale) * filtYadc);
  filtZadc = (zScale * aveZadc) + ((1 - zScale) * filtZadc);

  mapX = map(filtXadc, NEG_1G_ADC_VAL, POS_1G_ADC_VAL, MAP_MIN, MAP_MAX);
  mapY = map(filtYadc, NEG_1G_ADC_VAL, POS_1G_ADC_VAL, MAP_MIN, MAP_MAX);

#ifdef Z_INVERT
  mapZ = map(filtZadc, NEG_1G_ADC_VAL, POS_1G_ADC_VAL, MAP_MIN, MAP_MAX);
#else
  mapZ = map(filtZadc,  POS_1G_ADC_VAL, NEG_1G_ADC_VAL, MAP_MIN, MAP_MAX);
#endif

  //pro - limits map vals to +/- 1g, con - loses large range of accel inputs
  mapX = constrain(mapX, MAP_MIN, MAP_MAX);
  mapY = constrain(mapY, MAP_MIN, MAP_MAX);
  mapZ = constrain(mapZ, MAP_MIN, MAP_MAX);

  pitch = int(atan_degrees(mapX / abs(mapZ)));
  roll = int(atan_degrees(mapY / abs(mapZ)));

  RollandSurgeLights(filtXadc, roll, 3, 25);

//  echs = byte(constrain(127 + mapX, 0, 255));
//  why = byte(constrain(127 + mapY, 0, 255));
//  zed = byte(constrain(127 + mapZ, 0, 255));
//
//  WholeStripRGB(echs, why, zed);

  //  map_to_strip(rawXadc, rawYadc, rawZadc);
  
  strip.show();

  delay(25);
} //////////////////////////////////////////end main

void map_to_strip(int echs, int why, int zed)
{
  mapX = map(echs, ADC_MIN, ADC_MAX, MAP_MIN, MAP_MAX);
  mapY = map(why, ADC_MIN, ADC_MAX, MAP_MIN, MAP_MAX);
  mapZ = map(zed, ADC_MIN, ADC_MAX, MAP_MIN, MAP_MAX);

  echs = byte(constrain(127 + mapX, 0, 255));
  why = byte(constrain(127 + mapY, 0, 255));
  zed = byte(constrain(127 + mapZ, 0, 255));

  WholeStripRGB(echs, why, zed);
}

void WholeStripColour(uint32_t colour)
{
  for (uint8_t i = 0; i <= numPixels; i++)
  {
    strip.setPixelColor(i, colour);
  }
}

void WholeStripRGB(uint8_t red, uint8_t green, uint8_t blue)
{
  for (uint8_t i = 0; i <= numPixels; i++)
  {
    strip.setPixelColor(i, constrain(red, 0, 255), constrain(green, 0, 255), constrain(blue, 0, 255));
  }
}

void RangeStripColour(uint8_t start_pixel, uint8_t stop_pixel, uint32_t colour)
{
  for (uint8_t i = start_pixel; i <= stop_pixel; i++)
  {
    strip.setPixelColor(i, colour);
  }
}

void RangeStripRGB(uint8_t start_pixel, uint8_t stop_pixel, uint8_t red, uint8_t green, uint8_t blue)
{
  for (uint8_t i = start_pixel; i <= stop_pixel; i++)
  {
    strip.setPixelColor(i, constrain(red, 0, 255), constrain(green, 0, 255), constrain(blue, 0, 255));
  }
}

void PoliceLights()
{
  static bool toggle;

  if (toggle)
  {
    RangeStripColour(0, 6, OFF);
    RangeStripColour(7, 13, BLUE);
    RangeStripColour(14, 20, OFF);
    RangeStripColour(21, 28, BLUE);
  }
  else
  {
    RangeStripColour(0, 6, RED);
    RangeStripColour(7, 13, OFF);
    RangeStripColour(14, 20, RED);
    RangeStripColour(21, 28, OFF);
  }

  toggle = !toggle;
}

void RollLights(int angleInput, int minRollAngle, int maxRollAngle)
{
  byte left = 0;
  byte right = 0;

  if (angleInput >= 0)
  {
    angleInput = constrain(abs(angleInput), minRollAngle, maxRollAngle);
    left = byte(map(angleInput, minRollAngle, maxRollAngle, 0, 255));
  }
  else
  {
    angleInput = constrain(abs(angleInput), minRollAngle, maxRollAngle);
    right = byte(map(angleInput, minRollAngle, maxRollAngle, 0, 255));
  }

  WholeStripRGB(0, right, left);
}

void RollandSurgeLights(int surge, int angleInput, int minRollAngle, int maxRollAngle)
{
  byte left = 0;
  byte right = 0;

  if (angleInput >= 0)
  {
    angleInput = constrain(abs(angleInput), minRollAngle, maxRollAngle);
    left = byte(map(angleInput, minRollAngle, maxRollAngle, 0, 255));
  }
  else
  {
    angleInput = constrain(abs(angleInput), minRollAngle, maxRollAngle);
    right = byte(map(angleInput, minRollAngle, maxRollAngle, 0, 255));
  }

  surge = byte(int_map_and_constrain(surge,510,580,0,255));

  WholeStripRGB(surge, right, left);

  Serial.print(surge);
  Serial.print(" ");
  Serial.print(right);
  Serial.print(" ");
  Serial.print(left);
  Serial.println();

}

//when the acceleration passes flashThreshold, turn the whole strip to the color 'color'
void HiGFlash(uint32_t color, float accel, int flashThreshold)
{
  if (abs(accel) > flashThreshold)
  {
    WholeStripColour(color);
  }
  else
  {
    stripClear();
  }
}

void spiritLevel(int angle, uint32_t color, int minAngle, int maxAngle)
{
  static int position;

  angle = constrain(angle, minAngle, maxAngle);
  position = map(angle, minAngle, maxAngle, 0, numPixels - 1);

  strip.setPixelColor(position, color);
}

//sets all the leds in the strip to off
void stripClear()
{
  for (uint8_t i = 0; i < numPixels; i++)
  {
    strip.setPixelColor(i, 0, 0, 0);
  }
}

double atan_degrees(float oppOverAdj)
{
  return atan(oppOverAdj) * (180 / PI);
}

int rollingAverage(int numIn)
{
  const int arraySize = 16;
  static int array[arraySize];
  static int average;
  static boolean firstCall = true;

  //avoids a ramp up from zero
  if (firstCall)
  {
    for (int i = 0; i < arraySize; i++)
    {
      array[i] = numIn;
    }
    firstCall = false;
  }

  for (int i = 0; i < arraySize; i++)
  {
    array[i] = array[i + 1];
  }
  array[arraySize - 1] = numIn;

  average = 0;
  for (int k = 0; k < arraySize; k++)
  {
    average += array[k];
  }
  average = average / arraySize; //to divide by 16

  return average;
}

float f_map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int int_map_and_constrain(int x, int in_min, int in_max, int out_min, int out_max)
{
  int mapVal = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  return constrain(mapVal,out_min,out_max);
}

/* makes the ball appear to be a string of fading away leds */
void knight_rider(uint32_t color, int ball_size)
{
  static int position = 0;
  static bool direction = true;

  static byte r;
  static byte g;
  static byte b;

  r = byte((color & 0x00FF0000) >> 16);
  g = byte((color & 0x0000FF00) >> 8);
  b = byte(color & 0x000000FF);

  if (direction == true)
  {
    position++;
  }
  else
  {
    position--;
  }

  if (position == (numPixels - 1))
  {
    direction = false;
  }

  if (position == 0)
  {
    direction = true;
  }

  stripClear();

  if (direction)
  {
    for (int p = 0; p < ball_size; p++)
    {
      strip.setPixelColor(position - p, r / (p + 1), g / (p + 1), b / (p + 1)); //p+1 as if you divide by zero odd shit happens!
    }
  }
  else
  {
    for (int p = 0; p < ball_size; p++)
    {
      //strip.setPixelColor(position + b,color/(b+1));
      strip.setPixelColor(position + p, r / (p + 1), g / (p + 1), b / (p + 1));
    }
  }
}

void push_light(uint32_t color, float accel)
{
}

void rainbow(uint8_t wait)
{
  uint16_t i, j;

  for (j = 0; j < 256; j++)
  {
    for (i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  else
  {
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
