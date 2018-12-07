/*
  LED VU meter for Arduino and Adafruit NeoPixel LEDs.

  Hardware requirements:
  - Most Arduino or Arduino-compatible boards (ATmega 328P or better).
  - Adafruit Electret Microphone Amplifier (ID: 1063)
  - Adafruit Flora RGB Smart Pixels (ID: 1260)
   OR
  - Adafruit NeoPixel Digital LED strip (ID: 1138)
  - Optional: battery for portable use (else power through USB or adapter)
  Software requirements:
  - Adafruit NeoPixel library

  Connections:
  - 3.3V to mic amp +
  - GND to mic amp -
  - Analog pin to microphone output (configurable below)
  - Digital pin to LED data input (configurable below)
  See notes in setup() regarding 5V vs. 3.3V boards - there may be an
  extra connection to make and one line of code to enable or disable.

  Written by Adafruit Industries.  Distributed under the BSD license.
  This paragraph must be included in any redistribution.
*/

#include <Adafruit_NeoPixel.h>

#define MIC_PIN   A3  // Microphone is attached to this analog pin
#define LED_PIN_R    2  // NeoPixel LED strand is connected to this pin
#define LED_PIN_L    9  // NeoPixel LED strand is connected to this pin
#define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
#define NOISE     20  // Noise/hum/interference in mic signal
#define SAMPLES   60  // Length of buffer for dynamic level adjustment
#define TOP       (3+2) // Allow dot to go slightly off scale
#define PEAK_FALL 70  // Rate of peak falling dot

byte
peak      = 0,      // Used for falling dot
dotCount  = 0,      // Frame counter for delaying dot-falling speed
volCount  = 0;      // Frame counter for storing past volume data
int
vol[SAMPLES],       // Collection of prior volume samples
    lvl       = 10,      // Current "dampened" audio level
    minLvlAvg = 0,      // For dynamic adjustment of graph low & high
    maxLvlAvg = 512;

Adafruit_NeoPixel  Etta = Adafruit_NeoPixel(3, LED_PIN_L, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel  Gella = Adafruit_NeoPixel(3, LED_PIN_R, NEO_GRB + NEO_KHZ800);

void setup() {

  memset(vol, 0, sizeof(vol));
  Etta.begin();
  Gella.begin();
  Etta.show(); // Initialize all pixels to 'off'
  Gella.show(); // Initialize all pixels to 'off'
}

void loop() {
  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int      n, height;



  n   = analogRead(MIC_PIN);                        // Raw reading from mic
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)

  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

  if (height < 0L)       height = 0;     // Clip output
  else if (height > TOP) height = TOP;
  if (height > peak)     peak   = height; // Keep 'peak' dot at top


  // Color pixels based on rainbow gradient
  for (i = 0; i < 3; i++) {
    if (i >= height) {
      Etta.setPixelColor(i,   0,   0, 0);
      Gella.setPixelColor(i,   0,   0, 0);
    }
    else {
      Etta.setPixelColor(i, Wheel_left(map(i, 0, 2, 30, 150)));
      Gella.setPixelColor(i, Wheel_right(map(i, 0, 2, 30, 150)));
    }
  }

  // Draw peak dot
  if (peak > 0 && peak <= 2)
  {
    Etta.setPixelColor(peak, Wheel_left(map(peak, 0, 2, 30, 150)));
    Gella.setPixelColor(peak, Wheel_right(map(peak, 0, 2, 30, 150)));
  }



  Etta.show();
  Gella.show();

  // Every few frames, make the peak pixel drop by 1:

  if (++dotCount >= PEAK_FALL) { //fall rate

    if (peak > 0) peak--;
    dotCount = 0;
  }



  vol[volCount] = n;                      // Save sample for dynamic leveling
  if (++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter

  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for (i = 1; i < SAMPLES; i++) {
    if (vol[i] < minLvl)      minLvl = vol[i];
    else if (vol[i] > maxLvl) maxLvl = vol[i];
  }

  if ((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)

}

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel_left(byte WheelPos) {
  if (WheelPos < 85) {
    return Etta.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return Etta.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return Etta.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

uint32_t Wheel_right(byte WheelPos) {
  if (WheelPos < 85) {
    return Gella.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return Gella.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return Gella.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
