#include <SPI.h>
#include "RF24.h"
#include <TinyGPS++.h>
#include <AccelStepper.h>


static const int stepPin = 5, dirPin = 6;
byte addresses[][6] = {"1Node","2Node"};

struct GPS_coords {
  float latitude;
  float longitude;
};

GPS_coords mobile_coords;
GPS_coords base_coords;

RF24 radio(7,8);
AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin);

void setup() {
  radio.begin();

  radio.setPALevel(RF24_PA_LOW); // for testing
  //radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(1,addresses[0]);
  radio.startListening();
  while (base_coords.latitude == 0){
      if( radio.available()){
      while (radio.available()) {
        radio.read( &mobile_coords, sizeof(GPS_coords) );
        base_coords.latitude = mobile_coords.latitude;
        base_coords.longitude = mobile_coords.longitude;
      }
    }
  }
  stepper.setMaxSpeed(400.0);
  stepper.setAcceleration(200.0);
  stepper.setCurrentPosition(1600);
  stepper.runToNewPosition(1500);
  stepper.runToNewPosition(1700);
  stepper.runToNewPosition(1600);
}

void loop() {
    if( radio.available()){
      while (radio.available()) {
        radio.read( &mobile_coords, sizeof(GPS_coords) );
      }
      
      double courseToMobile =
        TinyGPSPlus::courseTo(
          base_coords.latitude,
          base_coords.longitude,
          mobile_coords.latitude,
          mobile_coords.longitude
          );
          
      stepper.moveTo(map(courseToMobile, 0, 360, 0, 3200));
   }
   stepper.run();
}
