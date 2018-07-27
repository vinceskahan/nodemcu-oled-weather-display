/*
 *
 * display temperature+humidity to the SSD1306 OLED monochrome display
 * using a nodeMCU arduino(ish) compute device
 *
 * this originally started with me buying a Squix ESP8266 WeatherStation Kit
 * plus some additional sensors, jumpers, breadboard, and the like.....
 *
 * (see https://www.amazon.com/ESP8266-Starter-OLED-display-Arduino/dp/B01KE7BA3O/)
 *
 * this is a merge of:
 *    - the adafruit ssd1306_128x32_i2c.ino example sketch from their library
 *    - the adafruit dht21 test utility from their library
 *    - my local tweaks, modifications, and crimes against arduino best practices
 *
 * all bugs are mine, please don't bother the nice folks at Adafruit using this code.
 *
 * ----------------------------------------------
 *
 * Wiring Setup:
 *
 *   DHT11:
 *     GRD to ground rail on breadboard
 *     VCC to power rail on breadboard
 *     DAT to GPIO13 (nodeMCU pin D7)
 *
 *   DHT21:
 *     +   to power rail on breadboard
 *     out to GPIO0 (nodeMCU pin D3)
 *     -   to ground rail on breadboard
 *
 *   OLED:
 *    SDA to GPIO4 (nodeMCU pin D2)
 *    SCL to GPIO5 (nodeMCU pin D1)
 *    GRD to ground rail on breadboard
 *    VCC to power rail on breadboard
 *
 *   nodeMCU:
 *    pin1 to OLED SCL
 *    pin2 to OLED SCL
 *    pin3 to DHT21 out
 *    pin7 to DHT11 DAT
 *    3V3  to power rail on breadboard
 *    GND  to ground rail on breaddboard
 *
 * ----------------------------------------------
 *
 * Software used here:
 *     Arduino IDE 1.8.5 on a Macbook Air
 *        at least one driver for the Air to be able to load the nodeMCU
 *        but which one has been list to the sands of time...
 *     Adafruit SSD1306 library version 1.1.2    
 *     Adafruit GFX library version 1.2.7
 *
 * Installed but probably not needed for the code below....
 *     DHT sensor library by Adafruit version 1.3.0
 *     ESP8266 Weather Station by ThingPulse version 1.3.2
 *     ESP8266 and ESP32 Oled Driver for SSD1306 by Daniel Eichhorn et.al. version 3.2.7
 *
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

/*
 * for the particular mono OLED display from the Weather Station kit
 * we need to fake the OLED_RESET setting below.  Some smart fellow
 * on internet used LED_BUILTIN to make this work, which worked for me
 * so I used it here too.  Unfortunately I lost the page/author of the
 * actual smarts (sorry) so I can't credit them with the wizardry.
*/

#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);

//----------------------------------------------------------
/*
 * uncomment and edit 'one' section here
*/

// #define DHTTYPE DHT11   // DHT 11
// #define DHTPIN 13       // nodemcu GPIO7

#define DHTTYPE DHT21      // DHT 21 (AM2301)
#define DHTPIN 0           // nodemcu GPIO3

//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTPIN 0        // nodemcu GPIO3

//----------------------------------------------------------

// make sure we have the right include file
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

//----------------------------------------------------------

void setup()   {

  dht.begin();
  
  Serial.begin(115200);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // init done

}

//----------------------------------------------------------

void loop() {
 
  //--------- read the sensor --------------
 
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  //-------- serial output for debugging -------
  //  Serial.print("Humidity: ");
  //  Serial.print(h);
  //  Serial.print(" %\t");
  //  Serial.print("Temperature: ");
  //  Serial.print(t);
  //  Serial.print(" *C ");
  //  Serial.print(f);
  //  Serial.print(" *F\t");
  //  Serial.print("Heat index: ");
  //  Serial.print(hic);
  //  Serial.print(" *C ");
  //  Serial.print(hif);
  //  Serial.println(" *F");
  //  delay(2000);
 
  // Clear the display buffer.
  display.clearDisplay();
  display.display();

  // show temperature and humidity
  // with a gentle blinking effect
  //  - two seconds on, two seconds blank
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(30,1);
  display.print(f);
  display.println(" F");
  display.setTextSize(1);
  display.setCursor(50,20);
  display.print(h);
  display.println(" %");
  display.display();
  delay(2000);  
  display.clearDisplay();
  display.println();
  display.display();
  delay(2000);

}
  
/*
 * the rest of this file is (sometimes required) notices from
 * the original example sketches this is based on.  Thanks to Adafruit
 * for writing/releasing this kind of stuff.  Really.
 *
 *
 *    *********************************************************************
 *    This is an example for our Monochrome OLEDs based on SSD1306 drivers
 *    
 *      Pick one up today in the adafruit shop!
 *      ------> http://www.adafruit.com/category/63_98
 *    
 *    This example is for a 128x32 size display using I2C to communicate
 *    3 pins are required to interface (2 I2C and one reset)
 *    
 *    Adafruit invests time and resources providing this open source code, 
 *    please support Adafruit and open-source hardware by purchasing 
 *    products from Adafruit!
 *    
 *    Written by Limor Fried/Ladyada  for Adafruit Industries.  
 *    BSD license, check license.txt for more information
 *    All text above, and the splash screen must be included in any redistribution
 *    *********************************************************************
 *
 * (and)
 *
 *    ********************************************************************
 *    // DHT Temperature & Humidity Sensor
 *    // Unified Sensor Library Example
 *    // Written by Tony DiCola for Adafruit Industries
 *    // Released under an MIT license.
 *    
 *    // Depends on the following Arduino libraries:
 *    // - Adafruit Unified Sensor Library: https://github.com/adafruit/Adafruit_Sensor
 *    // - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
 *    ********************************************************************
 *
*/     
