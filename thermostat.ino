#include <SparkFunHTU21D.h>
#include "SparkFun_Si7021_Breakout_Library.h"
#include <Wire.h>

// Set up our globals
float humidity = 0;
float realTemp = 0;
float holdTemp = 18;
int cycle = 0;
char furnaceState = LOW;

// Create Instance of HTU21D or SI7021 temp and humidity sensor and MPL3115A2 barrometric sensor
Weather sensor;

// Get the hardware ready
void setup()
{
  Serial.begin(9600);   // open serial over USB at 9600 baud
  pinMode(13, OUTPUT);  // initialize LED pin for output
  sensor.begin();       // connect to our temp and humidity sensor
}

// The main event
void loop()
{
  // If there's data in the serial buffer try and use it to adjust the temp
  if (Serial.available()) {
    adjustTemp(Serial.read());
  }

  updateStats(); // Update our temp and humidity stats
  setFurnaceState(); // Turn the furnace on or off if we need to

  // We loop every 500ms but we'll only print an update every 2s
  if (cycle == 4) {
    cycle = 0;
    printInfo();
  }
  
  cycle++;
  delay(500);
}

// Get current temp and humidity stats
void updateStats()
{
  // Measure current relative humidity
  humidity = sensor.getRH();
  // Measure current temperature
  realTemp = sensor.getTemp();
}

// Adjust the temp up or down as indicated
void adjustTemp(char tempDirection) 
{
  switch (tempDirection) {
    case 30: // Up
      if (holdTemp < 25) { // 25 seems like a good max temp
        holdTemp += 0.5;
      }
      break;
    case 31: // Down
      if (holdTemp > 15) { // 15 seems like a good min temp
        holdTemp -= 0.5;
      }
      break;
  }
}

// Turn the furnace on or off as necessary
void setFurnaceState() 
{
  int tempdiff = holdTemp - realTemp;
  bool stateChange = true;
  if (tempdiff >= 1 && furnaceState == LOW) {
    furnaceState = HIGH;
  } else if (tempdiff < 1 && furnaceState == HIGH) {
    furnaceState = LOW;
  } else {
    stateChange = false;
  }
  
  if (stateChange) {
    // Currently, just turn the LED pin on or off to simulate the furnace
    digitalWrite(13, furnaceState);
  }
}

// Print current stats and settings to the serial output in JSON format
void printInfo()
{
  Serial.print("{");

  Serial.print("\"CurrentTemp\":\"");
  Serial.print(realTemp);
  
  Serial.print("\",\"Humidity\":\"");
  Serial.print(humidity);
  
  Serial.print("\",\"HoldTemp\":\"");
  Serial.print(holdTemp);
  
  Serial.print("\",\"FurnaceState\":");

  if (furnaceState == HIGH) {
    Serial.print("\"ON\"");
  } else {
    Serial.print("\"OFF\"");
  }

  Serial.println("}");
}

