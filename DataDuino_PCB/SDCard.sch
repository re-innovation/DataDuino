EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:1wire
LIBS:arduino_shieldsNCL
LIBS:atmel-1
LIBS:atmel-2005
LIBS:philips
LIBS:nxp
LIBS:matts_components
LIBS:DAQ_Flow_LEEDR_v5-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Connection ~ 6150 4100
Wire Wire Line
	6150 4150 6150 4000
Connection ~ 4950 4950
Wire Wire Line
	4950 5100 4950 4950
Wire Wire Line
	5050 3350 5050 4350
Connection ~ 4850 3650
Wire Wire Line
	6250 3200 5450 3200
Wire Wire Line
	5450 3200 5450 3650
Wire Wire Line
	5450 3650 4500 3650
Wire Wire Line
	3800 3650 4000 3650
Wire Wire Line
	5250 3950 5250 4350
Wire Wire Line
	5050 4950 5050 4850
Wire Wire Line
	4650 4850 4650 4950
Wire Wire Line
	4650 4250 4650 4350
Wire Wire Line
	6250 3300 5850 3300
Wire Wire Line
	5850 3300 5850 3400
Connection ~ 5050 2550
Wire Wire Line
	6250 3400 6000 3400
Wire Wire Line
	6000 3400 6000 2550
Connection ~ 4400 2950
Wire Wire Line
	3700 2950 5050 2950
Wire Wire Line
	4000 2550 3550 2550
Connection ~ 3700 2550
Wire Wire Line
	4400 2850 4400 3050
Wire Wire Line
	5850 3650 5850 3600
Wire Wire Line
	5850 3600 6250 3600
Wire Wire Line
	3800 4250 4000 4250
Wire Wire Line
	6250 3500 5550 3500
Wire Wire Line
	5550 3500 5550 4250
Wire Wire Line
	5550 4250 4500 4250
Connection ~ 4650 4250
Wire Wire Line
	4850 4850 4850 4950
Connection ~ 4850 4950
Wire Wire Line
	4650 4950 5250 4950
Wire Wire Line
	5250 4950 5250 4850
Connection ~ 5050 4950
Wire Wire Line
	3800 3950 6050 3950
Wire Wire Line
	6050 3950 6050 3700
Wire Wire Line
	6050 3700 6250 3700
Connection ~ 5250 3950
Wire Wire Line
	4850 3650 4850 4350
Wire Wire Line
	3800 3350 4000 3350
Wire Wire Line
	4500 3350 5350 3350
Wire Wire Line
	5350 3350 5350 3100
Wire Wire Line
	5350 3100 6250 3100
Connection ~ 5050 3350
Wire Wire Line
	6250 2550 4800 2550
Connection ~ 6000 2550
Wire Wire Line
	6150 4100 6050 4100
Wire Wire Line
	6150 4000 6250 4000
Wire Wire Line
	6050 4650 6150 4650
NoConn ~ 6250 4300
NoConn ~ 6250 4200
NoConn ~ 6250 4100
NoConn ~ 6250 3800
NoConn ~ 6250 3900
$Comp
L +5V #PWR?
U 1 1 585B6FB5
P 6050 4650
F 0 "#PWR?" H 6050 4740 20  0001 C CNN
F 1 "+5V" H 6050 4740 30  0000 C CNN
F 2 "" H 6050 4650 60  0001 C CNN
F 3 "" H 6050 4650 60  0001 C CNN
	1    6050 4650
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-DAQ_Flow_LEEDR_v4 R?
U 1 1 585B6FBB
P 6150 4400
F 0 "R?" V 6230 4400 50  0000 C CNN
F 1 "10k" V 6150 4400 50  0000 C CNN
F 2 "matts_components:R3-LARGE_PADS_0_8_hole" H 6150 4400 60  0001 C CNN
F 3 "" H 6150 4400 60  0001 C CNN
	1    6150 4400
	1    0    0    -1  
$EndComp
Text GLabel 6050 4100 0    60   Output ~ 0
CD
Text GLabel 6250 2550 2    60   Output ~ 0
3v3
$Comp
L GND-RESCUE-DAQ_Flow_LEEDR_v4 #PWR?
U 1 1 585B6FC4
P 4950 5100
F 0 "#PWR?" H 4950 5100 30  0001 C CNN
F 1 "GND" H 4950 5030 30  0001 C CNN
F 2 "" H 4950 5100 60  0001 C CNN
F 3 "" H 4950 5100 60  0001 C CNN
	1    4950 5100
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-DAQ_Flow_LEEDR_v4 R?
U 1 1 585B6FCA
P 5250 4600
F 0 "R?" V 5330 4600 50  0000 C CNN
F 1 "100k" V 5250 4600 50  0000 C CNN
F 2 "matts_components:R3-LARGE_PADS_0_8_hole" H 5250 4600 60  0001 C CNN
F 3 "" H 5250 4600 60  0001 C CNN
	1    5250 4600
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-DAQ_Flow_LEEDR_v4 R?
U 1 1 585B6FD1
P 5050 4600
F 0 "R?" V 5130 4600 50  0000 C CNN
F 1 "3k3" V 5050 4600 50  0000 C CNN
F 2 "matts_components:R3-LARGE_PADS_0_8_hole" H 5050 4600 60  0001 C CNN
F 3 "" H 5050 4600 60  0001 C CNN
	1    5050 4600
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-DAQ_Flow_LEEDR_v4 #PWR?
U 1 1 585B6FD8
P 5850 3650
F 0 "#PWR?" H 5850 3650 30  0001 C CNN
F 1 "GND" H 5850 3580 30  0001 C CNN
F 2 "" H 5850 3650 60  0001 C CNN
F 3 "" H 5850 3650 60  0001 C CNN
	1    5850 3650
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-DAQ_Flow_LEEDR_v4 #PWR?
U 1 1 585B6FDE
P 5850 3400
F 0 "#PWR?" H 5850 3400 30  0001 C CNN
F 1 "GND" H 5850 3330 30  0001 C CNN
F 2 "" H 5850 3400 60  0001 C CNN
F 3 "" H 5850 3400 60  0001 C CNN
	1    5850 3400
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-DAQ_Flow_LEEDR_v4 #PWR?
U 1 1 585B6FE4
P 4400 3050
F 0 "#PWR?" H 4400 3050 30  0001 C CNN
F 1 "GND" H 4400 2980 30  0001 C CNN
F 2 "" H 4400 3050 60  0001 C CNN
F 3 "" H 4400 3050 60  0001 C CNN
	1    4400 3050
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR?
U 1 1 585B6FEA
P 3550 2550
F 0 "#PWR?" H 3550 2640 20  0001 C CNN
F 1 "+5V" H 3550 2640 30  0000 C CNN
F 2 "" H 3550 2550 60  0001 C CNN
F 3 "" H 3550 2550 60  0001 C CNN
	1    3550 2550
	1    0    0    -1  
$EndComp
$Comp
L CP1-RESCUE-DAQ_Flow_LEEDR_v4 C?
U 1 1 585B6FF0
P 5050 2750
F 0 "C?" H 5100 2850 50  0000 L CNN
F 1 "10uF" H 5100 2650 50  0000 L CNN
F 2 "matts_components:C2V8_large_pad" H 5050 2750 60  0001 C CNN
F 3 "" H 5050 2750 60  0001 C CNN
	1    5050 2750
	1    0    0    -1  
$EndComp
Text GLabel 3800 4250 0    60   Input ~ 0
D13
Text GLabel 3800 3950 0    60   Input ~ 0
D12
Text GLabel 3800 3650 0    60   Input ~ 0
D11
Text GLabel 3800 3350 0    60   Input ~ 0
D10
$Comp
L CP1-RESCUE-DAQ_Flow_LEEDR_v4 C?
U 1 1 585B6FFB
P 3700 2750
F 0 "C?" H 3750 2850 50  0000 L CNN
F 1 "10uF" H 3750 2650 50  0000 L CNN
F 2 "matts_components:C2V8_large_pad" H 3700 2750 60  0001 C CNN
F 3 "" H 3700 2750 60  0001 C CNN
	1    3700 2750
	1    0    0    -1  
$EndComp
$Comp
L 7805 U?
U 1 1 585B7002
P 4400 2600
F 0 "U?" H 4550 2404 60  0000 C CNN
F 1 "MCP1703" H 4400 2800 60  0000 C CNN
F 2 "matts_components:TO92_MCP9701_matt_2" H 4400 2600 60  0001 C CNN
F 3 "" H 4400 2600 60  0001 C CNN
	1    4400 2600
	1    0    0    -1  
$EndComp
$Comp
L CON-SD-MMC-3 J?
U 1 1 585B7009
P 6550 3700
F 0 "J?" H 6400 4430 50  0000 L BNN
F 1 "CON-SD-MMC-3" H 6400 2900 50  0000 L BNN
F 2 "matts_components:SD_holder_3M_matt" H 6550 3850 50  0001 C CNN
F 3 "" H 6550 3700 60  0001 C CNN
	1    6550 3700
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-DAQ_Flow_LEEDR_v4 R?
U 1 1 585B7010
P 4250 3650
F 0 "R?" V 4330 3650 50  0000 C CNN
F 1 "2k2" V 4250 3650 50  0000 C CNN
F 2 "matts_components:R3-LARGE_PADS_0_8_hole" H 4250 3650 60  0001 C CNN
F 3 "" H 4250 3650 60  0001 C CNN
	1    4250 3650
	0    -1   -1   0   
$EndComp
$Comp
L R-RESCUE-DAQ_Flow_LEEDR_v4 R?
U 1 1 585B7017
P 4850 4600
F 0 "R?" V 4930 4600 50  0000 C CNN
F 1 "3k3" V 4850 4600 50  0000 C CNN
F 2 "matts_components:R3-LARGE_PADS_0_8_hole" H 4850 4600 60  0001 C CNN
F 3 "" H 4850 4600 60  0001 C CNN
	1    4850 4600
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-DAQ_Flow_LEEDR_v4 R?
U 1 1 585B701E
P 4250 3350
F 0 "R?" V 4330 3350 50  0000 C CNN
F 1 "2k2" V 4250 3350 50  0000 C CNN
F 2 "matts_components:R3-LARGE_PADS_0_8_hole" H 4250 3350 60  0001 C CNN
F 3 "" H 4250 3350 60  0001 C CNN
	1    4250 3350
	0    -1   -1   0   
$EndComp
$Comp
L R-RESCUE-DAQ_Flow_LEEDR_v4 R?
U 1 1 585B7025
P 4650 4600
F 0 "R?" V 4730 4600 50  0000 C CNN
F 1 "3k3" V 4650 4600 50  0000 C CNN
F 2 "matts_components:R3-LARGE_PADS_0_8_hole" H 4650 4600 60  0001 C CNN
F 3 "" H 4650 4600 60  0001 C CNN
	1    4650 4600
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-DAQ_Flow_LEEDR_v4 R?
U 1 1 585B702C
P 4250 4250
F 0 "R?" V 4330 4250 50  0000 C CNN
F 1 "2k2" V 4250 4250 50  0000 C CNN
F 2 "matts_components:R3-LARGE_PADS_0_8_hole" H 4250 4250 60  0001 C CNN
F 3 "" H 4250 4250 60  0001 C CNN
	1    4250 4250
	0    -1   -1   0   
$EndComp
$EndSCHEMATC
