/*
 * Utilities for debug and print purposes
 * 
 */
void debugInit() {
  
  if (!debugmode) {return;}
  Serial.begin(9600);           //  setup serial  
  if (Serial) {
    Serial.println("STARTING");
  }
  if (lcd_en)  {
    lcd.begin(16,2);        // used when LCD is plugged for reading the device
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,0);
  }
}


void debugSensorDetection(String msg, int sensor, int val1, int val2) {

  if (!debugmode) {return;}
  if (Serial) {
    Serial.print("ADCFS=");Serial.println(ADCFS);
    Serial.print("THR1=");Serial.println(THR1);
    Serial.print("THR_Peet_low=");Serial.println(THR_Peet_low);
    Serial.print("THR_Peet_hi=");Serial.println(THR_Peet_hi); 
    Serial.print("THR_Shenzen_low=");Serial.println(THR_Shenzen_low);
    Serial.print("THR_Shenzen_hi=");Serial.println(THR_Shenzen_hi);
    Serial.print("THR_Davis_low=");Serial.println(THR_Davis_low);  
    Serial.print("THR_Davis_hi=");Serial.println(THR_Davis_hi); 
    Serial.print("Sensor detection, val1="); Serial.print(val1);
    Serial.print(" val2="); Serial.print(val2);
    Serial.print(" Detected="); Serial.println(sensor);
    Serial.println(msg);
  }
  if (lcd_en)  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Started sens=");lcd.print(sensor);
  }

}
  
void debugPrint(char* mystr, int mypar) {
  
  if (!debugmode) {return;}
  if (Serial) {
    Serial.print(mystr); Serial.println(mypar);
  }
  if (lcd_en)  {
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print(mystr);lcd.print(mypar,3);
  }

}
void debugPrintVbat(float v) {
  if (!debugmode) {return;}
  if (Serial) {
    Serial.print("VBAT = "); Serial.println(v,3); 
  }
  if (lcd_en)  {
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print("VBAT=");lcd.print(v,3);
    delay(3000);   
  }
}
void debugPrintMeasure(int ws, int wd) {
  
  if (!debugmode) {return;}
  if (Serial) {
    Serial.print("WS = "); Serial.print(ws); 
    Serial.print("km/h, WD = "); Serial.print(wd);Serial.print("  ");
    Serial.println(deg2dir(wd)); Serial.println("");
  }
  // print on lcd screen
  if (lcd_en)  {
    msnbr++;
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print(msnbr);lcd.print(" ");lcd.print(deg2dir(wd));lcd.print(" ");lcd.print(wd);
    lcd.print(" ");lcd.print(ws);
  }

}

/*
* Accessory to log data via Serial when in debug mode
*/
void debugPrintAvgMeas(int aws, int awd) {

  if(debugmode || lcd_en) {
    repnbr++;
    if(Serial) {
      Serial.print("Average Speed : ");
      Serial.print(aws);
      Serial.println("");
      Serial.print("Average Direction : ");
      Serial.print(awd);
      Serial.print(" ");
      Serial.print(deg2dir(awd));
      Serial.println("");
      Serial.print("Nombre of samples : ");
      Serial.print(cnt_ws_samples);
      Serial.print("  ");
      Serial.print(cnt_wd_samples);
      Serial.println("");
      Serial.println("====================");
    }
    if (lcd_en)  {
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print(repnbr);lcd.print(" ");
      lcd.print("Vm=");lcd.print(aws);lcd.print(" ");
      lcd.print("Dm=");lcd.print(deg2dir(awd));
    }
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
