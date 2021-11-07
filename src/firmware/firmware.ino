/*
*
*  Main program to control the KKRFOX 1200 board
*
*  Able to interface with Davis, Peet Bros or Shenzen device.
*
*/

#include "def.h"
#include <math.h> 



#include <LiquidCrystal.h>

////LiquidCrystal lcd(9, 8, 5, 4, 3, 2);
LiquidCrystal lcd(13, 12, 7, 8, 9, 10);


int lcd_en=1;

volatile unsigned long  last_sampleT   ;  // last time in millis a measure sampling has been done
volatile unsigned long  last_reportT   ;  // last time in millis a report has been done


int sensor=DAVIS;

void blinkLed(int times, int period) {

  for (int i=0; i<times; i++ ) {
    digitalWrite(Led,HIGH);
    delay(period);
    digitalWrite(Led,LOW);
    delay(period);
  }
 
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


void setup() {

  if (sensor==DAVIS) {
    Davis_setup();
  }
  
  // pins to sensor
  pinMode(Led,OUTPUT); // led used for debug or at power up
  Serial.begin(9600);           //  setup serial

  blinkLed(5,600); // say hello
  if (lcd_en)  {
    lcd.begin(16,2);        // used when LCD is plugged for reading the device
    lcd.clear();
    lcd.print("SWiM started 2002");
  }
  
}


void loop() {
  
  unsigned long now = millis();
  int    dt1 = now - last_sampleT;                          // ellapsed time since last sample
  int    dt2 = now - last_reportT;  // ellapsed time since last report
  int    ws  = -1;
  int    wd  = -1;
  
  // if it's time to sample, get measures and store for stats
  if (dt1 > SAMPLING_PERIOD) {
    last_sampleT = now;
      
    if (sensor==DAVIS) {  
      ws=Davis_takeWspeed(dt1);
      wd=Davis_takeWdir();
      Serial.print(wd);    // print directions
      Serial.println(ws);    // print directions
    }
     
    if (lcd_en)  {
      lcd.setCursor(0,0);
      lcd.print("                ");
      lcd.setCursor(0,0);
      lcd.print(deg2dir(wd));lcd.print(" ");lcd.print(wd);
      lcd.print(" ");lcd.print(ws);
    }
  } 
  // at every sigfox report period we send 2 packets of data
  // so at every half-report period we store data
  if (dt2 > REPORT_PERIOD/2) {
    makeReport();
  }

}

void makeReport() {
  Serial.print("Making report"); 

}
