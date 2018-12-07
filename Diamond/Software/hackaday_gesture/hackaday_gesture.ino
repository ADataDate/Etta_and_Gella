/****************************************************************
APDS-9960 RGB and Gesture Sensor
Mary West for "Designing Expressive Modular Wearable Hardware" Workshop
Hackaday Superconference 2018
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

// Pins
#define APDS9960_INT    7 // Needs to be an interrupt pin


// Global Variables
SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;

void setup() {

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
        break;
      case DIR_DOWN:
        SerialUSB.println("Right");
        break;
      case DIR_LEFT:
        SerialUSB.println("Down");
        break;
      case DIR_RIGHT:
        SerialUSB.println("Up");
        break;
      case DIR_NEAR:
        SerialUSB.println("NEAR");
        break;
      case DIR_FAR:
        SerialUSB.println("FAR");
        break;
      default:
        SerialUSB.println("NONE");
    }
  }
}
