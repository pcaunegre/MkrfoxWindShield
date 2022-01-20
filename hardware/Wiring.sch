EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Sensors interoperability"
Date "2021-11-15"
Rev ""
Comp ""
Comment1 "Wiring schematic"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Switch:SW_DIP_x01 DIR
U 1 1 619291CA
P 3900 3850
F 0 "DIR" V 3854 3980 50  0000 L CNN
F 1 "SW_DIP_x01" V 3945 3980 50  0000 L CNN
F 2 "" H 3900 3850 50  0001 C CNN
F 3 "~" H 3900 3850 50  0001 C CNN
	1    3900 3850
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_DIP_x01 SPEED
U 1 1 61929AEA
P 4700 3800
F 0 "SPEED" V 4654 3930 50  0000 L CNN
F 1 "SW_DIP_x01" V 4745 3930 50  0000 L CNN
F 2 "" H 4700 3800 50  0001 C CNN
F 3 "~" H 4700 3800 50  0001 C CNN
	1    4700 3800
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_DIP_x01 SPEED
U 1 1 61929C39
P 8550 3950
F 0 "SPEED" V 8504 4080 50  0000 L CNN
F 1 "SW_DIP_x01" V 8595 4080 50  0000 L CNN
F 2 "" H 8550 3950 50  0001 C CNN
F 3 "~" H 8550 3950 50  0001 C CNN
	1    8550 3950
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_DIP_x01 SPEED
U 1 1 61929D0A
P 6150 3900
F 0 "SPEED" V 6104 4030 50  0000 L CNN
F 1 "SW_DIP_x01" V 6195 4030 50  0000 L CNN
F 2 "" H 6150 3900 50  0001 C CNN
F 3 "~" H 6150 3900 50  0001 C CNN
	1    6150 3900
	0    1    1    0   
$EndComp
$Comp
L Device:R RS0
U 1 1 6192AC61
P 2700 2750
F 0 "RS0" V 2493 2750 50  0000 C CNN
F 1 "1K" V 2584 2750 50  0000 C CNN
F 2 "" V 2630 2750 50  0001 C CNN
F 3 "~" H 2700 2750 50  0001 C CNN
	1    2700 2750
	0    1    1    0   
$EndComp
$Comp
L Device:R RS1
U 1 1 6192C831
P 2850 2200
F 0 "RS1" V 2643 2200 50  0000 C CNN
F 1 "1K" V 2734 2200 50  0000 C CNN
F 2 "" V 2780 2200 50  0001 C CNN
F 3 "~" H 2850 2200 50  0001 C CNN
	1    2850 2200
	0    1    1    0   
$EndComp
$Comp
L Device:R RS2
U 1 1 6192CAC9
P 2500 2000
F 0 "RS2" V 2293 2000 50  0000 C CNN
F 1 "1K" V 2384 2000 50  0000 C CNN
F 2 "" V 2430 2000 50  0001 C CNN
F 3 "~" H 2500 2000 50  0001 C CNN
	1    2500 2000
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6192D335
P 1800 2500
F 0 "#PWR?" H 1800 2250 50  0001 C CNN
F 1 "GND" H 1805 2327 50  0000 C CNN
F 2 "" H 1800 2500 50  0001 C CNN
F 3 "" H 1800 2500 50  0001 C CNN
	1    1800 2500
	1    0    0    -1  
$EndComp
Text GLabel 1600 2750 0    50   Input ~ 0
D0
Text GLabel 1600 2200 0    50   Input ~ 0
D1
Text GLabel 1600 2000 0    50   Input ~ 0
D2
Wire Wire Line
	1600 2000 1700 2000
Text GLabel 1400 2100 0    50   Input ~ 0
A1
Text GLabel 1400 1850 0    50   Input ~ 0
A2
Wire Wire Line
	1700 1850 1700 2000
Wire Wire Line
	1400 1850 1700 1850
Connection ~ 1700 2000
Wire Wire Line
	1700 2000 1950 2000
Wire Wire Line
	1400 2100 1700 2100
Wire Wire Line
	1700 2100 1700 2200
Wire Wire Line
	1600 2200 1700 2200
Connection ~ 1700 2200
$Comp
L Device:R R_PULLUP
U 1 1 6192ECC6
P 1950 1650
F 0 "R_PULLUP" H 2020 1696 50  0000 L CNN
F 1 "10K" H 2020 1605 50  0000 L CNN
F 2 "" V 1880 1650 50  0001 C CNN
F 3 "~" H 1950 1650 50  0001 C CNN
	1    1950 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 1800 1950 2000
Connection ~ 1950 2000
Wire Wire Line
	1950 2000 2350 2000
Wire Wire Line
	1700 2200 2700 2200
Wire Wire Line
	1600 2750 2550 2750
Wire Wire Line
	3900 4150 3900 4300
Wire Wire Line
	3900 4300 4200 4300
Wire Wire Line
	4200 4300 4200 2500
Wire Wire Line
	4700 4100 4700 4300
Wire Wire Line
	4700 4300 5050 4300
Wire Wire Line
	3900 3550 3900 2200
Connection ~ 3900 2200
Wire Wire Line
	3900 2200 6150 2200
Wire Wire Line
	4700 3500 4700 2000
Wire Wire Line
	4700 2000 7050 2000
Wire Wire Line
	5050 2750 7300 2750
Wire Wire Line
	5050 4300 5050 2750
Wire Wire Line
	4200 2500 6500 2500
Text Notes 3800 5250 0    150  ~ 30
PEET BROS 
$Comp
L Device:R_POT DIR
U 1 1 6193F369
P 7050 3900
F 0 "DIR" H 6981 3946 50  0000 R CNN
F 1 "R_POT" H 6981 3855 50  0000 R CNN
F 2 "" H 7050 3900 50  0001 C CNN
F 3 "~" H 7050 3900 50  0001 C CNN
	1    7050 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6150 4200 6150 4250
Wire Wire Line
	6150 4250 6500 4250
Connection ~ 6500 2500
Wire Wire Line
	6150 3600 6150 2200
Connection ~ 6150 2200
Wire Wire Line
	6500 2500 6500 4250
Wire Wire Line
	7050 3750 7050 2000
Connection ~ 7050 2000
Wire Wire Line
	7200 3900 7300 3900
Wire Wire Line
	7300 3900 7300 2750
Connection ~ 7300 2750
Wire Wire Line
	7300 2750 8550 2750
Text Notes 6050 5250 0    150  ~ 30
Mi.Sol\n(SHENZEN)
Wire Wire Line
	8550 4250 8550 4350
Wire Wire Line
	8550 4350 9100 4350
Wire Wire Line
	9100 4350 9100 2500
Wire Wire Line
	6500 2500 9100 2500
Wire Wire Line
	8550 3650 8550 2750
$Comp
L Device:R_POT DIR
U 1 1 619498B3
P 9700 3950
F 0 "DIR" H 9631 3996 50  0000 R CNN
F 1 "R_POT" H 9631 3905 50  0000 R CNN
F 2 "" H 9700 3950 50  0001 C CNN
F 3 "~" H 9700 3950 50  0001 C CNN
	1    9700 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	9850 3950 9950 3950
Wire Wire Line
	9700 2000 9700 3800
Wire Wire Line
	7050 2000 9700 2000
Wire Wire Line
	9100 4350 9700 4350
Wire Wire Line
	9700 4350 9700 4100
Connection ~ 9100 4350
Wire Wire Line
	9950 3950 9950 2200
Wire Wire Line
	6150 2200 9950 2200
Text Notes 8750 5300 0    150  ~ 30
DAVIS
Wire Notes Line
	3350 5500 5300 5500
Wire Notes Line
	5300 5500 5300 1700
Wire Notes Line
	5300 1700 3400 1700
Wire Notes Line
	3400 1700 3400 5550
Wire Notes Line
	5600 1700 5600 5500
Wire Notes Line
	5600 5500 7550 5500
Wire Notes Line
	7550 5500 7550 1750
Wire Notes Line
	7550 1700 5600 1700
Wire Notes Line
	7850 1750 7850 5500
Wire Notes Line
	7850 5500 10200 5500
Wire Notes Line
	10200 5500 10200 1800
Wire Notes Line
	10150 1750 7850 1750
Text Notes 9750 4200 0    50   ~ 0
0 - 20 K
Text Notes 6850 4200 0    50   ~ 0
688 - 120 K
Text Notes 3200 2750 0    100  ~ 0
1
Text Notes 3200 2450 0    100  ~ 0
2
Text Notes 3200 2150 0    100  ~ 0
3
Text Notes 3200 1950 0    100  ~ 0
4
Wire Wire Line
	2650 2000 4700 2000
Connection ~ 4700 2000
Wire Wire Line
	3000 2200 3900 2200
Wire Wire Line
	1800 2500 4200 2500
Connection ~ 4200 2500
Wire Wire Line
	2850 2750 5050 2750
Connection ~ 5050 2750
Wire Notes Line
	3150 2950 3350 2950
Wire Notes Line
	3150 1000 3150 2950
Text Notes 3250 1300 0    100  ~ 0
RJ11 connector
Wire Notes Line
	4500 1000 4500 1450
Wire Notes Line
	4500 1450 3350 1450
Wire Notes Line
	3150 1000 4500 1000
Wire Notes Line
	3350 2950 3350 1450
Text Notes 8400 3500 0    100  ~ 0
Black
Text Notes 9000 3550 0    100  ~ 0
Red
Text Notes 9450 3550 0    100  ~ 0
Yellow
Text Notes 9800 3250 0    100  ~ 0
Green
Text Notes 6050 3550 0    100  ~ 0
Y
Text Notes 6400 3550 0    100  ~ 0
R
Text Notes 6850 3550 0    100  ~ 0
G
Text Notes 7200 3550 0    100  ~ 0
Y
Text Notes 3800 3550 0    100  ~ 0
G
Text Notes 4550 3550 0    100  ~ 0
Y
Text Notes 4100 3550 0    100  ~ 0
B
Text Notes 4950 3550 0    100  ~ 0
R
Text Notes 4950 1250 0    100  ~ 0
The same RJ11 connector works for any of the 3 possible sensors
Text Notes 3450 4550 0    100  ~ 0
WIND DIR
Text Notes 4350 4550 0    100  ~ 0
WIND SPEED
Text Notes 6800 4550 0    100  ~ 0
WIND DIR
Text Notes 5650 4550 0    100  ~ 0
WIND SPEED
Text Notes 9400 4600 0    100  ~ 0
WIND DIR
Text Notes 8000 4600 0    100  ~ 0
WIND SPEED
Wire Wire Line
	6500 4250 7050 4250
Wire Wire Line
	7050 4250 7050 4050
Connection ~ 6500 4250
Wire Wire Line
	1950 1500 1950 1350
Wire Wire Line
	1950 1350 1425 1350
Text GLabel 1425 1350 0    50   Input ~ 0
D3
Text Notes 5600 5850 0    79   ~ 0
note that for this sensor, yellow \nand green wires are twisted
$EndSCHEMATC
