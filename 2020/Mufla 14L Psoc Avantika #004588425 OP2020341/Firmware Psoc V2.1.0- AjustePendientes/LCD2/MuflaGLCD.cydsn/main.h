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
    
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) ((bitvalue) ? bitSet(value, bit) : bitClear(value, bit))    

/* Defines for ADC MUX channels */
#define TC_HOT_JN_CHANNEL               0
#define OFFSET_CHANNEL            		1

/* Filter coefficient for sensors */
#define MAX_FILTER_COEFF_SHIFT			8
#define TC_FILTER_COEFF_SHIFT           5

/* Filter Feed Forward Term. It is set equal to 640 ADC counts or 2°C.
 * 256 (2^MAX_FILTER_COEFF_SHIFT) is to make the comparison fair */
#define FILTER_FEEDFORWARD				((int32)640 * 256)

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
    
#define desvio                          5.0
    
#define Pasos 16
    
    
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


//----------Variables------------------------------------------------------------//
extern int8  Testerilizacion[4];   // Tiempo transcurrido
extern int8  Testerilizacionp[4];   // Tiempo transcurrido
extern int8  Contrasena[4];      // Contrasena de 4 digitos

extern int16 offset;
extern int16 coldJnTemp;
extern int32 tcColdJnuVolt;
extern int32 hotJnCount;
extern int32 filHotJnCount;
extern int32 tcHotJnuVolt;
extern int32 tcuVolt;
extern int32 tcTemp;
extern char displayStr[100];	
extern uint8 first;
extern uint8 const CYCODE LCD_customFonts[];  
extern uint8_t i,pwm1,pwm2,n,PantallaPrincipal,Programa,Opcion2,Derivativo,Integral,MediaMovil,Tciclo,Ajuste,Ajusten;
extern uint16 ctr,Setpoint;
extern uint32_t output,tGrafica;
extern short flagButton0,flagButton1,flagButton2,flagButton3,dibujo,pulsoAumentar,pulsoDisminuir,flagBorrado,Control,estadoSSR,estadoBuzzer,estadoVentilador;
extern short Flanco,Flanco1,Flanco2,Flanco3,flagFinalizo,visual,refresh,refresh2;
extern float valor,pendiente,b,Ganancia,GananciaIn;

#define RX_BUFFER_SIZE  10
extern char Rx_Buffer[RX_BUFFER_SIZE+1];
extern char Rx_Buffer2[RX_BUFFER_SIZE+1];
extern uint8 RX_Wr_Index;
extern uint8 RX_Rd_Index;
extern uint8 RX_Counter;

extern char Rx_Buffer3[RX_BUFFER_SIZE+1];
extern char Rx_Buffer4[RX_BUFFER_SIZE+1];
extern uint8 RX_Wr_Index2;
extern uint8 RX_Rd_Index2;
extern uint8 RX_Counter2;

extern float promedio[90];

extern uint8 Dato2,Dato3,Dato_Exitoso,Dato_Exitoso2,Menu,MenuAntt,MenuAnt,Menu2,MenuAntt2,z,TipoClave,Segmentos,CicloLibre,Opcion,h,l,ProgramaSeleccionado;
extern short Inicio,Inicio2,EstadoPaso,entro,GuardaEEPROM,rampa,Minutos,Horas,flagTemperatura,Test,x1,x10,x100;
extern uint8 Password[4];
extern uint8 tiempo_esterilizacion,y,hEst,mEst,hEstp,mEstp,Segmento,conteo,unidad,Pausa,conteoTecla,controlT;
extern uint16 tiempos,tiempos3,tiempoctrl,Tc,Tp,tiemporeset,TiempoControl,tiemporecta,tiemposetpoint,tinicio,tiempocalentamiento,tiempoup,tiempodn,Coeficiente,AlTem;
extern short mensaje,Lectura2,flag_setpoint,flag_tiempocalentamiento,Lectura,flagup,flagdn,flagRampa,flag_calcula;
extern short Finalizo,conteoPregunta,grafica,imprime,flag_display,Dibuja,Desconectado,flag_clear3,flag_ganancia,flag_clear2;
extern float X[2];
extern float Temperatura,Aumento,PendienteActual,DifPendiente,TempeInicial,Pendiente,TemperaturaIdeal,PotenciaEntregada,Tpulso,penInicial,penMufla;
extern float error,Potencia;
extern uint16 TiempoActual,MinutosActual,HorasActual,Tiempo,TiempoRampa,tPregunta,b16,tup,tdown,tright,tleft;
extern uint8 dataReady,Alarma,gananciaIn;
extern uint16 result;
extern float Prom,V0,Temperature,m;
extern int16 control1,control2,control3,patron1,patron2,patron3;
extern int16 Temperatura1[Pasos*5];
extern int16 Horas1[Pasos*5];
extern int16 Minutos1[Pasos*5];
extern uint8 m_int,m_dec,b_dec;
extern int8  Tempocp[4];

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
   char Letra[30];
}MENUU;
//----------Variables------------------------------------------------------------//


MENUU Menus;

void leeEEPROM(void);

uint8 extraeDecimal(float data);

float inttofloat(uint8 decimal);

uint8 convert8(uint16 data,uint8 nible);
uint16 convert16(uint8 nibleh,uint8 niblel);

#endif

/* [] END OF FILE */

/* [] END OF FILE */
