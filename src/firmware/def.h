#define SOFTDATE 20240701
#define SOFTVERSION 22


#define DAVIS       10   // sensor numbering
#define PEET        20   // sensor numbering
#define SHENZEN     30   // sensor number


#define Led LED_BUILTIN  // warning: Led uses pin D6 !
#define ARRAYLEN    120  // size of an array to store pulses (40rps gives 120 values in 3s)

#define CPU_FULL   1     // normal cpu rate (48Mhz)
#define CPU_SLOW   16    // divided by 16 so 3Mhz for power saving, so we have ~3.1mA and a peak of ~45mA during 3s every 10min

// periods in ms and multiple of CPU_SLOW (16)
#define SAMPLING_PERIOD          2992    // instantaneous wind is measured on a 3s period (common rule) so 2992ms to be a modulo 16 (clock divider)
// #define REPORT_PERIOD          600000UL  // in production, report period is 10min=600s (both the period to avg the wind speed and the sigfox report period)
#define REPORT_PERIOD          592000UL  // in production, report period is 10min=600s (both the period to avg the wind speed and the sigfox report period)
#define ADMIN_REPORT_FREQ         143    // after 143 measure reports we send a monitoring report
#define REBOOT_PERIOD         604800000UL  // reboot micro every 7 day

#define VCCMEAS    A3           // Analog Pin to measure VCC voltage
#define VINMEAS    A5           // Analog Pin to measure VIN voltage

#define SERIAL   Serial         // Serial connection for debug
#define DEBUGPIN   11           // Pin to detect debug mode (when low)
#define SIGDISAB   12           // Pin to disable sigfox messages (when low)
#define TESTPIN    10           // Pin to enable led to monitor activity (when low)

#define SENSPPIN   3            // Pin to power up the sensor 
#define RPULLUP    10.0         // Pullup in kOhm
#define RSERIAL    1.0          // Same value for RS1, RS2, RS3 in kOhm
#define ADCBITS    10           // Using default ADC setup is enough
const int ADCFS =  (1<<ADCBITS)-1; // ADC full scale = 2**10-1
#define TOL        0.07          // Tolerance on R values (7%): 5% was not enough

#define VBDIV      0.20         // Res Divider to measure Vbat (100k/(100k+400k))

#define RSHENMIN   0.688        // Min value of Dir Potentiometer (Shenzen sensor) in kOhm
#define RSHENMAX   120.0        // Max value of Dir Potentiometer (Shenzen sensor) in kOhm

#define RDAVISPOT  20.0         // Dir Potentiometer (Davis sensor) in kOhm

#define UNDEFINED  -1           // Return code for undef
#define SHORT_ERR  -2           // Error code when a short is detected
#define OPEN_ERR   -3           // Error code when a open is detected

#define TEMP25    298.15        // Temp 25 ref for CTN
#define BETA     3988           // Beta Coeff of CTN TDK B57863S0103+040 https://docs.rs-online.com/6a46/0900766b813c0ed3.pdf

/*
* Threshold computation
*/

/* D2 IN, D0 OUT 0, READ A2 */
/* Peet bros detection*/
/* if speed switch open */
const int THR1=int(ADCFS*0.97);                                                            // 992
/* if speed switch closed */
const int THR_Peet_low = int(ADCFS/(1+RPULLUP/(2*RSERIAL)*(1+TOL)/(1-TOL)));               // 156
const int THR_Peet_hi  = int(ADCFS/(1+RPULLUP/(2*RSERIAL)*(1-TOL)/(1+TOL)));               // 185

/* Shenzen detection*/
const int THR_Shenzen_low = int(ADCFS/(1.0+RPULLUP/(2*RSERIAL+RSHENMIN)*(1+TOL)/(1-TOL))); // 200
const int THR_Shenzen_hi  = int(ADCFS/(1.0+RPULLUP/(2*RSERIAL+RSHENMAX)*(1-TOL)/(1+TOL))); // 952

/* Davis detection*/
const int THR_Davis_low = int(ADCFS/(1.0+RPULLUP/(RSERIAL+RDAVISPOT)*(1+TOL)/(1-TOL)));    // 670
const int THR_Davis_hi  = int(ADCFS/(1.0+RPULLUP/(RSERIAL+RDAVISPOT)*(1-TOL)/(1+TOL)));    // 714



/*
* This part of code is about packing data to comply with the format expected by OpenWindMap
*
* Numbers are arranged to fit into 8 bytes (2 periods, 4 data)
* xxx[0] -> data from T-12 to T-6 min
* xxx[1] -> data from T-6 min to T
* T being the time of emission
*/
typedef struct __attribute__ ((packed)) sigfox_wind_message {
        int8_t speedMin[2];
        int8_t speedAvg[2];
        int8_t speedMax[2];
        int8_t directionAvg[2];
        int8_t batVin;
        int8_t batVcc;
        int8_t temperature;
        int8_t version; 
} SigfoxWindMessage_t;


// wind speed encoding over 1 byte
// 
static uint8_t encodeWindSpeed (float speedKmh) {
  uint8_t encodedSpeed;
  if (speedKmh < 10.) {
    // 0 to 9.75 kmh : 0.25 km/h resolution
    encodedSpeed = (uint8_t)(float)(speedKmh * 4. + 0.5);
  } else if (speedKmh < 80.) {
    // 10 to 79.5 kmh  : 0.5 km/h resolution
    encodedSpeed = (uint8_t)(float)(speedKmh * 2. + 0.5) + 20;
  } else if (speedKmh < 120.) {
    // 80 to 119 kmh  : 1 km/h resolution
    encodedSpeed = (uint8_t)(float)(speedKmh + 0.5) + 100;
  } else if (speedKmh < 190.) {
    // 120 to 188 kmh  : 2 km/h resolution
    encodedSpeed = (uint8_t)(float)(speedKmh / 2. + 0.5) + 160;
  } else {
    // 190 or + : out of range
    encodedSpeed = 0xFF;
  }
  return encodedSpeed;
}

// wind direction encoding over 1 byte
// here direction comes 0-359 degrees (not like in Pioupiou)
static uint8_t encodeWindDirection (int direction) {   // degrees

  // encode with 2 deg precision
  // add 0.5 for rounding when converting from (float) to (int)
  return (uint8_t)(float)(direction / 2. + 0.5);
}

// temperature encoding over 1 byte, should never be < -50 or > 100
static uint8_t encodeTemperature(float temperature) {
  return (uint8_t)(float)(temperature + 50.5);
}


// voltage encoded between 2.5V and 5.05V (2500 to 5050mV) -> 0 to 255
static uint8_t encodeVoltage(float milliVolts) {
  if (milliVolts<2500) return (0);
  return (uint8_t)(float)(milliVolts / 10. - 250);
}
