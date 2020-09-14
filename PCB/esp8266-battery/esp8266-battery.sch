EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date "2020-09-13"
Rev "1.0"
Comp "SilentWorks"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L RF_Module:ESP-07 U1
U 1 1 5F5E12D9
P 5300 4050
F 0 "U1" H 4850 4800 50  0000 C CNN
F 1 "ESP-07" H 5700 3400 50  0000 C CNN
F 2 "RF_Module:ESP-07" H 5300 4050 50  0001 C CNN
F 3 "http://wiki.ai-thinker.com/_media/esp8266/esp8266_series_modules_user_manual_v1.1.pdf" H 4950 4150 50  0001 C CNN
	1    5300 4050
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J3
U 1 1 5F5E72E9
P 8400 5200
F 0 "J3" H 8480 5192 50  0000 L CNN
F 1 "BME280 modul" H 8480 5101 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 8400 5200 50  0001 C CNN
F 3 "~" H 8400 5200 50  0001 C CNN
	1    8400 5200
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C2
U 1 1 5F5E7C03
P 5450 2850
F 0 "C2" H 5542 2896 50  0000 L CNN
F 1 "10u" H 5542 2805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5450 2850 50  0001 C CNN
F 3 "~" H 5450 2850 50  0001 C CNN
	1    5450 2850
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C1
U 1 1 5F5E946B
P 5150 2850
F 0 "C1" H 5058 2896 50  0000 R CNN
F 1 "100n" H 5058 2805 50  0000 R CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 5150 2850 50  0001 C CNN
F 3 "~" H 5150 2850 50  0001 C CNN
	1    5150 2850
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR01
U 1 1 5F5E991C
P 2750 3850
F 0 "#PWR01" H 2750 3700 50  0001 C CNN
F 1 "+3.3V" H 2765 4023 50  0000 C CNN
F 2 "" H 2750 3850 50  0001 C CNN
F 3 "" H 2750 3850 50  0001 C CNN
	1    2750 3850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 5F5EA4C5
P 5300 4750
F 0 "#PWR08" H 5300 4500 50  0001 C CNN
F 1 "GND" H 5305 4577 50  0000 C CNN
F 2 "" H 5300 4750 50  0001 C CNN
F 3 "" H 5300 4750 50  0001 C CNN
	1    5300 4750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 5F5EADC8
P 2750 4050
F 0 "#PWR02" H 2750 3800 50  0001 C CNN
F 1 "GND" H 2755 3877 50  0000 C CNN
F 2 "" H 2750 4050 50  0001 C CNN
F 3 "" H 2750 4050 50  0001 C CNN
	1    2750 4050
	-1   0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR07
U 1 1 5F5EE082
P 5300 2700
F 0 "#PWR07" H 5300 2550 50  0001 C CNN
F 1 "+3.3V" H 5315 2873 50  0000 C CNN
F 2 "" H 5300 2700 50  0001 C CNN
F 3 "" H 5300 2700 50  0001 C CNN
	1    5300 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 2700 5300 2750
$Comp
L power:GND #PWR09
U 1 1 5F5EF691
P 5450 2950
F 0 "#PWR09" H 5450 2700 50  0001 C CNN
F 1 "GND" H 5455 2777 50  0000 C CNN
F 2 "" H 5450 2950 50  0001 C CNN
F 3 "" H 5450 2950 50  0001 C CNN
	1    5450 2950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 5F5F0567
P 5150 2950
F 0 "#PWR06" H 5150 2700 50  0001 C CNN
F 1 "GND" H 5155 2777 50  0000 C CNN
F 2 "" H 5150 2950 50  0001 C CNN
F 3 "" H 5150 2950 50  0001 C CNN
	1    5150 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 2750 5300 2750
Connection ~ 5300 2750
Wire Wire Line
	5300 2750 5300 3250
Wire Wire Line
	5300 2750 5450 2750
NoConn ~ 4700 3850
Wire Wire Line
	4700 3450 4350 3450
$Comp
L Device:R_Small R1
U 1 1 5F5F1593
P 4250 3300
F 0 "R1" H 4191 3346 50  0000 R CNN
F 1 "10k" H 4191 3255 50  0000 R CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 4250 3300 50  0001 C CNN
F 3 "~" H 4250 3300 50  0001 C CNN
	1    4250 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 3400 4250 3450
Connection ~ 4250 3450
Wire Wire Line
	4250 3450 4050 3450
$Comp
L power:+3.3V #PWR04
U 1 1 5F5F228B
P 4250 3200
F 0 "#PWR04" H 4250 3050 50  0001 C CNN
F 1 "+3.3V" H 4265 3373 50  0000 C CNN
F 2 "" H 4250 3200 50  0001 C CNN
F 3 "" H 4250 3200 50  0001 C CNN
	1    4250 3200
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_SPST SW1
U 1 1 5F5F36DD
P 3850 3450
F 0 "SW1" H 3850 3685 50  0000 C CNN
F 1 "reset" H 3850 3594 50  0000 C CNN
F 2 "Button_Switch_SMD:SW_SPST_B3U-1000P" H 3850 3450 50  0001 C CNN
F 3 "~" H 3850 3450 50  0001 C CNN
	1    3850 3450
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5F5F66BD
P 3600 3500
F 0 "#PWR03" H 3600 3250 50  0001 C CNN
F 1 "GND" H 3605 3327 50  0000 C CNN
F 2 "" H 3600 3500 50  0001 C CNN
F 3 "" H 3600 3500 50  0001 C CNN
	1    3600 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 3450 3600 3450
Wire Wire Line
	3600 3450 3600 3500
$Comp
L Connector_Generic:Conn_01x04 J2
U 1 1 5F5F7537
P 8400 3600
F 0 "J2" H 8480 3592 50  0000 L CNN
F 1 "program" H 8480 3501 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 8400 3600 50  0001 C CNN
F 3 "~" H 8400 3600 50  0001 C CNN
	1    8400 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:D_Schottky_Small D1
U 1 1 5F5F7C14
P 6400 4450
F 0 "D1" H 6400 4550 50  0000 C CNN
F 1 "BAT-85" H 6400 4350 50  0000 C CNN
F 2 "Diode_SMD:D_SOT-23_ANK" V 6400 4450 50  0001 C CNN
F 3 "~" V 6400 4450 50  0001 C CNN
	1    6400 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5900 4450 6150 4450
Wire Wire Line
	6500 4450 6800 4450
Text Label 4650 3450 2    50   ~ 0
~RST
Text Label 6800 4450 2    50   ~ 0
~RST
Wire Wire Line
	8200 3500 8100 3500
Wire Wire Line
	8100 3500 8100 3450
$Comp
L power:+3.3V #PWR016
U 1 1 5F5FA61A
P 8100 3450
F 0 "#PWR016" H 8100 3300 50  0001 C CNN
F 1 "+3.3V" H 8115 3623 50  0000 C CNN
F 2 "" H 8100 3450 50  0001 C CNN
F 3 "" H 8100 3450 50  0001 C CNN
	1    8100 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR017
U 1 1 5F5FB8F8
P 8100 3850
F 0 "#PWR017" H 8100 3600 50  0001 C CNN
F 1 "GND" H 8105 3677 50  0000 C CNN
F 2 "" H 8100 3850 50  0001 C CNN
F 3 "" H 8100 3850 50  0001 C CNN
	1    8100 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	8200 3800 8100 3800
Wire Wire Line
	8100 3800 8100 3850
Wire Wire Line
	5900 3550 7450 3550
Wire Wire Line
	7750 3550 7750 3600
Wire Wire Line
	7750 3600 8200 3600
Wire Wire Line
	5900 3750 7450 3750
Wire Wire Line
	7750 3750 7750 3700
Wire Wire Line
	7750 3700 8200 3700
Text Label 8150 3600 2    50   ~ 0
RXD
Text Label 8150 3700 2    50   ~ 0
TXD
$Comp
L Device:R_Small R7
U 1 1 5F606123
P 7550 3550
F 0 "R7" V 7354 3550 50  0000 C CNN
F 1 "220R" V 7445 3550 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 7550 3550 50  0001 C CNN
F 3 "~" H 7550 3550 50  0001 C CNN
	1    7550 3550
	0    1    1    0   
$EndComp
Wire Wire Line
	7650 3550 7750 3550
$Comp
L Device:R_Small R8
U 1 1 5F6076E4
P 7550 3750
F 0 "R8" V 7654 3750 50  0000 C CNN
F 1 "220R" V 7745 3750 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 7550 3750 50  0001 C CNN
F 3 "~" H 7550 3750 50  0001 C CNN
	1    7550 3750
	0    1    1    0   
$EndComp
Wire Wire Line
	7650 3750 7750 3750
$Comp
L Device:R_Small R4
U 1 1 5F607D75
P 6500 3100
F 0 "R4" H 6559 3146 50  0000 L CNN
F 1 "10k" H 6559 3055 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 6500 3100 50  0001 C CNN
F 3 "~" H 6500 3100 50  0001 C CNN
	1    6500 3100
	-1   0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR012
U 1 1 5F609F69
P 6500 3000
F 0 "#PWR012" H 6500 2850 50  0001 C CNN
F 1 "+3.3V" H 6515 3173 50  0000 C CNN
F 2 "" H 6500 3000 50  0001 C CNN
F 3 "" H 6500 3000 50  0001 C CNN
	1    6500 3000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6500 3250 6500 3200
Text Label 6450 3450 2    50   ~ 0
IO0
$Comp
L Switch:SW_SPST SW2
U 1 1 5F60B48F
P 6850 3250
F 0 "SW2" H 6850 3485 50  0000 C CNN
F 1 "boot" H 6850 3394 50  0000 C CNN
F 2 "Button_Switch_SMD:SW_SPST_B3U-1000P" H 6850 3250 50  0001 C CNN
F 3 "~" H 6850 3250 50  0001 C CNN
	1    6850 3250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR013
U 1 1 5F60CF7A
P 7100 3250
F 0 "#PWR013" H 7100 3000 50  0001 C CNN
F 1 "GND" V 7105 3122 50  0000 R CNN
F 2 "" H 7100 3250 50  0001 C CNN
F 3 "" H 7100 3250 50  0001 C CNN
	1    7100 3250
	0    -1   1    0   
$EndComp
Wire Wire Line
	6650 3250 6500 3250
Connection ~ 6500 3250
Wire Wire Line
	6500 3250 6500 3450
Wire Wire Line
	5900 3450 6500 3450
$Comp
L power:GND #PWR019
U 1 1 5F62082A
P 8100 5500
F 0 "#PWR019" H 8100 5250 50  0001 C CNN
F 1 "GND" H 8105 5327 50  0000 C CNN
F 2 "" H 8100 5500 50  0001 C CNN
F 3 "" H 8100 5500 50  0001 C CNN
	1    8100 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8200 5100 8100 5100
Wire Wire Line
	8200 5200 8100 5200
Wire Wire Line
	8100 5200 8100 5500
Wire Wire Line
	7100 3950 7100 5300
Wire Wire Line
	7100 5300 7400 5300
Wire Wire Line
	7150 3850 7150 5400
Wire Wire Line
	7150 5400 7500 5400
$Comp
L Device:R_Small R6
U 1 1 5F625258
P 7500 5100
F 0 "R6" H 7441 5146 50  0000 R CNN
F 1 "1.5k" H 7441 5055 50  0000 R CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 7500 5100 50  0001 C CNN
F 3 "~" H 7500 5100 50  0001 C CNN
	1    7500 5100
	-1   0    0    -1  
$EndComp
$Comp
L Device:R_Small R5
U 1 1 5F625AD3
P 7400 5100
F 0 "R5" H 7459 5146 50  0000 L CNN
F 1 "1.5k" H 7459 5055 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 7400 5100 50  0001 C CNN
F 3 "~" H 7400 5100 50  0001 C CNN
	1    7400 5100
	-1   0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR014
U 1 1 5F626801
P 7450 4950
F 0 "#PWR014" H 7450 4800 50  0001 C CNN
F 1 "+3.3V" H 7465 5123 50  0000 C CNN
F 2 "" H 7450 4950 50  0001 C CNN
F 3 "" H 7450 4950 50  0001 C CNN
	1    7450 4950
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7400 5000 7400 4950
Wire Wire Line
	7400 4950 7450 4950
Wire Wire Line
	7500 4950 7500 5000
Connection ~ 7450 4950
Wire Wire Line
	7450 4950 7500 4950
Wire Wire Line
	7400 5200 7400 5300
Connection ~ 7400 5300
Wire Wire Line
	7400 5300 8200 5300
Wire Wire Line
	7500 5200 7500 5400
Connection ~ 7500 5400
Wire Wire Line
	7500 5400 8200 5400
Text Label 6000 3950 0    50   ~ 0
I2C-SCL
Text Label 6000 3850 0    50   ~ 0
I2C-SDA
$Comp
L power:+3.3V #PWR05
U 1 1 5F62BAE3
P 4500 3650
F 0 "#PWR05" H 4500 3500 50  0001 C CNN
F 1 "+3.3V" V 4515 3778 50  0000 L CNN
F 2 "" H 4500 3650 50  0001 C CNN
F 3 "" H 4500 3650 50  0001 C CNN
	1    4500 3650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4700 3650 4500 3650
$Comp
L Device:R_Small R2
U 1 1 5F62DBA1
P 6050 4700
F 0 "R2" H 6109 4746 50  0000 L CNN
F 1 "10k" H 6109 4655 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 6050 4700 50  0001 C CNN
F 3 "~" H 6050 4700 50  0001 C CNN
	1    6050 4700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 5F62E441
P 6050 4800
F 0 "#PWR010" H 6050 4550 50  0001 C CNN
F 1 "GND" H 6055 4627 50  0000 C CNN
F 2 "" H 6050 4800 50  0001 C CNN
F 3 "" H 6050 4800 50  0001 C CNN
	1    6050 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6050 4350 6050 4600
Wire Wire Line
	5900 4350 6050 4350
Wire Wire Line
	7100 3250 7050 3250
$Comp
L Device:R_Small R3
U 1 1 5F64BC0D
P 6150 3100
F 0 "R3" H 6209 3146 50  0000 L CNN
F 1 "10k" H 6209 3055 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 6150 3100 50  0001 C CNN
F 3 "~" H 6150 3100 50  0001 C CNN
	1    6150 3100
	-1   0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR011
U 1 1 5F64C0F7
P 6150 3000
F 0 "#PWR011" H 6150 2850 50  0001 C CNN
F 1 "+3.3V" H 6165 3173 50  0000 C CNN
F 2 "" H 6150 3000 50  0001 C CNN
F 3 "" H 6150 3000 50  0001 C CNN
	1    6150 3000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6150 3650 6150 3200
Wire Wire Line
	5900 3650 6150 3650
NoConn ~ 5900 4050
NoConn ~ 5900 4150
NoConn ~ 5900 4250
$Comp
L Connector_Generic:Conn_01x02 J1
U 1 1 5F656C3B
P 2050 3900
F 0 "J1" H 2130 3892 50  0000 L CNN
F 1 "2xAA" H 2130 3801 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 2050 3900 50  0001 C CNN
F 3 "~" H 2050 3900 50  0001 C CNN
	1    2050 3900
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2250 3900 2350 3900
Wire Wire Line
	2750 3900 2750 3850
Wire Wire Line
	2250 4000 2350 4000
Wire Wire Line
	2750 4000 2750 4050
$Comp
L Device:C_Small C4
U 1 1 5F65D245
P 8250 4650
F 0 "C4" H 8342 4696 50  0000 L CNN
F 1 "10u" H 8342 4605 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8250 4650 50  0001 C CNN
F 3 "~" H 8250 4650 50  0001 C CNN
	1    8250 4650
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C3
U 1 1 5F65D24B
P 7950 4650
F 0 "C3" H 7858 4696 50  0000 R CNN
F 1 "100n" H 7858 4605 50  0000 R CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 7950 4650 50  0001 C CNN
F 3 "~" H 7950 4650 50  0001 C CNN
	1    7950 4650
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR018
U 1 1 5F65D251
P 8100 4500
F 0 "#PWR018" H 8100 4350 50  0001 C CNN
F 1 "+3.3V" H 8115 4673 50  0000 C CNN
F 2 "" H 8100 4500 50  0001 C CNN
F 3 "" H 8100 4500 50  0001 C CNN
	1    8100 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8100 4500 8100 4550
$Comp
L power:GND #PWR020
U 1 1 5F65D258
P 8250 4750
F 0 "#PWR020" H 8250 4500 50  0001 C CNN
F 1 "GND" H 8255 4577 50  0000 C CNN
F 2 "" H 8250 4750 50  0001 C CNN
F 3 "" H 8250 4750 50  0001 C CNN
	1    8250 4750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR015
U 1 1 5F65D25E
P 7950 4750
F 0 "#PWR015" H 7950 4500 50  0001 C CNN
F 1 "GND" H 7955 4577 50  0000 C CNN
F 2 "" H 7950 4750 50  0001 C CNN
F 3 "" H 7950 4750 50  0001 C CNN
	1    7950 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	7950 4550 8100 4550
Connection ~ 8100 4550
Wire Wire Line
	8100 4550 8250 4550
Wire Wire Line
	8100 4550 8100 5100
$Comp
L Mechanical:MountingHole H2
U 1 1 5F67C70E
P 1150 6650
F 0 "H2" H 1250 6650 50  0000 L CNN
F 1 "MountingHole_Pad" H 1250 6608 50  0001 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5" H 1150 6650 50  0001 C CNN
F 3 "~" H 1150 6650 50  0001 C CNN
	1    1150 6650
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H1
U 1 1 5F67D64C
P 850 6650
F 0 "H1" H 950 6650 50  0000 L CNN
F 1 "MountingHole_Pad" H 950 6608 50  0001 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5" H 850 6650 50  0001 C CNN
F 3 "~" H 850 6650 50  0001 C CNN
	1    850  6650
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:Fiducial FID1
U 1 1 5F686AC0
P 850 7250
F 0 "FID1" H 935 7250 50  0000 L CNN
F 1 "Fiducial" H 935 7205 50  0001 L CNN
F 2 "Fiducial:Fiducial_0.75mm_Mask1.5mm" H 850 7250 50  0001 C CNN
F 3 "~" H 850 7250 50  0001 C CNN
	1    850  7250
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:Fiducial FID2
U 1 1 5F687903
P 850 7400
F 0 "FID2" H 935 7400 50  0000 L CNN
F 1 "Fiducial" H 935 7355 50  0001 L CNN
F 2 "Fiducial:Fiducial_0.75mm_Mask1.5mm" H 850 7400 50  0001 C CNN
F 3 "~" H 850 7400 50  0001 C CNN
	1    850  7400
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:Fiducial FID3
U 1 1 5F687B8B
P 850 7550
F 0 "FID3" H 935 7550 50  0000 L CNN
F 1 "Fiducial" H 935 7505 50  0001 L CNN
F 2 "Fiducial:Fiducial_0.75mm_Mask1.5mm" H 850 7550 50  0001 C CNN
F 3 "~" H 850 7550 50  0001 C CNN
	1    850  7550
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG01
U 1 1 5F69C04F
P 2350 3900
F 0 "#FLG01" H 2350 3975 50  0001 C CNN
F 1 "PWR_FLAG" H 2350 4073 50  0000 C CNN
F 2 "" H 2350 3900 50  0001 C CNN
F 3 "~" H 2350 3900 50  0001 C CNN
	1    2350 3900
	1    0    0    -1  
$EndComp
Connection ~ 2350 3900
Wire Wire Line
	2350 3900 2550 3900
$Comp
L power:PWR_FLAG #FLG02
U 1 1 5F69D04F
P 2350 4000
F 0 "#FLG02" H 2350 4075 50  0001 C CNN
F 1 "PWR_FLAG" H 2350 4173 50  0000 C CNN
F 2 "" H 2350 4000 50  0001 C CNN
F 3 "~" H 2350 4000 50  0001 C CNN
	1    2350 4000
	-1   0    0    1   
$EndComp
Connection ~ 2350 4000
Wire Wire Line
	2350 4000 2550 4000
$Comp
L SilentWorks:ICT-Point P1
U 1 1 5F5F740B
P 2550 3900
F 0 "P1" H 2603 3915 31  0000 L CNN
F 1 "ICT-Point" H 2550 3855 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D1.0mm" H 2550 3900 50  0001 C CNN
F 3 "" H 2550 3900 50  0001 C CNN
	1    2550 3900
	1    0    0    -1  
$EndComp
Connection ~ 2550 3900
Wire Wire Line
	2550 3900 2750 3900
$Comp
L SilentWorks:ICT-Point P2
U 1 1 5F5F926C
P 2550 4000
F 0 "P2" H 2603 4015 31  0000 L CNN
F 1 "ICT-Point" H 2550 3955 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D1.0mm" H 2550 4000 50  0001 C CNN
F 3 "" H 2550 4000 50  0001 C CNN
	1    2550 4000
	1    0    0    -1  
$EndComp
Connection ~ 2550 4000
Wire Wire Line
	2550 4000 2750 4000
$Comp
L SilentWorks:ICT-Point P4
U 1 1 5F5F94B4
P 6150 4450
F 0 "P4" H 6203 4465 31  0000 L CNN
F 1 "ICT-Point" H 6150 4405 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D1.0mm" H 6150 4450 50  0001 C CNN
F 3 "" H 6150 4450 50  0001 C CNN
	1    6150 4450
	1    0    0    -1  
$EndComp
Connection ~ 6150 4450
Wire Wire Line
	6150 4450 6300 4450
$Comp
L SilentWorks:ICT-Point P3
U 1 1 5F5FAAEB
P 4350 3450
F 0 "P3" H 4403 3465 31  0000 L CNN
F 1 "ICT-Point" H 4350 3405 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_D1.0mm" H 4350 3450 50  0001 C CNN
F 3 "" H 4350 3450 50  0001 C CNN
	1    4350 3450
	1    0    0    -1  
$EndComp
Connection ~ 4350 3450
Wire Wire Line
	4350 3450 4250 3450
Wire Wire Line
	5900 3850 7150 3850
Wire Wire Line
	5900 3950 7100 3950
$EndSCHEMATC
