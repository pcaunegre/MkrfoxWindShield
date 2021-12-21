/*
*
*  Davis' device-specific functions
*
*/

#include <Arduino.h>


#define DavisWire1  0    // wire1 (Black) to digital 0
//      DavisWire2       // wire2 (Red) to GND
#define DavisWire3  1    // wire3 (Green) to digital 1
#define DavisWire4  2    // wire4 (Yellow) to digital 2


#define DavisDirPin   A1  // analog direction - green wire
#define DavisSpeedPin 0   // dig speed on D0  - black wire



const int DavisDirFullScale=int(ADCFS*RDAVISPOT/(RPULLUP+RDAVISPOT));

volatile int            Davis_speed_cnt;  // numbre of anemometre turns


/*
 * Interface setup for Davis instrument 
 *
*/
void Davis_setup() {

  pinMode(DavisWire3, INPUT);  // not used    
  pinMode(DavisWire4, INPUT_PULLUP);  // powering the dir potentiometer      
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

  float mph = 2250 * (float)(Davis_speed_cnt) / (float)(deltaT * cpudiv); // mph= 2.25xpulse/Time
  int wspeed = int(1.609344 * mph);                         // speed in km/h 
  Davis_speed_cnt=0;                                        // reset counter
  return(wspeed);

}

/*
 * Capture a sample of wind direction
 * 
*/
int Davis_takeWdir() {

  pinMode(SENSPPIN, OUTPUT);
  digitalWrite(SENSPPIN,HIGH);
  delay(1); // settling 1ms since time constant = 100us
  int adcval = analogRead(DavisDirPin);                     // read the input pin

  digitalWrite(SENSPPIN,LOW);       // cut to save power (0.1mA)
  int wdir = int(360 * adcval / DavisDirFullScale);         // wind angle
  return(wdir);

}
