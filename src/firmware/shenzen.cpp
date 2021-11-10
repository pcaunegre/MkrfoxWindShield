/*
*
*  Shenzen's device-specific functions
*
*/

#include <Arduino.h>
#include "shenzen.h"

volatile int            Shenzen_speed_cnt;  // numbre of anemometre turns

/*
 * Interface setup for Davis instrument 
 *
*/
void Shenzen_setup() {

  pinMode(ShenzenWire4, INPUT);
  pinMode(ShenzenWire1, OUTPUT);
  digitalWrite(ShenzenWire1,LOW);
  pinMode(ShenzenSpeedPin, INPUT_PULLUP);       

  analogReference(AR_DEFAULT);      // use internal ADC ref   
  attachInterrupt(digitalPinToInterrupt(ShenzenSpeedPin), Shenzen_isr_speed, FALLING); // interruption
  Shenzen_speed_cnt=0;
  
} 

/*
 * Interrupt service routine called when the rotating part of the wind instrument
 * has completed one rotation.
 *
*/
void Shenzen_isr_speed() {

  Shenzen_speed_cnt++;

}

/*
 * Capture a sample of wind speed
 * 
*/
int Shenzen_takeWspeed(int deltaT) {

  int wspeed = int((2400 * Shenzen_speed_cnt) / deltaT);   // mph= 2.4xpulse/Time                      // speed in km/h   
  Shenzen_speed_cnt=0;                                   // reset counter
  return(wspeed);

}

/*
 * Capture a sample of wind direction
 * Doc says wind vane can take up to 16 positions
 * But in practice, we only see 8 positions
*/

int Shenzen_takeWdir() {

  // max adcval is normally 944
  int val = analogRead(ShenzenDirPin);                     // read the input pin 
  
  //Serial.print("ADC = ");  Serial.println(val);

  int wdir = -1;
  if (val<75) {
    wdir = 112.5;
  } else if (val < 89)  {
    wdir = 67.5;
  } else if (val < 110)  {
    wdir = 90;
  } else if (val < 155)  {
    wdir = 157.5;
  } else if (val < 214)  {
    wdir = 135.5;   
  } else if (val < 266)  {
    wdir = 202.5;   
  } else if (val < 347)  {
    wdir = 180;   
  } else if (val < 434) {
    wdir = 22.5;   
  } else if (val < 530) {
    wdir = 45;   
  } else if (val < 615) {
    wdir = 247.5;   
  } else if (val < 666) {
    wdir = 225;   
  } else if (val < 744) {
    wdir = 337.5;   
  } else if (val < 806) {
    wdir = 360;   
  } else if (val < 857) {
    wdir = 292.5;   
  } else if (val < 915) {
    wdir = 315;   
  } else {
    wdir = 270; 
  }
  return(wdir);

}
