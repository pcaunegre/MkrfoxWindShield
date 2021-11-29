/*
*
*  Peet's device-specific functions
*
*/

#include <Arduino.h>


#define PeetWire1  0    // wire1 (Black) to digital 0
//      PeetWire2       // wire2 (Red) to GND
#define PeetWire3  1    // wire3 (Green) to digital 1
#define PeetWire4  2    // wire4 (Yellow) to digital 2


#define PeetDirPin   1  // dig pulse on D1 for direction - green wire
#define PeetSpeedPin 2  // dig speed on D2  - yellow wire



volatile int            Peet_speed_cnt;  // number of anemometre turns
volatile int            speedTimeArray[ARRAYLEN];  // array to store times of speed sensor pulses
volatile int            Peet_speedHitCnt;          // counter of speed pulses
volatile int            dirTimeArray[ARRAYLEN];    // array to store times of dir sensor pulses
volatile int            Peet_dirHitCnt;            // counter of dir pulses




/*
 * Interface setup for Davis instrument 
 *
*/
void Peet_setup() {

  pinMode(PeetWire1, OUTPUT); 
  pinMode(PeetWire3, INPUT_PULLUP); 
  pinMode(PeetWire4, INPUT); // pull up is external
  pinMode(SENSPPIN, OUTPUT);
  digitalWrite(SENSPPIN,HIGH);

  attachInterrupt(digitalPinToInterrupt(PeetSpeedPin), Peet_isr_speed, FALLING); // interruption
  attachInterrupt(digitalPinToInterrupt(PeetDirPin), Peet_isr_direction, FALLING); // interruption
  
  Peet_speedHitCnt      = 0;
  Peet_dirHitCnt        = 0;
  Peet_clearArrays();
  
}

/*
 * Interrupt service routine called when the rotating part of the wind instrument
 * has completed one rotation.
 *
*/
void Peet_isr_speed() {
  
  int now = millis();
  speedTimeArray[Peet_speedHitCnt] = now-last_sampleT; // store relative time wrt last sample time
  Peet_speedHitCnt++;

}

/*
 * Interrupt service routine called when the direction reed sensor triggers.
 *
*/
void Peet_isr_direction() {
  
  int now = millis();
  dirTimeArray[Peet_dirHitCnt] = now-last_sampleT;
  Peet_dirHitCnt++;

}

// /*
//  * Compute the wind speed and dir over the sample period.
//  *
//  * update the ws and wd global vars
// */
int Peet_takeWspeed() {
  
  float mph;
   
  // not enough pulses to compute wind
  if (Peet_speedHitCnt<2) return(-1);
  
  // rotation per seconds
  int dt = (speedTimeArray[Peet_speedHitCnt-1]-speedTimeArray[0]);
  float rps = 1000.0 * (float)(Peet_speedHitCnt-1) / (float)dt;
  
  // calibration formulas given by Peet Bros sensor vendor
  // calibration formulas given by Peet Bros sensor vendor
  if      (rps < 0.010)  mph = 0.0;
  else if (rps < 3.229)  mph = -0.1095*rps*rps + 2.9318*rps - 0.1412;
  else if (rps < 54.362) mph =  0.0052*rps*rps + 2.1980*rps + 1.1091; // 54rps is 216km/h !
  // useless: the sensor will be destroyed before this wind speed !
  //else if (rps < 66.332) mph =  0.1104*rps*rps - 9.5685*rps + 329.87;
  
  int kmh = int(mph * 1.609344);  // be aware the sensor doc is wrong on that conversion !
  return(kmh);
}


/*
 * Compute the wind dir over the sample period.
 *
*/
int Peet_takeWdir() {

  int   wdir=-1;         // wind dir in degree to return
  int   dp;              // time of wind dir pulse
  float acc_ratio = 0.0; // sum of dir/speed times ratio to average over the sampling period
  int   hitCnt      = 0; // nbr of elements in the average calc
  
  // not enough pulses to compute wind
  if (Peet_speedHitCnt<2) return(wdir);
  
  for (int i = 0; i < Peet_speedHitCnt-1; i++) {
    // look for a dir pulse comprised between two bounds of speed pulses
    dp=searchElem(speedTimeArray[i],speedTimeArray[i+1]);
  
    if (dp>0) {
        acc_ratio += (float)(dp - speedTimeArray[i]) / (float)(speedTimeArray[i+1]-speedTimeArray[i]);
        hitCnt++;
    }
  }
  if (!hitCnt) return(wdir); // not possible to compute wind direction
  
  wdir = int(360 * acc_ratio / hitCnt);

  return(wdir);
}

/*
 * Accessory to search in the Dir pulse times array
 * a value between LL and UL
 * 
*/
int searchElem(int LL, int UL) {

  for (int i=0; i<Peet_dirHitCnt; i++ ) {
    if (dirTimeArray[i]> UL)  return(0); // 
    if ((dirTimeArray[i] >= LL) && (dirTimeArray[i] <= UL)) {
        return(dirTimeArray[i]);
    }
  }
  return(0);
}

/*
 * Accessory to reset the arrays.
 *
*/
void Peet_clearArrays() {
  for (int i = 0; i < ARRAYLEN; i++) {
    speedTimeArray[i]=0;
    dirTimeArray[i]=0;
  }
}

/*
 * Accessory to reset the sampler variables.
 *
*/
void Peet_resetSampler() {

  Peet_clearArrays();
  Peet_speedHitCnt = 0;
  Peet_dirHitCnt   = 0;
    
}
