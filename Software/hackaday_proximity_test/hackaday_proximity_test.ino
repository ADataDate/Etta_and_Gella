/****************************************************************
  ProximityTest.ino
  APDS-9960 RGB and Gesture Sensor
  Shawn Hymel @ SparkFun Electronics
  October 28, 2014
  https://github.com/sparkfun/APDS-9960_RGB_and_Gesture_Sensor

  Tests the proximity sensing abilities of the APDS-9960.
  Configures the APDS-9960 over I2C and polls for the distance to
  the object nearest the sensor.

  Hardware Connections:

  IMPORTANT: The APDS-9960 can only accept 3.3V!

  Arduino Pin  APDS-9960 Board  Function

  3.3V         VCC              Power
  GND          GND              Ground
  A4           SDA              I2C Data
  A5           SCL              I2C Clock

  Resources:
  Include Wire.h and SparkFun_APDS-9960.h

  Development environment specifics:
  Written in Arduino 1.0.5
  Tested with SparkFun Arduino Pro Mini 3.3V

  This code is beerware; if you see me (or any other SparkFun
  employee) at the local, and you've found our code helpful, please
  buy us a round!

  Distributed as-is; no warranty is given.
****************************************************************/

#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include <Adafruit_NeoPixel.h>

// Global Variables
SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint8_t proximity_data = 0;
#define LED_PIN_R    2  // NeoPixel LED strand is connected to this pin
#define LED_PIN_L    9  // NeoPixel LED strand is connected to this pin

Adafruit_NeoPixel  Etta = Adafruit_NeoPixel(6, LED_PIN_L, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel  Gella = Adafruit_NeoPixel(3, LED_PIN_R, NEO_GRB + NEO_KHZ800);

void setup() {

  // Initialize SerialUSB port
  SerialUSB.begin(9600);
  SerialUSB.println();
  SerialUSB.println(F("------------------------------------"));
  SerialUSB.println(F("SparkFun APDS-9960 - ProximitySensor"));
  SerialUSB.println(F("------------------------------------"));

  Etta.begin();
  Gella.begin();
  Etta.show(); // Initialize all pixels to 'off'
  Gella.show(); // Initialize all pixels to 'off'

  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    SerialUSB.println(F("APDS-9960 initialization complete"));
  } else {
    SerialUSB.println(F("Something went wrong during APDS-9960 init!"));
  }

  // Adjust the Proximity sensor gain
  if ( !apds.setProximityGain(PGAIN_4X) ) {
    SerialUSB.println(F("Something went wrong trying to set PGAIN"));
  }

  // Start running the APDS-9960 proximity sensor (no interrupts)
  if ( apds.enableProximitySensor(false) ) {
    SerialUSB.println(F("Proximity sensor is now running"));
  } else {
    SerialUSB.println(F("Something went wrong during sensor init!"));
  }
}

void loop() {

  // Read the proximity value
  if ( !apds.readProximity(proximity_data) ) {
    SerialUSB.println("Error reading proximity value");
  } else {
    SerialUSB.print("Proximity: ");
    SerialUSB.println(proximity_data);
  }
  for (int i = 0; i < 3; i++) {
    Etta.setBrightness(proximity_data);
    Gella.setBrightness(proximity_data);
    Etta.setPixelColor(i,  proximity_data ,   proximity_data, proximity_data);
    Gella.setPixelColor(i,   proximity_data,   proximity_data, proximity_data);
    
    Etta.show();
    Gella.show();
    delay(50);
  }
  


  // Wait 250 ms before next reading
  delay(50);
}
