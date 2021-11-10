/*
*
*  Davis' device-specific functions
*
*/

#include <Arduino.h>
#include "davis.h"

volatile int            Davis_speed_cnt;  // numbre of anemometre turns

/*
 * Interface setup for Davis instrument 
 *
*/
void Davis_setup() {

  pinMode(DavisWire3, INPUT);  // not used    
  pinMode(DavisWire4, INPUT);  // not used      

  pinMode(DavisSpeedPin, INPUT_PULLUP);      
  analogReference(AR_DEFAULT);      // use internal ADC ref   
  attachInterrupt(digitalPinToInterrupt(DavisSpeedPin), Davis_isr_speed, FALLING); // interruption
  Davis_speed_cnt=0;
  
} 

/*
 * Interrupt service routine called when the rotating part of the wind instrument
 * has completed one rotation.
 *
*/
void Davis_isr_speed() {

  Davis_speed_cnt++;

}

/*
 * Capture a sample of wind speed
 * 
*/
int Davis_takeWspeed(int deltaT) {

  float mph = 2250 * (float)(Davis_speed_cnt) / (float)deltaT; // mph= 2.25xpulse/Time
  int wspeed = int(1.609344 * mph);                         // speed in km/h   
  Davis_speed_cnt=0;                                        // reset counter
  return(wspeed);

}

/*
 * Capture a sample of wind direction
 * 
*/
int Davis_takeWdir() {

  int adcval = analogRead(DavisDirPin);                     // read the input pin
  int wdir = int(360 * adcval / DavisDirFullScale);         // wind angle
  return(wdir);

}
