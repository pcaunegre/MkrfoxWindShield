


#define SHENZEN 30         // sensor number

#define ShenzenWire1  0    // wire1 (Black) to digital 0
//      ShenzenWire2       // wire2 (Red) to GND
#define ShenzenWire3  1    // wire3 (Yellow) to digital 1
#define ShenzenWire4  2    // wire4 (Green) to digital 2


#define ShenzenDirPin   A2  // analog direction - wire4 (Green)
#define ShenzenSpeedPin 1   // dig speed on D1  - wire3 (Yellow)

void Shenzen_setup();
void Shenzen_isr_speed();
int  Shenzen_takeWspeed(int deltaT);
int  Shenzen_takeWdir();
