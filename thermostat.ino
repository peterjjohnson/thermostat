#include <SparkFunHTU21D.h>
#include "SparkFun_Si7021_Breakout_Library.h"
#include <Wire.h>

// Set up our globals
float humidity = 0;
float realTemp = 0;
float holdTemp = 18;
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
  updateStats(); // Update our temp and humidity stats
  setFurnaceState(); // Turn the furnace on or off if we need to
  
  // If there's data in the serial buffer try and use it to adjust the temp
  if (Serial.available()) {
    processRequest(Serial.readString());
  }
    
  printInfo(); // Print current info
  
  delay(250);
}

// Get current temp and humidity stats
void updateStats()
{
  // Measure current relative humidity
  humidity = sensor.getRH();
  // Measure current temperature
  realTemp = sensor.getTemp();
}

void processRequest(String request)
{
  String command = request.substring(request.lastIndexOf(";") + 1, request.lastIndexOf(":"));
  String value = request.substring(request.lastIndexOf(":") + 1);
  if (command == "setTemp") {
    holdTemp = value.toFloat();
  } else {
    Serial.print("Unrecognized request\n");
    Serial.print(command);
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
  
  Serial.print("\"Temp\":");
  Serial.print(realTemp);
  Serial.print(",");
  
  Serial.print("\"Humidity\":");
  Serial.print(humidity);
  Serial.print(",");
  
  Serial.print("\"HoldTemp\":");
  Serial.print(holdTemp);
  Serial.print(",");
  
  Serial.print("\"Furnace\":");

  if (furnaceState == HIGH) {
    Serial.print("\"ON\"");
  } else {
    Serial.print("\"OFF\"");
  }

  Serial.println("}");
}

