/*
 * Utilities for debug and print purposes
 * 
 */
void debugInit(bool sf_en, bool db_en) {
  
  if (!debugmode) {return;}
  
  SERIAL.begin(9600);           //  setup SERIAL  
  delay(1000);
  // when in debug mode (jumper on pin 11), program is blocked until USB SERIAL is connected
  while (!SERIAL) {}
  SigFox.begin();
  SERIAL.println("https://github.com/pcaunegre/MkrfoxWindShield");
  SERIAL.println("## mettre ici manuellement, a chaque compilation, le hash du commit courant ##");
  SERIAL.print(__DATE__); // macros du langage C. Indiquent la date et l'heure de la compilation
  SERIAL.print("\t");
  SERIAL.println(__TIME__);
  SERIAL.print(SigFox.ID());
  SERIAL.print("\t");
  SERIAL.println(SigFox.PAC());
  SigFox.end();
  SERIAL.print("STARTING version:");SERIAL.print(SOFTVERSION);SERIAL.print("/");
  SERIAL.print(SOFTDATE);
  SERIAL.print(" debug=");SERIAL.print(db_en);
  SERIAL.print(" sigfox=");SERIAL.println(sf_en);
  SERIAL.flush();
  
}


void debugSensorDetection(String msg, int sensor, int val1, int val2) {

  if (!debugmode) {return;}
  int prevcpudiv=cpudiv;
  set_cpu_speed(CPU_FULL);
  if (SERIAL) {
    SERIAL.print("ADCFS=");SERIAL.println(ADCFS);
    SERIAL.print("THR1=");SERIAL.println(THR1);
    SERIAL.print("THR_Peet_low=");SERIAL.println(THR_Peet_low);
    SERIAL.print("THR_Peet_hi=");SERIAL.println(THR_Peet_hi); 
    SERIAL.print("THR_Shenzen_low=");SERIAL.println(THR_Shenzen_low);
    SERIAL.print("THR_Shenzen_hi=");SERIAL.println(THR_Shenzen_hi);
    SERIAL.print("THR_Davis_low=");SERIAL.println(THR_Davis_low);  
    SERIAL.print("THR_Davis_hi=");SERIAL.println(THR_Davis_hi); 
    SERIAL.print("Sensor detection, val1="); SERIAL.print(val1);
    SERIAL.print(" val2="); SERIAL.print(val2);
    SERIAL.print(" Detected="); SERIAL.println(sensor);
    SERIAL.println(msg);
    SERIAL.flush();
  }
  set_cpu_speed(prevcpudiv);

}
  
void debugPrintMsg(String msg) {
    if (!debugmode) {return;}
    if (SERIAL) {
      SERIAL.println(msg);
    }
}

void debugPrint(const char* mystr, unsigned int mypar) {
  
  if (!debugmode) {return;}
  int prevcpudiv=cpudiv;
  set_cpu_speed(CPU_FULL);
  if (SERIAL) {
    SERIAL.print(mystr); SERIAL.println(mypar);
    SERIAL.flush();
  }
  set_cpu_speed(prevcpudiv);

}
void debugPrintVbat(int vpin, float v) {
  if (!debugmode) {return;}
  int prevcpudiv=cpudiv;
  set_cpu_speed(CPU_FULL);
  if (SERIAL) {
    if (vpin==VCCMEAS) SERIAL.print(" VCC = ");
    if (vpin==VINMEAS) SERIAL.print(" VIN = ");
    SERIAL.println(v,0);
    SERIAL.flush();
  }
  set_cpu_speed(prevcpudiv);
}

void debugPrintTemp(float tp) {
  if (!debugmode) {return;}
  int prevcpudiv=cpudiv;
  SERIAL.print("temperature adc = ");SERIAL.println(tp,2);
  set_cpu_speed(prevcpudiv);
}

void debugPrintMeasure(int ws, int wd) {
  if (!debugmode) {return;}
  msnbr++;
  int prevcpudiv=cpudiv;
  set_cpu_speed(CPU_FULL);
  if (SERIAL) {
    SERIAL.print(msnbr);SERIAL.print(" WS = "); SERIAL.print(ws); 
    SERIAL.print("km/h, WD = "); SERIAL.print(wd);SERIAL.print("  ");
    SERIAL.print(deg2dir(wd)); SERIAL.println(millis()); SERIAL.println("  ");
    SERIAL.flush();
  }
  set_cpu_speed(prevcpudiv);

}

/*
* Accessory to log data via SERIAL when in debug mode
*/
void debugPrintAvgMeas(int aws, int awd) {

  if (debugmode) {
    repnbr++;
    int prevcpudiv=cpudiv;
    set_cpu_speed(CPU_FULL);
    if(SERIAL) {
      SERIAL.print("Average Speed : ");
      SERIAL.print(aws);
      SERIAL.println("");
      SERIAL.print("Average Direction : ");
      SERIAL.print(awd);
      SERIAL.print(" ");
      SERIAL.print(deg2dir(awd));
      SERIAL.println("");
      SERIAL.print("Nombre of samples : ");
      SERIAL.print(cnt_ws_samples);
      SERIAL.print("  ");
      SERIAL.print(cnt_wd_samples);
      SERIAL.println("");
      SERIAL.println("====================");
      SERIAL.flush();
    }
    set_cpu_speed(prevcpudiv);
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
