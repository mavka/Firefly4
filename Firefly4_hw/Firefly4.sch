EESchema Schematic File Version 2
LIBS:Connectors_kl
LIBS:pcb_details
LIBS:power
LIBS:Power_kl
LIBS:Tittar_kl
LIBS:st_kl
LIBS:Transistors_kl
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_1 XL?
U 1 1 5665DAA8
P 1650 6600
F 0 "XL?" V 1600 6610 40  0000 C CNN
F 1 "CONN_1" V 1660 6620 40  0000 C CNN
F 2 "" H 1650 6600 60  0000 C CNN
F 3 "" H 1650 6600 60  0000 C CNN
	1    1650 6600
	-1   0    0    1   
$EndComp
$Comp
L CONN_1 XL?
U 1 1 5665DB21
P 1650 7050
F 0 "XL?" V 1600 7060 40  0000 C CNN
F 1 "CONN_1" V 1660 7070 40  0000 C CNN
F 2 "" H 1650 7050 60  0000 C CNN
F 3 "" H 1650 7050 60  0000 C CNN
	1    1650 7050
	-1   0    0    1   
$EndComp
$Comp
L SWITCH SW?
U 1 1 5665DB52
P 2700 6600
F 0 "SW?" H 2450 6850 60  0000 C CNN
F 1 "SWITCH" H 2800 6850 60  0000 C CNN
F 2 "" H 2700 6600 60  0000 C CNN
F 3 "" H 2700 6600 60  0000 C CNN
	1    2700 6600
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1850 6600 2450 6600
$Comp
L L L?
U 1 1 5665DCB2
P 4250 6600
F 0 "L?" V 4150 6460 40  0000 C CNN
F 1 "22uH" V 4150 6600 40  0000 C CNN
F 2 "IND_0402" V 4310 6610 40  0000 C CNN
F 3 "" H 4250 6600 60  0000 C CNN
	1    4250 6600
	0    1    1    0   
$EndComp
$Comp
L C C?
U 1 1 5665DD35
P 3550 6850
F 0 "C?" H 3450 6750 50  0000 L CNN
F 1 "0.1u" H 3450 6950 50  0000 L CNN
F 2 "CAP_0603" V 3650 6700 28  0000 C BNN
F 3 "" H 3550 6850 60  0000 C CNN
	1    3550 6850
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 5665DD62
P 3850 6850
F 0 "C?" H 3750 6750 50  0000 L CNN
F 1 "10u" H 3750 6950 50  0000 L CNN
F 2 "CAP_0603" V 3950 6700 28  0000 C BNN
F 3 "" H 3850 6850 60  0000 C CNN
	1    3850 6850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 6600 4050 6600
Wire Wire Line
	3550 6400 3550 6650
Connection ~ 3550 6600
Wire Wire Line
	3850 6450 3850 6650
Connection ~ 3850 6600
$Comp
L Fuse0R F?
U 1 1 5665DDA4
P 2750 6800
F 0 "F?" V 2680 6800 50  0000 C CNN
F 1 "Fuse0R" V 2750 6800 50  0000 C CNN
F 2 "" H 2750 6800 60  0000 C CNN
F 3 "" H 2750 6800 60  0000 C CNN
	1    2750 6800
	0    1    1    0   
$EndComp
Wire Wire Line
	2500 6800 2350 6800
Wire Wire Line
	2350 6800 2350 6600
Connection ~ 2350 6600
Wire Wire Line
	3000 6800 3200 6800
Wire Wire Line
	3200 6800 3200 6600
Connection ~ 3200 6600
NoConn ~ 2450 6500
$Comp
L GND #PWR?
U 1 1 5665DEAB
P 3550 7150
F 0 "#PWR?" H 3640 7130 30  0001 C CNN
F 1 "GND" H 3550 7070 30  0001 C CNN
F 2 "" H 3550 7150 60  0000 C CNN
F 3 "" H 3550 7150 60  0000 C CNN
	1    3550 7150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5665DECD
P 3850 7150
F 0 "#PWR?" H 3940 7130 30  0001 C CNN
F 1 "GND" H 3850 7070 30  0001 C CNN
F 2 "" H 3850 7150 60  0000 C CNN
F 3 "" H 3850 7150 60  0000 C CNN
	1    3850 7150
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 7050 3550 7150
Wire Wire Line
	3850 7050 3850 7150
$Comp
L +BATT #PWR?
U 1 1 5665DF1E
P 3550 6400
F 0 "#PWR?" H 3550 6360 30  0001 C CNN
F 1 "+BATT" H 3630 6430 30  0000 C CNN
F 2 "" H 3550 6400 60  0000 C CNN
F 3 "" H 3550 6400 60  0000 C CNN
	1    3550 6400
	0    -1   -1   0   
$EndComp
$Comp
L PWR_FLAG #FLG?
U 1 1 5665DF40
P 3850 6450
F 0 "#FLG?" H 3850 6720 30  0001 C CNN
F 1 "PWR_FLAG" H 3850 6650 30  0000 C CNN
F 2 "" H 3850 6450 60  0000 C CNN
F 3 "" H 3850 6450 60  0000 C CNN
	1    3850 6450
	1    0    0    -1  
$EndComp
Text Label 1950 6600 0    60   ~ 0
BAT_IN
$Comp
L NCP1400 DA?
U 1 1 5665E1EE
P 5100 6950
F 0 "DA?" H 4900 7200 60  0000 C CNN
F 1 "NCP1400" H 5300 7200 60  0000 C CNN
F 2 "" H 5100 6950 60  0000 C CNN
F 3 "" H 5100 6950 60  0000 C CNN
	1    5100 6950
	1    0    0    -1  
$EndComp
$Comp
L D_Shottky D?
U 1 1 5665E249
P 5050 6600
F 0 "D?" H 5050 6700 40  0000 C CNN
F 1 "BAT54WS" H 5250 6700 40  0000 C CNN
F 2 "" H 5050 6600 60  0000 C CNN
F 3 "" H 5050 6600 60  0000 C CNN
	1    5050 6600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 6600 4850 6600
Wire Wire Line
	4650 6850 4650 6600
Connection ~ 4650 6600
$Comp
L GND #PWR?
U 1 1 5665E371
P 4650 7150
F 0 "#PWR?" H 4740 7130 30  0001 C CNN
F 1 "GND" H 4650 7070 30  0001 C CNN
F 2 "" H 4650 7150 60  0000 C CNN
F 3 "" H 4650 7150 60  0000 C CNN
	1    4650 7150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 7050 4650 7150
$Comp
L C C?
U 1 1 5665E3B3
P 5800 6850
F 0 "C?" H 5700 6750 50  0000 L CNN
F 1 "0.1u" H 5700 6950 50  0000 L CNN
F 2 "CAP_0603" V 5900 6700 28  0000 C BNN
F 3 "" H 5800 6850 60  0000 C CNN
	1    5800 6850
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 5665E3DE
P 6100 6850
F 0 "C?" H 6000 6750 50  0000 L CNN
F 1 "10u" H 6000 6950 50  0000 L CNN
F 2 "CAP_0603" V 6200 6700 28  0000 C BNN
F 3 "" H 6100 6850 60  0000 C CNN
	1    6100 6850
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR?
U 1 1 5665E409
P 6400 6600
F 0 "#PWR?" H 6400 6560 30  0001 C CNN
F 1 "+3.3V" H 6480 6630 30  0000 C CNN
F 2 "" H 6400 6600 60  0000 C CNN
F 3 "" H 6400 6600 60  0000 C CNN
	1    6400 6600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 6600 6400 6600
Wire Wire Line
	5800 6650 5800 6600
Connection ~ 5800 6600
Wire Wire Line
	6100 6650 6100 6600
Connection ~ 6100 6600
Wire Wire Line
	5500 6950 5550 6950
Wire Wire Line
	5550 6600 5550 7050
Connection ~ 5550 6600
Wire Wire Line
	5550 7050 5500 7050
Connection ~ 5550 6950
$Comp
L GND #PWR?
U 1 1 5665E4E3
P 5800 7150
F 0 "#PWR?" H 5890 7130 30  0001 C CNN
F 1 "GND" H 5800 7070 30  0001 C CNN
F 2 "" H 5800 7150 60  0000 C CNN
F 3 "" H 5800 7150 60  0000 C CNN
	1    5800 7150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5665E50D
P 6100 7150
F 0 "#PWR?" H 6190 7130 30  0001 C CNN
F 1 "GND" H 6100 7070 30  0001 C CNN
F 2 "" H 6100 7150 60  0000 C CNN
F 3 "" H 6100 7150 60  0000 C CNN
	1    6100 7150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 7050 5800 7150
Wire Wire Line
	6100 7050 6100 7150
$Comp
L GND #PWR?
U 1 1 5665EA45
P 2000 7200
F 0 "#PWR?" H 2090 7180 30  0001 C CNN
F 1 "GND" H 2000 7120 30  0001 C CNN
F 2 "" H 2000 7200 60  0000 C CNN
F 3 "" H 2000 7200 60  0000 C CNN
	1    2000 7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1850 7050 2000 7050
Wire Wire Line
	2000 7050 2000 7200
Text Notes 1100 6650 0    60   ~ 0
+BATTERY
Text Notes 1100 7100 0    60   ~ 0
-BATTERY
$EndSCHEMATC
