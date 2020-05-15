#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include "RF24.h"


static const int RXPin = 4, TXPin = 3;

#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"


byte addresses[][6] = {"1Node","2Node"};

struct GPS_coords {
  float latitude;
  float longitude;
};

GPS_coords coords;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
RF24 radio(7,8);

void setup()
{
  ss.begin(9600);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW); // for testing
  //radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(addresses[0]);
  radio.stopListening();
  ss.println(PMTK_SET_NMEA_UPDATE_10HZ);
}

void loop()
{
  while (ss.available() > 0) gps.encode(ss.read());
  if (gps.location.isUpdated())
  {
    coords.latitude = gps.location.lat();
    coords.longitude = gps.location.lng();
    radio.write( &coords, sizeof(GPS_coords) ); 
  }
}
