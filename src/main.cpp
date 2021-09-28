#include "FastLED.h"
#include "U8g2lib.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE); // All Boards without Reset of the Display
const uint8_t NUM_LEDS = 24;
#define FRAMES_PER_SECOND 60

CRGBArray<NUM_LEDS> leds;
#define STRING_MODE 0
#define BLINK_MODE 1
#define PRIDE_MODE 2
#define DEMO_MODE 3
#define TWINKLE2_MODE 4
#define PATTERN1_MODE 5
#define PATTERN2_MODE 6
#define PATTERN3_MODE 7
#define PATTERN4_MODE 8
#define MAX_MODES 8
void btn_pressed();
void string_animation(uint8_t speed);
void blink(uint8_t speed);
void pride();
void demo();
void nextPattern();
void rainbow();
void rainbowWithGlitter();
void addGlitter(fract8 chanceOfGlitter);
void confetti();
void sinelon();
void bpm();
void juggle();
void twinkle2();
void pattern1(uint16_t speed);
void pattern2(uint16_t speed);
void pattern3(uint16_t speed);
void pattern4(uint16_t speed);
void chooseNextColorPalette(CRGBPalette16 &pal);
void coolLikeIncandescent(CRGB &c, uint8_t phase);
uint8_t attackDecayWave8(uint8_t i);
CRGB computeOneTwinkle(uint32_t ms, uint8_t salt);
void drawTwinkles(CRGBSet &L);
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
uint8_t currentMode = BLINK_MODE;
uint8_t previousMode = currentMode;
bool btnPressed = false;
const uint8_t mapPattern2[24] = {20, 19, 18, 17, 16, 15, 13, 12, 14, 23, 22, 21, 4, 7, 5, 6, 3, 8, 10, 1, 9, 2, 11};
// Overall twinkle speed.
// 0 (VERY slow) to 8 (VERY fast).
// 4, 5, and 6 are recommended, default is 4.
#define TWINKLE_SPEED 5

// Overall twinkle density.
// 0 (NONE lit) to 8 (ALL lit at once).
// Default is 5.
#define TWINKLE_DENSITY 1

// How often to change color palettes.
#define SECONDS_PER_PALETTE 30
// Also: toward the bottom of the file is an array
// called "ActivePaletteList" which controls which color
// palettes are used; you can add or remove color palettes
// from there freely.

// Background color for 'unlit' pixels
// Can be set to CRGB::Black if desired.
CRGB gBackgroundColor = CRGB::Black;
// Example of dim incandescent fairy light background color
// CRGB gBackgroundColor = CRGB(CRGB::FairyLight).nscale8_video(16);

// If AUTO_SELECT_BACKGROUND_COLOR is set to 1,
// then for any palette where the first two entries
// are the same, a dimmed version of that color will
// automatically be used as the background color.
#define AUTO_SELECT_BACKGROUND_COLOR 0

// If COOL_LIKE_INCANDESCENT is set to 1, colors will
// fade out slighted 'reddened', similar to how
// incandescent bulbs change color as they get dim down.
#define COOL_LIKE_INCANDESCENT 1
// A mostly red palette with green accents and white trim.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM =
    {CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
     CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
     CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray,
     CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green};

// A mostly (dark) green palette with red berries.
#define Holly_Green 0x00580c
#define Holly_Red 0xB00402
const TProgmemRGBPalette16 Holly_p FL_PROGMEM =
    {Holly_Green, Holly_Green, Holly_Green, Holly_Green,
     Holly_Green, Holly_Green, Holly_Green, Holly_Green,
     Holly_Green, Holly_Green, Holly_Green, Holly_Green,
     Holly_Green, Holly_Green, Holly_Green, Holly_Red};

// A red and white striped palette
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 RedWhite_p FL_PROGMEM =
    {CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
     CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray,
     CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
     CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray};

// A mostly blue palette with white accents.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM =
    {CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
     CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
     CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
     CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray};

// A pure "fairy light" palette with some brightness variations
#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM =
    {CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight,
     HALFFAIRY, HALFFAIRY, CRGB::FairyLight, CRGB::FairyLight,
     QUARTERFAIRY, QUARTERFAIRY, CRGB::FairyLight, CRGB::FairyLight,
     CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight};

// A palette of soft snowflakes with the occasional bright one
const TProgmemRGBPalette16 Snow_p FL_PROGMEM =
    {0x304048, 0x304048, 0x304048, 0x304048,
     0x304048, 0x304048, 0x304048, 0x304048,
     0x304048, 0x304048, 0x304048, 0x304048,
     0x304048, 0x304048, 0x304048, 0xE0F0FF};

// A palette reminiscent of large 'old-school' C9-size tree lights
// in the five classic colors: red, orange, green, blue, and white.
#define C9_Red 0xB80400
#define C9_Orange 0x902C02
#define C9_Green 0x046002
#define C9_Blue 0x070758
#define C9_White 0x606820
const TProgmemRGBPalette16 RetroC9_p FL_PROGMEM =
    {C9_Red, C9_Orange, C9_Red, C9_Orange,
     C9_Orange, C9_Red, C9_Orange, C9_Red,
     C9_Green, C9_Green, C9_Green, C9_Green,
     C9_Blue, C9_Blue, C9_Blue,
     C9_White};

// A cold, icy pale blue palette
#define Ice_Blue1 0x0C1040
#define Ice_Blue2 0x182080
#define Ice_Blue3 0x5080C0
const TProgmemRGBPalette16 Ice_p FL_PROGMEM =
    {
        Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
        Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
        Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
        Ice_Blue2, Ice_Blue2, Ice_Blue2, Ice_Blue3};

// Add or remove palette names from this list to control which color
// palettes are used, and in what order.
const TProgmemRGBPalette16 *ActivePaletteList[] = {
    &RetroC9_p,
    &BlueWhite_p,
    &RainbowColors_p,
    &FairyLight_p,
    &RedGreenWhite_p,
    &PartyColors_p,
    &RedWhite_p,
    &Snow_p,
    &Holly_p,
    &Ice_p};
CRGBPalette16 gCurrentPalette;
CRGBPalette16 gTargetPalette;
// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns
void setup()
{
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, 2>(leds, NUM_LEDS);
  FastLED.setBrightness(128);
  //FastLED.addLeds<WS2812B, 3>(leds, NUM_LEDS);
  attachInterrupt(digitalPinToInterrupt(4), btn_pressed, FALLING);
  u8g2.begin();
  u8g2.setFont(u8g2_font_profont17_tf); // choose a suitable font
  u8g2.clearBuffer();
  String s = "Merry X-Mas!";
  //s += currentMode;
  u8g2.drawStr(0, 40, s.c_str());
  u8g2.sendBuffer(); // transfer internal memory to the display
  previousMode = currentMode;
}
void loop()
{
  if (currentMode != previousMode)
  {
    FastLED.clear();
  }
  switch (currentMode)
  {
  case STRING_MODE:
    string_animation(100);
    break;
  case BLINK_MODE:
    blink(2);
    break;
  case PRIDE_MODE:
    pride();
    break;
  case DEMO_MODE:
    demo();
    break;
  case TWINKLE2_MODE:
    twinkle2();
    break;
  case PATTERN1_MODE:
    pattern1(50); //every half a second
    break;
  case PATTERN2_MODE:
    pattern2(25);
    break;
  case PATTERN3_MODE:
    pattern3(100);
    break;
  case PATTERN4_MODE:
    pattern4(100);
    break;
  default:
    currentMode = STRING_MODE;
  }
  if (currentMode != previousMode)
  {
    u8g2.clearBuffer();
    String s = "Merry X-Mas!";
    //s += currentMode;
    u8g2.drawStr(0, 40, s.c_str());
    u8g2.sendBuffer(); // transfer internal memory to the display
    previousMode = currentMode;
    FastLED.clear();
  }
}

void pattern1(uint16_t speed)
{
  static int streamer = 0;
  static byte hue = HUE_RED;
  EVERY_N_MILLIS(speed)
  {
    leds[23] = CRGB::Red;
    leds[22] = CRGB::RoyalBlue;
    leds[21] = CRGB::RoyalBlue;
    for (int i = 0; i < 12; i++)
    {
      leds[i].setHue(hue);
      
    }
    for(int i=12;i<21;i++)
    {
      leds[i].setHue(255-hue);
    }
    FastLED.show();
    hue++;
  }
}

void pattern2(uint16_t speed)
{

  static uint8_t count = 0;
  static byte hue = HUE_RED;
  EVERY_N_MILLIS(speed)
  {
    leds[mapPattern2[count]].setHue(hue);
    hue++;
    FastLED.show();
    count++;
    if (count == 24)
    {
      FastLED.clear();
      count = 0;
    }
  }
}

void pattern3(uint16_t speed)
{
  static byte hue = HUE_RED;
  EVERY_N_MILLIS(speed)
  {
    for (int i = 0; i < 12; i++)
    {
      leds[i] = CRGB::White;
    }
    for (int i = 12; i < 21; i++)
    {
      leds[i].setHue(hue);
    }
    hue++;
    leds[23] = CRGB::Red;
    leds[22] = CRGB::White;
    leds[21] = CRGB::White;
    FastLED.show();
  }
}

void pattern4(uint16_t speed)
{
  static bool left = true;
  leds[23] = CRGB::Red;
  EVERY_N_SECONDS(2)
  {
    FastLED.clear();
    left = !left;
    if (left)
    {
      for (int i = 0; i < 6; i++)
      {
        leds[i] = CRGB::White;
      }
    }
    else
    {
      for (int i = 6; i < 12; i++)
      {
        leds[i] = CRGB::White;
      }
    }
    leds[23] = CRGB::Red;
    leds[22] = CRGB::RoyalBlue;
    leds[21] = CRGB::RoyalBlue;
    for (int i = 12; i < 21; i++)
    {
      leds[i] = CRGB::Salmon;
    }
    FastLED.show();
  }
  EVERY_N_MILLIS(speed)
  {
    if (left)
    {
      for (int i = 0; i < 6; i++)
      {
        leds[i].fadeToBlackBy(25);
      }
    }
    else
    {
      for (int i = 6; i < 12; i++)
      {
        leds[i].fadeToBlackBy(25);
      }
    }
    FastLED.show();
  }
}

void twinkle2()
{
  EVERY_N_SECONDS(SECONDS_PER_PALETTE)
  {
    chooseNextColorPalette(gTargetPalette);
  }

  EVERY_N_MILLISECONDS(10)
  {
    nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 12);
  }

  drawTwinkles(leds);
  FastLED.show();
}

void demo()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS(20) { gHue++; }  // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS(3) { nextPattern(); } // change patterns periodically
}

void string_animation(uint8_t speed)
{
  const byte fadeAmt = 96;
  const int cometSize = 5;
  const int deltaHue = 4;
  static byte hue = HUE_RED;
  static int iDirection = 1;
  static int iPos = 0;
  for (int i = 21; i < 24; i++)
  {
    leds[i] = CRGB::GhostWhite;
  }
  hue += deltaHue;
  iPos += iDirection;

  if (iPos == (20) || iPos == 0)
  {
    iDirection *= -1;
  }

  for (int i = 0; i < cometSize; i++)
  {
    leds[iPos].setHue(hue);
  }

  for (int i = 0; i < 21; i++)
  {
    leds[i] = leds[i].fadeToBlackBy(fadeAmt);
  }
  FastLED.show();
  delay(speed);
}

void blink(uint8_t speed)
{
  EVERY_N_SECONDS(speed)
  {
    FastLED.clear();
    FastLED.show();
    leds[23] = CRGB::Red;
    for(int i=0;i<21;i++)
    {
      leds[i] = CRGB::Green;
    }
    int rand = random(100);
    leds[22].setRGB(32,32,32);
    leds[21].setRGB(32,32,32);
    if (rand % 2 == 0)
    {
      leds[22] = CRGB::White;
      FastLED.show();
      delay(100);
      leds[22] = CRGB::Black;
      FastLED.show();
      delay(100);
      leds[22] = CRGB::White;
      FastLED.show();
      delay(100);
      leds[22] = CRGB::Black;
      FastLED.show();
    }
    else
    {
      leds[21] = CRGB::White;
      FastLED.show();
      delay(100);
      leds[21] = CRGB::Black;
      FastLED.show();
      delay(100);
      leds[21] = CRGB::White;
      FastLED.show();
      delay(100);
      leds[21] = CRGB::Black;
      FastLED.show();
    }
    leds[22].setRGB(32,32,32);
    leds[21].setRGB(32,32,32);
    FastLED.show();
    
  }
}
void btn_pressed()
{
  if (currentMode == 0)
    gCurrentPalette = Snow_p;
  if (currentMode == 3)
    chooseNextColorPalette(gTargetPalette);
  currentMode++;
  btnPressed = true;
  FastLED.clear();
}

// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride()
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88(87, 220, 250);
  uint8_t brightdepth = beatsin88(341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16; //gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis;
  sLastMillis = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88(400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV(hue8, sat8, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS - 1) - pixelnumber;

    nblend(leds[pixelnumber], newcolor, 64);
  }
  FastLED.show();
}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter(fract8 chanceOfGlitter)
{
  if (random8() < chanceOfGlitter)
  {
    leds[random16(NUM_LEDS)] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS; i++)
  { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle()
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++)
  {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

//  This function loops over each pixel, calculates the
//  adjusted 'clock' that this pixel should use, and calls
//  "CalculateOneTwinkle" on each pixel.  It then displays
//  either the twinkle color of the background color,
//  whichever is brighter.
void drawTwinkles(CRGBSet &L)
{
  // "PRNG16" is the pseudorandom number generator
  // It MUST be reset to the same starting value each time
  // this function is called, so that the sequence of 'random'
  // numbers that it generates is (paradoxically) stable.
  uint16_t PRNG16 = 11337;

  uint32_t clock32 = millis();

  // Set up the background color, "bg".
  // if AUTO_SELECT_BACKGROUND_COLOR == 1, and the first two colors of
  // the current palette are identical, then a deeply faded version of
  // that color is used for the background color
  CRGB bg;
  if ((AUTO_SELECT_BACKGROUND_COLOR == 1) &&
      (gCurrentPalette[0] == gCurrentPalette[1]))
  {
    bg = gCurrentPalette[0];
    uint8_t bglight = bg.getAverageLight();
    if (bglight > 64)
    {
      bg.nscale8_video(16); // very bright, so scale to 1/16th
    }
    else if (bglight > 16)
    {
      bg.nscale8_video(64); // not that bright, so scale to 1/4th
    }
    else
    {
      bg.nscale8_video(86); // dim, scale to 1/3rd.
    }
  }
  else
  {
    bg = gBackgroundColor; // just use the explicitly defined background color
  }

  uint8_t backgroundBrightness = bg.getAverageLight();

  for (CRGB &pixel : L)
  {
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    uint16_t myclockoffset16 = PRNG16;         // use that number as clock offset
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384; // next 'random' number
    // use that number as clock speed adjustment factor (in 8ths, from 8/8ths to 23/8ths)
    uint8_t myspeedmultiplierQ5_3 = ((((PRNG16 & 0xFF) >> 4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
    uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
    uint8_t myunique8 = PRNG16 >> 8; // get 'salt' value for this pixel

    // We now have the adjusted 'clock' for this pixel, now we call
    // the function that computes what color the pixel should be based
    // on the "brightness = f( time )" idea.
    CRGB c = computeOneTwinkle(myclock30, myunique8);

    uint8_t cbright = c.getAverageLight();
    int16_t deltabright = cbright - backgroundBrightness;
    if (deltabright >= 32 || (!bg))
    {
      // If the new pixel is significantly brighter than the background color,
      // use the new color.
      pixel = c;
    }
    else if (deltabright > 0)
    {
      // If the new pixel is just slightly brighter than the background color,
      // mix a blend of the new color and the background color
      pixel = blend(bg, c, deltabright * 8);
    }
    else
    {
      // if the new pixel is not at all brighter than the background color,
      // just use the background color.
      pixel = bg;
    }
  }
}

//  This function takes a time in pseudo-milliseconds,
//  figures out brightness = f( time ), and also hue = f( time )
//  The 'low digits' of the millisecond time are used as
//  input to the brightness wave function.
//  The 'high digits' are used to select a color, so that the color
//  does not change over the course of the fade-in, fade-out
//  of one cycle of the brightness wave function.
//  The 'high digits' are also used to determine whether this pixel
//  should light at all during this cycle, based on the TWINKLE_DENSITY.
CRGB computeOneTwinkle(uint32_t ms, uint8_t salt)
{
  uint16_t ticks = ms >> (8 - TWINKLE_SPEED);
  uint8_t fastcycle8 = ticks;
  uint16_t slowcycle16 = (ticks >> 8) + salt;
  slowcycle16 += sin8(slowcycle16);
  slowcycle16 = (slowcycle16 * 2053) + 1384;
  uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);

  uint8_t bright = 0;
  if (((slowcycle8 & 0x0E) / 2) < TWINKLE_DENSITY)
  {
    bright = attackDecayWave8(fastcycle8);
  }

  uint8_t hue = slowcycle8 - salt;
  CRGB c;
  if (bright > 0)
  {
    c = ColorFromPalette(gCurrentPalette, hue, bright, NOBLEND);
    if (COOL_LIKE_INCANDESCENT == 1)
    {
      coolLikeIncandescent(c, fastcycle8);
    }
  }
  else
  {
    c = CRGB::Black;
  }
  return c;
}

// This function is like 'triwave8', which produces a
// symmetrical up-and-down triangle sawtooth waveform, except that this
// function produces a triangle wave with a faster attack and a slower decay:
//
//     / \ 
//    /     \ 
//   /         \ 
//  /             \ 
//

uint8_t attackDecayWave8(uint8_t i)
{
  if (i < 86)
  {
    return i * 3;
  }
  else
  {
    i -= 86;
    return 255 - (i + (i / 2));
  }
}

// This function takes a pixel, and if its in the 'fading down'
// part of the cycle, it adjusts the color a little bit like the
// way that incandescent bulbs fade toward 'red' as they dim.
void coolLikeIncandescent(CRGB &c, uint8_t phase)
{
  if (phase < 128)
    return;

  uint8_t cooling = (phase - 128) >> 4;
  c.g = qsub8(c.g, cooling);
  c.b = qsub8(c.b, cooling * 2);
}

// Advance to the next color palette in the list (above).
void chooseNextColorPalette(CRGBPalette16 &pal)
{
  const uint8_t numberOfPalettes = sizeof(ActivePaletteList) / sizeof(ActivePaletteList[0]);
  static uint8_t whichPalette = -1;
  whichPalette = addmod8(whichPalette, 1, numberOfPalettes);

  pal = *(ActivePaletteList[whichPalette]);
}