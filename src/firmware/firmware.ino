/*
*
*  Main program to control the KKRFOX 1200 board
*
*  Able to interface with Davis, Peet Bros or Shenzen device.
*
*/

#include <math.h>
#include "def.h"


#include <LiquidCrystal.h>

////LiquidCrystal lcd(9, 8, 5, 4, 3, 2);
LiquidCrystal lcd(13, 12, 7, 8, 9, 10);


int lcd_en=1;

volatile unsigned long  last_sampleT   ;  // last time in millis a measure sampling has been done
volatile unsigned long  last_reportT   ;  // last time in millis a report has been done

volatile int repnbr=0; // debug

int sensor=0;




void setup() {

  pinMode(Led,OUTPUT); // led used for debug or at power up
  Serial.begin(9600);           //  setup serial
  while (!Serial) {}            //  opening serial monitor is blocking
  Serial.println("STARTING");
  blinkLed(10,300); // say hello 10 flashes

  // device initialization
  if (sensor == 0) {
    delay(1000);
    detectSensorType();
  } 
  
  switch (sensor) {
    case DAVIS:
      blinkLed(1,1500); // say Davis detected
      Davis_setup();    break;
    case PEET:
      //Peet_setup();     break;
      blinkLed(2,1500); // say Peet detected
    case SHENZEN:
      Shenzen_setup();  break;
      blinkLed(3,1500); // say Shenzen detected
    default:
      // say detection failed: blink forever
      blinkLed(-1,800);
  } 


  if (lcd_en)  {
    lcd.begin(16,2);        // used when LCD is plugged for reading the device
    lcd.clear();
    lcd.print("Starting, sensor=");lcd.print(sensor);
    delay(1000);
  }
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
      last_sampleT = now;
        
      switch (sensor) {
        case DAVIS:
          ws=Davis_takeWspeed(dt1);
          wd=Davis_takeWdir();
          break;
        case PEET:
          
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
      
      Serial.println("M ");    // print directions
      Serial.println(ws);  // print speed
      Serial.print(wd); Serial.print(" ");   // print directions
      Serial.println(deg2dir(wd));    // print directions
       
      if (lcd_en)  {
        repnbr++;
        lcd.setCursor(0,0);
        lcd.print("                ");
        lcd.setCursor(0,0);
        lcd.print(repnbr);lcd.print(" ");lcd.print(deg2dir(wd));lcd.print(" ");lcd.print(wd);
        lcd.print(" ");lcd.print(ws);
      }
    } 
    // at every sigfox report period we send 2 packets of data
    // so at every half-report period we store data
    if (dt2 > REPORT_PERIOD/2) {
      //makeReport();
    }
  }  
}

/*
*
*  Report
*
*/
void makeReport() {
  Serial.print("Making report"); 

}

/*
*  Utility to find out which sensor is connected
*  period in millis
*
*/void blinkLed(int times, int period) {

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
  
  analogReference(AR_DEFAULT);
  Serial.print("ADCFS =");Serial.println(ADCFS);
  Serial.print("THR1 =");Serial.println(THR1);
  Serial.print("THR_Peet_low =");Serial.println(THR_Peet_low);
  Serial.print("THR_Peet_hi =");Serial.println(THR_Peet_hi); 
  Serial.print("THR_Shenzen_low =");Serial.println(THR_Shenzen_low);
  Serial.print("THR_Shenzen_hi =");Serial.println(THR_Shenzen_hi);
  Serial.print("THR_Davis_low =");Serial.println(THR_Davis_low);  
  Serial.print("THR_Davis_hi =");Serial.println(THR_Davis_hi); 

    
  // step 1 : D2R, D0R
  pinMode(2,INPUT_PULLUP); // install V+ through pull-up
  pinMode(0,INPUT);
  delay(100);
  int val1 = analogRead(A2);
  Serial.println("Initial analog 1 =");
  Serial.println(val1);
 
  // step 2 : D2R, D0W0
  pinMode(2,INPUT_PULLUP); // install V+ through pull-up
  pinMode(0,OUTPUT);
  digitalWrite(0,LOW);
  delay(100);
  int val2 = analogRead(A2);
  pinMode(0,INPUT_PULLUP);
  SerialDebug("Initial analog 2 == ", val2);
  

  if (val1 > THR1)  {
    // can be shenzen or peet
    if ((val2 > THR_Peet_low) && (val2 < THR_Peet_hi)) { 
      // we have peet sensor
      sensor=PEET;
      blinkLed(2,1000);
    } else if ((val2 > THR_Shenzen_low) && (val2 < THR_Shenzen_hi)) {
      // confirm we have shenzen sensor
      sensor=SHENZEN;
      blinkLed(3,1000);
    } else {
      sensor=-1;
      Serial.println("Detection issue between shenzen and peet");
    }
    
  } else if ((val1 > THR_Davis_low) && (val1 < THR_Davis_hi)) {
    // confirm we have Davis  
    if ((val2 > THR_Davis_low) && (val2 < THR_Davis_hi)) {
      // val2 should not differ from val1
      sensor=DAVIS;
      blinkLed(1,1000);
    } else {
      sensor=-1;
      Serial.println("Detection issue with Davis");
    }
  }

  Serial.println("Sensor detected =");
  Serial.println(sensor);
}


String deg2dir(int wd) {
    if      (wd <0)   return(String("-"));
    else if (wd <22)  return(String("N"));
    else if (wd <67)  return(String("NE"));
    else if (wd <112) return(String("E"));
    else if (wd <157) return(String("SE"));
    else if (wd <202) return(String("S"));
    else if (wd <247) return(String("SW"));
    else if (wd <292) return(String("W"));
    else if (wd <337) return(String("NW"));
    else              return(String("N"));    
}
