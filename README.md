# MkrfoxWindShield
Arduino Mkrfox1200 interface to wind sensors


## This project includes several independant modules:

### Hardware
    The hardware is an Arduino shield compatible with the MKRFOX1200 board.
    The shield provides an interface with one among three types of wind sensors:
    - Davis
    - Peet Bros
    - Shenzen


### Software 
    The software automatically identifies the sensor type, controls the wind sensor 
    and sends measures via Sigfox.
    If the device is registered by the OpenWindMap network, measures will appear 
    on the web (http://www.openwindmap.org/)

