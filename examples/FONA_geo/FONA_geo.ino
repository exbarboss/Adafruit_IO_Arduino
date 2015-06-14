// Adafruit IO REST API geo example with Adafruit FONA
//
// Designed specifically to work with the Adafruit FONA:
//  ----> http://www.adafruit.com/products/1946
//  ----> http://www.adafruit.com/products/1963
//  ----> http://www.adafruit.com/products/2468
//  ----> http://www.adafruit.com/products/2542
//
// These cellular modules use TTL Serial to communicate, 2 pins are 
// required to interface.
//
// Adafruit invests time and resources providing this open source code, 
// please support Adafruit and open-source hardware by purchasing 
// products from Adafruit!
//
// Written by Tony DiCola for Adafruit Industries.  
// MIT license, all text above must be included in any redistribution.
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include "Adafruit_IO_FONA.h"


// Configure the connection to the FONA below.  This configuration works with
// the shield by default too.
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

// Configure Adafruit IO access.
#define AIO_KEY "...your Adafruit IO key value ..."


// Create a software serial instance to talk to FONA.
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);

// Create a FONA library instance.
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

// Create an Adafruit IO FONA instance.  Notice that this needs to take a
// Adafruit_FONA object as the first parameter, and as the second parameter a
// default Adafruit IO key to use when accessing feeds (however each feed can
// override this default key value if required, see further below).
Adafruit_IO_FONA aio = Adafruit_IO_FONA(fona, AIO_KEY);

// Finally create instances of Adafruit_IO_Feed objects, one per feed.  Do this
// by calling the getFeed function on the Adafruit_IO_FONA object and passing
// it at least the name of the feed, and optionally a specific AIO key to use
// when accessing the feed (the default is to use the key set on the
// Adafruit_IO_FONA class).
Adafruit_IO_Feed testFeed = aio.getFeed("fona-geo-testfeed");

// Global state to increment a number and send it to the feed.
unsigned int count = 0;

void setup() {
  // Setup serial port access.
  Serial.begin(115200);
  Serial.println(F("Adafruit IO FONA geo test!"));

  // Setup FONA software serial port access.
  fonaSS.begin(4800);
  
  // Initialize the FONA library just like a normal FONA sketch.
  if (!fona.begin(fonaSS)) {
    Serial.println(F("Couldn't find FONA!"));
    while (1);
  }
  
  // Set the APN for access to GPRS on FONA below.  You might not need to do
  // this depending on your SIM card and provider.  When in doubt check with the
  // SIM provider to find out what value needs to be set.
  // For example you can set an APN with username and password with:
  //fona.setGPRSNetworkSettings(F("your APN"), F("your username"), F("your password"));
  // Or set an APN only:
  //fona.setGPRSNetworkSettings(F("your APN"));
  // Or for example to use a Ting SIM card use just the APN value "wholesale":
  //fona.setGPRSNetworkSettings(F("wholesale"));  // For Ting SIM cards.
  
  // Initialize the Adafruit IO FONA class.
  aio.begin();

  // Delay a short period to make sure the GPRS modem is enabled and ready.
  delay(2000);

  Serial.println(F("Ready!"));
}

void loop() {
  // Get GPRS location from FONA.
  char* lat;
  char* lon;
  char replybuffer[255];
  uint16_t returncode;
  if (fona.getGSMLoc(&returncode, replybuffer, 250) && (returncode == 0)) {
    // Parse out lat and long values from comma separated values in response.
    lon = strtok(replybuffer, ",");
    lat = strtok(NULL, ",");
    Serial.print(F("Latitude: ")); Serial.println(lat);
    Serial.print(F("Longitude: ")); Serial.println(lon);
  }
  else {
    Serial.println(F("Failed to get GPRS location!"));
  }
  
  // Increment the count value and write it to the feed.
  count += 1;
  // To write a value just call the feed's send function and pass it the value.
  // Send will create the feed on Adafruit IO if it doesn't already exist and
  // then add the value to it.  Send returns a boolean that's true if it works
  // and false if it fails for some reason.  Notice that send can also take
  // a latitude, longitude (and not shown but also elevation) value.  These can
  // be either string or float values.
  if (testFeed.send(count, lat, lon)) {
    Serial.print(F("Wrote value to feed: ")); Serial.println(count, DEC);
  }
  else {
    Serial.println(F("Error writing value to feed!"));
  }

  // Now wait 10 seconds and loop.
  Serial.println(F("Waiting 10 seconds and then writing again."));
  delay(10000);
}
