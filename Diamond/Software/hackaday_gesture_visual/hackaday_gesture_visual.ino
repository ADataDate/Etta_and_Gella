/****************************************************************
  APDS-9960 RGB and Gesture Sensor
  Mary West for "Designing Expressive Modular Wearable Hardware" Workshop
  Hackaday Superconference 2018
  Based on code developed by Shawn Hymel for SparkFun Electronics
  Nov 2, 2018

  Resources:
  https://github.com/sparkfun/APDS-9960_RGB_and_Gesture_Sensor

  Tests the gesture sensing abilities of the APDS-9960. Configures
  APDS-9960 over I2C and waits for gesture events. Calculates the
  direction of the swipe (up, down, left, right) and displays it
  on a SerialUSB console.

  To perform a NEAR gesture, hold your hand
  far above the sensor and move it close to the sensor (within 2
  inches). Hold your hand there for at least 1 second and move it
  away.

  To perform a FAR gesture, hold your hand within 2 inches of the
  sensor for at least 1 second and then move it above (out of
  range) of the sensor.

*/

#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include <Adafruit_NeoPixel.h>

// Pins
#define APDS9960_INT    7
#define PIN_L           9
#define PIN_R           2


// Global Variables
SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;

Adafruit_NeoPixel Etta = Adafruit_NeoPixel(3, PIN_L, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel Gella = Adafruit_NeoPixel(3, PIN_R, NEO_GRB + NEO_KHZ800);

void setup() {
  Etta.begin();
  Gella.begin();
  Etta.show(); // Initialize all pixels to 'off'
  Gella.show(); // Initialize all pixels to 'off'


  // Set interrupt pin as input
  pinMode(APDS9960_INT, INPUT);

  // Initialize SerialUSB port
  SerialUSB.begin(115200);
  while (!SerialUSB);
  SerialUSB.println();
  SerialUSB.println(F("--------------------------------"));
  SerialUSB.println(F("Designing Expressive Modular Wearable Hardware"));
  SerialUSB.println(F("Hackaday Superconference Workshop - GestureTest"));
  SerialUSB.println(F("--------------------------------"));

  // Initialize interrupt service routine
  attachInterrupt(0, interruptRoutine, FALLING);

  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    SerialUSB.println(F("APDS-9960 initialization complete"));
  } else {
    SerialUSB.println(F("Something went wrong during APDS-9960 init!"));
  }

  // Start running the APDS-9960 gesture sensor engine
  if ( apds.enableGestureSensor(true) ) {
    SerialUSB.println(F("Waiting for gesture"));
  } else {
    SerialUSB.println(F("Something went wrong during gesture sensor init!"));
  }
}

void loop() {
  handleGesture();
}

void interruptRoutine() {
  isr_flag = 1;
}

void handleGesture() {
  if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_UP:
        SerialUSB.println("Left");
        colorWipe_L(Etta.Color(232, 148, 32), 50); // Orange
        break;
      case DIR_DOWN:
        SerialUSB.println("Right");
        colorWipe_R(Gella.Color(131, 43, 175), 50); // Purple
        break;
      case DIR_LEFT:
        SerialUSB.println("Down");
        colorWipe_down(85); //green
        break;
      case DIR_RIGHT:
        SerialUSB.println("Up");
        colorWipe_up(85); //blue

        break;
      case DIR_NEAR:
        SerialUSB.println("NEAR");
        brighten();
        break;
      case DIR_FAR:
        SerialUSB.println("FAR");
        darken();
        break;
      default:
        SerialUSB.println("NONE");
    }
  }
}

void colorWipe_L(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < Etta.numPixels(); i++) {
    Etta.setPixelColor(i, c);
    Etta.show();
    delay(wait);
  }
  for (uint16_t i = 0; i < Etta.numPixels(); i++) {
    Etta.setPixelColor(i, 0 , 0, 0);
    Etta.show();
    delay(wait);
  }
}

void colorWipe_R(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < Gella.numPixels(); i++) {
    Gella.setPixelColor(i, c);
    Gella.show();
    delay(wait);
  }
  for (uint16_t i = 0; i < Gella.numPixels(); i++) {
    Gella.setPixelColor(i, 0, 0, 0);
    Gella.show();
    delay(wait);
  }
}
void colorWipe_down(uint8_t wait) {
  for (uint16_t i = 0; i < 3; i++) {
    Etta.setPixelColor(i, 0, 149, 0);
    Gella.setPixelColor(i, 0, 149, 0);
    Etta.show();
    Gella.show();
    delay(wait);
  }
  for (uint16_t i = 0; i < 3; i++)
  {
    Etta.setPixelColor(i, 0, 0, 0);
    Gella.setPixelColor(i, 0, 0, 0);
    Etta.show();
    Gella.show();
    delay(wait);
  }
}
void colorWipe_up(uint8_t wait) {
  for (uint16_t i = 0; i < 3; i++) {
    Etta.setPixelColor((2 - i), 0, 0, 149);
    Gella.setPixelColor((2 - i), 0, 0, 149);
    Etta.show();
    Gella.show();
    delay(wait);
  }
  for (uint16_t i = 0; i < 3; i++)
  {
    Etta.setPixelColor((2 - i), 0, 0, 0);
    Gella.setPixelColor((2 - i), 0, 0, 0);
    Etta.show();
    Gella.show();
    delay(wait);
  }
}

void brighten() {
  uint16_t i, j;

  for (j = 0; j < 200; j++) {
    for (i = 0; i < 3; i++) {
      Etta.setPixelColor(i, j, j, j);
      Gella.setPixelColor(i, j, j, j);
    }
    Etta.show();
    Gella.show();
    delay(10);
  }
  delay(500);
  for (uint16_t i = 0; i < 3; i++)
  {
    Etta.setPixelColor((2 - i), 0, 0, 0);
    Gella.setPixelColor((2 - i), 0, 0, 0);
    Etta.show();
    Gella.show();
  }
}

void darken() {
  uint16_t i, j;
  for (j = 200; j > 0; j--) {
    for (i = 0; i < 3; i++) {
      Etta.setPixelColor(i, j, j, j);
      Gella.setPixelColor(i, j, j, j);
    }
    Etta.show();
    Gella.show();
    delay(10);
  }
  delay(500);
   for (uint16_t i = 0; i < 3; i++)
  {
    Etta.setPixelColor((2 - i), 0, 0, 0);
    Gella.setPixelColor((2 - i), 0, 0, 0);
    Etta.show();
    Gella.show();
  }

}

