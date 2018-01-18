#include "FastLED.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// RGB Calibration code
//
// Use this sketch to determine what the RGB ordering for your chipset should be.  Steps for setting up to use:

// * Uncomment the line in setup that corresponds to the LED chipset that you are using.  (Note that they
//   all explicitly specify the RGB order as RGB)
// * Define DATA_PIN to the pin that data is connected to.
// * (Optional) if using software SPI for chipsets that are SPI based, define CLOCK_PIN to the clock pin
// * Compile/upload/run the sketch

// You should see six leds on.  If the RGB ordering is correct, you should see 1 red led, 2 green
// leds, and 3 blue leds.  If you see different colors, the count of each color tells you what the
// position for that color in the rgb orering should be.  So, for example, if you see 1 Blue, and 2
// Red, and 3 Green leds then the rgb ordering should be BRG (Blue, Red, Green).

// You can then test this ordering by setting the RGB ordering in the addLeds line below to the new ordering
// and it should come out correctly, 1 red, 2 green, and 3 blue.
//
//////////////////////////////////////////////////

#define NUM_LEDS 100
#define NUM_STRIPS 4

#define s_p Serial.print
#define s_pl Serial.println

// Data pin that led data will be written out over
// Clock pin only needed for SPI based chipsets when not using hardware SPI
//#define CLOCK_PIN 8

CRGB leds[NUM_STRIPS][NUM_LEDS];

void setup() {
  delay(2000);

  FastLED.addLeds<WS2811, 3, RGB>(leds[0], NUM_LEDS);
  FastLED.addLeds<WS2811, 4, RGB>(leds[1], NUM_LEDS);
  FastLED.addLeds<WS2811, 5, RGB>(leds[2], NUM_LEDS);
  FastLED.addLeds<WS2811, 6, RGB>(leds[1], NUM_LEDS);

  FastLED.setBrightness(20);
}

unsigned long boo[NUM_STRIPS] = { 2, 2, 2, 2 };
bool pos[NUM_STRIPS] = { true, true, true, true };
unsigned long oldMillis[NUM_STRIPS] = { 0, 0, 0, 0
};
unsigned long frameMillis = 0;

uint8_t newByte = 0;
uint8_t curByte = 0;
uint8_t oldByte = 0;

void loop() {
  for (int i = 0; i < NUM_STRIPS; i++)
  {
    newByte <<= 1;
    if (analogRead(i) > 400)
    {
      newByte |= 1;
    }
  }

  curByte = newByte & ~oldByte;

  s_p("newByte: ");
  s_pl(newByte);
  s_p("curByte: ");
  s_pl(curByte);
  s_p("oldByte: ");
  s_pl(oldByte);

  for (int i = 0; i < NUM_STRIPS; i++)
  {
    if ((curByte >> i) & 0x01)
    {
      tril(i);
    }
  }
  oldByte = newByte;

  for (int i = 0; i < NUM_STRIPS; i++)
  {
    trilHandler(i);
  }

  FastLED.show();

  if (Serial.available() > 0)
  {
    switch (Serial.read())
    {
      case '1':
        tril(0);
        break;
      case '2':
        tril(1);
        break;
      default:
        s_pl("invalid msg.");
        break;
    }
  }

}

void tril(int a)
{
  //Reset trilHandler delay;
  boo[a] = 2;
}

void trilHandler(int a)
{
  if ((boo[a] <= 300) && (millis() > (oldMillis[a] + boo[a])))
  {
    if (pos[a])
    {

      for (int i = 0; i <= NUM_LEDS; i += 3)
      {
        //pixels.setPixelColor(i, pixels.Color(255, 255, 255));
        leds[a][i] = CRGB(255, 255, 255);
        leds[a][i + 1] = CRGB(0, 0, 0);
      }

      pos[a] = false;
    }
    else
    {
      for (int i = 1; i <= NUM_LEDS; i += 3)
      {
        //pixels.setPixelColor(i, pixels.Color(255, 255, 255));
        leds[a][i] = CRGB(255, 255, 255);
        leds[a][i - 1] = CRGB(0, 0, 0);
      }

      pos[a] = true;
    }
    oldMillis[a] = millis();
    boo[a] += (boo[a] / 2);
    s_pl(boo[a]);
  }
}

