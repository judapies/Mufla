/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef MAIN_H
#define MAIN_H
#include "project.h"
#include <stdio.h>

/* Defines for ADC MUX channels */
#define TC_HOT_JN_CHANNEL               0
#define OFFSET_CHANNEL            		1

/* Filter coefficient for sensors */
#define MAX_FILTER_COEFF_SHIFT			8
#define TC_FILTER_COEFF_SHIFT           5

/* Filter Feed Forward Term. It is set equal to 640 ADC counts or 2°C.
 * 256 (2^MAX_FILTER_COEFF_SHIFT) is to make the comparison fair */
#define FILTER_FEEDFORWARD				((int32)1280 * 256)

/* Zero degree offset voltage of DS600(509 mV from sensor datasheet) */
#define ZERO_DEGREE_OFFSET_VOLTAGE      509000

/* Temperature coefficient for DS600(6.45 mV/°C from sensor datasheet) */
#define VOLTAGE_PER_DEGREE              645
#define SCALING							10

/* Voltage threshold (in uV) based on which broken thermocouple alert is made*/
#define BROKEN_TC_THRESHOLD				-10000    
    
/* Altura de cuadro en grafica*/
#define ALTURA                          44.0

/* Ancho de cuadro en grafica*/
#define ANCHO                           43.0    

/* Division para ajuste de valores en grafica ALTURA/200*/
#define DIVISION                        0.22
    

#define desvio                          3.0
    
/* Function to initialize components */
void InitializeComponents(void);

/* Function to measure cold junction temperature */
int32 MeasureColdJnSensorTemp(void);

/* Funtion to measure IC temperature */
int32 MeasureICTemp(void);

/* Funtion to measure thermistor temperature */
int32 MeasureThermistorTemp(void);

/* Function prototype for IIR filter */
int32 FilterSignal(int32 ADCSample);

/* Function prototype to measure offset voltage */
int32 MeasureOffset(void);

/*Interrupt handler function*/
CY_ISR_PROTO(switchISRhandler);

extern uint8 Password[4];
extern uint8 Derivativo,Integral,MediaMovil,Tciclo,Ajuste,Ajusten,dataReady;
extern float Ganancia,GananciaIn;
extern uint16 result;

typedef struct{
   uint8 TemperaturaDec;
   uint8 TemperaturaInt;
   int8 Humedad;
   uint8 Horas;
   uint8 Minutos;
   uint16 Temperatura;
}Personalizado;
//Personalizado rampas[20];

typedef struct{
   Personalizado rampas[20];
}Program;
Program Programas[5];

void leeEEPROM(void);

uint8 extraeDecimal(float data);

float inttofloat(uint8 decimal);

uint8 convert8(uint16 data,uint8 nible);
uint16 convert16(uint8 nibleh,uint8 niblel);

#endif

/* [] END OF FILE */

/* [] END OF FILE */
