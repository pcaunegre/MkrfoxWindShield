/*
*
*  Main program to control the KKRFOX 1200 board
*
*  Able to interface with Davis, Peet Bros or Shenzen device.
*
*/

#include <SigFox.h>
#include <math.h>
#include "def.h"

#include <LiquidCrystal.h>


LiquidCrystal lcd(13, 12, 7, 8, 9, 10); //lcd(rs, en, a, b, c, d)


volatile unsigned long  last_sampleT   ;  // last time in millis a measure sampling has been done
volatile unsigned long  last_reportT   ;  // last time in millis a report has been done

volatile int            max_wspeed    ;  // max wind speed over the sample period
volatile int            min_wspeed    ;  // min wind speed over the sample period
volatile int            cnt_ws_samples;  // count the number of samples we have to compute speed avg
volatile int            cnt_wd_samples;  // count the number of samples we have to compute dir avg
volatile int            acc_wspeed    ;  // wind speed accumulator to compute avg
volatile float          acc_wdX       ;  // accumulate wind dir projection on X axis (cos)
volatile float          acc_wdY       ;  // accumulate wind dir projection on X axis (sin)
volatile int            prevWindDir   ;  // memorize the last dir in case we cannot compute new one (speed too low)
volatile int            statReportCnt ;  // counter : every 2 hits data are sent thru sigfox 
volatile SigfoxWindMessage msg        ;  // create an instance of the struct to receive the wind data frame

volatile bool           debugmode = true ;  // 1=debug mode
volatile bool           lcd_en    = true ;  // lcd plugged or not
volatile int            repnbr=0;           // debug
volatile int            msnbr=0;            // debug

int sensor=0;          // sensor number




void setup() {

  pinMode(Led,OUTPUT); // led used for debug or at power up
  blinkLed(10,400); // say hello 10 flashes

  // device detection at boot
  if (sensor == 0) {
    delay(3000);
    detectSensorType();
  } 
  
  // device initialization
  switch (sensor) {
    case DAVIS:
      Davis_setup();    break;
    case PEET:
      Peet_setup();     break;
    case SHENZEN:
      Shenzen_setup();  break;
    default:
      // say detection failed: blink forever
      blinkLed(-1,800);
  } 

  statReportCnt    = 0;
  prevWindDir      = -1;
  repnbr           = 0;
  last_sampleT     = millis();
  last_reportT     = millis(); 
  reset_stat(); 
  delay(1000);
}


void loop() {
  
  if (sensor>0) {
    
    // normal operation
    unsigned long now = millis();
    int    dt1 = now - last_sampleT;  // ellapsed time since last sample
    int    dt2 = now - last_reportT;  // ellapsed time since last report
    int    ws  = -1;
    int    wd  = -1;
    
    // if it's time to sample, get measures and store for stats
    if (dt1 > SAMPLING_PERIOD) {
      noInterrupts();
      switch (sensor) {
        case DAVIS:
          ws=Davis_takeWspeed(dt1);
          wd=Davis_takeWdir();
          break;
        case PEET:
          ws=Peet_takeWspeed();
          wd=Peet_takeWdir();
          Peet_resetSampler();       
          break;
        case SHENZEN:
          ws=Shenzen_takeWspeed(dt1);
          wd=Shenzen_takeWdir();        
          break;
        default:
          // NOT initialized: run init procedure
          // TBD
          break;
      }
      store_for_stat(ws,wd); // storing for 
      debugPrintMeasure(ws,wd);         
      interrupts();
      last_sampleT = now;
    }
    
    // at every sigfox report period we send 2 packets of data
    // so at every half-report period we store data
    if (dt2 > REPORT_PERIOD/2) {
      noInterrupts();
      makeReport();
      interrupts();
      last_reportT = now;
    }
  }  
}

/*
 * Take measures and compute some statistics over a report period
 * min, max, accumulate for average, mean wind dir
 * 
*/
void store_for_stat(int ws, int wd)  {
  // wind speed in km/h
  // wind dir in degrees
  // -1 means meas not valid
  
  if (ws>-1)  {
    if (ws > max_wspeed)  max_wspeed = ws;
    if (ws < min_wspeed)  min_wspeed = ws;
    acc_wspeed += ws;
    cnt_ws_samples++;
  }
  
  if (wd>-1)  {
    // projection of direction is weighted by the speed
    acc_wdX += ws * cos(PI*wd/180.);
    acc_wdY += ws * sin(PI*wd/180.);
    cnt_wd_samples++;
  }

}

/*
 * Resets all counters and accumulator when a report has been done
 * min, max, accumulate for average, mean wind dir
 * 
*/
void reset_stat()  {
  max_wspeed    = 0  ;
  min_wspeed    = 999;
  acc_wspeed    = 0  ;
  cnt_ws_samples= 0  ;
  cnt_wd_samples= 0  ;
  acc_wdX       = 0.0;
  acc_wdY       = 0.0;
}

/*
 * Computes avg wind speed
*/
int wspeed_avg()  {
    return(int(acc_wspeed/cnt_ws_samples));
}

/*
 * Computes avg wind direction
*/
int wdir_avg()  {
    
    int avgdir;
    // when no dir samples, keep the previous direction
    if (!cnt_wd_samples) return(prevWindDir);

    if (acc_wdY==0.0 && acc_wdX==0.0)  {
      // when speed is null over the whole period, we cannot compute the avg direction
      avgdir=prevWindDir;
    } else {
      avgdir = int(atan2(acc_wdY,acc_wdX)*180/PI);
      if (avgdir<0)  avgdir += 360;
      prevWindDir = avgdir;   // store this in a globvar
    }  
    return(avgdir);
    
}


/*
*
*  Report
*
*/
void makeReport() {

  int avws, avwd;
    
  // report the values
  avws = wspeed_avg();
  avwd = wdir_avg();
  debugPrintAvgMeas(avws,avwd); // only for reading through lcd display or via usb 
  
  // store in msg structure with OpenWindMap-expected data encoding
  msg.speedMin[statReportCnt]     = encodeWindSpeed(min_wspeed);
  msg.speedAvg[statReportCnt]     = encodeWindSpeed(avws);
  msg.speedMax[statReportCnt]     = encodeWindSpeed(max_wspeed);
  msg.directionAvg[statReportCnt] = encodeWindDirection(avwd);  

  // we send telegram half the time
  if (statReportCnt==1) {
    // send sigfox telegram this time
    sendSigFoxMessage();
    statReportCnt=0;
  } else {
    // sigfox telegram will be next turn
    statReportCnt=1;
  }
  
  // clear stats
  reset_stat();

}

/*
*  Utility to find out which sensor is connected
*  period in millis
*
*/
void blinkLed(int times, int period) {

  for (int i=0; i<times; i++ ) {
    digitalWrite(Led,HIGH);
    delay(period);
    digitalWrite(Led,LOW);
    delay(period);
  }
 
}

/*
*
*  Utility to find out which sensor is connected
* 
*  We power up the lines through 2 configurations and measure the analog value on A2
*  Based on the resistor dividers, tolerances, we have thresholds to identify the sensor.
*
*/
void detectSensorType() {
  
  String msg="";
  analogReference(AR_DEFAULT);
    
  // step 1 : D2R, D0R
  pinMode(2,INPUT); // install V+ through pull-up
  pinMode(0,INPUT);
  delay(100);
  int val1 = analogRead(A2);
 
  // step 2 : D2R, D0W0
  pinMode(2,INPUT); // install V+ through pull-up
  pinMode(0,OUTPUT);
  digitalWrite(0,LOW);
  delay(100);
  int val2 = analogRead(A2);
  pinMode(0,INPUT_PULLUP);

  if (val1 > THR1)  {
    // can be shenzen or peet
    if ((val2 > THR_Peet_low) && (val2 < THR_Peet_hi)) { 
      // we have peet sensor
      sensor=PEET;
      blinkLed(2,1500); // say Peet detected
    } else if ((val2 > THR_Shenzen_low) && (val2 < THR_Shenzen_hi)) {
      // confirm we have shenzen sensor
      sensor=SHENZEN;
      blinkLed(3,1500); // say Shenzen detected
    } else {
      sensor=-1;
      msg="Detection issue between shenzen and peet";
    }
    
  } else if ((val1 > THR_Davis_low) && (val1 < THR_Davis_hi)) {
    // confirm we have Davis  
    if ((val2 > THR_Davis_low) && (val2 < THR_Davis_hi)) {
      // val2 should not differ from val1
      sensor=DAVIS;
      blinkLed(1,1500); // say Davis detected
    } else {
      sensor=-1;
      msg="Detection issue with Davis";
    }
  } else {
    sensor=-1;
    msg="Sensor Detection issue";
  }
  debugSensorDetection(msg,sensor,val1,val2);

}

/*
 * functions dedicated to Sigfox message sending
 * 
 *speedMin[0], speedMin[1], speedAvg[0], speedAvg[1], speedMax[0], speedMax[1], dirAvg[0], dirAvg[1]
*/
void sendSigFoxMessage() {
  
  debugPrint("Sending Sigfox Msg",0);
  return;
  
  // Start the module  
  delay(10);
  SigFox.begin();
//   if (!SigFox.begin()) {
//     Serial.println("SigFox error, rebooting");
//     reboot();
//   }
  // Wait at least 30mS after first configuration (100mS before)
  delay(100);
  SigFox.debug();

  // Clears all pending interrupts
  SigFox.status();
  delay(1);
  SigFox.beginPacket();
  
  // OpenWindMap specific data frame  
  SigFox.write((uint8_t*)&msg, sizeof(SigfoxWindMessage));
  
  int ret = SigFox.endPacket();
  SigFox.end();

}
