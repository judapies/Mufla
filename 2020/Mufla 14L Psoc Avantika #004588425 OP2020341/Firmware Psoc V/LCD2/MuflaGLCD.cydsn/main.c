
/* ========================================
 *
 * Copyright JP Bioingenieria SAS, 2020
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * Para calibrar la temperatura de la mufla de acuerdo a un patron de temperatura se debe realizar lo siguiente:
 * Se mide con el patron la temperatura en dos puntos y se registran las diferencias con respecto al control
 * Se dibuja una grafica XY, donde en el eje Y este la temperatura del control y en el eje X la temperatura del patron
 * Se traza la recta con los valores obtenidos y se calcula la pendiente m y el punto de corte b, estos valores se añadiran
 * a  la lectura de la termocupla de acuerdo a la siguiente ecuacion: T=(Tcontrol-b)/m;
 *
 * ========================================
*/
#include <device.h>
#include <stdio.h>
#include <main.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <Funciones.h>
#include "ks0108.h"

#if defined (__GNUC__)
    /* Add an explicit reference to the floating point printf library */
    /* to allow the usage of floating point conversion specifiers. */
    /* This is not linked in by default with the newlib-nano library. */
    asm (".global _printf_float");
#endif

#define OFF 0
#define ON  1

int8  Testerilizacion[4]={0,0,0,0};   // Tiempo transcurrido
int8  Testerilizacionp[4]={0,0,0,0};   // Tiempo transcurrido
int8  Contrasena[4]={0,0,0,0};      // Contrasena de 4 digitos

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
short flagButton0,flagButton1,flagButton2,flagButton3,dibujo,pulsoAumentar,pulsoDisminuir,flagBorrado,Control,estadoSSR,estadoBuzzer,estadoVentilador;
short Flanco,Flanco1,Flanco2,Flanco3,flagFinalizo,visual;
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
float promedio[90];

uint8 Dato2,Dato3,Dato_Exitoso,Dato_Exitoso2,Menu,MenuAntt,MenuAnt,Menu2,MenuAntt2,z,TipoClave,Segmentos,CicloLibre,Opcion,h,l,ProgramaSeleccionado;
short Inicio,Inicio2,EstadoPaso,entro,GuardaEEPROM,rampa,Minutos,Horas,flagTemperatura,Test,x1,x10,x100;
uint8 Password[4];
uint8 tiempo_esterilizacion,y,hEst,mEst,hEstp,mEstp,Segmento,conteo,unidad,Pausa;
uint16 tiempos,tiempos3,tiempoctrl,Tc,Tp,tiemporeset,TiempoControl,tiemporecta,tiemposetpoint,tinicio,tiempocalentamiento,tiempoup,tiempodn,Coeficiente,AlTem;
short mensaje,Lectura2,flag_setpoint,flag_tiempocalentamiento,Lectura,flagup,flagdn,flagRampa,flag_calcula;
short Finalizo,conteoPregunta,grafica,imprime,flag_display,Dibuja,Desconectado,flag_clear3,flag_ganancia,flag_clear2;
float X[2]={0.0,0.0};
float Temperatura,Aumento,PendienteActual,DifPendiente,TempeInicial,Pendiente,TemperaturaIdeal,PotenciaEntregada,Tpulso,penInicial,penMufla=50.0;
float error,Potencia;
uint16 TiempoActual,MinutosActual,HorasActual,Tiempo,TiempoRampa,tPregunta,b16;
uint8 dataReady = 0u,Alarma,gananciaIn;
uint16 result;
float Prom,V0,Temperature,m;
int16 Temperatura1[Pasos*5];
int16 Horas1[Pasos*5];
int16 Minutos1[Pasos*5];
uint8 m_int,m_dec,b_dec;

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
      HorasActual=(Testerilizacion[3]*10)+Testerilizacion[2];
      MinutosActual=(Testerilizacion[1]*10)+Testerilizacion[0];
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
            Tiempo=(Horas1[ProgramaSeleccionado*Pasos]*3600)+(Minutos1[ProgramaSeleccionado*Pasos]*60);         
            Setpoint=Temperatura1[(ProgramaSeleccionado*Pasos)+Segmento];
            Pendiente=(((float)Setpoint-TempeInicial)/(float)Tiempo);
            Testerilizacionp[3]=(uint8)Horas1[(ProgramaSeleccionado*Pasos)+Segmento]/10;
            Testerilizacionp[2]=Horas1[(ProgramaSeleccionado*Pasos)+Segmento]-(Testerilizacionp[3]*10);
            Testerilizacionp[1]=Minutos1[(ProgramaSeleccionado*Pasos)+Segmento]/10;
            Testerilizacionp[0]=Minutos1[(ProgramaSeleccionado*Pasos)+Segmento]-(Testerilizacionp[1]*10);
            flag_calcula=1;
            Ganancia=GananciaIn*(Setpoint/100);
         }
      }else{
         if(flag_calcula==0){
            Tiempo=(Horas1[(ProgramaSeleccionado*Pasos)+Segmento]*3600)+(Minutos1[(ProgramaSeleccionado*Pasos)+Segmento]*60);
            Pendiente=((float)(Temperatura1[(ProgramaSeleccionado*Pasos)+Segmento]-Temperatura1[(ProgramaSeleccionado*Pasos)+Segmento-1])/(float)Tiempo);
            Setpoint=Temperatura1[(ProgramaSeleccionado*Pasos)+Segmento];
            Testerilizacionp[3]=(uint8)Horas1[(ProgramaSeleccionado*Pasos)+Segmento]/10;
            Testerilizacionp[2]=Horas1[(ProgramaSeleccionado*Pasos)+Segmento]-(Testerilizacionp[3]*10);
            Testerilizacionp[1]=Minutos1[(ProgramaSeleccionado*Pasos)+Segmento]/10;
            Testerilizacionp[0]=Minutos1[(ProgramaSeleccionado*Pasos)+Segmento]-(Testerilizacionp[1]*10);
            flag_calcula=1;
            Ganancia=GananciaIn*(Setpoint/100);
         }
      }
   
  
   if(Pendiente!=0){// Si Esta realizando una Rampa debe realizar el control de acuerdo a la pendiente de Temperatura. Aprox 10°C * min Maximo Valor de Pendiente.      
      //TiempoRampa=((HorasActual*3600)+(MinutosActual*60)+tinicio);
    TiempoRampa=(((Testerilizacion[3]*10)+Testerilizacion[2])*3600)+(((Testerilizacion[1]*10)+Testerilizacion[0])*60)+tinicio;
      if(Segmento==0){
         TemperaturaIdeal=(Pendiente*TiempoRampa)+TempeInicial;
      }else{
         TemperaturaIdeal=(Pendiente*TiempoRampa)+Temperatura1[(ProgramaSeleccionado*Pasos)+Segmento-1];
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
            
      if(tmp>Setpoint)   // Anti Wind-UP_Read()==0      
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

float leeTermocupla(uint8 media){
    float Tref;
    Tref=0.0;
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
    Tref=(Prom-b)/m;
   
    return Tref;
    //return Temperature;
}

CY_ISR(Reloj2){ 
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


int main(void)
{ 
    CyGlobalIntEnable; /* Enable global interrupts. */    
    
    Reloj_StartEx(Reloj2);
    UART_1_Start();
    RX1_StartEx(inteRX1);  
    Timer_Start();  
    EEPROM_Start(); 
    ADC_Start();
    ADC_SAR_Start();
    ADC_SAR_IRQ_StartEx(ADC_SAR_ISR_LOC);
    ADC_SAR_StartConvert();
    
    LCD2_Write(1);
    LCD1_Write(1);
    LCD3_Write(1);
    
    glcd_init(ON);             //Inicializa la glcd
    glcd_fillScreen(OFF);      //Limpia la pantalla
    glcd_rect(0,61,127,63,YES,ON);
    strcpy(Menus.Letra,"JP Inglobal");
    displayMenu(Menus.Letra,0,0,0,2);     
    strcpy(Menus.Letra,"    Mufla Digital");
    displayMenu(Menus.Letra,0,20,0,1);       
    strcpy(Menus.Letra,"     con Rampas");
    displayMenu(Menus.Letra,0,29,0,1);       
    strcpy(Menus.Letra,"     14L");
    displayMenu(Menus.Letra,0,45,0,2);

    leeEEPROM();
    glcd_fillScreen(OFF);      //Limpia la pantalla
    displayContrasena(Contrasena[0],20,30,1);
    displayContrasena(Contrasena[1],40,30,0);
    displayContrasena(Contrasena[2],60,30,0);
    displayContrasena(Contrasena[3],80,30,0); 
    
    for(i=0;i<80;i++){
        Temperatura=leeTermocupla(MediaMovil);
    }
    penInicial=penMufla;
    Opcion=1;Control=0;    
    
    BUZZER_Write(1);
    CyDelay(100);
    BUZZER_Write(0); 
    Menu=0;  
    unidad=1;
    
    if(UP_Read()==0 && DOWN_Read()==0){
        for(;;){ 
            displayTemperatura((uint16)Temperatura,30,45,0,1);
            displayFloat(Temperatura,30,15,1);
            displayTemperatura((uint16)coldJnTemp,30,0,0,1);
            glcd_fillScreen(OFF);
            glcd_update();  
            Temperatura=leeTermocupla(20);
            if(Lectura2){
                grafica=ON;
                Lectura2=0;
                Temperatura=leeTermocupla(20);
                
                if(!Test){
                    if(coldJnTemp>=3000){
                        VENT1_Write(0);    
                    }else if(coldJnTemp<2500){
                        VENT1_Write(1);   
                    }
                }
            }
        }
    }
    
    for(;;)
    {   
        if(Lectura2){
            grafica=ON;
            Lectura2=0;
            Temperatura=leeTermocupla(MediaMovil);
            if(!Test){
                if(coldJnTemp>=3000 || Temperatura>150){
                    VENT1_Write(0);    
                }else if(coldJnTemp<2500){
                    VENT1_Write(1);   
                }
            }
        }
        
    glcd_update();            
      
//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==0){ //Menu de Contraseña.
      
      strcpy(Menus.Letra,"Clave");
      displayMenu(Menus.Letra,30,0,1,2);     
      
      if(UP_Read()==0){
         BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
         CyDelay(200);
         for(i=1;i<=4;i++){
            if(unidad==i){
               Contrasena[i-1]++;
               if(Contrasena[i-1]>9)   
                  Contrasena[i-1]=0;
               displayContrasena(Contrasena[i-1],i*20,30,1);
            }else{
               displayContrasena(Contrasena[i-1],i*20,30,0);
            }
         }
      }
            
      if(DOWN_Read()==0){
         BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
         CyDelay(200);
         for(i=1;i<=4;i++){
            if(unidad==i){
               Contrasena[i-1]--;
               if(Contrasena[i-1]<0)
                  Contrasena[i-1]=9;
               displayContrasena(Contrasena[i-1],i*20,30,1);}
            else
            displayContrasena(Contrasena[i-1],i*20,30,0);
         }        
      }
            
      if(RIGHT_Read()==0){
         BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
         CyDelay(200);unidad++;
         for(i=1;i<=4;i++){
            if(unidad==i)
            {displayContrasena(Contrasena[i-1],i*20,30,1);}
            else
            {displayContrasena(Contrasena[i-1],i*20,30,0);}
         }
      }
    
      if(unidad>4){
        glcd_fillScreen(OFF);unidad=4;
         if(Contrasena[0]==3&&Contrasena[1]==8&&Contrasena[2]==9&&Contrasena[3]==2) // Si Ingresa clave para reset general del sistema.
            {EEPROM_WriteByte(0,460);CyDelay(20);EEPROM_WriteByte(0,461);CyDelay(20);// Reestablece a contraseña de Fabrica y reinicia Programa.
             EEPROM_WriteByte(0,462);CyDelay(20);EEPROM_WriteByte(0,463);CyDelay(20);
             CySoftwareReset();}        
         
         if((Contrasena[0]==Password[0])&&(Contrasena[1]==Password[1])&&(Contrasena[2]==Password[2])&&(Contrasena[3]==Password[3])){
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra,"Clave");
            displayMenu(Menus.Letra,30,0,0,2);
            strcpy(Menus.Letra,"Correcta");
            displayMenu(Menus.Letra,15,30,0,2);
            Menu=1;glcd_update();CyDelay(1000);
            glcd_fillScreen(OFF);unidad=1;
            Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
         }else{
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra,"Clave");
            displayMenu(Menus.Letra,30,0,0,2);
            strcpy(Menus.Letra,"Incorrecta");
            displayMenu(Menus.Letra,0,30,0,2);
            Menu=0;glcd_update();CyDelay(1000);
            glcd_fillScreen(OFF);unidad=1;
            Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
            displayContrasena(Contrasena[0],20,30,1);
            displayContrasena(Contrasena[1],40,30,0);
            displayContrasena(Contrasena[2],60,30,0);
            displayContrasena(Contrasena[3],80,30,0);
         }
      }
    }//Clave OK
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==1){ //Menu Principal.      
      
         if(Opcion>5)
         {Opcion=1;}
         if(Opcion<1)
         {Opcion=5;}
         
         strcpy(Menus.Letra,"Menu");
         displayMenu(Menus.Letra,30,0,1,2);
      
         if(Opcion==1)   
            {strcpy(Menus.Letra,"Iniciar Programa");
            displayMenu(Menus.Letra,0,18,1,1);}
         else
            {strcpy(Menus.Letra,"Iniciar Programa");
            displayMenu(Menus.Letra,0,18,0,1);}
         
         if(Opcion==2)   
            {strcpy(Menus.Letra,"Editar Programa");
            displayMenu(Menus.Letra,0,27,1,1);}
         else
            {strcpy(Menus.Letra,"Editar Programa");
            displayMenu(Menus.Letra,0,27,0,1);}
            
         if(Opcion==3)   
            {strcpy(Menus.Letra,"Borrar Programa");
            displayMenu(Menus.Letra,0,36,1,1);}
         else
            {strcpy(Menus.Letra,"Borrar Programa");
            displayMenu(Menus.Letra,0,36,0,1);}   
                        
         if(Opcion==4)   
            {strcpy(Menus.Letra,"Ajustes");
            displayMenu(Menus.Letra,0,45,1,1);}
         else
            {strcpy(Menus.Letra,"Ajustes");
            displayMenu(Menus.Letra,0,45,0,1);}     
         
         if(Opcion==5)   
            {strcpy(Menus.Letra,"Cambio Clave");
            displayMenu(Menus.Letra,0,54,1,1);}
         else
            {strcpy(Menus.Letra,"Cambio Clave");
            displayMenu(Menus.Letra,0,54,0,1);}     
         
         if(UP_Read()==0)//Si oprime hacia arriba
         {
            BUZZER_Write(1);CyDelay(30);Opcion--;BUZZER_Write(0);
         }
                  
         if(DOWN_Read()==0)//Si oprime hacia abajo
         {
            BUZZER_Write(1);CyDelay(30);Opcion++; BUZZER_Write(0);
         }
               
         if(RIGHT_Read()==0)//Si oprime SET
         { 
            BUZZER_Write(1);CyDelay(50);Menu=Opcion+3;Opcion=1;glcd_fillScreen(OFF);BUZZER_Write(0);
            if(Menu==7){
               //Menu=100;
            CyDelay(100);
            glcd_fillScreen(OFF);
            while(!PidePassword(1,1,100,4,4,4,4)){
               PidePassword(1,1,100,4,4,4,4);glcd_update();
            }            
            glcd_fillScreen(OFF);Opcion=1;unidad=1;
         
      
            }
         }
      

   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==4){ //Menu de Continuar
      
      if(Opcion>5)
         Opcion=1;
         
      if(Opcion<1)
         Opcion=5;
      
         strcpy(Menus.Letra,"Seleccione");
         displayMenu(Menus.Letra,0,0,1,2);
         
         strcpy(Menus.Letra," Programa");
         displayMenu(Menus.Letra,0,16,1,2);
          
         
         if(Opcion==1)
         {
            strcpy(Menus.Letra,"Programa 01");
            displayMenu(Menus.Letra,0,38,0,2);
         }
         
         if(Opcion==2)
         {
            strcpy(Menus.Letra,"Programa 02");
            displayMenu(Menus.Letra,0,38,0,2);
         }
         
         if(Opcion==3)
         {
            strcpy(Menus.Letra,"Programa 03");
            displayMenu(Menus.Letra,0,38,0,2);
         }
         
         if(Opcion==4)
         {
            strcpy(Menus.Letra,"Programa 04");
            displayMenu(Menus.Letra,0,38,0,2);
         }
         
         if(Opcion==5)
         {
            strcpy(Menus.Letra,"Programa 05");
            displayMenu(Menus.Letra,0,38,0,2);
         }
         
        
      if(UP_Read()==0)//Si oprime hacia arriba
      {
         BUZZER_Write(1);CyDelay(30);Opcion++;BUZZER_Write(0);
         glcd_rect(40, 35, 85, 60, YES, OFF);
      }
            
      if(DOWN_Read()==0)//Si oprime hacia abajo
      {  
         BUZZER_Write(1);CyDelay(30);Opcion--;BUZZER_Write(0);
         glcd_rect(40, 35, 85, 60, YES, OFF);
      }
            
      if(RIGHT_Read()==0)//Si oprime SET
      {  
         Menu=20;glcd_fillScreen(OFF);
         for(i=0;i<Pasos;i++){
            if(Temperatura1[i+((Opcion-1)*Pasos)]==0){
               Segmentos=i;
               break;
            }
            else{
               Segmentos=Pasos-1;
            }
            //Horas1[i+((Opcion-1)*10)]=0;
            //Minutos1[i+((Opcion-1)*10)]=0;
         }
         if(Segmentos>0){
            ProgramaSeleccionado=Opcion-1;
            glcd_fillScreen(OFF);
            
            strcpy(Menus.Letra," Segmentos");
            displayMenu(Menus.Letra,0,0,0,2);
            
            displayTiempo(Segmentos,63,30,3,2);
            CyDelay(2000);
            glcd_fillScreen(OFF);
            Opcion=1;visual=1;//Faltaa cuadrar esta parte
            TempeInicial=Temperatura;
            Ganancia=GananciaIn*(Setpoint/Coeficiente);
            
            Tiempo=(Horas1[ProgramaSeleccionado*Pasos]*3600)+(Minutos1[ProgramaSeleccionado*Pasos]*60);         
            Setpoint=Temperatura1[(ProgramaSeleccionado*Pasos)+Segmento];
            Pendiente=((Setpoint-TempeInicial)/Tiempo);
            Testerilizacionp[3]=(int8)Horas1[(ProgramaSeleccionado*Pasos)+Segmento]/10;
            Testerilizacionp[2]=Horas1[(ProgramaSeleccionado*Pasos)+Segmento]-(Testerilizacionp[3]*10);
            Testerilizacionp[1]=Minutos1[(ProgramaSeleccionado*Pasos)+Segmento]/10;
            Testerilizacionp[0]=Minutos1[(ProgramaSeleccionado*Pasos)+Segmento]-(Testerilizacionp[1]*10);
            
         }
         else{
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra," !Programa");
            displayMenu(Menus.Letra,0,0,0,2);
            strcpy(Menus.Letra,"    Vacio!");
            displayMenu(Menus.Letra,0,20,0,2);
            Opcion=1;BUZZER_Write(0);CyDelay(2000);Menu=1;glcd_fillScreen(OFF);Opcion=1;visual=1;
         }
      } 
                  
      if(LEFT_Read()==0)//Si oprime boton de Toma.
      {
         BUZZER_Write(1);Opcion=1;BUZZER_Write(0);CyDelay(50);Menu=1;glcd_fillScreen(OFF);Opcion=1;visual=1;
      } 
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==5){ //Menu Editar Programa
      
         if(Opcion>5)
         {Opcion=1;}
         if(Opcion<1)
         {Opcion=5;}
         
         strcpy(Menus.Letra,"Editar");
         displayMenu(Menus.Letra,30,0,1,2);
      
         if(Opcion==1)   
            {strcpy(Menus.Letra,"Programa 01");
            displayMenu(Menus.Letra,0,18,1,1);}
         else
            {strcpy(Menus.Letra,"Programa 01");
            displayMenu(Menus.Letra,0,18,0,1);}
         
         if(Opcion==2)   
            {strcpy(Menus.Letra,"Programa 02");
            displayMenu(Menus.Letra,0,27,1,1);}
         else
            {strcpy(Menus.Letra,"Programa 02");
            displayMenu(Menus.Letra,0,27,0,1);}
            
         if(Opcion==3)   
            {strcpy(Menus.Letra,"Programa 03");
            displayMenu(Menus.Letra,0,36,1,1);}
         else
            {strcpy(Menus.Letra,"Programa 03");
            displayMenu(Menus.Letra,0,36,0,1);}   
                        
         if(Opcion==4)   
            {strcpy(Menus.Letra,"Programa 04");
            displayMenu(Menus.Letra,0,45,1,1);}
         else
            {strcpy(Menus.Letra,"Programa 04");
            displayMenu(Menus.Letra,0,45,0,1);}     
         
         if(Opcion==5)   
            {strcpy(Menus.Letra,"Programa 05");
            displayMenu(Menus.Letra,0,54,1,1);}
         else
            {strcpy(Menus.Letra,"Programa 05");
            displayMenu(Menus.Letra,0,54,0,1);}     
         
         if(UP_Read()==0)//Si oprime hacia arriba
         {
            BUZZER_Write(1);CyDelay(30);Opcion--;BUZZER_Write(0);
         }
                  
         if(DOWN_Read()==0)//Si oprime hacia abajo
         {
            BUZZER_Write(1);CyDelay(30);Opcion++; BUZZER_Write(0);
         }
               
         if(RIGHT_Read()==0)//Si oprime SET
         { 
            BUZZER_Write(1);CyDelay(50);Menu=Opcion+10;Opcion=1;glcd_fillScreen(OFF);BUZZER_Write(0);i=0;
         }
         
         if(LEFT_Read()==0){
            BUZZER_Write(1);CyDelay(50);Menu=1;Opcion=1;glcd_fillScreen(OFF);BUZZER_Write(0);
         }
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==6){ //Menu de Borrar Programa
      
      if(Opcion>5)
         Opcion=1;
         
      if(Opcion<1)
         Opcion=5;
      
         strcpy(Menus.Letra,"  Elimine");
         displayMenu(Menus.Letra,0,0,1,2);
         
         strcpy(Menus.Letra," Programa");
         displayMenu(Menus.Letra,0,16,1,2);
          
         
         if(Opcion==1)
         {
            strcpy(Menus.Letra,"Programa 01");
            displayMenu(Menus.Letra,0,38,0,2);
         }
         
         if(Opcion==2)
         {
            strcpy(Menus.Letra,"Programa 02");
            displayMenu(Menus.Letra,0,38,0,2);
         }
         
         if(Opcion==3)
         {
            strcpy(Menus.Letra,"Programa 03");
            displayMenu(Menus.Letra,0,38,0,2);
         }
         
         if(Opcion==4)
         {
            strcpy(Menus.Letra,"Programa 04");
            displayMenu(Menus.Letra,0,38,0,2);
         }
         
         if(Opcion==5)
         {
            strcpy(Menus.Letra,"Programa 05");
            displayMenu(Menus.Letra,0,38,0,2);
         }
         
        
      if(UP_Read()==0)//Si oprime hacia arriba
      {
         BUZZER_Write(1);CyDelay(30);Opcion++;BUZZER_Write(0);
         glcd_rect(40, 35, 85, 60, YES, OFF);
      }
            
      if(DOWN_Read()==0)//Si oprime hacia abajo
      {  
         BUZZER_Write(1);CyDelay(30);Opcion--;BUZZER_Write(0);
         glcd_rect(40, 35, 85, 60, YES, OFF);
      }
            
      if(RIGHT_Read()==0)//Si oprime SET
      {  
         BUZZER_Write(1);
         for(i=0;i<Pasos;i++){
            Temperatura1[i+((Opcion-1)*Pasos)]=0;
            Horas1[i+((Opcion-1)*Pasos)]=0;
            Minutos1[i+((Opcion-1)*Pasos)]=0;
         }
         
         for(i=0;i<Pasos;i++){
            CyDelay(10);    
            EEPROM_WriteByte(0,(i*2)+((Pasos*4)*(Opcion-1)));
            CyDelay(10);    
            EEPROM_WriteByte(0,(i*2)+1+((Pasos*4)*(Opcion-1)));
            CyDelay(10);    
            EEPROM_WriteByte(0,i+(((Pasos*4)*Opcion)-(Pasos*2)));
            CyDelay(10);    
            EEPROM_WriteByte(0,i+(((Pasos*4)*Opcion)-Pasos));
         } 
         
         CyDelay(200);BUZZER_Write(0);Menu=1;glcd_fillScreen(OFF);Opcion=1;visual=1;
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Programa");
         displayMenu(Menus.Letra,15,0,0,2);
         strcpy(Menus.Letra,"Eliminado");
         displayMenu(Menus.Letra,15,30,0,2);
         Menu=1;glcd_update();CyDelay(1000);
         glcd_fillScreen(OFF);unidad=1;
      } 
                  
      if(LEFT_Read()==0)//Si oprime boton de Toma.
      {
         BUZZER_Write(1);Opcion=1;BUZZER_Write(0);CyDelay(500);Menu=1;glcd_fillScreen(OFF);Opcion=1;visual=1;
      } 
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==8){ // Menu de Cambio de Contraseña
        
      strcpy(Menus.Letra,"Clave");
      displayMenu(Menus.Letra,30,0,1,2);     
      
      if(unidad==1)
      {
         displayContrasena(Contrasena[0],20,30,1);
         displayContrasena(Contrasena[1],40,30,0);
         displayContrasena(Contrasena[2],60,30,0);
         displayContrasena(Contrasena[3],80,30,0);
      }
      else
      {
         if(unidad==2)
         {
            displayContrasena(Contrasena[0],20,30,0);
            displayContrasena(Contrasena[1],40,30,1);
            displayContrasena(Contrasena[2],60,30,0);
            displayContrasena(Contrasena[3],80,30,0);
         }
         else
         {
            if(unidad==3)
            {
               displayContrasena(Contrasena[0],20,30,0);
               displayContrasena(Contrasena[1],40,30,0);
               displayContrasena(Contrasena[2],60,30,1);
               displayContrasena(Contrasena[3],80,30,0);
            }
            else
            {
               if(unidad==4)
               {
                  displayContrasena(Contrasena[0],20,30,0);
                  displayContrasena(Contrasena[1],40,30,0);
                  displayContrasena(Contrasena[2],60,30,0);
                  displayContrasena(Contrasena[3],80,30,1);
               }
            }
         }
      }
      
      if(UP_Read()==0)//Si oprime hacia arriba
      {
         BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
         CyDelay(30);
         for(i=1;i<=4;i++)
         {
            if(unidad==i)
            {
               Contrasena[i-1]++;
               if(Contrasena[i-1]>9)   
               {
                  Contrasena[i-1]=0;
               }
            }
         }
      }
            
      if(DOWN_Read()==0)//Si oprime hacia abajo
      {
         BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
         CyDelay(30);
         for(i=1;i<=4;i++)
         {
            if(unidad==i)
            {Contrasena[i-1]--; 
            if(Contrasena[i-1]<0)
            {Contrasena[i-1]=9;}
            }
         }  
      }
            
      if(RIGHT_Read()==0)//Si oprime RIGHT_Read()==0
      {
         BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);CyDelay(30);unidad++;
      }
      
      if(LEFT_Read()==0)//Si oprime Izquierda
      {
         BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);CyDelay(700);Menu=1;glcd_fillScreen(OFF);
      }
      
      if(unidad>4)
      {glcd_fillScreen(OFF);
         
         if((Contrasena[0]==Password[0])&&(Contrasena[1]==Password[1])&&(Contrasena[2]==Password[2])&&(Contrasena[3]==Password[3]))
         {
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra,"Clave");
            displayMenu(Menus.Letra,30,0,0,2);
            strcpy(Menus.Letra,"Correcta");
            displayMenu(Menus.Letra,15,30,0,2);
            Menu=91;glcd_update();CyDelay(1000);
            glcd_fillScreen(OFF);unidad=1;
            Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
         }// Esta parte se puede agregar en el Menu 1 dependiendo de la ubicación del vidrio.
         else
         {
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra,"Clave");
            displayMenu(Menus.Letra,30,0,0,2);
            strcpy(Menus.Letra,"Incorrecta");
            displayMenu(Menus.Letra,0,30,0,2);
            Menu=1;glcd_update();CyDelay(1000);
            glcd_fillScreen(OFF);unidad=1;
            Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
         }
      }
    }    
//_--------------------------------------------------------------------------------------------------------------------------------------------------------    

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==91){ // Menu de Cambio de Contraseña
        
      strcpy(Menus.Letra,"Clave Nueva");
      displayMenu(Menus.Letra,0,0,1,2);     
      
      if(unidad==1)
      {
         displayContrasena(Contrasena[0],20,30,1);
         displayContrasena(Contrasena[1],40,30,0);
         displayContrasena(Contrasena[2],60,30,0);
         displayContrasena(Contrasena[3],80,30,0);
      }
      else
      {
         if(unidad==2)
         {
            displayContrasena(Contrasena[0],20,30,0);
            displayContrasena(Contrasena[1],40,30,1);
            displayContrasena(Contrasena[2],60,30,0);
            displayContrasena(Contrasena[3],80,30,0);
         }
         else
         {
            if(unidad==3)
            {
               displayContrasena(Contrasena[0],20,30,0);
               displayContrasena(Contrasena[1],40,30,0);
               displayContrasena(Contrasena[2],60,30,1);
               displayContrasena(Contrasena[3],80,30,0);
            }
            else
            {
               if(unidad==4)
               {
                  displayContrasena(Contrasena[0],20,30,0);
                  displayContrasena(Contrasena[1],40,30,0);
                  displayContrasena(Contrasena[2],60,30,0);
                  displayContrasena(Contrasena[3],80,30,1);
               }
            }
         }
      }
      
      if(UP_Read()==0)//Si oprime hacia arriba
      {
         BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
         CyDelay(30);
         for(i=1;i<=4;i++)
         {
            if(unidad==i)
            {
               Password[i-1]++;
               if(Contrasena[i-1]>9)   
               {
                  Contrasena[i-1]=0;
               }
            }
         }
      }
            
      if(DOWN_Read()==0)//Si oprime hacia abajo
      {
         BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
         CyDelay(30);
         for(i=1;i<=4;i++)
            {
               if(unidad==i)
               {
                  Contrasena[i-1]--;
                  if(Contrasena[i-1]<0)
                  {
                     Contrasena[i-1]=9;
                  }
               }
            }       
      }
            
      if(RIGHT_Read()==0)//Si oprime RIGHT_Read()==0
      {
         BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);CyDelay(30);unidad++;
      }
      
      if(LEFT_Read()==0)//Si oprime Izquierda
      {
         BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);CyDelay(700);Menu=1;glcd_fillScreen(OFF);
      }
      
      if(unidad>4)
      {
         Password[0]=Contrasena[0];
         Password[1]=Contrasena[1];
         Password[2]=Contrasena[2];
         Password[3]=Contrasena[3];
         Opcion=1;unidad=1;
         EEPROM_WriteByte(Password[0],460);
         CyDelay(50);   
         EEPROM_WriteByte(Password[1],461);
         CyDelay(50);   
         EEPROM_WriteByte(Password[2],462);
         CyDelay(50);   
         EEPROM_WriteByte(Password[3],463);
         CyDelay(1000);glcd_fillScreen(OFF);Menu=1;
      }
    }    
//_--------------------------------------------------------------------------------------------------------------------------------------------------------    

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu>10 && Menu<16){ //Menu Editar Programa 01, Programa 02, Programa 03, Programa 04 y Programa 05
      
         if(Opcion>3)
         {
            Opcion=1;
            if(i<15){
               i++;  
            }
         }
         
         if(Opcion<1)
         {
            if(i>0)
               i--;
               
            Opcion=3;
         }
         
         if(Menu==11)
            Programa=1;
         
         if(Menu==12)
            Programa=2;
            
         if(Menu==13)
            Programa=3;
            
         if(Menu==14)
            Programa=4;
            
         if(Menu==15)
            Programa=5;   
         
         MuestraSegmento(Opcion,i,Programa);
         
         if(RIGHT_Read()==0 && LEFT_Read()==0){
            BUZZER_Write(1);Menu=5;Opcion=1;glcd_fillScreen(OFF);CyDelay(500);BUZZER_Write(0);
            
            for(i=0;i<Pasos;i++){
               CyDelay(10);    
               EEPROM_WriteByte(convert8(Temperatura1[i+((Programa-1)*Pasos)],0),(i*2)+((Pasos*4)*(Programa-1)));
               CyDelay(10);    
               EEPROM_WriteByte(convert8(Temperatura1[i+((Programa-1)*Pasos)],1),(i*2)+1+((Pasos*4)*(Programa-1)));
               CyDelay(10);    
               EEPROM_WriteByte(Horas1[i+((Programa-1)*Pasos)],i+(((Pasos*4)*Programa)-((Pasos*2))));
               CyDelay(10);    
               EEPROM_WriteByte(Minutos1[i+((Programa-1)*Pasos)],i+(((Pasos*4)*Programa)-Pasos));
            } 
         }
         
         if(RIGHT_Read()==0)//Si oprime SET
         { 
            BUZZER_Write(1);CyDelay(50);Opcion++;glcd_fillScreen(OFF);BUZZER_Write(0);
         }
         
         if(LEFT_Read()==0){
            BUZZER_Write(1);CyDelay(50);Opcion--;glcd_fillScreen(OFF);BUZZER_Write(0);
         }
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==20){ //Menu de Funcionamiento
      
      // Muestra imagen de fondo en Display GLCD      
      glcd_imagen(5);      

      if(UP_Read()==0 && DOWN_Read()==0)//Si oprime hacia abajo
      {
         glcd_init(ON);CyDelay(200);flag_display=!flag_display;         
      }
    
      
      if(LEFT_Read()==0){//Si oprime boton de Toma.
         if(Pausa<3){
            if(Pausa!=2){
               CyDelay(500);Menu=30;glcd_fillScreen(OFF);Opcion=1;visual=1;Pausa=1;Menu=1;
            }
         }
      }

      if (tcHotJnuVolt < BROKEN_TC_THRESHOLD){
         glcd_rect(0, 55, 127, 63, YES, OFF);
         strcpy(Menus.Letra,"Status:Sensor Descon.");
         displayMenu(Menus.Letra,0,55,2,1);
         tiempo_esterilizacion=0;
         SSR_Write(0);
         Tpulso=0.0;
         flag_tiempocalentamiento=0;
         BUZZER_Write(1);
         CyDelay(500);
         BUZZER_Write(0);
         CyDelay(250);
         Control=0;
      }else{
         if(Pausa==0 && DOOR_Read()==0){
            Control=1;
            if(Temperatura>Setpoint+AlTem && AlTem>0){
               Alarma=5;
            }else{  
               Alarma=10;
               if((Temperatura>=Setpoint-10) && (Temperatura<Setpoint+50)){
                  if(Pendiente!=0){
                     tiempo_esterilizacion=1;
                     if(flag_display==0){
                        glcd_rect(0, 55, 127, 63, YES, OFF);
                        strcpy(Menus.Letra,"Status:Controlando");
                        displayMenu(Menus.Letra,0,55,2,1);
                     }
                  }
                  
                  if(Pendiente==0){
                     if(flag_display==0){
                        glcd_rect(0, 55, 127, 63, YES, OFF);
                        strcpy(Menus.Letra,"Status:Meseta  ");
                        displayMenu(Menus.Letra,0,55,2,1);
                     }
                     tiempo_esterilizacion=2;
                  }
                  Controla();
                  flag_tiempocalentamiento=0;
               }else{
                  if(Pendiente!=0){
                     tiempo_esterilizacion=1;
                     if(flag_display==0){
                        glcd_rect(0, 55, 127, 63, YES, OFF);
                        strcpy(Menus.Letra,"Status:Rampa  ");
                        displayMenu(Menus.Letra,0,55,2,1);
                     }
                  }
                  
                  if(Pendiente==0){
                     if(flag_display==0){
                        glcd_rect(0, 55, 127, 63, YES, OFF);
                        strcpy(Menus.Letra,"Status:Calentando");
                        displayMenu(Menus.Letra,0,55,2,1);
                     }
                     tiempo_esterilizacion=0;
                  }
                  Controla();
               }
            }
         }
         
         if(Pausa==1 && DOOR_Read()==1){
            flag_tiempocalentamiento=0;
            strcpy(Menus.Letra,"Status:Pausado");
            displayMenu(Menus.Letra,0,55,2,1);
            tiempo_esterilizacion=0;
            Control=0;
            SSR_Write(0);
            Tpulso=0.0;
         }
         
         if(DOOR_Read()==1){
            Control=0;            
            flag_tiempocalentamiento=0;
            tiempo_esterilizacion=0;
            SSR_Write(0);
            Tpulso=0.0;
            if(flag_clear2==0){
               Ganancia=GananciaIn*(Setpoint/Coeficiente);
               glcd_fillScreen(OFF);
               glcd_init(ON);
               glcd_rect(0, 55, 127, 63, YES, OFF);
               Tpulso=0.0;
               flag_clear2=1;
               flag_ganancia=0;
            }
            strcpy(Menus.Letra,"Status:Puerta Abierta");
            displayMenu(Menus.Letra,0,55,2,1);
            BUZZER_Write(1);
            CyDelay(1000);
            BUZZER_Write(0);
            CyDelay(1000);
         }else{
            if(flag_clear2==1){
               glcd_fillScreen(OFF);
               glcd_rect(0, 55, 127, 63, YES, OFF);
               flag_clear2=0;
            }
            if(Alarma==5){
                Control=0;            
                flag_tiempocalentamiento=0;
                tiempo_esterilizacion=0;
                SSR_Write(0);
                Tpulso=0.0;
                if(flag_clear3==0){
                    glcd_rect(0, 55, 127, 63, YES, OFF);
                    tiempo_esterilizacion=0;
                    SSR_Write(0);
                    Tpulso=0.0;
                    flag_clear3=1;
                    flag_ganancia=0;
                }
                strcpy(Menus.Letra,"Status:Sobre Temp");
                displayMenu(Menus.Letra,0,55,2,1); 
                Ganancia=gananciaIn*(Setpoint/150);
                BUZZER_Write(1);
                CyDelay(500);
                BUZZER_Write(0);
                CyDelay(500);
            }else{
                Alarma=0;
                if(flag_clear3==1){
                    glcd_rect(0, 55, 127, 63, YES, OFF);
                    flag_clear3=0;
                }
                BUZZER_Write(0);
            } 
         }
      }
      
      if(RIGHT_Read()==0)//Si oprime SET
      {  
         if(Pausa!=2){
            if(Pausa==0){
               CyDelay(200);Pausa=1;
            }else{
               CyDelay(200);Pausa=0;
            }
            glcd_rect(0, 55, 127, 63, YES, OFF);
         }
      }
      
        
      if(Lectura==1){// Utilizado para visualizar la lectura de la temperatura
         Lectura=0;
         Potencia=(100.0/Tciclo)*Tpulso;
 //----------------Muestra Temperatura--------------------------
         if (tcHotJnuVolt < BROKEN_TC_THRESHOLD){
            glcd_rect(15, 29, 69, 45, YES, OFF);
            strcpy(Menus.Letra,"-----");displayMenu(Menus.Letra,15,29,2,2);
            Desconectado=1;
            Control=0;
         }else{
            if(Desconectado==1){
               Desconectado=0;
               CyDelay(2000);
               for(i=0;i<80;i++){
                  Temperatura=leeTermocupla(MediaMovil);
               }
            }
            displayT(Temperatura,15,29,2);
         }
            Dibuja=0;
            glcd_circle(74,30,2,NO,ON);
            strcpy(Menus.Letra,"C");displayMenu(Menus.Letra,75,30,2,2);
            
            displayTemperatura(Setpoint,30,45,0,1);
            glcd_circle(60,47,1,NO,ON);
            strcpy(Menus.Letra,"C");displayMenu(Menus.Letra,63,45,2,1);
            strcpy(Menus.Letra,"Temp:");displayMenu(Menus.Letra,0,45,2,1); 
            
            strcpy(Menus.Letra,"Power:");displayMenu(Menus.Letra,91,32,2,1);
            displayTiempo(Potencia,97,42,3,1);strcpy(Menus.Letra,"%");displayMenu(Menus.Letra,115,42,2,1);
            strcpy(Menus.Letra,"Segm.");displayMenu(Menus.Letra,95,10,2,1);
            displayTiempo(Segmento,96,20,3,1);
            glcd_rect(90, 0, 127, 54, NO, ON);
         
         if(flag_display==1){
            displayTiempo(penMufla,97,1,3,1);
            displayFloat(TemperaturaIdeal,97,10,1);
         }
         
         // Muestra Valor de esterilizacion Programado.
            displayTiempo(Testerilizacion[3],20,2,0,2);displayTiempo(Testerilizacion[2],33,2,0,2);
            strcpy(Menus.Letra,":");displayMenu(Menus.Letra,46,2,2,2); 
            displayTiempo(Testerilizacion[1],55,2,0,2);displayTiempo(Testerilizacion[0],68,2,0,2);
            
            // Muestra Valor de esterilizacion actual.
            strcpy(Menus.Letra,"Time:");displayMenu(Menus.Letra,0,18,2,1); 
            //displayTiempos(Horas1[Segmento],30,18,2,1);//displayTiempo(Testerilizacionp[2],36,18,0,1);
            displayTiempo(Testerilizacionp[3],30,18,0,1);displayTiempo(Testerilizacionp[2],36,18,0,1);
            strcpy(Menus.Letra,":");displayMenu(Menus.Letra,42,18,2,1); 
            //displayTiempos(Minutos1[Segmento],48,18,2,1);//displayTiempo(Testerilizacionp[0],54,18,0,1);
            displayTiempo(Testerilizacionp[1],48,18,0,1);displayTiempo(Testerilizacionp[0],54,18,0,1);
            strcpy(Menus.Letra,"(H:M)");displayMenu(Menus.Letra,60,18,2,1); 
         
            glcd_rect(0, 0, 89, 27, NO, ON);
            glcd_rect(0, 27, 89, 54, NO, ON);
      } 
     
      // Si cumplio el tiempo de esterilización, Muestre ciclo finalizado y detenga control de temperatura.
      if((Testerilizacion[0]>=Testerilizacionp[0])&&(Testerilizacion[1]>=Testerilizacionp[1])&&(Testerilizacion[2]>=Testerilizacionp[2])&&(Testerilizacion[3]>=Testerilizacionp[3]) && Pausa<3){
         flag_calcula=0;
         if(Segmento<Segmentos){
            Segmento++;
            Testerilizacion[0]=0;
            Testerilizacion[1]=0;
            Testerilizacion[2]=0;
            Testerilizacion[3]=0;
         }else{
            Pausa=3;
         }
         glcd_fillScreen(OFF);
         BUZZER_Write(1);
         CyDelay(1000);
         BUZZER_Write(0);
         CyDelay(1000);
         glcd_init(ON);
         strcpy(Menus.Letra,"Status:Cambio Seg.");
         displayMenu(Menus.Letra,0,55,2,1);         
         CyDelay(500);
         
         if(Pausa==3){
            glcd_fillScreen(OFF);
         }
      } 
      
      if(tinicio>=60){
         tinicio=0;   
         Testerilizacion[0]++;
         if(Testerilizacion[0]>9){
            Testerilizacion[0]=0;
            Testerilizacion[1]++;
         }
         if(Testerilizacion[1]>5){
            Testerilizacion[1]=0;
            Testerilizacion[2]++;
         }
         if(Testerilizacion[2]>9){
            Testerilizacion[2]=0;
            Testerilizacion[3]++;
         }
         if(Testerilizacion[3]>9){
            Testerilizacion[3]=0;
         }
      }
      
      if(Pausa==3 && DOOR_Read()==0){         
         //strcpy(Menus.Letra,"Status:Finalizado");
         if(Temperatura>80){
            if(mensaje){
               glcd_rect(0, 55, 127, 63, YES, OFF);
               strcpy(Menus.Letra,"NO APAGUE EL EQUIPO");
               displayMenu(Menus.Letra,0,55,2,1);
            }else{
               glcd_rect(0, 55, 127, 63, YES, OFF);
               strcpy(Menus.Letra,"Status:Finalizado");
               displayMenu(Menus.Letra,0,55,2,1);
            }
         }else{
            if(mensaje){
               glcd_rect(0, 55, 127, 63, YES, OFF);
               strcpy(Menus.Letra,"Puede apagar la Mufla");
               displayMenu(Menus.Letra,0,55,2,1);
            }else{
               glcd_rect(0, 55, 127, 63, YES, OFF);
               strcpy(Menus.Letra,"Status:Finalizado");
               displayMenu(Menus.Letra,0,55,2,1);
            }
         }
         tiempo_esterilizacion=4;
         Control=0;
         SSR_Write(0);
         Tpulso=0.0;
      }
        
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==100){ //Menu de Configuración de Parametros de Control
      
      if(Opcion>12)
      {Opcion=1;}
      if(Opcion<1)
      {Opcion=12;}
      
      if(Opcion<=6){
         if(Opcion==1)   
            {strcpy(Menus.Letra,"Proporcional");
            displayMenu(Menus.Letra,0,0,1,1);}
         else
            {strcpy(Menus.Letra,"Proporcional");
            displayMenu(Menus.Letra,0,0,0,1);}
         
         if(Opcion==2)   
            {strcpy(Menus.Letra,"Integral");
            displayMenu(Menus.Letra,0,9,1,1);}
         else
            {strcpy(Menus.Letra,"Integral");
            displayMenu(Menus.Letra,0,9,0,1);}
            
         if(Opcion==3)   
            {strcpy(Menus.Letra,"Derivativo");
            displayMenu(Menus.Letra,0,18,1,1);}
         else
            {strcpy(Menus.Letra,"Derivativo");
            displayMenu(Menus.Letra,0,18,0,1);}   
         
         if(Opcion==4)   
            {strcpy(Menus.Letra,"Tciclo");
            displayMenu(Menus.Letra,0,27,1,1);}
         else
            {strcpy(Menus.Letra,"Tciclo");
            displayMenu(Menus.Letra,0,27,0,1);}
            
         if(Opcion==5)   
            {strcpy(Menus.Letra,"Ajuste +");
            displayMenu(Menus.Letra,0,36,1,1);}
         else
            {strcpy(Menus.Letra,"Ajuste +");
            displayMenu(Menus.Letra,0,36,0,1);}   
                        
         if(Opcion==6)   
            {strcpy(Menus.Letra,"Ajuste -");
            displayMenu(Menus.Letra,0,45,1,1);}
         else
            {strcpy(Menus.Letra,"Ajuste -");
            displayMenu(Menus.Letra,0,45,0,1);}     
      }
      else{
         if(Opcion==7)   
            {strcpy(Menus.Letra,"Media Movil");
            displayMenu(Menus.Letra,0,0,1,1);}
         else
            {strcpy(Menus.Letra,"Media Movil");
            displayMenu(Menus.Letra,0,0,0,1);}     
            
         if(Opcion==8)   
            {strcpy(Menus.Letra,"Coeficiente");
            displayMenu(Menus.Letra,0,9,1,1);}
         else
            {strcpy(Menus.Letra,"Coeficiente");
            displayMenu(Menus.Letra,0,9,0,1);}  
            
         if(Opcion==9)   
            {strcpy(Menus.Letra,"Alarma SobreT");
            displayMenu(Menus.Letra,0,18,1,1);}
         else
            {strcpy(Menus.Letra,"Alarma SobreT");
            displayMenu(Menus.Letra,0,18,0,1);}     
            
         if(Opcion==10)   
            {strcpy(Menus.Letra,"m Mufla");
            displayMenu(Menus.Letra,0,27,1,1);}
         else
            {strcpy(Menus.Letra,"m Mufla");
            displayMenu(Menus.Letra,0,27,0,1);}     
            
         if(Opcion==11)   
            {strcpy(Menus.Letra,"m");
            displayMenu(Menus.Letra,0,36,1,1);}
         else
            {strcpy(Menus.Letra,"m");
            displayMenu(Menus.Letra,0,36,0,1);}     
         
         if(Opcion==12)
            {strcpy(Menus.Letra,"b");
            displayMenu(Menus.Letra,0,45,1,1);}
         else
            {strcpy(Menus.Letra,"b");
            displayMenu(Menus.Letra,0,45,0,1);}       
         
            strcpy(Menus.Letra,"   ");
            displayMenu(Menus.Letra,0,54,0,1);
         
      }   
      
         if(UP_Read()==0)//Si oprime hacia arriba
         {
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            if(Flanco == 0)
               {Flanco = 1;CyDelay(30);Opcion--;visual=1;
               }
         }
            else
               {Flanco = 0;}
                  
         if(DOWN_Read()==0)//Si oprime hacia abajo
         {
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            if(Flanco2 == 0)
               {Flanco2 = 1;CyDelay(30);Opcion++;visual=1;
               }     
         }
            else
               {Flanco2 = 0;}  
               
         if(RIGHT_Read()==0)//Si oprime SET
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            if(Flanco1 == 0)
               {Flanco1 = 1;CyDelay(30);Menu=Opcion+100;Opcion=1;glcd_fillScreen(OFF);visual=1;
               }
         }
            else
               {Flanco1 = 0;} 
               
         if(LEFT_Read()==0)//Si oprime boton de Toma.
         {
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(700);Menu=1;glcd_fillScreen(OFF);Opcion=1;visual=1;            
         }      
        
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==101){ // Menu de Ajuste de Ganancia
      
      if(Ganancia>40){
         Ganancia=1;
      }
    
      if(Ganancia<1){
         Ganancia=40;
      }
    
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"P");
         displayMenu(Menus.Letra,0,0,1,2);
         
         displayTemperatura(Ganancia,30,30,0,3);
         
         visual=0;
      }
      
         if(UP_Read()==0){  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);Ganancia+=1.0;visual=1;
         }
               
         if(DOWN_Read()==0){  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);Ganancia-=1.0;visual=1;     
         }
               
         if(RIGHT_Read()==0)//Si oprime SET
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);visual=1;
         } 
                  
         if(LEFT_Read()==0)//Si oprime boton de Toma.
         {
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(700);Menu=100;glcd_fillScreen(OFF);Opcion=1;visual=1;
            EEPROM_WriteByte((uint8)Ganancia,450);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==102){ // Menu de Ajuste de Integral
      
      if(Integral>5){
         Integral=1;
      }
    
      if(Integral<1){
         Integral=5;
      }
    
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"I");
         displayMenu(Menus.Letra,0,0,1,2);
         displayTemperatura(Integral,30,30,0,3);
         visual=0;
      }
      
         if(UP_Read()==0)//Si oprime hacia arriba
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);Integral+=1.0;visual=1;
         }
               
         if(DOWN_Read()==0)//Si oprime hacia abajo
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);Integral-=1.0;visual=1;     
         }
               
         if(RIGHT_Read()==0)//Si oprime SET
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);visual=1;
         } 
                  
         if(LEFT_Read()==0)//Si oprime boton de Toma.
         {
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(700);Menu=100;glcd_fillScreen(OFF);Opcion=1;visual=1;
            EEPROM_WriteByte((uint8)Integral,470);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==103){ // Menu de Ajuste de Derivativo
      
      if(Derivativo>40){
         Derivativo=5;
      }
    
      if(Derivativo<5){
         Derivativo=40;
      }
    
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"D");
         displayMenu(Menus.Letra,0,0,1,2);
         displayTemperatura(Derivativo,30,30,0,3);
         visual=0;
      }
      
         if(UP_Read()==0)//Si oprime hacia arriba
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);Derivativo+=1.0;visual=1;
         }
               
         if(DOWN_Read()==0)//Si oprime hacia abajo
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);Derivativo-=1.0;visual=1;     
         }
               
         if(RIGHT_Read()==0)//Si oprime SET
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);visual=1;
         } 
                  
         if(LEFT_Read()==0)//Si oprime boton de Toma.
         {
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(700);Menu=100;glcd_fillScreen(OFF);Opcion=1;visual=1;
            EEPROM_WriteByte((uint8)Derivativo,471);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==104){ // Menu de Tciclo
      
      if(Tciclo>20){
         Tciclo=5;
      }
    
      if(Tciclo<5){
         Tciclo=20;
      }
    
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Tciclo");
         displayMenu(Menus.Letra,0,0,1,2);
         displayTemperatura(Tciclo,30,30,0,3);
         visual=0;
      }
      
         if(UP_Read()==0)//Si oprime hacia arriba
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);Tciclo+=1.0;visual=1;
         }
               
         if(DOWN_Read()==0)//Si oprime hacia abajo
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);Tciclo-=1.0;visual=1;     
         }
               
         if(RIGHT_Read()==0)//Si oprime SET
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);visual=1;
         } 
                  
         if(LEFT_Read()==0)//Si oprime boton de Toma.
         {
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(700);Menu=100;glcd_fillScreen(OFF);Opcion=1;visual=1;
            EEPROM_WriteByte((uint8)Tciclo,451);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==105){ // Menu de Ajuste de Ajuste
      
      if(Ajuste>30){
         Ajuste=0;
      }
      
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Ajuste");
         displayMenu(Menus.Letra,0,0,1,2);
         displayTemperatura(Ajuste,30,30,0,3);
         visual=0;
      }
      
         if(UP_Read()==0)//Si oprime hacia arriba
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            if(Ajuste>30)
            {
               Ajuste=30;CyDelay(100);visual=1;
            }
            else
            {
               CyDelay(30);Ajuste+=1.0;visual=1;
            }
         }
               
         if(DOWN_Read()==0)//Si oprime hacia abajo
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            if(Ajuste>0)
            {
               CyDelay(30);Ajuste-=1.0;visual=1;     
            }
         }
               
         if(RIGHT_Read()==0)//Si oprime SET
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);visual=1;
         } 
                  
         if(LEFT_Read()==0)//Si oprime boton de Toma.
         {
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(700);Menu=100;glcd_fillScreen(OFF);Opcion=1;visual=1;
            EEPROM_WriteByte(Ajuste,452);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==106){ // Menu de Ajuste de Ajusten
      
      if(Ajusten>30){
         Ajusten=0;
      }
      
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Ajusten");
         displayMenu(Menus.Letra,0,0,1,2);
         displayTemperatura(Ajusten,30,30,0,3);
         visual=0;
      }
      
         if(UP_Read()==0)//Si oprime hacia arriba
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            if(Ajusten>30)
            {
               Ajusten=30;CyDelay(100);visual=1;
            }
            else
            {
               CyDelay(30);Ajusten+=1.0;visual=1;
            }
         }
               
         if(DOWN_Read()==0)//Si oprime hacia abajo
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            if(Ajusten>0)
            {
               CyDelay(30);Ajusten-=1.0;visual=1;     
            }
         }
               
         if(RIGHT_Read()==0)//Si oprime SET
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);visual=1;
         } 
                  
         if(LEFT_Read()==0)//Si oprime boton de Toma.
         {
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(700);Menu=100;glcd_fillScreen(OFF);Opcion=1;visual=1;
            EEPROM_WriteByte(Ajusten,453);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==107){ // Menu de Ajuste de Media Movil
      
      if(MediaMovil>80){
         MediaMovil=10;
      }
      
      if(MediaMovil<10){
         MediaMovil=80;
      }
    
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Media");
         displayMenu(Menus.Letra,0,0,1,2);
         displayTemperatura(MediaMovil,30,30,0,3);
         visual=0;
      }
      
         if(UP_Read()==0)//Si oprime hacia arriba
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);MediaMovil+=1.0;visual=1;
         }
               
         if(DOWN_Read()==0)//Si oprime hacia abajo
         {  
            CyDelay(30);MediaMovil-=1.0;visual=1;     
         }
               
         if(RIGHT_Read()==0)//Si oprime SET
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);visual=1;
         } 
                  
         if(LEFT_Read()==0)//Si oprime boton de Toma.
         {
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(700);Menu=100;glcd_fillScreen(OFF);Opcion=1;visual=1;
            EEPROM_WriteByte((uint8)MediaMovil,454);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==108){ // Menu de Ajuste de Coeficiente Regulador
      
      if(Coeficiente>250){
         Coeficiente=100;
      }
      
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Coeficiente");
         displayMenu(Menus.Letra,0,0,1,2);
         displayTemperatura(Coeficiente,30,30,0,3);
         visual=0;
      }
      
         if(UP_Read()==0)//Si oprime hacia arriba
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            if(Coeficiente>250)
            {
               Coeficiente=250;CyDelay(100);visual=1;
            }
            else
            {
               CyDelay(30);Coeficiente+=1.0;visual=1;
            }
         }
               
         if(DOWN_Read()==0)//Si oprime hacia abajo
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            if(Coeficiente>1)
            {
               CyDelay(30);Coeficiente-=1.0;visual=1;     
            }
         }
               
         if(RIGHT_Read()==0)//Si oprime SET
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);visual=1;
         } 
                  
         if(LEFT_Read()==0)//Si oprime boton de Toma.
         {
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(700);Menu=100;glcd_fillScreen(OFF);Opcion=1;visual=1;
            EEPROM_WriteByte((uint8)Coeficiente,455);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==109){ // Menu de Alarma Sobretemperatura
      
      if(AlTem>500)
         AlTem=500;
      
      
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Alarma");
         displayMenu(Menus.Letra,0,0,1,2);
         displayTemperatura(AlTem,30,30,0,3);
         visual=0;
      }
      
         if(UP_Read()==0)//Si oprime hacia arriba
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            if(AlTem>500)
            {
               AlTem=500;CyDelay(100);visual=1;
            }
            else
            {
               CyDelay(30);AlTem+=1.0;visual=1;
            }
         }
               
         if(DOWN_Read()==0)//Si oprime hacia abajo
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            if(AlTem>0)
            {
               CyDelay(30);AlTem-=1.0;visual=1;     
            }
         }
               
         if(RIGHT_Read()==0)//Si oprime SET
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);visual=1;
         } 
                  
         if(LEFT_Read()==0)//Si oprime boton de Toma.
         {
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(700);Menu=100;glcd_fillScreen(OFF);Opcion=1;visual=1;
            EEPROM_WriteByte(convert8(AlTem,0),472);
            CyDelay(10);
            EEPROM_WriteByte(convert8(AlTem,1),473);
            CyDelay(10);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==110){ // Menu de Pendiente de calentamiento de mufla
      
      if(penMufla>150)      
         penMufla=10;
      else if(penMufla<10)
         penMufla=150;
      
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"m Mufla");
         displayMenu(Menus.Letra,0,0,1,2);
         displayTemperatura(penMufla,30,30,0,3);
         visual=0;
      }
      
         if(UP_Read()==0)//Si oprime hacia arriba
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            visual=1;
            penMufla+=1.0;
            CyDelay(30);
         }
               
         if(DOWN_Read()==0)//Si oprime hacia abajo
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            visual=1;
            penMufla-=1.0;
            CyDelay(30);
         }
               
         if(RIGHT_Read()==0)//Si oprime SET
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);visual=1;
         } 
                  
         if(LEFT_Read()==0)//Si oprime boton de Toma.
         {
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(700);Menu=100;glcd_fillScreen(OFF);Opcion=1;visual=1;
            EEPROM_WriteByte((uint8)penMufla,458);
            CyDelay(10);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==111){ // Menu de Pendiente de termocupla
      
      if(m>2.0)      
         m=0.1;
      else if(m<0.1)
         m=2.0;
      
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"m");
         displayMenu(Menus.Letra,0,0,1,2);
         displayFloat(m,30,30,3);
         visual=0;
      }
      
         if(UP_Read()==0)//Si oprime hacia arriba
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            visual=1;
            m+=0.01;
            CyDelay(10);
         }
               
         if(DOWN_Read()==0)//Si oprime hacia abajo
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            visual=1;
            m-=0.01;
            CyDelay(10);
         }
               
         if(RIGHT_Read()==0)//Si oprime SET
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);visual=1;
         } 
                  
         if(LEFT_Read()==0)//Si oprime boton de Toma.
         {
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(700);Menu=100;glcd_fillScreen(OFF);Opcion=1;visual=1;
            EEPROM_WriteByte((uint8)m,490);
            CyDelay(10);
            EEPROM_WriteByte(extraeDecimal(m),491);
            CyDelay(10);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==112){ // Menu de b de termocupla
      
      if(b>200)      
         b=0;
      else if(b<0)
         b=200;
      
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"b");
         displayMenu(Menus.Letra,0,0,1,2);
         displayFloat(b,30,30,3);
         visual=0;
      }
      
         if(UP_Read()==0)//Si oprime hacia arriba
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            visual=1;
            b+=0.1;
            CyDelay(10);
         }
               
         if(DOWN_Read()==0)//Si oprime hacia abajo
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            visual=1;
            b-=0.1;
            CyDelay(10);
         }
               
         if(RIGHT_Read()==0)//Si oprime SET
         {  
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(30);visual=1;
         } 
                  
         if(LEFT_Read()==0)//Si oprime boton de Toma.
         {
            BUZZER_Write(1);CyDelay(10);BUZZER_Write(0);
            CyDelay(700);Menu=100;glcd_fillScreen(OFF);Opcion=1;visual=1;
            b16=(uint16)b;
            EEPROM_WriteByte(convert8(b16,0),492);
            CyDelay(10);
            EEPROM_WriteByte(convert8(b16,1),493);
            CyDelay(10);
            EEPROM_WriteByte(extraeDecimal(b),494);
            CyDelay(10);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

      if(tiemporeset>=60){
         if(Menu!=0 && Menu<100){
            glcd_init(ON);
            tiemporeset=0;
         }
      } 

    }
}
