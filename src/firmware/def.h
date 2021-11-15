#define SOFTVERSION yymmdd







#define Led LED_BUILTIN  // warning: Led uses pin D6 !



#define SAMPLING_PERIOD 500     // instantaneous wind is measured on a period of 3s (common rule)
#define REPORT_PERIOD  30000    // in production, report period is 10min=600s (both the period to avg the wind speed and the sigfox report period)

#define RPULLUP    10000        // Pullup
#define RSERIAL    1000         // Same value for RS1, RS2, RS3
#define ADCBITS    10           // Using default ADC setup is enough
#define ADCFS      2**ADCBITS-1 // ADC full scale = 2**10-1
#define TOL        0.1          // Tolerance on R values (10%)

#define RSHENMIN   688          // Min value of Dir Potentiometer (Shenzen sensor)
#define RSHENMAX   120000       // Max value of Dir Potentiometer (Shenzen sensor)

#define RDAVISPOT  20000        // Dir Potentiometer (Davis sensor)

/*
* Threshold computation
*/

/* D2 IN, D0 OUT 0, READ A2 */
/* Peet bros detection*/
/* if speed switch open */
const THR1=ADCFS*0.95;                                                               // 971
/* if speed switch closed */
const THR_Peet_low = int(ADCFS/(1+RPULLUP/(2*RSERIAL)*(1+TOL)/(1-TOL)));             // 143
const THR_Peet_hi  = int(ADCFS/(1+RPULLUP/(2*RSERIAL)*(1-TOL)/(1+TOL)));             // 200

/* Shenzen detection*/
const THR_Shenzen_low = int(ADCFS/(1+RPULLUP/(2*RSERIAL+RSHENMIN)*(1+TOL)/(1-TOL))); // 184
const THR_Shenzen_hi  = int(ADCFS/(1+RPULLUP/(2*RSERIAL+RSHENMAX)*(1-TOL)/(1+TOL))); // 958

/* Davis detection*/
const THR_Davis_low = int(ADCFS/(1+RPULLUP/(RSERIAL+RDAVISPOT)*(1+TOL)/(1-TOL)));    // 646
const THR_Davis_hi  = int(ADCFS/(1+RPULLUP/(RSERIAL+RDAVISPOT)*(1-TOL)/(1+TOL)));    // 736


/*
* This part of code is about packing data to comply with the format expected by OpenWindMap
*
* Numbers are arranged to fit into 8 bytes (2 periods, 4 data)
* xxx[0] -> data from T-10 to T-5 min
* xxx[1] -> data from T-5 min to T
* T being the time of emission
*/
typedef struct __attribute__ ((packed)) sigfox_wind_message {
        int8_t speedMin[2];
        int8_t speedAvg[2];
        int8_t speedMax[2];
        int8_t directionAvg[2];
} SigfoxWindMessage;


// wind speed encoding over 1 byte
// 
uint8_t encodeWindSpeed (float speedKmh) {
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
uint8_t encodeWindDirection (int direction) {   // degrees

  // encode with 2 deg precision
  // add 0.5 for rounding when converting from (float) to (int)
  return (uint8_t)(float)(direction / 2. + 0.5);
}