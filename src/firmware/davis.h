
#define DAVIS       10   // sensor numbering

#define DavisWire1  0    // wire1 (Black) to digital 0
//      DavisWire2       // wire2 (Red) to GND
#define DavisWire3  1    // wire3 (Green) to digital 1
#define DavisWire4  2    // wire4 (Yellow) to digital 2


#define DavisDirPin   A1  // analog direction - green wire
#define DavisSpeedPin 0   // dig speed on D0  - black wire
#define DavisDirFullScale  682  // ADC value for 360 degrees


void Davis_setup();
void Davis_isr_speed();
int  Davis_takeWspeed(int deltaT);
int  Davis_takeWdir();
