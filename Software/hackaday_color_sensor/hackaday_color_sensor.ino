/****************************************************************
  Mary West for "Designing Expressive Modular Wearable Hardware" Workshop
  Hackaday Superconference 2018
  Based on code developed by Shawn Hymel for SparkFun Electronics
  Nov 2, 2018

  Resources:
  https://github.com/ADataDate/Etta_and_Gella
  https://github.com/sparkfun/APDS-9960_RGB_and_Gesture_Sensor


  Tests the color sensing abilities of the APDS-9960. Configures
  APDS-9960 over I2C and waits for chnges in light/color. Calculates the
  RGB value and displays it on the chevron LEDs and on a SerialUSB console.

  In this workshop we will explore the color sensor using paper color swatches

*/

#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include <Adafruit_NeoPixel.h>

uint16_t ambient_light = 0;
uint16_t red = 0;
uint16_t green = 0;
uint16_t blue = 0;

#define PIN_L           9
#define PIN_R           2


SparkFun_APDS9960 apds = SparkFun_APDS9960();

Adafruit_NeoPixel Etta = Adafruit_NeoPixel(3, PIN_L, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel Gella = Adafruit_NeoPixel(3, PIN_R, NEO_GRB + NEO_KHZ800);

void setup() {
  Etta.begin();
  Gella.begin();
  Etta.show(); // Initialize all pixels to 'off'
  Gella.show(); // Initialize all pixels to 'off'

  // Initialize SerialUSBUSB port
  SerialUSB.begin(115200);
  while (!SerialUSB);
  SerialUSB.println();
  SerialUSB.println(F("--------------------------------"));
  SerialUSB.println(F("Designing Expressive Modular Wearable Hardware"));
  SerialUSB.println(F("Hackaday Superconference Workshop - Color Test"));
  SerialUSB.println(F("--------------------------------"));


  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    SerialUSB.println(F("APDS-9960 initialization complete"));
  } else {
    SerialUSB.println(F("Something went wrong during APDS-9960 init!"));
  }

  // Start running the APDS-9960 light sensor (no interrupts)
  if ( apds.enableLightSensor(false) ) {
    SerialUSB.println(F("Color sensor is now running"));
  } else {
    SerialUSB.println(F("Something went wrong during color sensor init!"));
  }

  // Wait for initialization and calibration to finish
  delay(500);
}

void loop() {
  // Read the light levels (ambient, red, green, blue)
  if (  !apds.readAmbientLight(ambient_light) ||
        !apds.readRedLight(red) ||
        !apds.readGreenLight(green) ||
        !apds.readBlueLight(blue) ) {
    SerialUSB.println("Error reading light values");
  } else {
    SerialUSB.print("Ambient: ");
    SerialUSB.print(ambient_light);
    SerialUSB.print(" Red: ");
    SerialUSB.print(red);
    SerialUSB.print(" Green: ");
    SerialUSB.print(green);
    SerialUSB.print(" Blue: ");
    SerialUSB.println(blue);
    for (int i = 0; i < 3; i++) {
      Etta.setPixelColor(i, Etta.Color(red, green, blue));
      Gella.setPixelColor(i, Gella.Color(red, green, blue));

      Etta.show();
      Gella.show();

      delay(1000); // Delay for a period of time (in milliseconds).

    }
  }

  // Wait 1 second before next reading
  delay(1000);
}





