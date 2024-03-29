#include <JeeLib.h>
#include "Wire.h"
// requires I2Cdev library: https://github.com/jrowberg/i2cdevlib
#include "I2Cdev.h"

// requires MPU-6050 part of the I2Cdev lib: https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050
#include "MPU6050.h"
typedef float * pFloat; // define pInt as a pointer to an int
//float smoothval;

int ledPins[] = { 
 4,14,5,15,6,16,7 };       // an array of pin numbers to which LEDs are attached
int pinCount = 7;           // the number of pins (i.e. the length of the array)

int ii;
// sensor
MPU6050 sensor;
int16_t ax, ay, az;
int16_t gx, gy, gz;
float gain;

// filtering
float v[9];
float currval, maxval, smoothval, mag;
uint32_t lastmax;


void setup() {
  Wire.begin();  
  rf12_initialize(1, RF12_868MHZ, 33);  
  Serial.begin(57600);
     //LEDS
  for (int thisPin = 0; thisPin < pinCount; thisPin++)  {
    pinMode(ledPins[thisPin], OUTPUT);      
  }
  
  
  // initialize the filter
  for (ii=0; ii<9; ii++)
    v[ii] = 0.0;
  currval = 0.0;
  maxval = 0.0;
  smoothval = 0.0;
  lastmax = millis();

  // set up the MPU
  sensor.initialize();

  gain = 4.0; // higher -> more sensitive

}

void loop() {
   
 
 
  
 
  if (rf12_recvDone()) {
        // something has been received
        if (rf12_crc == 0) {
            // CRC is 0, no errors

            // read from right to left:
            // interpret rf12_data as pointer to int,
            // and assign the thing it points to (an int)
            // to angle
           float smoothval = *(pFloat)rf12_data;

            Serial.println(smoothval);
              // loop from the lowest pin to the highest:
  for (int thisPin = 0; thisPin < pinCount; thisPin++) { 


    if (smoothval*gain > (thisPin+1)/(pinCount+1.0))
         // turn the pin on:
    digitalWrite(ledPins[thisPin], HIGH); 
    //two.digiWrite(HIGH);  // led 1 blinks every 2x 0.16 sec
    else
      // turn the pin off:
    digitalWrite(ledPins[thisPin], LOW);   
  }
        }
    }
}
// given a new value x, step the filter forward and return the newest filtered value
// code generated by http://www.schwietering.com/jayduino/filtuino/
// 4th order Butterwoth filter centered on 13 Hz
float step_filter (float x)
{
  v[0] = v[1];
  v[1] = v[2];
  v[2] = v[3];
  v[3] = v[4];
  v[4] = v[5];
  v[5] = v[6];
  v[6] = v[7];
  v[7] = v[8];
  v[8] = (4.392209311882e-5 * x)
    + ( -0.6378838221 * v[0])
      + (  5.1150034552 * v[1])
        + (-18.2346630494 * v[2])
          + ( 37.7215398561 * v[3])
            + (-49.5091483922 * v[4])
              + ( 42.2118284653 * v[5])
                + (-22.8337680359 * v[6])
                  + (  7.1670057953 * v[7]);

  return (v[0] + v[8]) - 4*(v[2] + v[6]) + 6*v[4];
}
