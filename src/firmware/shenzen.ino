/*
*
*  Shenzen's device-specific functions
*
*/

#include <Arduino.h>


#define ShenzenWire1  0    // wire1 (Black) to digital 0
//      ShenzenWire2       // wire2 (Red) to GND
#define ShenzenWire3  1    // wire3 (Yellow) to digital 1
#define ShenzenWire4  2    // wire4 (Green) to digital 2


#define ShenzenDirPin   A2  // analog direction - wire4 (Green)
#define ShenzenSpeedPin 1   // dig speed on D1  - wire3 (Yellow)



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

  int wdir = UNDEFINED;
  int val = analogRead(ShenzenDirPin);                     // read the input pin 
  
  //Serial.print("ADC = ");  Serial.println(val);
  
  // calibration originally done with 10bits, but still parametrizable
  if (val<171*ADCFS/1023) {
    wdir = SHORT_ERR;   // Value not possible => may be a short circuit
  } else if (val < 223*ADCFS/1023) {
    wdir = 112.5;
  } else if (val < 233*ADCFS/1023) {
    wdir = 67.5;
  } else if (val < 248*ADCFS/1023) {
    wdir = 90;
  } else if (val < 282*ADCFS/1023) {
    wdir = 157.5;
  } else if (val < 325*ADCFS/1023) {
    wdir = 135.5;   
  } else if (val < 364*ADCFS/1023) {
    wdir = 202.5;   
  } else if (val < 426*ADCFS/1023) {
    wdir = 180;   
  } else if (val < 495*ADCFS/1023) {
    wdir = 22.5;   
  } else if (val < 574*ADCFS/1023) {
    wdir = 45;   
  } else if (val < 645*ADCFS/1023) {
    wdir = 247.5;   
  } else if (val < 690*ADCFS/1023) {
    wdir = 225;   
  } else if (val < 759*ADCFS/1023) {
    wdir = 337.5;   
  } else if (val < 815*ADCFS/1023) {
    wdir = 360;   
  } else if (val < 862*ADCFS/1023) {
    wdir = 292.5;   
  } else if (val < 918*ADCFS/1023) {
    wdir = 315;   
  } else if (val < 949*ADCFS/1023) {
    wdir = 270; 
  } else {
    wdir = OPEN_ERR; // Value not possible => may be a open circuit
  }
  return(wdir);

}
