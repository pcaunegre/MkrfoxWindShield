/*
*
*  Main program to control the KKRFOX 1200 board
*
*  Able to interface with Davis, Peet Bros or Shenzen device.
*
*/

#include <math.h>
#include "def.h"
#include "davis.h"
#include "shenzen.h"

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
  while (!Serial) {}
  Serial.println("STARTING");
  blinkLed(10,300); // say hello 10 flashes

  // device initialization
  if (sensor == 0) {
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
  
}


void loop() {
  
    
//     // normal operation
//     unsigned long now = millis();
//     int    dt1 = now - last_sampleT;  // ellapsed time since last sample
//     int    dt2 = now - last_reportT;  // ellapsed time since last report
//     int    ws  = -1;
//     int    wd  = -1;
//     
//     // if it's time to sample, get measures and store for stats
//     if (dt1 > SAMPLING_PERIOD) {
//       last_sampleT = now;
//         
//       switch (sensor) {
//         case DAVIS:
//           ws=Davis_takeWspeed(dt1);
//           wd=Davis_takeWdir();
//           break;
//         case PEET:
//           
//           break;
//         case SHENZEN:
//           ws=Shenzen_takeWspeed(dt1);
//           wd=Shenzen_takeWdir();        
//           break;
//         default:
//           // NOT initialized: run init procedure
//           // TBD
//           break;
//       }
//       
//       Serial.println("M ");    // print directions
//       Serial.println(ws);  // print speed
//       Serial.print(wd); Serial.print(" ");   // print directions
//       Serial.println(deg2dir(wd));    // print directions
//        
//       if (lcd_en)  {
//         repnbr++;
//         lcd.setCursor(0,0);
//         lcd.print("                ");
//         lcd.setCursor(0,0);
//         lcd.print(repnbr);lcd.print(" ");lcd.print(deg2dir(wd));lcd.print(" ");lcd.print(wd);
//         lcd.print(" ");lcd.print(ws);
//       }
//     } 
//     // at every sigfox report period we send 2 packets of data
//     // so at every half-report period we store data
//     if (dt2 > REPORT_PERIOD/2) {
//       //makeReport();
//     }

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
*/
void detectSensorType() {
  
  analogReference(AR_DEFAULT);
  
  // step 1 : D2R, D0R
  pinMode(2,INPUT_PULLUP); // install V+ through pull-up
  pinMode(0,INPUT_PULLUP);
  int val1 = analogRead(A2);
  Serial.println("Initial analog 1 =");
  Serial.println(val1);
 
  // step 2 : D2R, D0W0
  pinMode(2,INPUT_PULLUP); // install V+ through pull-up
  pinMode(0,OUTPUT);
  digitalWrite(0,LOW);
  delay(1);
  int val2 = analogRead(A2);
  pinMode(0,INPUT_PULLUP);
  Serial.println("Initial analog 2 =");
  Serial.println(val2);

//////// TODO: replace constant by values computed in def.h
  if (val1 > 1000)  {
    // can be shenzen or peet
    if ((val2 > 150) && (val2 < 193)) { 
      // we have peet sensor
      sensor=PEET;
    } else if ((val2 > 196) && (val2 < 958)) {
      // confirm we have shenzen sensor
      sensor=SHENZEN;
    } else {
      sensor=-1;
      Serial.println("Detection issue between shenzen and peet");
    }
    
  } else if ((val1 > 660) && (val1 < 726)) {
    // confirm we have Davis  
    if ((val2 > 660) && (val2 < 726)) {
      // val2 should not differ from val1
      sensor=DAVIS;
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
