/*
*
*  Main program to control the KKRFOX 1200 board
*
*  Able to interface with Davis, Peet Bros or Shenzen device.
*
*
*
*/

#include <SigFox.h>
#include <math.h>
#include "def.h"

#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 7, 8, 9, 10); //lcd(rs, en, a, b, c, d)


volatile unsigned long  last_sampleT  ;  // last time in millis a measure sampling has been done
volatile unsigned long  last_reportT  ;  // last time in millis a report has been done
volatile unsigned long  last_adminT   ;  // last time in millis a vbat report has been done

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

volatile int            cpudiv        ;  // value of cpu freq divisor

volatile bool           debugmode = false ;  // 1=debug mode
volatile bool           sigfox_en = true ;  // enable sigfox
volatile bool           lcd_en    = false ;  // lcd plugged or not
volatile int            repnbr=0;           // debug
volatile int            msnbr=0;            // debug

int sensor=0;          // sensor number




void setup() {
  
  int blinknbr=SOFTVERSION;
  // a jumper between 14 and GND will disable sigfox
  pinMode(14,INPUT_PULLUP); 
  sigfox_en = digitalRead(14);

  // a jumper between 11 and GND will put debug mode and disable sigfox
  pinMode(11,INPUT_PULLUP); 
  if (digitalRead(11) == 0) {
    debugmode=true; blinknbr=15;
  }

  cpudiv = CPU_FULL;
  pinMode(Led,OUTPUT); // led used for debug or at power up
  debugInit(sigfox_en,debugmode);
  blinkLed(blinknbr,400/cpudiv); // say hello 10 flashes
  
  // getBatterieVoltage
  float vbat=getBatteryVoltage();
  float   tp=getTemperature();
  
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
      // say detection failed: blink 1000 times and reboot
      blinkLed(1000,500/cpudiv);
      reboot();
  }   
  makeAdminReport(vbat,tp);
  statReportCnt    = 0;
  prevWindDir      = -1;
  repnbr           = 0;
  last_sampleT     = millis();
  last_reportT     = millis();
  last_adminT      = millis();
  reset_stat(); 
  if (!debugmode) set_cpu_speed(CPU_SLOW);
}


void loop() {

  if (sensor>0) {
    
    // normal operation
    unsigned long now = millis();
    int    dt1 = now - last_sampleT;  // ellapsed time since last sample
    int    dt2 = now - last_reportT;  // ellapsed time since last report
    int    dt3 = now - last_adminT;   // ellapsed time since last vbat report
    int    ws  = -1;
    int    wd  = -1;
    
    // if it's time to sample, get measures and store for stats
    if (dt1 > SAMPLING_PERIOD/cpudiv) {
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
      last_sampleT = now;
      interrupts();
    }
    
    if (dt3 > ADMIN_REPORT_PERIOD/cpudiv) {
      noInterrupts();
      float vbat=getBatteryVoltage();
      float   tp=getTemperature();
      makeAdminReport(vbat,tp);
      last_adminT = now;
      last_reportT = now;
      interrupts();
    } else if (dt2 > (REPORT_PERIOD/2)/cpudiv) {
      // at every sigfox report period we send 2 packets of data
      // so at every half-report period we store data     
      noInterrupts();
      makeReport();
      last_reportT = now;
      interrupts();
    }

    if (now > REBOOT_PERIOD/cpudiv) reboot(); // avoid managing millis value wrapping (every 2**32-1 ms)
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
  
    if (wd>-1)  {
      // projection of direction is weighted by the speed
      acc_wdX += ws * cos(PI*wd/180.);
      acc_wdY += ws * sin(PI*wd/180.);
      cnt_wd_samples++;
    }
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
    debugPrint("sum WS =",acc_wspeed);
    debugPrint("cnt WS =",cnt_ws_samples);
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
*  Data Report creation/dispach
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
    sendSigFoxMessage(8);
    statReportCnt=0;
  } else {
    // sigfox telegram will be next turn
    statReportCnt=1;
  }
  
  // clear stats
  reset_stat();

}

/*
*  Admin report 
*
*/
void makeAdminReport(float vb, float tp) {

  msg.batVolt     = encodeVoltage(vb);
  msg.temperature = encodeTemperature(tp);
  msg.sensor      = sensor;
  msg.softversion = SOFTVERSION;
  // send sigfox telegram
  sendSigFoxMessage(12);
  debugPrint("Admin RPT",sensor);

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
 * system functions
*/
void reboot() {
  NVIC_SystemReset();
  while (1);
}

/*
 * CPU underclocking: clock is divided by divisor
*/
void set_cpu_speed(int divisor){
  cpudiv=divisor;
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(divisor) |         // Divide the 48MHz clock source by divisor 48: 48MHz/48=1MHz
                   GCLK_GENDIV_ID(0);            // Select Generic Clock (GCLK) 0
  while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization      
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
  analogReadResolution(ADCBITS);
  analogReference(AR_DEFAULT);
  pinMode(SENSPPIN,OUTPUT); // sensor potentiommeters are powered by this pin
  digitalWrite(SENSPPIN,HIGH);
    
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
  digitalWrite(SENSPPIN,LOW); //stop powering sensor

  if (val1 > THR1)  {
    // can be shenzen or peet
    if (val2 > THR1) { 
      // we have peet sensor
      sensor=PEET;
      blinkLed(sensor/10,1500/cpudiv); // say Peet detected
    } else if ((val2 > THR_Shenzen_low) && (val2 < THR_Shenzen_hi)) {
      // confirm we have shenzen sensor
      sensor=SHENZEN;
      blinkLed(sensor/10,1500/cpudiv); // say Shenzen detected
    } else if (val2 < THR_Peet_hi) {
      // Peet when dir switch is closed
      sensor=PEET;
      blinkLed(sensor/10,1500/cpudiv); // say Peet detected
    } else {
      sensor=-1;
      msg="Detection issue between shenzen and peet";
    }
    
  } else if ((val1 > THR_Davis_low) && (val1 < THR_Davis_hi)) {
    // confirm we have Davis  
    if ((val2 > THR_Davis_low) && (val2 < THR_Davis_hi)) {
      // val2 should not differ from val1
      sensor=DAVIS;
      blinkLed(1,1500/cpudiv); // say Davis detected
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
*  Utility to measure Battery voltage, return mV
*/
float getBatteryVoltage() {

  analogReadResolution(ADCBITS);
  analogReference(AR_INTERNAL1V0);
  delay(100);
  float vb=(analogRead(A3)/(ADCFS*VBDIV))*1000.0; // read vbat through k~1/5 divider so v=adc/(k*adcfs)
  debugPrintVbat(vb);
  analogReference(AR_DEFAULT);  // restore to the default ref used in other parts of the code
  delay(100);
  return(vb);

}

/*
*  Utility to measure temperature
*/
float getTemperature() {
  
  analogReadResolution(12);      
  analogReference(AR_DEFAULT); // to give a ratio of the resistor divider
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);
  delay(100);
  int adc=analogRead(A4);
  
  analogReadResolution(ADCBITS);
  digitalWrite(4,LOW);
  delay(100);
  
  float tp=25+(1.0-1.0/((4095.0/adc)-1))/0.04; // 0.04 is NTC sensitivity 4%/C, roughly around 25C
  debugPrintTemp(tp);
  return(tp);

}


/*
 * functions dedicated to Sigfox message sending
 * 
 *speedMin[0], speedMin[1], speedAvg[0], speedAvg[1], speedMax[0], speedMax[1], dirAvg[0], dirAvg[1]
 *
 * message len=8 for a normal measure transmission, 12 for an admin transmission
*/
void sendSigFoxMessage(int len) {
    
  if (sigfox_en) {
    debugPrint("SFX Msg ",len);
    set_cpu_speed(CPU_FULL);
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
    SigFox.write((uint8_t*)&msg, len); 
    
    int ret = SigFox.endPacket();
    SigFox.end();
    if (!debugmode) set_cpu_speed(CPU_SLOW);
  } else {
    debugPrint("Sigfox disabled ",len);
  }

}
