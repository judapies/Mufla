
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
#include <project.h>
#include <stdio.h>
#include <main.h>
#include <math.h>
#include <stdlib.h>

#if defined (__GNUC__)
    /* Add an explicit reference to the floating point printf library */
    /* to allow the usage of floating point conversion specifiers. */
    /* This is not linked in by default with the newlib-nano library. */
    asm (".global _printf_float");
#endif

#define OFF 0
#define ON  1

int16 offset;
int16 coldJnTemp;
int32 tcColdJnuVolt;
int32 hotJnCount;
int32 filHotJnCount;
int32 tcHotJnuVolt;
int32 tcuVolt;
int32 tcTemp;
char displayStr[100];	
uint8 first = 0;
extern uint8 const CYCODE LCD_customFonts[];  
uint8_t i,pwm1,pwm2,n,PantallaPrincipal,Programa,Opcion2,Derivativo,Integral,MediaMovil,Tciclo,Ajuste,Ajusten;
uint16 ctr,Setpoint;
uint32_t output,tGrafica;
short flagButton0,flagButton1,flagButton2,flagButton3,dibujo,pulsoAumentar,pulsoDisminuir,flagBorrado,Control,estadoSSR,estadoBuzzer,estadoVentilador,flagFinalizo;
float valor,pendiente,b,Ganancia,GananciaIn;

#define RX_BUFFER_SIZE  10
char Rx_Buffer[RX_BUFFER_SIZE+1];
char Rx_Buffer2[RX_BUFFER_SIZE+1];
uint8 RX_Wr_Index=0;
uint8 RX_Rd_Index=0;
uint8 RX_Counter=0;

char Rx_Buffer3[RX_BUFFER_SIZE+1];
char Rx_Buffer4[RX_BUFFER_SIZE+1];
uint8 RX_Wr_Index2=0;
uint8 RX_Rd_Index2=0;
uint8 RX_Counter2=0;

uint8 i2cbuf[4];
uint8 iconoGrande[15]={75,76,77,78,79,27,45};
uint8 iconoPeque[15]={80,81,82,83,84,37,16};
char const texto[9][20]={"PROGRAMA 1","PROGRAMA 2","PROGRAMA 3","PROGRAMA 4","PROGRAMA 5","TEST","CLAVE"};
float promedio[90];

uint8 Dato2,Dato3,Dato_Exitoso,Dato_Exitoso2,Menu,MenuAntt,MenuAnt,Menu2,MenuAntt2,z,TipoClave,Segmentos,CicloLibre,Opcion,h,l;
short Inicio,Inicio2,EstadoPaso,entro,GuardaEEPROM,rampa,Minutos,Horas,flagTemperatura,Test,x1,x10,x100;
uint8 Password[4];
uint8 tiempo_esterilizacion,y,hEst,mEst,hEstp,mEstp,Segmento,conteo;
uint16 tiempos,tiempos3,tiempoctrl,Tc,Tp,tiemporeset,TiempoControl,tiemporecta,tiemposetpoint,tinicio,tiempocalentamiento,tiempoup,tiempodn;
short mensaje,Lectura2,flag_setpoint,flag_tiempocalentamiento,Lectura,flagup,flagdn,flagRampa,flag_calcula;
short Finalizo,conteoPregunta,grafica,imprime;
float X[2]={0.0,0.0};
float Temperatura,Aumento,PendienteActual,DifPendiente,TempeInicial,Pendiente,TemperaturaIdeal,PotenciaEntregada,Tpulso,penInicial,penMufla=50.0;
float error,Potencia;
uint16 TiempoActual,MinutosActual,HorasActual,Tiempo,TiempoRampa,tPregunta;
uint8 dataReady = 0u;
uint16 result;
float Prom,V0,Temperature;

CY_ISR(ADC_SAR_ISR_LOC)
{
        result = CY_GET_REG16(ADC_SAR_SAR_WRK_PTR);
        dataReady = 1u;
}

void ActualizaRecta(){
   X[y]=Temperatura;
   y++;
   if(y>=2){
      y=0;
      Aumento=X[1]-X[0];
      HorasActual=hEst;
      MinutosActual=mEst;
      TiempoActual=Tiempo-((HorasActual*3600)+(MinutosActual*60)+tinicio);
      PendienteActual=((Setpoint-Temperatura)/TiempoActual);
      DifPendiente=(PendienteActual*20)-Aumento;
      DifPendiente=DifPendiente*5;
   }
}

void Controla(){
//-------------------------------Control de Calentamiento------------------------------------------------------------//         
   float tmp=0.0;
      if(tiemporecta>=10){
         ActualizaRecta();
         tiemporecta=0;
      }
    
      if(Segmento==0){
         if(flag_calcula==0){
            Tiempo=(Programas[Programa-1].rampas[0].Horas*3600)+(Programas[Programa-1].rampas[0].Minutos*60);         
            Setpoint=Programas[Programa-1].rampas[0].Temperatura;
            Pendiente=(((float)Setpoint-TempeInicial)/(float)Tiempo);
            hEstp=Programas[Programa-1].rampas[0].Horas;
            mEstp=Programas[Programa-1].rampas[0].Minutos;
            flag_calcula=1;
            Ganancia=GananciaIn*(Setpoint/100);
         }
      }else{
         if(flag_calcula==0){
            Tiempo=(Programas[Programa-1].rampas[Segmento].Horas*3600)+(Programas[Programa-1].rampas[Segmento].Minutos*60);         
            Setpoint=Programas[Programa-1].rampas[Segmento].Temperatura;
            Pendiente=(((float)Programas[Programa-1].rampas[Segmento].Temperatura-(float)Programas[Programa-1].rampas[Segmento-1].Temperatura)/(float)Tiempo);
            hEstp=Programas[Programa-1].rampas[Segmento].Horas;
            mEstp=Programas[Programa-1].rampas[Segmento].Minutos;
            flag_calcula=1;
            Ganancia=GananciaIn*(Setpoint/100);
         }
      }
   
  
   if(Pendiente!=0){// Si Esta realizando una Rampa debe realizar el control de acuerdo a la pendiente de Temperatura. Aprox 10°C * min Maximo Valor de Pendiente.
      //PendienteActual=((Setpoint-Temperatura)/TiempoActual);
      //PendienteMinutos=PendienteActual*60;
      //PotenciaEntregada=((PendienteMinutos+DifPendiente)*100)/penMufla;
      TiempoRampa=((hEst*3600)+(mEst*60)+tinicio);
      if(Segmento==0){
         TemperaturaIdeal=(Pendiente*TiempoRampa)+TempeInicial;
      }else{
         TemperaturaIdeal=(Pendiente*TiempoRampa)+Programas[Programa-1].rampas[Segmento-1].Temperatura;
      }
      PotenciaEntregada=((TemperaturaIdeal-Temperatura)*100)/penMufla;
// Pendiente determinar si el comportamiento de la mufla es lineal 10°C/min o si tiene un comportamiento diferente.         
      
      if(Temperatura<450 && Temperatura>=200)
         penMufla=penInicial*0.9;
         
      if(Temperatura<600 && Temperatura>=450)
         penMufla=penInicial*0.8;
         
      if(Temperatura<900 && Temperatura>=600)
         penMufla=penInicial*0.7;  
         
      if(Temperatura<=1200 && Temperatura>=900)
         penMufla=penInicial*0.6;   
      
      if(PotenciaEntregada>100.0){
         PotenciaEntregada=100.0;
      }
      
      if(PotenciaEntregada<0){
         PotenciaEntregada=0;
      }
      
      Tpulso=(PotenciaEntregada*Tciclo)/100;  
   }else{
               
      error=desvio+Setpoint-Temperatura;      // Cálculo del error ******
      
      if(error>3.0 && Aumento<1){
         if(Potencia<90.0){
            if(TiempoControl>=Derivativo){
               TiempoControl=0;
               if(Aumento<-2){
                  Ganancia+=Integral+1;
               }else{
                  if(Aumento<-1){
                     Ganancia+=Integral;
                  }else{
                     Ganancia+=Integral-0.5;
                  }
               }
            }
         }
      }
      
      if(Ganancia<0.5){
         Ganancia=0.5;
      }
      
      
      if(error<0.0)     // Anti Wind_DOWN    
         error=0.0;
   
      tmp=error*Ganancia;  // Control Proporcional.
            
      if(tmp>Setpoint)   // Anti Wind-UP      
         tmp=Setpoint;
                  
      if(tmp<0.0)     // Anti Wind_DOWN    
         tmp=0.0;
      
      // Tpulso(t)= Tciclo*(y(t)-ymin)/(ymax - ymin); calculo de ciclo util para control de resistencia.ymax=140.ymin=0;
      // Tpulso es float, hay que pasar ese ciclo a una salida de un puerto usando el TMR1.
      Tpulso=(tmp/Setpoint)*Tciclo; 
      
   }
   Tp=(Tpulso/0.002);
   Tc=(Tciclo/0.002);
   Control=1;     
//--------------------------------------------------------------------------------------------------------------------//   
  
}

void CuentaTiempo(){
      if(tinicio>=60){
         tinicio=0;
         mEst+=1.0;
         if(mEst>59){
            mEst=0;
            hEst+=1.0;
         }
      }
}

void iniciaConteo(){
   if(Pendiente!=0){
        if(flagRampa)
            tiempo_esterilizacion=1;
        else
            tiempo_esterilizacion=0;
   }else if(Pendiente==0){
      if(flagTemperatura && flagRampa)
         tiempo_esterilizacion=1;
      else
         tiempo_esterilizacion=0;
   }
}

void escribeDisplay1(char Buffer[30]){
    UART_1_PutString(Buffer);
    CyDelayUs(10);
    UART_1_PutChar(0xFF);
    CyDelayUs(10);
    UART_1_PutChar(0xFF);
    CyDelayUs(10);
    UART_1_PutChar(0xFF);
    CyDelayUs(10);
}

void escribeDisplay2(char Buffer[30]){
    UART_2_PutString(Buffer);
    CyDelayUs(10);
    UART_2_PutChar(0xFF);
    CyDelayUs(10);
    UART_2_PutChar(0xFF);
    CyDelayUs(10);
    UART_2_PutChar(0xFF);
    CyDelayUs(10);
}

void dibujaGrafico(){
             
                //Segmento 1
                pendiente=(float)(Programas[Programa-1].rampas[0].Temperatura-Temperatura)/ANCHO;
                b=Temperatura;
                
                for(n=0;n<ANCHO;n++){
                    valor=(pendiente*n)+b;
                    sprintf(displayStr,"add 1,0,%u",(uint8)(valor*DIVISION));
                    escribeDisplay1(displayStr);  
                    CyDelayUs(1);
                }  
                                        
                //Segmento 2
                if(Programas[Programa-1].rampas[1].Horas!=0 || Programas[Programa-1].rampas[1].Minutos!=0){
                    pendiente=(float)(Programas[Programa-1].rampas[1].Temperatura-Programas[Programa-1].rampas[0].Temperatura)/ANCHO;
                    b=Programas[Programa-1].rampas[0].Temperatura;
                }else{
                    pendiente=0;
                    b=0;
                }
                for(n=0;n<ANCHO;n++){
                    valor=(pendiente*n)+b;
                    sprintf(displayStr,"add 1,0,%u",(uint8)(valor*DIVISION));
                    escribeDisplay1(displayStr);  
                    CyDelayUs(1);
                }

                //Segmento 3
                if(Programas[Programa-1].rampas[2].Horas!=0 || Programas[Programa-1].rampas[2].Minutos!=0){
                    pendiente=(float)(Programas[Programa-1].rampas[2].Temperatura-Programas[Programa-1].rampas[1].Temperatura)/ANCHO;
                    b=Programas[Programa-1].rampas[1].Temperatura;
                }else{
                    pendiente=0;
                    b=0;
                }  
                  
                for(n=0;n<ANCHO;n++){
                    valor=(pendiente*n)+b;
                    sprintf(displayStr,"add 1,0,%u",(uint8)(valor*DIVISION));
                    escribeDisplay1(displayStr);  
                    CyDelayUs(1);
                }
            
                //Segmento 4
                if(Programas[Programa-1].rampas[3].Horas!=0 || Programas[Programa-1].rampas[3].Minutos!=0){
                    pendiente=(float)(Programas[Programa-1].rampas[3].Temperatura-Programas[Programa-1].rampas[2].Temperatura)/ANCHO;
                    b=Programas[Programa-1].rampas[2].Temperatura;
                }else{
                    pendiente=0;
                    b=0;
                } 
                    
                for(n=0;n<ANCHO;n++){
                    valor=(pendiente*n)+b;
                    sprintf(displayStr,"add 1,0,%u",(uint8)(valor*DIVISION));
                    escribeDisplay1(displayStr);  
                    CyDelayUs(1);
                }
             
            
                //Segmento 5
                if(Programas[Programa-1].rampas[4].Horas!=0 || Programas[Programa-1].rampas[4].Minutos!=0){
                    pendiente=(float)(Programas[Programa-1].rampas[4].Temperatura-Programas[Programa-1].rampas[3].Temperatura)/ANCHO;
                    b=Programas[Programa-1].rampas[3].Temperatura;
                }else{
                    pendiente=0;
                    b=0;
                }   
                   
                for(n=0;n<ANCHO;n++){
                    valor=(pendiente*n)+b;
                    sprintf(displayStr,"add 1,0,%u",(uint8)(valor*DIVISION));
                    escribeDisplay1(displayStr);  
                    CyDelayUs(1);
                }
             
             
            
                //Segmento 6
                if(Programas[Programa-1].rampas[5].Horas!=0 || Programas[Programa-1].rampas[5].Minutos!=0){
                    pendiente=(float)(Programas[Programa-1].rampas[5].Temperatura-Programas[Programa-1].rampas[4].Temperatura)/ANCHO;
                    b=Programas[Programa-1].rampas[4].Temperatura;
                }else{
                    pendiente=0;
                    b=0;
                }  
                for(n=0;n<ANCHO;n++){
                    valor=(pendiente*n)+b;
                    sprintf(displayStr,"add 1,0,%u",(uint8)(valor*DIVISION));
                    escribeDisplay1(displayStr);  
                    CyDelayUs(1);
                }
            
                //Segmento 7
                if(Programas[Programa-1].rampas[6].Horas!=0 || Programas[Programa-1].rampas[6].Minutos!=0){
                    pendiente=(float)(Programas[Programa-1].rampas[6].Temperatura-Programas[Programa-1].rampas[5].Temperatura)/ANCHO;
                    b=Programas[Programa-1].rampas[5].Temperatura;
                }else{
                    pendiente=0;
                    b=0;
                }   
                for(n=0;n<ANCHO;n++){
                    valor=(pendiente*n)+b;
                    sprintf(displayStr,"add 1,0,%u",(uint8)(valor*DIVISION));
                    escribeDisplay1(displayStr);  
                    CyDelayUs(1);
                }
            
                //Segmento 8
                if(Programas[Programa-1].rampas[7].Horas!=0 || Programas[Programa-1].rampas[7].Minutos!=0){
                    pendiente=(float)(Programas[Programa-1].rampas[7].Temperatura-Programas[Programa-1].rampas[6].Temperatura)/ANCHO;
                    b=Programas[Programa-1].rampas[6].Temperatura;
                }else{
                    pendiente=0;
                    b=0;
                }    
                for(n=0;n<ANCHO;n++){
                    valor=(pendiente*n)+b;
                    sprintf(displayStr,"add 1,0,%u",(uint8)(valor*DIVISION));
                    escribeDisplay1(displayStr);  
                    CyDelayUs(1);
                }
            
                //Segmento 9
                if(Programas[Programa-1].rampas[8].Horas!=0 || Programas[Programa-1].rampas[8].Minutos!=0){
                    pendiente=(float)(Programas[Programa-1].rampas[8].Temperatura-Programas[Programa-1].rampas[7].Temperatura)/ANCHO;
                    b=Programas[Programa-1].rampas[7].Temperatura;
                }else{
                    pendiente=0;
                    b=0;
                }   
                for(n=0;n<ANCHO;n++){
                    valor=(pendiente*n)+b;
                    sprintf(displayStr,"add 1,0,%u",(uint8)(valor*DIVISION));
                    escribeDisplay1(displayStr);  
                    CyDelayUs(1);
                }
            
                //Segmento 10
                if(Programas[Programa-1].rampas[9].Horas!=0 || Programas[Programa-1].rampas[9].Minutos!=0){
                    pendiente=(float)(Programas[Programa-1].rampas[9].Temperatura-Programas[Programa-1].rampas[8].Temperatura)/ANCHO;
                    b=Programas[Programa-1].rampas[8].Temperatura;
                }else{
                    pendiente=0;
                    b=0;
                }
                
                for(n=0;n<ANCHO;n++){
                    valor=(pendiente*n)+b;
                    sprintf(displayStr,"add 1,0,%u",(uint8)(valor*DIVISION));
                    escribeDisplay1(displayStr);  
                    CyDelayUs(1);
                }                
}

void LeeDisplay(void){    
    if(Dato_Exitoso==5){
         Rx_Buffer[0]=Rx_Buffer2[0];
         Rx_Buffer[1]=Rx_Buffer2[1];
         Rx_Buffer[2]=Rx_Buffer2[2];
         Rx_Buffer[3]=Rx_Buffer2[3];
         Rx_Buffer[4]=Rx_Buffer2[4];
         Rx_Buffer[5]=Rx_Buffer2[5];
         Rx_Buffer[6]=Rx_Buffer2[6];
         Rx_Buffer[7]=Rx_Buffer2[7];
         Rx_Buffer[8]=Rx_Buffer2[8];
         Rx_Buffer[9]=Rx_Buffer2[9];
         
      if(Rx_Buffer[3]==0x01){// Pregunta por la pagina en la que esta el display,01 es Contraseña de Acceso
         Menu=0;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(Rx_Buffer[3]==0x02){//02 es Menu Principal con PantallaPriincipal=0, y con PantallaPrincipal=1 es Ciclo Libre
         Menu=1;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(Rx_Buffer[3]==0x04){//03 es Tiempo Esterilizacion
         Menu=2;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(Rx_Buffer[3]==0x05){//05 es Temperatura
         Menu=3;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(Rx_Buffer[3]==0x06){//06 es Nivel
         Menu=5;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(Rx_Buffer[3]==0x07){//07 es Test de Componentes
         Menu=6;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(Rx_Buffer[3]==0x08){//08 es Fecha y hora
         Menu=7;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(Rx_Buffer[3]==0x09){//09 es Rampas
         Menu=8;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(Rx_Buffer[3]==0x0a){//0a es Pulsos de Vacio
         Menu=9;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(Rx_Buffer[3]==0x0f){//0f es Recibe caracteres de contraseña desde display
         
      }else if(Rx_Buffer[3]==0x1a){//1a es Menu de Funcionamiento
         Menu=20;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(Rx_Buffer[3]==0x1b){//1b es Menu de clave correcta
         Menu=15;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(Rx_Buffer[3]==0x1c){//1c es Menu de clave incorrecta
         Menu=16;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(Rx_Buffer[3]==0x1d){//1d es Menu de Eliminado Programa
         Menu=17;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(Rx_Buffer[3]==0x1e){//1d es Menu de Pregunta
         Menu=18;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(Rx_Buffer[3]==0x2c){//2c es Menu de Configuracion de Parametros
         Menu=100;
         if(MenuAntt!=Menu)
            MenuAntt=Menu;
      }else if(Rx_Buffer[3]==0xcc){//cc es Menu de Bienvenida
         Menu=120;
         //if(MenuAntt!=120)
            //reset_cpu();
      }
   }else{
      for(z=0;z<RX_BUFFER_SIZE;z++){
            //Rx_Buffer[z]=0;
            //Rx_Buffer2[z]=0;
         }
   }

       if(Dato_Exitoso2==5){
         Rx_Buffer3[0]=Rx_Buffer4[0];
         Rx_Buffer3[1]=Rx_Buffer4[1];
         Rx_Buffer3[2]=Rx_Buffer4[2];
         Rx_Buffer3[3]=Rx_Buffer4[3];
         Rx_Buffer3[4]=Rx_Buffer4[4];
         Rx_Buffer3[5]=Rx_Buffer4[5];
         Rx_Buffer3[6]=Rx_Buffer4[6];
         Rx_Buffer3[7]=Rx_Buffer4[7];
         Rx_Buffer3[8]=Rx_Buffer4[8];
         Rx_Buffer3[9]=Rx_Buffer4[9];
         
      if(Rx_Buffer3[3]==0x01){// Pregunta por la pagina en la que esta el display,01 es Contraseña de Acceso
         Menu2=0;
         if(MenuAntt2!=Menu2)
            MenuAntt2=Menu2;
      }else if(Rx_Buffer3[3]==0x02){//02 es Menu Principal con PantallaPriincipal=0, y con PantallaPrincipal=1 es Ciclo Libre
         Menu2=1;
         if(MenuAntt2!=Menu2)
            MenuAntt2=Menu2;
      }else if(Rx_Buffer3[3]==0x04){//03 es Tiempo Esterilizacion
         Menu2=2;
         if(MenuAntt2!=Menu2)
            MenuAntt2=Menu2;
      }else if(Rx_Buffer3[3]==0x05){//05 es Temperatura
         Menu2=3;
         if(MenuAntt2!=Menu2)
            MenuAntt2=Menu2;
      }else if(Rx_Buffer3[3]==0x06){//06 es Nivel
         Menu2=5;
         if(MenuAntt2!=Menu2)
            MenuAntt2=Menu2;
      }else if(Rx_Buffer3[3]==0x07){//07 es Test de Componentes
         Menu2=6;
         if(MenuAntt2!=Menu2)
            MenuAntt2=Menu2;
      }else if(Rx_Buffer3[3]==0x08){//08 es Fecha y hora
         Menu2=7;
         if(MenuAntt2!=Menu2)
            MenuAntt2=Menu2;
      }else if(Rx_Buffer3[3]==0x09){//09 es Rampas
         Menu2=8;
         if(MenuAntt2!=Menu2)
            MenuAntt2=Menu2;
      }else if(Rx_Buffer3[3]==0x0a){//0a es Grafica
         Menu2=9;
         if(MenuAntt2!=Menu2)
            MenuAntt2=Menu2;
      }else if(Rx_Buffer3[3]==0x0f){//0f es Recibe caracteres de contraseña desde display
         
      }else if(Rx_Buffer3[3]==0x1a){//1a es Menu de Funcionamiento
         Menu2=20;
         if(MenuAntt2!=Menu2)
            MenuAntt2=Menu2;
      }else if(Rx_Buffer3[3]==0x1b){//1b es Menu de clave correcta
         Menu2=15;
         if(MenuAntt2!=Menu2)
            MenuAntt2=Menu2;
      }else if(Rx_Buffer3[3]==0x1c){//1c es Menu de clave incorrecta
         Menu2=16;
         if(MenuAntt2!=Menu2)
            MenuAntt2=Menu2;
      }else if(Rx_Buffer3[3]==0x2c){//2c es Menu de Configuracion de Parametros
         Menu2=100;
         if(MenuAntt2!=Menu2)
            MenuAntt2=Menu2;
      }else if(Rx_Buffer3[3]==0xcc){//cc es Menu de Bienvenida
         Menu2=120;
         //if(MenuAntt!=120)
            //reset_cpu();
      }
   }else{
         for(z=0;z<RX_BUFFER_SIZE;z++){
            //Rx_Buffer[z]=0;
            //Rx_Buffer2[z]=0;
         }
   }
}

void controlaRampa(){
      if(Temperatura>=Setpoint-10.0 && Temperatura<=Setpoint+40){
         if(Pendiente!=0){
            escribeDisplay2("t1.txt=\"Controlando\"");
            flagTemperatura=ON;            
         }
         
         if(Pendiente==0){
            escribeDisplay2("t1.txt=\"Meseta\"");
            flagTemperatura=ON;
         }
         Controla();
         flag_tiempocalentamiento=0;
      }else{
         if(Pendiente!=0){
            if(Pendiente>0)
                escribeDisplay2("t1.txt=\"Rampa +\"");
            if(Pendiente<0)
                escribeDisplay2("t1.txt=\"Rampa -\"");    
            flagTemperatura=ON;            
         }
         if(Pendiente==0){
            escribeDisplay2("t1.txt=\"Calentando\"");
            flagTemperatura=OFF;            
         }
         Controla();
      }
}

float leeTermocupla(uint8 media){
    Prom=0.0;
    /* Measure cold junction temperature */
	coldJnTemp = MeasureColdJnSensorTemp();
	/* Convert cold junction temp to cold junction compensation voltage */
	tcColdJnuVolt = Thermocouple_1_GetVoltage(coldJnTemp);
	/* Read thermocouple hot junction voltage */
	//ADCMux_FastSelect(TC_HOT_JN_CHANNEL);
	ADC_SelectConfiguration(ADC_TC_Config,1);
	ADC_StartConvert();
    /*Note in this example a blocking call is used, this can be replaced
    with a polling method is required in your application*/
	ADC_IsEndConversion(ADC_WAIT_FOR_RESULT); 
	ADC_StopConvert();
	hotJnCount = ADC_GetResult32();
	/* Read ground reference voltage */
	offset = 0;//MeasureOffset();
	/* Correlated double sampling to remove ADC offset */
	hotJnCount -= offset;
	/* Filter signal */
	filHotJnCount = FilterSignal(hotJnCount);
	/* Find thermo-emf in microvolts */
	tcHotJnuVolt = ADC_CountsTo_uVolts(filHotJnCount);
	/* Perform Cold junction compensation */
	tcuVolt = tcColdJnuVolt + tcHotJnuVolt;
	/* Convert thermo-emf to temperature */
	tcTemp = Thermocouple_1_GetTemperature(tcuVolt);
    V0=(float)(tcTemp/100)+(float)(tcTemp-((tcTemp/100)*100))/100;
            
    if(l>(media-1))
        l=0;
   //promedio[l]=Temperature;l++;
    promedio[l]=V0;l++;
         
    for(h=0;h<media;h++){
        Prom+=promedio[h];
    } 
    Prom=Prom/media;   
   
    Prom+=((float)Ajuste-(float)Ajusten);
   
    return Prom;
    //return Temperature;
}

CY_ISR(Reloj2){ 
    //CapSense_DisplayState();
    tiempos++;
    tiempos3++;
    tiempoctrl++;
    
    if(Control==1 && tcHotJnuVolt > BROKEN_TC_THRESHOLD){
        if(tiempoctrl>=Tc) // Si el tiempo de control es mayor al tiempo de ciclo se reinicia tiempo de control
            tiempoctrl=0;
        // tiempoctrl incrementa una unidad cada 5ms, por eso se dividen los tiempos por 0.005.
      
        if(tiempoctrl<=Tp) // Si el tiempo es menor o igual al tiempo de control se enciende la resistencia de lo contrario se apaga.
            SSR_Write(1);
        else         
            SSR_Write(0);
    }else{
        if(!Test)
            SSR_Write(0);
    }
   
    if(tiempos3>=200){
        Lectura2=1;
        if(flagup)
            tiempoup++;
        if(flagdn)
            tiempodn++;   
        tiempos3=0;
    }
    
    if(tiempos>=500){  // Cuenta cada 2ms        
        tiempos=0;Lectura=1;tiemporeset++;TiempoControl++;tiemporecta++;mensaje=!mensaje;
   
        if(conteoPregunta)tPregunta++;
        
        if(flag_setpoint==1)
            tiemposetpoint++;
      
        if(tiempo_esterilizacion==1 || tiempo_esterilizacion==2){               
            tinicio++;
            tGrafica++;
        }
             
        if(flag_tiempocalentamiento==1)
            tiempocalentamiento++;
    }     
}

CY_ISR(inteRX){
    Dato2=UART_2_GetByte();
   if(Dato2==0x65){//Inicio Comunicacion
      Inicio=1;
      RX_Wr_Index =0;
   }
   if(Inicio==1){
      Rx_Buffer2[RX_Wr_Index] = Dato2;
      RX_Wr_Index++;
   }
   if(RX_Wr_Index >= RX_BUFFER_SIZE){
      RX_Wr_Index =0;
      Inicio=0;
   }
   if(RX_Wr_Index==0){
      if(Rx_Buffer2[0]==0x65 && Rx_Buffer2[1]==0xff && Rx_Buffer2[2]==0xff && Rx_Buffer2[8]==0x00 && Rx_Buffer2[9]==0xff )
         Dato_Exitoso=5;
      else
         Dato_Exitoso=10;
   }
   //if(Menu==20)
//     Buzzer_on;
}

CY_ISR(inteRX1){
    Dato3=UART_1_GetByte();
   if(Dato3==0x65){//Inicio Comunicacion
      Inicio2=1;
      RX_Wr_Index2 =0;
   }
   if(Inicio2==1){
      Rx_Buffer4[RX_Wr_Index2] = Dato3;
      RX_Wr_Index2++;
   }
   if(RX_Wr_Index2 >= RX_BUFFER_SIZE){
      RX_Wr_Index2 =0;
      Inicio2=0;
   }
   if(RX_Wr_Index2==0){
      if(Rx_Buffer4[0]==0x65 && Rx_Buffer4[1]==0xff && Rx_Buffer4[2]==0xff && Rx_Buffer4[8]==0x00 && Rx_Buffer4[9]==0xff )
         Dato_Exitoso2=5;
      else
         Dato_Exitoso2=10;
   }
}

CY_ISR(lee){
    
}

int main(void)
{ 
    CyGlobalIntEnable; /* Enable global interrupts. */    

    Reloj_StartEx(Reloj2);
    leeT_StartEx(lee);
    UART_1_Start();
    UART_2_Start();
    UART_3_Start();
    RX2_StartEx(inteRX);
    RX1_StartEx(inteRX1);  
    Timer_Start();  
    Timer2_Start();  
    EEPROM_Start(); 
    ADC_Start();
    ADC_SAR_Start();
    ADC_SAR_IRQ_StartEx(ADC_SAR_ISR_LOC);
    ADC_SAR_StartConvert();
    I2C_Start();
    
    LCD2_Write(1);
    LCD1_Write(1);
    CyDelay(100);
    
    for(i=0;i<80;i++){
        Temperatura=leeTermocupla(MediaMovil);
    }
    penInicial=penMufla;
    Opcion=1;Control=0;
    Menu=120;Menu2=120;
    
    escribeDisplay1("page Bienvenida");
    escribeDisplay1("bkcmd=0");
    CyDelay(10);
    escribeDisplay2("page Bienvenida");
    escribeDisplay2("bkcmd=0");
    
    LED_Write(1);
    leeEEPROM();
    LED_Write(0);

    for(;;)
    { 
        if((tGrafica>=(uint32)(((hEstp*3600)+(mEstp*60))/ANCHO)) && Menu==20){
            tGrafica=0;
            escribeDisplay1("code_c");
            sprintf(displayStr,"add 1,1,%u",(uint8)(Temperatura*DIVISION));
            escribeDisplay1(displayStr);
        }
        if(Lectura2){
            grafica=ON;
            Lectura2=0;
            Temperatura=leeTermocupla(MediaMovil);
            i2cbuf[0]=(uint8)Temperatura;
            i2cbuf[1]=extraeDecimal(Temperatura);
            i2cbuf[2]=0xff;
            i2cbuf[3]=0x0e;
            I2C_MasterWriteBuf(0xB0,i2cbuf,1,I2C_MODE_COMPLETE_XFER);
            if (tcHotJnuVolt < BROKEN_TC_THRESHOLD){
                sprintf(displayStr,"T:----- \r\n");
            }else{
                sprintf(displayStr,"T:%1.2fC \r\n",Temperatura);
            }
            UART_3_PutString(displayStr);
            
            if(!Test){
                if(coldJnTemp>=3000){
                    VENT1_Write(0);    
                }else if(coldJnTemp<2500){
                    VENT1_Write(1);   
                }
            }
        }
        LeeDisplay();
        
//_--------------------------------------------------------------------------------------------------------------------------------------------------------
    if(Menu == 120){ //Menu de Bienvenida.
            escribeDisplay2("bkcmd=0");
            escribeDisplay1("bkcmd=0");
            CyDelay(1500);
            escribeDisplay2("page Clave");
            //escribeDisplay2("page Test");
        }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------
        
//_--------------------------------------------------------------------------------------------------------------------------------------------------------
    if(Menu==0){ //Menu de Contraseña de acceso.
      //ApagaSalidas();
      entro=0;
      if(Rx_Buffer[4]==0x11){//11
         if(TipoClave!=0){
            escribeDisplay2("page Principal");
         }
      }
      
      if(TipoClave==3){
         escribeDisplay2("titulo.txt=\"Clave Nueva\"");
         escribeDisplay2("doevents");
      }else if(TipoClave==2){
         escribeDisplay2("titulo.txt=\"Clave Actual\"");         
         escribeDisplay2("doevents");
      }else if(TipoClave==1 || TipoClave==4){
         escribeDisplay2("titulo.txt=\"Clave Tecnico\"");         
         escribeDisplay2("doevents");         
      }else if(TipoClave==0){
        sprintf(displayStr,"titulo.txt=\"Ingrese Clave\"");
        escribeDisplay2(displayStr);
      }
      
      if(Rx_Buffer[3]==0x0f){//0f, recibe caracteres ingresados desde el Display
         if(TipoClave==0){
            if(Rx_Buffer[4]>=0x30 && Rx_Buffer[5]>=0x30 && Rx_Buffer[6]>=0x30 && Rx_Buffer[7]>=0x30
            && Rx_Buffer[4]<=0x39 && Rx_Buffer[5]<=0x39 && Rx_Buffer[6]<=0x39 && Rx_Buffer[7]<=0x39){
            if(Rx_Buffer[4]==0x33&&Rx_Buffer[5]==0x38&&Rx_Buffer[6]==0x39&&Rx_Buffer[7]==0x32){ // Si Ingresa clave para reset general del sistema.
               EEPROM_WriteByte(0,0);CyDelay(20);EEPROM_WriteByte(0,1);CyDelay(20);// Reestablece a contraseña de Fabrica y reinicia Programa.
               EEPROM_WriteByte(0,2);CyDelay(20);EEPROM_WriteByte(0,3);CyDelay(20);
               CySoftwareReset();
            }
            }
            
            if(Rx_Buffer[4]>=0x30 && Rx_Buffer[5]>=0x30 && Rx_Buffer[6]>=0x30 && Rx_Buffer[7]>=0x30
            && Rx_Buffer[4]<=0x39 && Rx_Buffer[5]<=0x39 && Rx_Buffer[6]<=0x39 && Rx_Buffer[7]<=0x39){
            if((Rx_Buffer[4]==Password[0]+0x30)&&(Rx_Buffer[5]==Password[1]+0x30)&&(Rx_Buffer[6]==Password[2]+0x30)&&(Rx_Buffer[7]==Password[3]+0x30)){
                  escribeDisplay2("page ClaveCorrecta");
                  dibujo=OFF;
                  //RX_Buffer[3]=0x00;RX_Buffer2[3]=0x00;  
               }else{
                  escribeDisplay2("page ClaveBad");
                  //RX_Buffer[3]=0x00;RX_Buffer2[3]=0x00;  
               } 
            }
         }else if(TipoClave==1){
            if(Rx_Buffer[4]>=0x30 && Rx_Buffer[5]>=0x30 && Rx_Buffer[6]>=0x30 && Rx_Buffer[7]>=0x30
            && Rx_Buffer[4]<=0x39 && Rx_Buffer[5]<=0x39 && Rx_Buffer[6]<=0x39 && Rx_Buffer[7]<=0x39){
            if(Rx_Buffer[4]==0x34&&Rx_Buffer[5]==0x34&&Rx_Buffer[6]==0x34&&Rx_Buffer[7]==0x34){ // Si Ingresa clave de Servicio Tecnico
               escribeDisplay2("page Config");
            }else{
               escribeDisplay2("page MenuPrincipal");
            }
            }
         }else if(TipoClave==2){
            if(Rx_Buffer[4]>=0x30 && Rx_Buffer[5]>=0x30 && Rx_Buffer[6]>=0x30 && Rx_Buffer[7]>=0x30
            && Rx_Buffer[4]<=0x39 && Rx_Buffer[5]<=0x39 && Rx_Buffer[6]<=0x39 && Rx_Buffer[7]<=0x39){
            if((Rx_Buffer[4]==Password[0]+0x30)&&(Rx_Buffer[5]==Password[1]+0x30)&&(Rx_Buffer[6]==Password[2]+0x30)&&(Rx_Buffer[7]==Password[3]+0x30)){
               escribeDisplay2("page Clave");
               escribeDisplay2("titulo.txt=\"Clave Nueva\"");
               TipoClave=3;
               Rx_Buffer[3]=0x00;
               Rx_Buffer2[3]=0x00;
            }else{
               escribeDisplay2("page MenuPrincipal");
               //RX_Buffer[3]=0x00;
               //RX_Buffer2[3]=0x00;
            } 
            }
         }else if(TipoClave==3){
            escribeDisplay2("titulo.txt=\"Clave Nueva\"");
            escribeDisplay2("page MenuPrincipal");
            if(!GuardaEEPROM){
               //write_eeprom(10,Rx_Buffer[4]-0x30);delay_ms(20);write_eeprom(11,Rx_Buffer[5]-0x30);delay_ms(20);
               EEPROM_WriteByte(Rx_Buffer[4]-0x30,0);CyDelay(20);
               EEPROM_WriteByte(Rx_Buffer[5]-0x30,1);CyDelay(20);
               EEPROM_WriteByte(Rx_Buffer[6]-0x30,2);CyDelay(20);
               EEPROM_WriteByte(Rx_Buffer[7]-0x30,3);CyDelay(20);
               GuardaEEPROM=ON;
            }
            Password[0]=Rx_Buffer[4]-0x30;Password[1]=Rx_Buffer[5]-0x30;Password[2]=Rx_Buffer[6]-0x30;Password[3]=Rx_Buffer[7]-0x30;
            //RX_Buffer[3]=0x00;
            //RX_Buffer2[3]=0x00;
         }else if(TipoClave==4){
            if(Rx_Buffer[4]==0x34&&Rx_Buffer[5]==0x34&&Rx_Buffer[6]==0x34&&Rx_Buffer[7]==0x34){ // Si Ingresa clave de Servicio Tecnico
               escribeDisplay2("page Test");
            }else{
               escribeDisplay2("page MenuPrincipal");
            }
            //RX_Buffer[3]=0x00;
            //RX_Buffer2[3]=0x00;
         }
      }
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
   if(Menu==1){ //Menu Principal.
      GuardaEEPROM=OFF;
      Test=OFF;
      dibujo=OFF;
      if(Rx_Buffer[3]==0x0f){//0f, recibe caracteres ingresados desde el Display
         CyDelay(5);
         if(Rx_Buffer[3]==0x0f){
            if(Rx_Buffer[4]==0x37){//Ajustes y pasa a solicitud de password de tecnico
               escribeDisplay2("page Clave");
               MenuAnt=Menu;
               TipoClave=1;
               Rx_Buffer[3]=0x00;
               Rx_Buffer2[3]=0x00;
               Opcion=0;
            } 
            if(Opcion==7){// Cambio de Clave
               escribeDisplay2("page Clave");
               MenuAnt=Menu;
               TipoClave=2;
               Rx_Buffer[3]=0x00;
               Rx_Buffer2[3]=0x00;
               Opcion=0;
            }else if(Opcion==6){//Test
               escribeDisplay2("page Test");
               PantallaPrincipal=0;
               MenuAnt=Menu;
            }else if(Opcion==5){//Programa 5
               escribeDisplay2("page Rampas");
               MenuAnt=Menu;
               Programa=5;
            }else if(Opcion==4){//Programa 4
               escribeDisplay2("page Rampas");
               MenuAnt=2;
               Programa=4;
            }else if(Opcion==1){//Programa 1
               escribeDisplay2("page Rampas");
               MenuAnt=2;
               Programa=1;
            }else if(Opcion==3){//Programa 3
               escribeDisplay2("page Rampas");
               MenuAnt=Menu;               
               Programa=3;
            }else if(Opcion==2){//Programa 2
               escribeDisplay2("page Rampas");
               MenuAnt=Menu;
               Programa=2;
            }
         }
      }else {
         if(Opcion>7)
            Opcion=1;
            
         if(Opcion<1)
            Opcion=7;
         
         if(Opcion==1){
            sprintf(displayStr,"bc.pic=%u",iconoGrande[Opcion-1]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"bd.pic=%u",iconoPeque[Opcion]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"bi.pic=%u",iconoPeque[6]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"bc.pic2=%u",iconoGrande[Opcion-1]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"bd.pic2=%u",iconoPeque[Opcion]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"bi.pic2=%u",iconoPeque[6]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"textop.txt=\"%s\"",texto[Opcion-1]);
            escribeDisplay2(displayStr);
         }else if(Opcion==7){
            sprintf(displayStr,"bc.pic=%u",iconoGrande[Opcion-1]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"bd.pic=%u",iconoPeque[0]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"bi.pic=%u",iconoPeque[Opcion-2]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"bc.pic2=%u",iconoGrande[Opcion-1]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"bd.pic2=%u",iconoPeque[0]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"bi.pic2=%u",iconoPeque[Opcion-2]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"textop.txt=\"%s\"",texto[Opcion-1]);
            escribeDisplay2(displayStr);
         }else{
            sprintf(displayStr,"bc.pic=%u",iconoGrande[Opcion-1]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"bd.pic=%u",iconoPeque[Opcion]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"bi.pic=%u",iconoPeque[Opcion-2]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"bc.pic2=%u",iconoGrande[Opcion-1]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"bd.pic2=%u",iconoPeque[Opcion]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"bi.pic2=%u",iconoPeque[Opcion-2]);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"textop.txt=\"%s\"",texto[Opcion-1]);
            escribeDisplay2(displayStr);
         }
                  
         if(Rx_Buffer[4]==0x0d || Rx_Buffer[4]==0x0c){//Tecla Derecha Oprimida
            Opcion++;
            Rx_Buffer[4]=0x00;
            Rx_Buffer2[4]=0x00;
         }
         
         if(Rx_Buffer[4]==0x0e || Rx_Buffer[4]==0x0f){//Tecla Izquierda Oprimida
            Opcion--;
            Rx_Buffer[4]=0x00;
            Rx_Buffer2[4]=0x00;
         }
                  
      }
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       
    
//_--------------------------------------------------------------------------------------------------------------------------------------------------------
    if(Menu==2){ //Menu de Tiempo de Duraciòn
        dibujo=OFF;
        imprime=OFF;
            if(Rx_Buffer[4]==0x0a){//Selecciono Minutos
               Minutos=OFF;
               Horas=ON;
            }
            
            if(Rx_Buffer[4]==0x0b){//Selecciono Segundos
               Minutos=ON;
               Horas=OFF;
            }           
            
            if(Rx_Buffer[4]==0x11){//Selecciono Regresar
               n=0;
               conteo=0;
               CyDelay(200);
               if(Rx_Buffer[4]==0x11){
                  Minutos=OFF;
                  Horas=OFF;
                  escribeDisplay2("page Rampas");
                  if(!GuardaEEPROM){
                     EEPROM_WriteByte((int8)Programas[Programa-1].rampas[CicloLibre-1].Horas,103+((Programa-1)*50)+((CicloLibre-1)*5));
                     CyDelay(10);
                     EEPROM_WriteByte((int8)Programas[Programa-1].rampas[CicloLibre-1].Minutos,104+((Programa-1)*50)+((CicloLibre-1)*5));
                     CyDelay(10);
                     GuardaEEPROM=ON;
                  }
               }
            escribeDisplay1("page Grafica");
            }
            
            if(Rx_Buffer[4]==0x0d){//Tecla Arriba Oprimida
                pulsoAumentar=ON;
                Rx_Buffer[4]=0x00;  
                Rx_Buffer2[4]=0x00;            
            }
        
            if(Rx_Buffer[4]==0x0e){//Tecla Arriba Suelta
                pulsoAumentar=OFF;
                Rx_Buffer[4]=0x00;  
                Rx_Buffer2[4]=0x00;            
            }
            
            if(Rx_Buffer[4]==0x0c){//Tecla Abajo Oprimida
                pulsoDisminuir=ON;
                Rx_Buffer[4]=0x00;
                Rx_Buffer2[4]=0x00;                        
            }
        
            if(Rx_Buffer[4]==0x10){//Tecla Abajo Suelta
                pulsoDisminuir=OFF;
                Rx_Buffer[4]=0x00;
                Rx_Buffer2[4]=0x00;                        
            }  
            
            
            if(pulsoAumentar){//Tecla arriba Oprimida
               if(Minutos){
                  Programas[Programa-1].rampas[CicloLibre-1].Minutos+=1.0;
               }
               if(Horas){
                  Programas[Programa-1].rampas[CicloLibre-1].Horas+=1.0;
               }  
               CyDelay(300);
            }
            
            if(pulsoDisminuir){//Tecla abajo oprimida
               if(Minutos && Programas[Programa-1].rampas[CicloLibre-1].Minutos>0){
                  Programas[Programa-1].rampas[CicloLibre-1].Minutos-=1.0;
               }
               if(Horas  && Programas[Programa-1].rampas[CicloLibre-1].Horas>0){
                  Programas[Programa-1].rampas[CicloLibre-1].Horas-=1.0;
               }
               CyDelay(300);
            }
            
            if(Programas[Programa-1].rampas[CicloLibre-1].Horas>99)Programas[Programa-1].rampas[CicloLibre-1].Horas=1;
            if(Programas[Programa-1].rampas[CicloLibre-1].Minutos>59)Programas[Programa-1].rampas[CicloLibre-1].Minutos=0;
               
            sprintf(displayStr,"tminsec.txt=\"%02u\"",Programas[Programa-1].rampas[CicloLibre-1].Horas);
            escribeDisplay2(displayStr);
            sprintf(displayStr,"tsecsec.txt=\"%02u\"",Programas[Programa-1].rampas[CicloLibre-1].Minutos);
            escribeDisplay2(displayStr);
         
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
    if(Menu==3){ //Menu de Temperatura
         dibujo=OFF;
        imprime=OFF;
         if(Programas[Programa-1].rampas[CicloLibre-1].Temperatura<10)
            Programas[Programa-1].rampas[CicloLibre-1].Temperatura=1200;
         if(Programas[Programa-1].rampas[CicloLibre-1].Temperatura>1200)
            Programas[Programa-1].rampas[CicloLibre-1].Temperatura=10;
         
         if(Rx_Buffer[4]==0x0d){//Tecla Arriba Oprimida
           pulsoAumentar=ON;
           Rx_Buffer[4]=0x00;  
           Rx_Buffer2[4]=0x00;            
         }
        
         if(Rx_Buffer[4]==0x0e){//Tecla Arriba Suelta
            pulsoAumentar=OFF;
            Rx_Buffer[4]=0x00;  
            Rx_Buffer2[4]=0x00;            
         }
        
         if(Rx_Buffer[4]==0x0c){//Tecla Abajo Oprimida
           pulsoDisminuir=ON;
           Rx_Buffer[4]=0x00;
           Rx_Buffer2[4]=0x00;                        
         }
        
         if(Rx_Buffer[4]==0x0b){//Tecla Abajo Oprimida
            pulsoDisminuir=OFF;
            Rx_Buffer[4]=0x00;
            Rx_Buffer2[4]=0x00;                        
         }   
            
         if(Rx_Buffer[4]==0x11){//Selecciono Regresar
            n=0;
            conteo=0;
            Horas=OFF;
            Minutos=OFF;
            escribeDisplay2("page Rampas");
            if(!GuardaEEPROM){
               EEPROM_WriteByte((int8)Programas[Programa-1].rampas[CicloLibre-1].Temperatura,100+((Programa-1)*50)+((CicloLibre-1)*5));
               EEPROM_WriteByte(convert8(Programas[Programa-1].rampas[CicloLibre-1].Temperatura,1),101+((Programa-1)*50)+((CicloLibre-1)*5));
               CyDelay(10);
               GuardaEEPROM=ON;
            }
            escribeDisplay1("page Grafica");
         }
        
        if(Rx_Buffer[4]==0x20){//Selecciono x1
            x1=ON;
            x10=OFF;
            x100=OFF;
            escribeDisplay2("c0.val=1");
            escribeDisplay2("c1.val=0");
            escribeDisplay2("c2.val=0");
            Rx_Buffer[4]=0x00;  
            Rx_Buffer2[4]=0x00;     
        }
        
        if(Rx_Buffer[4]==0x21){//Selecciono x10
            x1=OFF;
            x10=ON;
            x100=OFF;
            escribeDisplay2("c0.val=0");
            escribeDisplay2("c1.val=1");
            escribeDisplay2("c2.val=0");
            Rx_Buffer[4]=0x00;  
            Rx_Buffer2[4]=0x00;     
        }
        
        if(Rx_Buffer[4]==0x22){//Selecciono x100
            x1=OFF;
            x10=OFF;
            x100=ON;
            escribeDisplay2("c0.val=0");
            escribeDisplay2("c1.val=0");
            escribeDisplay2("c2.val=1");
            Rx_Buffer[4]=0x00;  
            Rx_Buffer2[4]=0x00;     
        }
        if(pulsoAumentar){
            if(x1)
                Programas[Programa-1].rampas[CicloLibre-1].Temperatura+=1;    
            else if(x10)    
                Programas[Programa-1].rampas[CicloLibre-1].Temperatura+=10;    
            else if(x100)    
                Programas[Programa-1].rampas[CicloLibre-1].Temperatura+=100;
                
            CyDelay(200);    
        }
        
        if(pulsoDisminuir){
            if(x1)
                Programas[Programa-1].rampas[CicloLibre-1].Temperatura-=1;    
            else if(x10)    
                Programas[Programa-1].rampas[CicloLibre-1].Temperatura-=10;    
            else if(x100)    
                Programas[Programa-1].rampas[CicloLibre-1].Temperatura-=100;
                
            CyDelay(200);
        }
        
         sprintf(displayStr,"tsettem.txt=\"%04u\"",Programas[Programa-1].rampas[CicloLibre-1].Temperatura);
         escribeDisplay2(displayStr);
      
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
    if(Menu==6){//Menu de Test
        Test=ON;
        if(Rx_Buffer[4]==0x50){
            escribeDisplay2("page MenuPrincipal");
            escribeDisplay1("page Bienvenida");
            Test=OFF;
            Rx_Buffer[4]=0x00;
            Rx_Buffer2[4]=0x00;   
        }
        
        if(Rx_Buffer[4]==0x01){
            estadoSSR=!estadoSSR;
            Rx_Buffer[4]=0x00;
            Rx_Buffer2[4]=0x00;   
        }
        
        if(Rx_Buffer[4]==0x02){
            estadoBuzzer=!estadoBuzzer;
            Rx_Buffer[4]=0x00;
            Rx_Buffer2[4]=0x00;   
        }
        
        if(Rx_Buffer[4]==0x03){
            estadoVentilador=!estadoVentilador;
            Rx_Buffer[4]=0x00;
            Rx_Buffer2[4]=0x00;   
        }
        
        if(estadoSSR){
            SSR_Write(1); 
            escribeDisplay2("b0.bco=1024");
        }else{
            SSR_Write(0);
            escribeDisplay2("b0.bco=63488");
        }
        
        if(estadoBuzzer){
            LED_Write(1);    
            escribeDisplay2("b1.bco=1024");
        }else{
            LED_Write(0);
            escribeDisplay2("b1.bco=63488");
        }
        
        if(estadoVentilador){
            VENT1_Write(0);    
            escribeDisplay2("b2.bco=1024");
        }else{
            VENT1_Write(1);
            escribeDisplay2("b2.bco=63488");
        }
        
        if(DOOR_Read()==0)
            escribeDisplay2("b3.bco=1024");
        else
            escribeDisplay2("b3.bco=63488");
        
      sprintf(displayStr,"b8.txt=\"%u \"",coldJnTemp);
      escribeDisplay2(displayStr);
      sprintf(displayStr,"b7.txt=\"A0:%ld\"",hotJnCount);
      escribeDisplay2(displayStr);
      sprintf(displayStr,"b6.txt=\"TK:%+1.2f V0:%1.1f\"",Temperatura,V0);
      escribeDisplay2(displayStr);
      sprintf(displayStr,"b4.txt=\"T:%ld.%ld \"",tcTemp/100,tcTemp-((tcTemp/100)*100));
      escribeDisplay2(displayStr);
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------          
    
//_--------------------------------------------------------------------------------------------------------------------------------------------------------
   if(Menu==8){ //Menu de Rampas
        flagBorrado=OFF;
        for(i=0;i<10;i++){
            if(Programas[Programa-1].rampas[i].Horas==0 && Programas[Programa-1].rampas[i].Minutos==0){
                Segmentos=i;  
                break;
            }else{
                Segmentos=i+1;  
            }
        }
    
        if(Menu2!=9){
            escribeDisplay1("page Grafica");
            CyDelay(10);
        }else{
            if(Segmentos>0){
                if(!dibujo){
                    escribeDisplay1("t5.txt=\"DIBUJANDO GRAFICA DE RAMPA PROGRAMADA...\"");
                    sprintf(displayStr,"t6.txt=\"%02u:%02u\"",Programas[Programa-1].rampas[0].Horas,Programas[Programa-1].rampas[0].Minutos);
                    escribeDisplay1(displayStr); 
                    sprintf(displayStr,"t7.txt=\"%02u:%02u\"",Programas[Programa-1].rampas[1].Horas,Programas[Programa-1].rampas[1].Minutos);
                    escribeDisplay1(displayStr); 
                    sprintf(displayStr,"t8.txt=\"%02u:%02u\"",Programas[Programa-1].rampas[2].Horas,Programas[Programa-1].rampas[2].Minutos);
                    escribeDisplay1(displayStr); 
                    sprintf(displayStr,"t9.txt=\"%02u:%02u\"",Programas[Programa-1].rampas[3].Horas,Programas[Programa-1].rampas[3].Minutos);
                    escribeDisplay1(displayStr); 
                    sprintf(displayStr,"t10.txt=\"%02u:%02u\"",Programas[Programa-1].rampas[4].Horas,Programas[Programa-1].rampas[4].Minutos);
                    escribeDisplay1(displayStr); 
                    sprintf(displayStr,"t11.txt=\"%02u:%02u\"",Programas[Programa-1].rampas[5].Horas,Programas[Programa-1].rampas[5].Minutos);
                    escribeDisplay1(displayStr); 
                    sprintf(displayStr,"t12.txt=\"%02u:%02u\"",Programas[Programa-1].rampas[6].Horas,Programas[Programa-1].rampas[6].Minutos);
                    escribeDisplay1(displayStr); 
                    sprintf(displayStr,"t13.txt=\"%02u:%02u\"",Programas[Programa-1].rampas[7].Horas,Programas[Programa-1].rampas[7].Minutos);
                    escribeDisplay1(displayStr); 
                    sprintf(displayStr,"t14.txt=\"%02u:%02u\"",Programas[Programa-1].rampas[8].Horas,Programas[Programa-1].rampas[8].Minutos);
                    escribeDisplay1(displayStr); 
                    sprintf(displayStr,"t15.txt=\"%02u:%02u\"",Programas[Programa-1].rampas[9].Horas,Programas[Programa-1].rampas[9].Minutos);
                    escribeDisplay1(displayStr);     
                    dibujaGrafico();
                    dibujo=ON;
                }else{
                    escribeDisplay1("t5.txt=\"GRAFICA DE RAMPA PROGRAMADA\"");      
                }
            }
        }
    
        sprintf(displayStr,"t0.txt=\"Pasos Programados %02u; Paso: %02u\"",Segmentos,CicloLibre);
        escribeDisplay2(displayStr);
        GuardaEEPROM=OFF;
      
        if(Rx_Buffer[4]>0x00 && Rx_Buffer[4]<11){//Personalizados
            if(CicloLibre>1){
                sprintf(displayStr,"t2.txt=\"TA:%04u°C\"",Programas[Programa-1].rampas[CicloLibre-2].Temperatura);
                escribeDisplay2(displayStr); 
            }else{
                sprintf(displayStr,"t2.txt=\"TA:---\"");
                escribeDisplay2(displayStr); 
            }
            sprintf(displayStr,"temp.txt=\"T:%04u°C\"",Programas[Programa-1].rampas[Rx_Buffer[4]-1].Temperatura);
            escribeDisplay2(displayStr); 
            sprintf(displayStr,"tiempo.txt=\"%02u:%02u \"",Programas[Programa-1].rampas[Rx_Buffer[4]-1].Horas,Programas[Programa-1].rampas[Rx_Buffer[4]-1].Minutos);
            escribeDisplay2(displayStr); 
            CicloLibre=Rx_Buffer[4];
        }else if(CicloLibre==0x00){
            sprintf(displayStr,"t2.txt=\"TA:---\"");
            escribeDisplay2(displayStr); 
            sprintf(displayStr,"temp.txt=\"T: --\"");
            escribeDisplay2(displayStr); 
            sprintf(displayStr,"tiempo.txt=\"--:--\"");
            escribeDisplay2(displayStr); 
        }else if(CicloLibre>0 && CicloLibre<11){
            if(CicloLibre>1){
                sprintf(displayStr,"t2.txt=\"TA:%04u°C\"",Programas[Programa-1].rampas[CicloLibre-2].Temperatura);
                escribeDisplay2(displayStr); 
            }else{
                sprintf(displayStr,"t2.txt=\"TA:---\"");
                escribeDisplay2(displayStr); 
            }
            sprintf(displayStr,"temp.txt=\"T:%04u°C\"",Programas[Programa-1].rampas[CicloLibre-1].Temperatura);
            escribeDisplay2(displayStr); 
            sprintf(displayStr,"tiempo.txt=\"%02u:%02u \"",Programas[Programa-1].rampas[CicloLibre-1].Horas,Programas[Programa-1].rampas[CicloLibre-1].Minutos);
            escribeDisplay2(displayStr); 
        }
      
        if(Rx_Buffer[4]==0x20 && CicloLibre>0){//20, Temperatura
            escribeDisplay2("page Temperatura");
            rampa=ON;
            x1=ON;
            x10=OFF;
            x100=OFF;
        }
      
        if(Rx_Buffer[4]==0x40 && CicloLibre>0){//40, Tiempo
            escribeDisplay2("page Tiempo");
            rampa=ON;
        }
      
        if(Rx_Buffer[4]==0x50){//50, Iniciar
            rampa=OFF;
            flagRampa=ON;
            TempeInicial=Temperatura;
            flagFinalizo=OFF;
            escribeDisplay2("page Funcionamiento");
        }
    
        if(Rx_Buffer[4]==0x60){//60, Regresar
            rampa=OFF;
            escribeDisplay2("page MenuPrincipal");
            escribeDisplay1("page Bienvenida");
            CicloLibre=0;
        }
        
        if(Rx_Buffer[4]==0x70){//70, Borrar  
            rampa=OFF;
            for(i=0;i<10;i++){
                LED_Write(1);
                CyDelay(50);
                LED_Write(0);
                CyDelay(50);
                Programas[Programa-1].rampas[i].Horas=0;
                Programas[Programa-1].rampas[i].Minutos=0;
                EEPROM_WriteByte(0,103+((Programa-1)*50)+((i)*5));
                EEPROM_WriteByte(0,104+((Programa-1)*50)+((i)*5));
                Programas[Programa-1].rampas[i].Temperatura=10;
                EEPROM_WriteByte(10,100+((Programa-1)*50)+((i)*5));
                EEPROM_WriteByte(10,101+((Programa-1)*50)+((i)*5));
            }
            escribeDisplay2("page Eliminado");
        }
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       
        
//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
    if(Menu==15){//Menu de Clave Correcta
      if(!entro){
         CyDelay(2000);
         entro=ON;
      }
      escribeDisplay2("page MenuPrincipal");
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------      

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
    if(Menu==16){//Menu de Clave InCorrecta
      if(!entro){
         CyDelay(2000);
         entro=ON;
      }
      escribeDisplay2("page Clave");
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
    
//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
    if(Menu==17){//Menu de Programa Eliminado
      if(!flagBorrado){
         CyDelay(2000);
         flagBorrado=ON;
      }
      escribeDisplay2("page MenuPrincipal");
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------          

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
    if(Menu==18){//Menu de Pegunta
        conteoPregunta=ON;
        if(tPregunta>10){
            escribeDisplay2("page Funcionamiento");
        }
        
      if(Rx_Buffer[4]==0x01){//Si
         escribeDisplay2("page MenuPrincipal");
         escribeDisplay1("page Bienvenida");
         mEst=0;
         hEst=0;
         CicloLibre=0;
         SSR_Write(0);         
         Control=0;
         Tpulso=0.0;
         tiempo_esterilizacion=0;
         flag_tiempocalentamiento=0;
         Segmento=0;
         Ganancia=GananciaIn*(Setpoint/100);
         tinicio=0;
         Finalizo=OFF;
         flag_calcula=OFF;
         flagTemperatura=OFF;
         flagRampa=OFF;
         flagFinalizo=OFF;
         Rx_Buffer[4]=0x00;
         Rx_Buffer2[4]=0x00;
      }
    
      if(Rx_Buffer[4]==0x02){//No
         escribeDisplay2("page Funcionamiento");
         Rx_Buffer[4]=0x00;
         Rx_Buffer2[4]=0x00;
      }
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------          

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==20){ //Menu de Funcionamiento
      GuardaEEPROM=OFF;
      dibujo=OFF;
      conteoPregunta=OFF;
      tPregunta=0;
      sprintf(displayStr,"t5.txt=\"Temperatura Ideal:%u°C  TA:%02u°C \"",(uint16)TemperaturaIdeal,coldJnTemp/100);
      escribeDisplay1(displayStr);
      escribeDisplay2("t0.txt=\"Mufla Digital JP Inglobal \"");  
      if(Rx_Buffer[4]==0x01){//Temperatura
         escribeDisplay2("page Pregunta");
      }
      
      if(Rx_Buffer[4]==0x02){//Inicia Control Rampa
         flagRampa=!flagRampa;
         // Configura parametros para inicio de rampa 
        if(flagRampa){
            TempeInicial=Temperatura;
            Tiempo=(Programas[Programa-1].rampas[Segmento].Horas*3600)+(Programas[Programa-1].rampas[Segmento].Minutos*60);         
            Setpoint=Programas[Programa-1].rampas[Segmento].Temperatura;
            Pendiente=((Setpoint-TempeInicial)/Tiempo);
            hEstp=Programas[Programa-1].rampas[Segmento].Horas;
            mEstp=Programas[Programa-1].rampas[Segmento].Minutos;
            escribeDisplay2("b1.pic=72");       
        }
        Rx_Buffer[4]=0x00;
        Rx_Buffer2[4]=0x00;
      }
      
      if(!Finalizo){
           if(DOOR_Read()==1){
               Control=0;
               flag_tiempocalentamiento=0;
               Ganancia=GananciaIn*(Setpoint/100);
               tiempo_esterilizacion=0;
               SSR_Write(0);
               Tpulso=0.0;
               escribeDisplay2("t1.txt=\"Puerta Abierta\"");
                LED_Write(1);
                CyDelay(500);
                LED_Write(0);
                CyDelay(500);
            }else{
                if(flagRampa){
                    escribeDisplay2("b1.pic=72");
                    iniciaConteo(); 
                    controlaRampa();         
                }else{
                    escribeDisplay2("b1.pic=73");  
                    Control=0;
                    flag_tiempocalentamiento=0;
                    tiempo_esterilizacion=0;
                    SSR_Write(0);
                    Tpulso=0.0;
                    if(DOOR_Read()==0)
                        escribeDisplay2("t1.txt=\"Pausado\"");   
                }
            }
      }else{
         if(!flagFinalizo){
            for(i=0;i<5;i++){
                LED_Write(1);
                CyDelay(1000);
                LED_Write(0);
                CyDelay(1000);
            }
            flagFinalizo=ON;
         }
         escribeDisplay2("t1.txt=\"Proceso Finalizado\"");
         SSR_Write(0);         
         Control=0;
         Tpulso=0.0;
         tiempo_esterilizacion=0;
         flagTemperatura=OFF;
         flagRampa=OFF;
      }
      
      if(Lectura==1){// Utilizado para visualizar la lectura de la temperatura
         Lectura=0;
         Potencia=(100.0/Tciclo)*Tpulso; 
         if (tcHotJnuVolt < BROKEN_TC_THRESHOLD){
		    sprintf(displayStr,"t2.txt=\"T:---\"");
            escribeDisplay2(displayStr);	
            SSR_Write(0);         
            Control=0;
            Tpulso=0.0;
            tiempo_esterilizacion=0;
            flagTemperatura=OFF;
            flagRampa=OFF;
		 }else{	
            if(Temperatura<1000){
                sprintf(displayStr,"t2.txt=\"T:%1.1f°C\"",Temperatura);
                escribeDisplay2(displayStr);
            }else{
                sprintf(displayStr,"t2.txt=\"T:%1.0f°C\"",Temperatura);
                escribeDisplay2(displayStr);
            }
		 }
         
         sprintf(displayStr,"t3.txt=\"SET:%04u°C\"",Setpoint);
         escribeDisplay2(displayStr);
         sprintf(displayStr,"t4.txt=\"SET:%02u:%02u (HH:MM)\"",hEstp,mEstp);
         escribeDisplay2(displayStr);
         sprintf(displayStr,"t5.txt=\"%02u:%02u\"",hEst,mEst);
         escribeDisplay2(displayStr);
         sprintf(displayStr,"t8.txt=\"Pasos: %02u \"",Segmentos);
         escribeDisplay2(displayStr);         
         sprintf(displayStr,"t9.txt=\"Paso Actual: %02u \"",Segmento+1);
         escribeDisplay2(displayStr);         
         sprintf(displayStr,"t10.txt=\"Potencia:%03u%% \"",(uint8)Potencia);
         escribeDisplay2(displayStr);         
         sprintf(displayStr,"t11.txt=\"Pendiente:%2.2f°C/s \"",Pendiente);
         escribeDisplay2(displayStr);
      } 
      
    
      // Si cumplio el tiempo de esterilización, Muestre ciclo finalizado y detenga control de temperatura.
      if((mEstp>0)||(hEstp>0)){
         CuentaTiempo();  
         if((mEst>=mEstp)&&(hEst>=hEstp)&& Finalizo==0){
            flag_calcula=0;
            if(Segmento<Segmentos-1){
               Segmento++;
               hEst=0;
               mEst=0;
            }else{
               Finalizo=ON;
            }
            for(i=0;i<2;i++){
               LED_Write(1);
               CyDelay(1000);
               LED_Write(0);
               CyDelay(1000);
            }
         }
      }
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       
     
//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==100){ //Menu de Configuración de Parametros de Control
     
      if(Opcion2>7)
         Opcion2=1;
      if(Opcion2<1)
         Opcion2=7;
      
      if(Opcion2==1){
         escribeDisplay2("Config.t2c.txt=\"Proporcional\"");      
         sprintf(displayStr,"t3c.txt=\"%u\"",(uint8)Ganancia);
         escribeDisplay2(displayStr);
         
         if(Rx_Buffer[4]==0x0a){//Tecla Arriba Oprimida
            CyDelay(20);
            if(Rx_Buffer[4]==0x0a){
               Ganancia+=1.0;
               Rx_Buffer[4]=0x00;  
               Rx_Buffer2[4]=0x00;            
            }
         }
            
         if(Rx_Buffer[4]==0x0b){//Tecla Abajo Oprimida
            CyDelay(20);
            if(Rx_Buffer[4]==0x0b){
               Ganancia-=1.0;
               Rx_Buffer[4]=0x00;
               Rx_Buffer2[4]=0x00;            
            }
         }
         
         if(Ganancia>20)
            Ganancia=1;
         if(Ganancia<1)
            Ganancia=20;
      }else if(Opcion2==2){
         escribeDisplay2("Config.t2c.txt=\"Derivativo\"");
         sprintf(displayStr,"t3c.txt=\"%u\"",Derivativo);
         escribeDisplay2(displayStr); 
        
         if(Rx_Buffer[4]==0x0a){//Tecla Arriba Oprimida
            CyDelay(20);
            if(Rx_Buffer[4]==0x0a){
               Derivativo++;
               Rx_Buffer[4]=0x00;  
               Rx_Buffer2[4]=0x00;                           
            }
         }
            
         if(Rx_Buffer[4]==0x0b){//Tecla Abajo Oprimida
            CyDelay(20);
            if(Rx_Buffer[4]==0x0b){
               Derivativo--;
               Rx_Buffer[4]=0x00;
               Rx_Buffer2[4]=0x00;                           
            }
         }
         
         if(Derivativo>80)
            Derivativo=1;
         if(Derivativo<1)
            Derivativo=80;
      }else if(Opcion2==3){
         escribeDisplay2("Config.t2c.txt=\"Integral\"");
         sprintf(displayStr,"t3c.txt=\"%u\"",Integral);
         escribeDisplay2(displayStr); 
         if(Rx_Buffer[4]==0x0a){//Tecla Arriba Oprimida
            CyDelay(20);
            if(Rx_Buffer[4]==0x0a){
               Integral++;
               Rx_Buffer[4]=0x00;  
               Rx_Buffer2[4]=0x00;            
            }
         }
            
         if(Rx_Buffer[4]==0x0b){//Tecla Abajo Oprimida
            CyDelay(20);
            if(Rx_Buffer[4]==0x0b){
               Integral--;
               Rx_Buffer[4]=0x00;
               Rx_Buffer2[4]=0x00;                           
            }
         }
         
         if(Integral>4)
            Integral=1;
         if(Integral<1)
            Integral=4;
      }else if(Opcion2==4){
         escribeDisplay2("Config.t2c.txt=\"Tciclo\"");
         sprintf(displayStr,"t3c.txt=\"%u\"",Tciclo);
         escribeDisplay2(displayStr); 
         if(Rx_Buffer[4]==0x0a){//Tecla Arriba Oprimida
            CyDelay(20);
            if(Rx_Buffer[4]==0x0a){
               Tciclo+=1.0;
               Rx_Buffer[4]=0x00;  
               Rx_Buffer2[4]=0x00;            
            }
         }
            
         if(Rx_Buffer[4]==0x0b){//Tecla Abajo Oprimida
            CyDelay(20);
            if(Rx_Buffer[4]==0x0b){
               Tciclo-=1.0;
               Rx_Buffer[4]=0x00;
               Rx_Buffer2[4]=0x00;            
            }
         }
         
         if(Tciclo>20)
            Tciclo=5;
         if(Tciclo<5)
            Tciclo=20;
      }else if(Opcion2==5){
         escribeDisplay2("Config.t2c.txt=\"Ajuste +\"");  
         sprintf(displayStr,"t3c.txt=\"%u\"",Ajuste);
         escribeDisplay2(displayStr); 
         if(Rx_Buffer[4]==0x0a){//Tecla Arriba Oprimida
            CyDelay(20);
            if(Rx_Buffer[4]==0x0a){
               if(Ajuste<40)
                  Ajuste++;
               Rx_Buffer[4]=0x00;  
               Rx_Buffer2[4]=0x00;            
            }
         }
            
         if(Rx_Buffer[4]==0x0b){//Tecla Abajo Oprimida
            CyDelay(20);
            if(Rx_Buffer[4]==0x0b){
               if(Ajuste>0)
                  Ajuste--;
               Rx_Buffer[4]=0x00;
               Rx_Buffer2[4]=0x00;            
            }
         }
      }else if(Opcion2==6){
         escribeDisplay2("Config.t2c.txt=\"Ajuste -\"");
         sprintf(displayStr,"t3c.txt=\"%u\"",Ajusten);
         escribeDisplay2(displayStr); 
         if(Rx_Buffer[4]==0x0a){//Tecla Arriba Oprimida
            CyDelay(20);
            if(Rx_Buffer[4]==0x0a){
               if(Ajusten<40)
                  Ajusten++;
               Rx_Buffer[4]=0x00;  
               Rx_Buffer2[4]=0x00;            
            }         
         }
            
         if(Rx_Buffer[4]==0x0b){//Tecla Abajo Oprimida
            CyDelay(20);
            if(Rx_Buffer[4]==0x0b){
               if(Ajusten>0)
                  Ajusten--;
               Rx_Buffer[4]=0x00;
               Rx_Buffer2[4]=0x00;            
            }
         }
      }else if(Opcion2==7){
         escribeDisplay2("Config.t2c.txt=\"Media\"");    
         sprintf(displayStr,"t3c.txt=\"%u\"",MediaMovil);
         escribeDisplay2(displayStr);
         if(Rx_Buffer[4]==0x0a){//Tecla Arriba Oprimida
            CyDelay(20);
            if(Rx_Buffer[4]==0x0a){
               if(MediaMovil<80)
                  MediaMovil++;
               Rx_Buffer[4]=0x00;  
               Rx_Buffer2[4]=0x00;            
            }
         }
            
         if(Rx_Buffer[4]==0x0b){//Tecla Abajo Oprimida
            CyDelay(20);
            if(Rx_Buffer[4]==0x0b){
               if(MediaMovil>10)
                  MediaMovil--;
               Rx_Buffer[4]=0x00;
               Rx_Buffer2[4]=0x00;            
            }            
         }
      }
      
      if(Rx_Buffer[4]==0x0c){//Tecla Izquierda Oprimida
         CyDelay(20);
         if(Rx_Buffer[4]==0x0c){
            Opcion2++;
            Rx_Buffer[4]=0x00;  
            Rx_Buffer2[4]=0x00;            
         }
      }
         
      if(Rx_Buffer[4]==0x0d){//Tecla Derecha Oprimida
         CyDelay(20);
         if(Rx_Buffer[4]==0x0d){
            Opcion2--;
            Rx_Buffer[4]=0x00;
            Rx_Buffer2[4]=0x00;            
         }
      }
      
      if(Rx_Buffer[4]==0x11){//Tecla Regresar
         CyDelay(20);
         if(Rx_Buffer[4]==0x11){
            if(!GuardaEEPROM){
               EEPROM_WriteByte(Integral,32);
               CyDelay(10);
               EEPROM_WriteByte(Derivativo,31);
               CyDelay(10);
               EEPROM_WriteByte((uint8)Ganancia,30);
               CyDelay(10);
               EEPROM_WriteByte(Ajuste,34);
               CyDelay(10);
               EEPROM_WriteByte(Ajusten,35);
               CyDelay(10);
               EEPROM_WriteByte(MediaMovil,36);
               CyDelay(10);
               EEPROM_WriteByte(Tciclo,33);
               CyDelay(10);
               GuardaEEPROM=ON;
            }
            escribeDisplay2("page MenuPrincipal");
         }
      }
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------           
    
    }
}
