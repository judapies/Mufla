// Control para mufla con rampas 
// Se pueden programar 5 Programas de 10 Segmentos cada Uno.
// En un segmento se puede programar una rampa o una meseta.


#include <18F4685.h>
#device adc=10
#fuses HS,NOWDT,NOPROTECT,NOLVP,NODEBUG,NOSTVREN,NOPUT,NOCPD,NOWRT,NOIESO,NOFCMEN,NOPBADEN,NOWRTC,NOWRTB,NOEBTR,NOEBTRB,NOCPB,NOXINST,NOLPT1OSC,NOMCLR
#use delay(clock=20000000)
#include "HDM64GS12.c"
#include "GRAPHICS.c"
#include "imagen.h"
#include <MATH.h>

#define SPI_MODE_0  (SPI_L_TO_H | SPI_XMIT_L_TO_H) 
#define SPI_MODE_1  (SPI_L_TO_H) 
#define SPI_MODE_2  (SPI_H_TO_L) 
#define SPI_MODE_3  (SPI_H_TO_L | SPI_XMIT_L_TO_H) 
#define SS PIN_C0 

// Definición de teclado - NO
#define   UP               !input(PIN_A5)
#define   DOWN             !input(PIN_E0)
#define   RIGHT            !input(PIN_E1)
#define   LEFT             !input(PIN_E2)

#define   DOOR             input(PIN_A3)
#define   TERM             input(PIN_A4)

#define   Resistencia_on   output_bit(PIN_C0,1)
#define   Resistencia_off  output_bit(PIN_C0,0)
#define   Buzz_on          output_bit(PIN_C7,1)
#define   Buzz_off         output_bit(PIN_C7,0)

float promedio[90]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float X[2]={0.0,0.0};
short flagup=0,flagdn=0,Desconectado=0;
int8 h=0,l=0,Ajuste=0,Ajusten=0,MediaMovil=10,tiemporecta=0,flag_ganancia=0,Lectura2=0,tiempoup=0,tiempodn=0,Segmentos=0,Segmento=0,Modo=5;
short Flanco=0,Flanco1=0,Flanco2=0,visual=1,Lectura=0,flag_calcula=0;
short flag_clear2=0,flag_tiempocalentamiento=0,flag_display=0,flag_setpoint=0,Dibuja=0;
int8 y=0,tiemporeset=0,Programa=1,Control=0;
int8 Menu=0, unidad=1,i=0,j=1,Pausa=0,Pausa_ant=0,TiempoControl=0;
int8 Opcion=1,tiempo_esterilizacion=0,ProgramaSeleccionado=0,Alarma=10;
int16 tiempos=0,tiempos3=0,tiempoctrl=0,tinicio=0,tiempocalentamiento=0,tiemposetpoint=0,Tp=0,Tc=0;
float PendienteMinutos=0,TempeInicial=20,Tiempo=0,TiempoActual=0,PendienteActual=0,HorasActual=0,MinutosActual=0,TiempoRampa=0;
float Potencia=0.0,PotenciaEntregada=0.0;
signed int  Testerilizacion[4]={0,0,0,0};   // Tiempo transcurrido
signed int  Testerilizacionp[4]={0,0,0,0};   // Tiempo transcurrido
signed int  Contrasena[4]={0,0,0,0};      // Contrasena de 4 digitos
signed int  Password[4]={0,0,0,0};        // Contrasena Almacenada de 4 digitos
float V0=0.0,Tciclo=10.0,Tpulso=0.0,error=0.0,Prom=0.0,Temperature=0.0,tmp=0.0,Ganancia=3.0,desvio=3.0,Coeficiente=150.0,AlTem=20.0,resistencia=240.0;
float Aumento=0.0,Temperatura=0.0,Pendiente=0.0,DifPendiente=0.0,Setpoint=0,Integral=0,Derivativo=0,m=0,b=0,adcSup=0,adcInf=0,Tsup=1250,Tinf=0,penMufla=50;
float TemperaturaIdeal=0,penInicial=0;
signed int16 Temperatura1[50];
signed int16 Horas1[50];
signed int16 Minutos1[50];

typedef struct{
   char Letra[30];
}MENUU;

MENUU Menus;

void ActualizaRecta(){
   X[y]=Temperatura;
   y++;
   if(y>=2)
   {
      y=0;
      Aumento=X[1]-X[0];
      HorasActual=(Testerilizacion[3]*10)+Testerilizacion[2];
      MinutosActual=(Testerilizacion[1]*10)+Testerilizacion[0];
      TiempoActual=Tiempo-((HorasActual*3600)+(MinutosActual*60)+tinicio);
      PendienteActual=((Setpoint-Temperatura)/TiempoActual);
      DifPendiente=(PendienteActual*20)-Aumento;
      DifPendiente=DifPendiente*5;
      //penMufla=(6000*(Aumento/20))/PotenciaEntregada;
   }
}

// Funcion para conversor analogo-digital
float sensores(int x){
float y;set_adc_channel(x);delay_ms(1);y=read_adc();return (y);
}
float Leer_Sensor(int media)
{
   Prom=0.0;
   V0 = sensores(0);   //Lectura de Divisor de Voltaje de PT100 con resistencia de 6.18k (+-2%)   
            
   if(l>(media-1))
   {l=0;}
   //promedio[l]=Temperature;l++;
   promedio[l]=V0;l++;
         
   for(h=0;h<media;h++)
   {
      Prom+=promedio[h];
   } 
   Prom=Prom/media;   
   //Temperature=(Prom-100.0)/(0.385);  // Ecuación determinada por linealización en Matlab. Revisar este polinomio.  
   //Temperature=(1.5275*Prom)-299.23; // Recta dada por T 0°C-1200°C y ADC Modo Real   
   Temperature=(m*Prom)+b; // Recta dada por T 0°C-1200°C y ADC Modo Real
   
   Temperature+=((float)Ajuste-(float)Ajusten);
   
   //return Prom;
   return Temperature;
}

void Controla(){
//-------------------------------Control de Calentamiento------------------------------------------------------------//         
   if(tiemporecta>=10)
   {
      ActualizaRecta();
      tiemporecta=0;
   }
  
   if(Segmento==0){
      if(flag_calcula==0){
         Tiempo=(Horas1[ProgramaSeleccionado*10]*3600)+(Minutos1[ProgramaSeleccionado*10]*60);         
         Setpoint=Temperatura1[(ProgramaSeleccionado*10)+Segmento];
         Pendiente=((Setpoint-TempeInicial)/Tiempo);
         Testerilizacionp[3]=(int8)Horas1[(ProgramaSeleccionado*10)+Segmento]/10;
         Testerilizacionp[2]=Horas1[(ProgramaSeleccionado*10)+Segmento]-(Testerilizacionp[3]*10);
         Testerilizacionp[1]=Minutos1[(ProgramaSeleccionado*10)+Segmento]/10;
         Testerilizacionp[0]=Minutos1[(ProgramaSeleccionado*10)+Segmento]-(Testerilizacionp[1]*10);
         flag_calcula=1;
         Ganancia=read_eeprom(250);
         delay_ms(10);
         Ganancia=Ganancia*(Setpoint/Coeficiente);
      }
   }
   else{
      if(flag_calcula==0){
         Tiempo=(Horas1[(ProgramaSeleccionado*10)+Segmento]*3600)+(Minutos1[(ProgramaSeleccionado*10)+Segmento]*60);
         Pendiente=((Temperatura1[(ProgramaSeleccionado*10)+Segmento]-Temperatura1[(ProgramaSeleccionado*10)+Segmento-1])/Tiempo);
         Setpoint=Temperatura1[(ProgramaSeleccionado*10)+Segmento];
         Testerilizacionp[3]=(int8)Horas1[(ProgramaSeleccionado*10)+Segmento]/10;
         Testerilizacionp[2]=Horas1[(ProgramaSeleccionado*10)+Segmento]-(Testerilizacionp[3]*10);
         Testerilizacionp[1]=Minutos1[(ProgramaSeleccionado*10)+Segmento]/10;
         Testerilizacionp[0]=Minutos1[(ProgramaSeleccionado*10)+Segmento]-(Testerilizacionp[1]*10);
         flag_calcula=1;
         Ganancia=read_eeprom(250);
         delay_ms(10);
         Ganancia=Ganancia*(Setpoint/Coeficiente);
      }
   }
   
   if(Pendiente==0){// Si Esta realizando una Meseta debe realizar el control JuDifuso
      error=desvio+Setpoint-Temperatura;      // Cálculo del error ******
      
      if(error>3.0 && Aumento<1)
      {
         if(Potencia<90.0)
         {
            if(TiempoControl>=Derivativo)
            {
               TiempoControl=0;
               if(Aumento<-2)
               {
                  Ganancia+=Integral+1;
               }
               else
               {
                  if(Aumento<-1)
                  {
                     Ganancia+=Integral;
                  }   
                  else
                  {
                     Ganancia+=Integral-0.5;
                  }
               }
            }
         }
      }
      
      //if(error<-5)
      //{
      //   Ganancia=6*(Setpoint/100);
      //}
      
      if(Ganancia<0.5)
      {
         Ganancia=0.5;
      }
      
      
      if(error<0.0)     // Anti Wind_DOWN    
      {
         error=0.0;
      }
   
      tmp=error*Ganancia;  // Control Proporcional.
            
      if(tmp>Setpoint)   // Anti Wind-UP      
      {
         tmp=Setpoint;
      }
                  
      if(tmp<0.0)     // Anti Wind_DOWN    
      {
         tmp=0.0;
      }
      
      // Tpulso(t)= Tciclo*(y(t)-ymin)/(ymax - ymin); calculo de ciclo util para control de resistencia.ymax=140.ymin=0;
      // Tpulso es float, hay que pasar ese ciclo a una salida de un puerto usando el TMR1.
      Tpulso=(tmp/Setpoint)*Tciclo; 
   }
   else{// Si Esta realizando una Rampa debe realizar el control de acuerdo a la pendiente de Temperatura. Aprox 10°C * min Maximo Valor de Pendiente.
      PendienteMinutos=PendienteActual*60;
      //PotenciaEntregada=((PendienteMinutos+DifPendiente)*100)/penMufla;
      TiempoRampa=((HorasActual*3600)+(MinutosActual*60)+tinicio);
      if(Segmento==0){
         TemperaturaIdeal=(Pendiente*TiempoRampa)+TempeInicial;
      }else{
         TemperaturaIdeal=(Pendiente*TiempoRampa)+Temperatura1[(ProgramaSeleccionado*10)+Segmento-1];
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
   }
   Tp=(int16)(Tpulso/0.005);
   Tc=(int16)(Tciclo/0.005);
         
//--------------------------------------------------------------------------------------------------------------------//   
}

void displayMenu(char palabra[30], int x, int y, int sombreado, int tamano)
{
   strcpy(Menus.Letra,palabra);
   
   if(sombreado==3)
      {glcd_text57(x+1, y+1, Menus.Letra, tamano, OFF);}
   if(sombreado==2)
      {glcd_text57(x+1, y+1, Menus.Letra, tamano, ON);}
   if(sombreado==1)
      {glcd_rect(0, y, 127, y+(tamano*8), YES, ON);glcd_text57(x+1, y+1, Menus.Letra, tamano, OFF);}
   if(sombreado==0)
      {glcd_rect(0, y, 127, y+(tamano*8), YES, OFF);glcd_text57(x+1, y+1, Menus.Letra, tamano, ON);}
   glcd_update();
}

void displayTemps(char palabra[30], int16 digito, int x, int y, int sombreado, int tamano)
{
   char voltage[9];
   
   if(sombreado==11)
   {
      sprintf(voltage, "%02Lu", digito);
      glcd_rect(0, y-2, 127, y+(tamano*8), YES, ON);
      strcpy(Menus.Letra,palabra);
      glcd_text57(x+90, y+1, voltage, tamano, OFF);
      glcd_text57(x+1, y+1, Menus.Letra, tamano, OFF);
   }
   if(sombreado==10)
   {
      sprintf(voltage, "%02Lu", digito);
      glcd_rect(0, y-2, 127, y+(tamano*8), YES, OFF);
      strcpy(Menus.Letra,palabra);
      glcd_text57(x+90, y+1, voltage, tamano, ON);
      glcd_text57(x+1, y+1, Menus.Letra, tamano, ON);
   }
   
   if(sombreado==1)
   {
      sprintf(voltage, "%04Lu", digito);
      glcd_rect(0, y-2, 127, y+(tamano*8), YES, ON);
      strcpy(Menus.Letra,palabra);
      glcd_text57(x+90, y+1, voltage, tamano, OFF);
      glcd_text57(x+1, y+1, Menus.Letra, tamano, OFF);
      strcpy(Menus.Letra,"C");
      glcd_text57(x+120, y+1, Menus.Letra, tamano, OFF);
      glcd_circle(x+115,y-1,1,NO,OFF);
   }
   if(sombreado==0)
   {
      sprintf(voltage, "%04Lu", digito);
      glcd_rect(0, y-2, 127, y+(tamano*8), YES, OFF);
      strcpy(Menus.Letra,palabra);
      glcd_text57(x+90, y+1, voltage, tamano, ON);
      glcd_text57(x+1, y+1, Menus.Letra, tamano, ON);
      strcpy(Menus.Letra,"C");
      glcd_text57(x+120, y+1, Menus.Letra, tamano, ON);
      glcd_circle(x+115,y-1,1,NO,ON);
   }
   glcd_update();
}

void displayTiempo(int digito, int x, int y, int sombreado, int tamano)
{
   char voltage[9];
   
   if(sombreado==2)
   {
      sprintf(voltage, "%03u", digito);
      glcd_rect(x, y, x+(tamano*18), y+(tamano*8), YES, ON);
      glcd_text57(x+1, y+1, voltage, tamano, OFF);
   }
   else
   {
      if(sombreado==3)
      {
         sprintf(voltage, "%03u", digito);
         glcd_rect(x, y, x+(tamano*18), y+(tamano*8), YES, OFF);
         glcd_text57(x+1, y+1, voltage, tamano, ON);
      }
      sprintf(voltage, "%u", digito);
      if(sombreado==1)
         {glcd_rect(x, y, x+(tamano*5), y+(tamano*8), YES, ON);glcd_text57(x+1, y+1, voltage, tamano, OFF);}
      if(sombreado==0)
         {glcd_rect(x, y, x+(tamano*5), y+(tamano*8), YES, OFF);glcd_text57(x+1, y+1, voltage, tamano, ON);}
   }
   glcd_update();
}

void displayTemperatura(int16 digito, int x, int y, int sombreado, int tamano)
{
   char voltage[9];
   
   if(sombreado==2)
   {
      sprintf(voltage, "%04Lu", digito);
      glcd_rect(x, y, x+(tamano*18), y+(tamano*8), YES, ON);
      glcd_text57(x+1, y+1, voltage, tamano, OFF);
   }
   else
   {
      sprintf(voltage, "%04Lu", digito);
   
      if(sombreado==4)
      {
         glcd_rect(x, y, x+(tamano*16), y+(tamano*8), YES, OFF);
         glcd_text57(x+1, y+1, voltage, tamano, ON);
      }
      if(sombreado==1)
         {glcd_rect(x, y, x+(tamano*5), y+(tamano*8), YES, ON);glcd_text57(x+1, y+1, voltage, tamano, OFF);}
      if(sombreado==0)
         {glcd_rect(x, y, x+(tamano*5), y+(tamano*8), YES, OFF);glcd_text57(x+1, y+1, voltage, tamano, ON);}
   }
   glcd_update();
}

void glcd_imagen(int8 x)
{
   char i,j;
   signed char k; 
   
   if(x==5){
   for( i = 0 ; i < 64 ; i ++ )
   {  
      for( j = 0 ; j < 16 ; j ++)
      {    
         for(k=7;k>-1;k--)
         {      
            if( bit_test(Horno[i][j] ,7-k )) 
               glcd_pixel( j*8+k,i, ON );
         }  
      }
   }
   }
   
}

#int_TIMER1
void temp1s(void){
   //set_timer1(45612);  // 5 ms 5536 , Posiblemente cargar con 22144 debido a F=12MHz
   set_timer1(40536);
   tiempos++;
   tiempos3++;
   tiempoctrl++;
   
   if(Control==1){
      if(tiempoctrl>=Tc) // Si el tiempo de control es mayor al tiempo de ciclo se reinicia tiempo de control
      {
         tiempoctrl=0;
      } // tiempoctrl incrementa una unidad cada 5ms, por eso se dividen los tiempos por 0.005.
      
      if(tiempoctrl<=Tp) // Si el tiempo es menor o igual al tiempo de control se enciende la resistencia de lo contrario se apaga.
      {
         Resistencia_on;
      }
      else         
      {
         Resistencia_off;
      }
   }
   else{
      Resistencia_off;
   }
   
   if(tiempos3>=200)
   {
      Lectura2=1;
      if(flagup)
         tiempoup++;
      if(flagdn)
         tiempodn++;   
      tiempos3=0;
   }
    
   if(tiempos>=200)  // 12000 para que incremente cada minuto, 200para que incremente cada segundo.
   {tiempos=0;Lectura=1;tiemporeset++;TiempoControl++;tiemporecta++;
   
      if(flag_setpoint==1)
         tiemposetpoint++;
      
      if(tiempo_esterilizacion==1 || tiempo_esterilizacion==2){               
         tinicio++;
      }
         
      if(flag_tiempocalentamiento==1)
      {
         tiempocalentamiento++;
      }
   }        
}
   
void Limpia(int x1,int x2,int y){
   glcd_rect(x1, y, x2, y+7, YES, OFF);
}

void displayT(int16 adc,int x,int y,int w) {
   char voltage[9];    
   sprintf(voltage, "%04Lu",adc); // Converts adc to text
   glcd_rect(x, y, x+(w*27), y+(w*8), YES, OFF);
   glcd_text57(x, y, voltage, w, ON);
}

void displayFloat(float adc,int x,int y,int w) {
   char voltage[9];    
   sprintf(voltage, "%03.1f",adc); // Converts adc to text
   glcd_rect(x, y, x+(w*27), y+(w*8), YES, OFF);
   glcd_text57(x, y, voltage, w, ON);
}

void displayContrasena(int digito, int x, int y, int sombreado)
{
   char voltage[9];
   sprintf(voltage, "%i", digito);
   
   if(sombreado==1)
      {glcd_rect(x, y, x+18, y+24, YES, ON);glcd_text57(x+2, y+1, voltage, 2, OFF);}
   else   
      {glcd_rect(x, y, x+18, y+24, NO, OFF);glcd_text57(x+2, y+1, voltage, 2, ON);}

}

void MuestraSegmento(int8 Opcio,int8 l,int8 Prog ){
         if(Temperatura1[l+((Prog-1)*10)]>1200)
            Temperatura1[l+((Prog-1)*10)]=0;
         
         strcpy(Menus.Letra,"P 0");
         displayMenu(Menus.Letra,30,0,1,2);
         displayTiempo(Prog,68,0,1,2);
         
         strcpy(Menus.Letra,"Segmento 0");
         displayMenu(Menus.Letra,0,20,1,1);
         displayTiempo(l,63,20,1,1);
         
         strcpy(Menus.Letra,"TA:");
         displayMenu(Menus.Letra,80,20,3,1);
         
         if(l>0)
            displayTemperatura(Temperatura1[l-1+((Prog-1)*10)],100,20,1,1);
         
         if(Opcio==1){
            strcpy(Menus.Letra,"Temperatura:");
            displayTemps(Menus.Letra,Temperatura1[l+((Prog-1)*10)],0,32,1,1);
            strcpy(Menus.Letra,"Horas:");
            displayTemps(Menus.Letra,Horas1[l+((Prog-1)*10)],0,44,10,1);
            strcpy(Menus.Letra,"Minutos:");
            displayTemps(Menus.Letra,Minutos1[l+((Prog-1)*10)],0,55,10,1);
            if(UP)//Si oprime hacia arriba
            {
               Buzz_on;delay_ms(10);Buzz_off;
               if(tiempoup<4){
                  flagup=1;
                  Temperatura1[l+((Prog-1)*10)]++;
               }
               else{
                  flagup=0;
                  Temperatura1[l+((Prog-1)*10)]+=10;
               } 
               if(l<9)
                  Temperatura1[l+1+((Prog-1)*10)]=Temperatura1[l+((Prog-1)*10)]; 
                  
               if(Temperatura1[l+((Prog-1)*10)]>1200)
                  Temperatura1[l+((Prog-1)*10)]=0;
            }
            else
            {
               flagup=0;tiempoup=0;
            }
                     
            if(DOWN)//Si oprime hacia abajo
            {
               Buzz_on;delay_ms(10);Buzz_off;
               if(tiempodn<4){
                  flagdn=1;
                  Temperatura1[l+((Prog-1)*10)]--;
               }
               else{
                  flagdn=0;
                  Temperatura1[l+((Prog-1)*10)]-=10;
               }
               Temperatura1[l+1+((Prog-1)*10)]=Temperatura1[l+((Prog-1)*10)]; 
               
               if(Temperatura1[l+((Prog-1)*10)]<0)
                  Temperatura1[l+((Prog-1)*10)]=1200;
            }
            else{
               flagdn=0;tiempodn=0;
            }
         }
         
         if(Opcio==2){
            strcpy(Menus.Letra,"Temperatura:");
            displayTemps(Menus.Letra,Temperatura1[l+((Prog-1)*10)],0,32,0,1);
            strcpy(Menus.Letra,"Horas:");
            displayTemps(Menus.Letra,Horas1[l+((Prog-1)*10)],0,44,11,1);
            strcpy(Menus.Letra,"Minutos:");
            displayTemps(Menus.Letra,Minutos1[l+((Prog-1)*10)],0,55,10,1);
            if(UP)//Si oprime hacia arriba
            {
               Buzz_on;delay_ms(10);Buzz_off;
               if(tiempoup<4){
                  flagup=1;
                  Horas1[l+((Prog-1)*10)]++;
               }
               else{
                  flagup=0;
                  Horas1[l+((Prog-1)*10)]+=10;
               }
               if(Horas1[l+((Prog-1)*10)]>99)
                  Horas1[l+((Prog-1)*10)]=0;
            }
            else{
               flagup=0;tiempoup=0;
            }
                     
            if(DOWN)//Si oprime hacia abajo
            {
               Buzz_on;delay_ms(10);Buzz_off;
               if(tiempodn<4){
                  flagdn=1;
                  Horas1[l+((Prog-1)*10)]--;
               }
               else{
                  flagdn=0;
                  Horas1[l+((Prog-1)*10)]-=10;
               }
               if(Horas1[l+((Prog-1)*10)]<0)
                  Horas1[l+((Prog-1)*10)]=99;
            }
            else{
               flagdn=0;tiempodn=0;
            }
         }
         
         if(Opcio==3){
            strcpy(Menus.Letra,"Temperatura:");
            displayTemps(Menus.Letra,Temperatura1[l+((Prog-1)*10)],0,32,0,1);
            strcpy(Menus.Letra,"Horas:");
            displayTemps(Menus.Letra,Horas1[l+((Prog-1)*10)],0,44,10,1);
            strcpy(Menus.Letra,"Minutos:");
            displayTemps(Menus.Letra,Minutos1[l+((Prog-1)*10)],0,55,11,1);
            if(UP)//Si oprime hacia arriba
            {
               Buzz_on;delay_ms(10);Buzz_off;
               if(tiempoup<4){
                  flagup=1;
                  Minutos1[l+((Prog-1)*10)]++;
               }  
               else{
                  flagup=0;
                  Minutos1[l+((Prog-1)*10)]+=10;
               }
               if(Minutos1[l+((Prog-1)*10)]>59)
                  Minutos1[l+((Prog-1)*10)]=0;
            }
            else{
               flagup=0;tiempoup=0;
            }
                     
            if(DOWN)//Si oprime hacia abajo
            {
               Buzz_on;delay_ms(10);Buzz_off;
               if(tiempodn<4){
                  flagdn=1;
                  Minutos1[l+((Prog-1)*10)]--;
               }
               else{
                  flagdn=0;
                  Minutos1[l+((Prog-1)*10)]-=10;
               }
               if(Minutos1[l+((Prog-1)*10)]<0)
                  Minutos1[l+((Prog-1)*10)]=59;
            }
            else{
               flagdn=0;tiempodn=0;
            }
         }
}

int8 spival[4]; 

/******************************************************************************* 
//Read SPI data 

*******************************************************************************/  
void readMAX() 
{ 
      output_low(SS); 
      delay_us(10); 
      spival[3]=spi_read(0); 
      delay_us(10); 
      spival[2]=spi_read(0); 
      delay_us(10); 
      spival[1]=spi_read(0); 
      delay_us(10); 
      spival[0]=spi_read(0); 
      delay_us(10); 
      output_high(SS); 
} 

/******************************************************************************* 
//Fault detection. 
//Returns >0 if FAULT. If necessary do a bitwise analisys to check fault source 
*******************************************************************************/ 
int tempFault() 
{ 
      int1 Fvdd=0,Fgnd=0,Fopen=0,fault=0; 
  
      fault=spival[2]&0x01; // pelos menos uma falha 
      Fvdd=(spival[0]>>2)&0x01; 
      Fgnd=(spival[0]>>1)&0x01; 
      Fopen=spival[0]&0x01; 
      
      return (fault*1+Fvdd*2,Fgnd*4,Fopen*8); 
} 

/******************************************************************************* 
//Read thermocouple temperature 
//Returns returns signed temperature in ºC approximately 
*******************************************************************************/ 
float readExtTemp() 
{ 
      int8 aux; 
      int16 temp1,temp2; 
      float entero,decimal,temperatura;
      
      aux=spival[2]>>2; 
      temp1=spival[3]; 
      temp1<<=6; 
      temp1+=aux; 
      temp2=temp1&0x03;
      if(temp2==1)
         decimal=0.25;
      if(temp2==2)
         decimal=0.5;
      if(temp2==3)
         decimal=0.75;
      if(temp2==0)
         decimal=0;
      entero=temp1>>2;
      temperatura=entero+decimal;
      //return temp1/=4; 
      //return temp1*=0.25; 
      return temperatura; 
} 

/******************************************************************************* 
//Read internal temperature 
//Returns returns signed temperature in ºC approximately 
*******************************************************************************/ 
int16 readIntTemp() 
{ 
      int8 aux; 
      int16 temp2; 
      
      temp2=spival[1]; 
      temp2<<=4; 
      aux=spival[0]>>4; 
      temp2=spival[1]<<4; 
      temp2+=aux; 
      
      return temp2/=16; 
} 

void main() 
{ 
   delay_ms(1000);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);
   enable_interrupts(INT_TIMER1);
   setup_adc_ports(AN0_TO_AN2);
   setup_adc(ADC_CLOCK_DIV_64);
   /*setup_ccp1 (CCP_PWM);
   setup_timer_2(T2_DIV_BY_4, 255, 1);   
   set_pwm1_duty(0);*/
   enable_interrupts(global);
   
   if(Ganancia>10.0)
   {
      Ganancia=2.0;
   }
   
   glcd_init(ON);             //Inicializa la glcd
   glcd_fillScreen(OFF);      //Limpia la pantalla
   strcpy(Menus.Letra,"JP Inglobal");
   displayMenu(Menus.Letra,0,0,0,2);     
   strcpy(Menus.Letra,"      Horno");
   displayMenu(Menus.Letra,0,20,0,1);       
   strcpy(Menus.Letra,"     Tubular");
   displayMenu(Menus.Letra,0,30,0,1);
   strcpy(Menus.Letra,"    3 Zonas");
   displayMenu(Menus.Letra,0,45,0,2);
   delay_ms(1000);
   
   Resistencia_off;
   glcd_fillScreen(OFF);      //Limpia la pantalla
   displayContrasena(Contrasena[0],20,30,1);
   displayContrasena(Contrasena[1],40,30,0);
   displayContrasena(Contrasena[2],60,30,0);
   displayContrasena(Contrasena[3],80,30,0);
   
   for(j=1;j<6;j++){
      for(i=0;i<10;i++){
         delay_ms(10); 
         Temperatura1[i+((j-1)*10)]=make16(read_eeprom((i*2)+1+((j-1)*40)),read_eeprom((i*2)+((j-1)*40)));
         delay_ms(10);
         Horas1[i+((j-1)*10)]=read_eeprom(i+((40*j)-20));
         delay_ms(10);    
         Minutos1[i+((j-1)*10)]=read_eeprom(i+((40*j)-10));
      }
   }
   
   for(i=0;i<50;i++){
      if(Temperatura1[i]>1200)
         Temperatura1[i]=0;
      if(Horas1[i]>99)
         Horas1[i]=0;
      if(Minutos1[i]>59)
         Minutos1[i]=0;
   }
   
   delay_ms(50);   
   Ganancia=read_eeprom(250);
   delay_ms(50);   
   Tciclo=read_eeprom(251);
   delay_ms(50);   
   Ajuste=read_eeprom(252);
   delay_ms(50);   
   Ajusten=read_eeprom(253);
   delay_ms(50);   
   MediaMovil=read_eeprom(254);
   delay_ms(50);   
   Coeficiente=read_eeprom(255);
   delay_ms(50);   
   AlTem=read_eeprom(256);
   delay_ms(50); 
   resistencia=read_eeprom(257);
   delay_ms(50); 
   penMufla=read_eeprom(258);
   delay_ms(50); 
   
   Password[0]=read_eeprom(260);
   delay_ms(50);   
   Password[1]=read_eeprom(261);
   delay_ms(50);   
   Password[2]=read_eeprom(262);
   delay_ms(50);   
   Password[3]=read_eeprom(263);
   delay_ms(50);   
   
   Integral=read_eeprom(270);
   delay_ms(50);   
   Derivativo=read_eeprom(271);
   delay_ms(50); 
   Modo=read_eeprom(280);
   delay_ms(50); 
   
   Menu=0;
   
   glcd_fillScreen(OFF);      //Limpia la pantalla
   displayContrasena(Contrasena[0],20,30,1);
   displayContrasena(Contrasena[1],40,30,0);
   displayContrasena(Contrasena[2],60,30,0);
   displayContrasena(Contrasena[3],80,30,0);
   for(i=0;i<80;i++)
   {
      Temperatura=Leer_Sensor(MediaMovil);
   }

   adcInf=(0.004*resistencia*1023)/5.0;
   adcSup=(0.02*resistencia*1023)/5.0;
   m=(Tsup-Tinf)/(adcSup-adcInf);
   b=Tsup-(m*adcSup);
   penInicial=penMufla;
   while(TRUE) 
   { 
//*****************Lectura con MAX31855********************************************************************************************************//   
      /*//do{ 
      readMAX(); 
      //}while(tempFault()!=0); 
      if(tempFault()==0)
      {
         lcd_gotoxy(1,1);
         printf(lcd_putc,"Tem:%4.2f ",readExtTemp());
         
         lcd_gotoxy(1,2);
         printf(lcd_putc,"0:%03u 1:%03u ",spival[0],spival[1]);
         lcd_gotoxy(1,3);
         printf(lcd_putc,"2:%03u 3:%03u ",spival[2],spival[3]);
      //printf("\n\n Thermocouple: %2.2f, Ambient: %Ld\n\r,%u %u %u %u",readExtTemp(),readIntTemp(),spival[0],spival[1],spival[2],spival[3]); 
      }
      else
      {
         lcd_gotoxy(1,1);
         printf(lcd_putc," ERROR ");
      
      }
     delay_ms(500); */
//*****************Fin de Lectura con MAX31855********************************************************************************************************//    
      glcd_update();            
      
//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==0){ //Menu de Contraseña.
      
      strcpy(Menus.Letra,"Clave");
      displayMenu(Menus.Letra,30,0,1,2);     
      
      if(UP)//Si oprime hacia arriba
      {
         Buzz_on;delay_ms(10);Buzz_off;
         delay_ms(200);
         for(i=1;i<=4;i++)
         {
            if(unidad==i)
            {
               Contrasena[i-1]++;
               if(Contrasena[i-1]>9)   
               {
                  Contrasena[i-1]=0;
               }
               displayContrasena(Contrasena[i-1],i*20,30,1);
            }
            else
            {
               displayContrasena(Contrasena[i-1],i*20,30,0);
            }
         }
      }
            
      if(DOWN)//Si oprime hacia abajo
      {
         Buzz_on;delay_ms(10);Buzz_off;
         delay_ms(200);
         for(i=1;i<=4;i++)
         {
            if(unidad==i)
            {
               Contrasena[i-1]--;
               if(Contrasena[i-1]<0)
                  {Contrasena[i-1]=9;}
               displayContrasena(Contrasena[i-1],i*20,30,1);}
            else
            {displayContrasena(Contrasena[i-1],i*20,30,0);}
         }        
      }
            
      if(RIGHT)//Si oprime RIGHT
      {
         Buzz_on;delay_ms(10);Buzz_off;
         delay_ms(200);unidad++;
         for(i=1;i<=4;i++)
         {
            if(unidad==i)
            {displayContrasena(Contrasena[i-1],i*20,30,1);}
            else
            {displayContrasena(Contrasena[i-1],i*20,30,0);}
         }
      }
    
      if(unidad>4)
      {glcd_fillScreen(OFF);unidad=4;
         if(Contrasena[0]==3&&Contrasena[1]==8&&Contrasena[2]==9&&Contrasena[3]==2) // Si Ingresa clave para reset general del sistema.
            {write_eeprom(260,0);delay_ms(20);write_eeprom(261,0);delay_ms(20);// Reestablece a contraseña de Fabrica y reinicia Programa.
             write_eeprom(262,0);delay_ms(20);write_eeprom(263,0);delay_ms(20);
             reset_cpu();}        
         
         if((Contrasena[0]==Password[0])&&(Contrasena[1]==Password[1])&&(Contrasena[2]==Password[2])&&(Contrasena[3]==Password[3]))
         {
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra,"Clave");
            displayMenu(Menus.Letra,30,0,0,2);
            strcpy(Menus.Letra,"Correcta");
            displayMenu(Menus.Letra,15,30,0,2);
            Menu=1;glcd_update();delay_ms(1000);
            glcd_fillScreen(OFF);unidad=1;
         }// Esta parte se puede agregar en el Menu 1 dependiendo de la ubicación del vidrio.
         else
         {
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra,"Clave");
            displayMenu(Menus.Letra,30,0,0,2);
            strcpy(Menus.Letra,"Incorrecta");
            displayMenu(Menus.Letra,0,30,0,2);
            Menu=0;glcd_update();delay_ms(1000);
            glcd_fillScreen(OFF);unidad=1;
            Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
            displayContrasena(Contrasena[0],20,30,1);
            displayContrasena(Contrasena[1],40,30,0);
            displayContrasena(Contrasena[2],60,30,0);
            displayContrasena(Contrasena[3],80,30,0);
         }
      }
    }
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
         
         if(UP)//Si oprime hacia arriba
         {
            Buzz_on;delay_ms(30);Opcion--;Buzz_off;
         }
                  
         if(DOWN)//Si oprime hacia abajo
         {
            Buzz_on;delay_ms(30);Opcion++; Buzz_off;
         }
               
         if(RIGHT)//Si oprime SET
         { 
            Buzz_on;delay_ms(50);Menu=Opcion+3;Opcion=1;glcd_fillscreen(OFF);Buzz_off;
            if(Menu==7)
               Menu=100;
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
         
        
      if(UP)//Si oprime hacia arriba
      {
         Buzz_on;delay_ms(30);Opcion++;Buzz_off;
         glcd_rect(40, 35, 85, 60, YES, OFF);
      }
            
      if(DOWN)//Si oprime hacia abajo
      {  
         Buzz_on;delay_ms(30);Opcion--;Buzz_off;
         glcd_rect(40, 35, 85, 60, YES, OFF);
      }
            
      if(RIGHT)//Si oprime SET
      {  
         Menu=20;glcd_fillscreen(OFF);
         for(i=0;i<10;i++){
            if(Temperatura1[i+((Opcion-1)*10)]==0){
               Segmentos=i;
               break;
            }
            else{
               Segmentos=9;
            }
            //Horas1[i+((Opcion-1)*10)]=0;
            //Minutos1[i+((Opcion-1)*10)]=0;
         }
         if(Segmentos>0){
            ProgramaSeleccionado=Opcion-1;
            glcd_fillscreen(OFF);
            
            strcpy(Menus.Letra," Segmentos");
            displayMenu(Menus.Letra,0,0,0,2);
            
            displayTiempo(Segmentos,63,30,3,2);
            delay_ms(2000);
            glcd_fillscreen(OFF);
            Opcion=1;visual=1;//Faltaa cuadrar esta parte
            TempeInicial=Temperatura;
            Ganancia=Ganancia*(Setpoint/Coeficiente);
            
            Tiempo=(Horas1[ProgramaSeleccionado*10]*3600)+(Minutos1[ProgramaSeleccionado*10]*60);         
            Setpoint=Temperatura1[(ProgramaSeleccionado*10)+Segmento];
            Pendiente=((Setpoint-TempeInicial)/Tiempo);
            Testerilizacionp[3]=(int8)Horas1[(ProgramaSeleccionado*10)+Segmento]/10;
            Testerilizacionp[2]=Horas1[(ProgramaSeleccionado*10)+Segmento]-(Testerilizacionp[3]*10);
            Testerilizacionp[1]=Minutos1[(ProgramaSeleccionado*10)+Segmento]/10;
            Testerilizacionp[0]=Minutos1[(ProgramaSeleccionado*10)+Segmento]-(Testerilizacionp[1]*10);
         }
         else{
            glcd_fillscreen(OFF);
            strcpy(Menus.Letra," !Programa");
            displayMenu(Menus.Letra,0,0,0,2);
            strcpy(Menus.Letra,"    Vacio!");
            displayMenu(Menus.Letra,0,20,0,2);
            Opcion=1;Buzz_off;delay_ms(2000);Menu=1;glcd_fillscreen(OFF);Opcion=1;visual=1;
         }
      } 
                  
      if(LEFT)//Si oprime boton de Toma.
      {
         Buzz_on;Opcion=1;Buzz_off;delay_ms(50);Menu=1;glcd_fillscreen(OFF);Opcion=1;visual=1;
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
         
         if(UP)//Si oprime hacia arriba
         {
            Buzz_on;delay_ms(30);Opcion--;Buzz_off;
         }
                  
         if(DOWN)//Si oprime hacia abajo
         {
            Buzz_on;delay_ms(30);Opcion++; Buzz_off;
         }
               
         if(RIGHT)//Si oprime SET
         { 
            Buzz_on;delay_ms(50);Menu=Opcion+10;Opcion=1;glcd_fillscreen(OFF);Buzz_off;i=0;
         }
         
         if(LEFT){
            Buzz_on;delay_ms(50);Menu=1;Opcion=1;glcd_fillscreen(OFF);Buzz_off;
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
         
        
      if(UP)//Si oprime hacia arriba
      {
         Buzz_on;delay_ms(30);Opcion++;Buzz_off;
         glcd_rect(40, 35, 85, 60, YES, OFF);
      }
            
      if(DOWN)//Si oprime hacia abajo
      {  
         Buzz_on;delay_ms(30);Opcion--;Buzz_off;
         glcd_rect(40, 35, 85, 60, YES, OFF);
      }
            
      if(RIGHT)//Si oprime SET
      {  
         Buzz_on;
         for(i=0;i<10;i++){
            Temperatura1[i+((Opcion-1)*10)]=0;
            Horas1[i+((Opcion-1)*10)]=0;
            Minutos1[i+((Opcion-1)*10)]=0;
         }
         
         for(i=0;i<10;i++){
            delay_ms(10);    
            write_eeprom((i*2)+(40*(Opcion-1)),0);
            delay_ms(10);    
            write_eeprom((i*2)+1+(40*(Opcion-1)),0);
            delay_ms(10);    
            write_eeprom(i+((40*Opcion)-20),0);
            delay_ms(10);    
            write_eeprom(i+((40*Opcion)-10),0);
         } 
         
         delay_ms(200);Buzz_off;Menu=1;glcd_fillscreen(OFF);Opcion=1;visual=1;
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Programa");
         displayMenu(Menus.Letra,30,0,0,2);
         strcpy(Menus.Letra,"Eliminado");
         displayMenu(Menus.Letra,15,30,0,2);
         Menu=1;glcd_update();delay_ms(1000);
         glcd_fillScreen(OFF);unidad=1;
      } 
                  
      if(LEFT)//Si oprime boton de Toma.
      {
         Buzz_on;Opcion=1;Buzz_off;delay_ms(500);Menu=1;glcd_fillscreen(OFF);Opcion=1;visual=1;
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
      
      if(UP)//Si oprime hacia arriba
      {
         Buzz_on;delay_ms(10);Buzz_off;
         delay_ms(30);
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
            
      if(DOWN)//Si oprime hacia abajo
      {
         Buzz_on;delay_ms(10);Buzz_off;
         delay_ms(30);
         for(i=1;i<=4;i++)
         {
            if(unidad==i)
            {Contrasena[i-1]--; 
            if(Contrasena[i-1]<0)
            {Contrasena[i-1]=9;}
            }
         }  
      }
            
      if(RIGHT)//Si oprime RIGHT
      {
         Buzz_on;delay_ms(10);Buzz_off;delay_ms(30);unidad++;
      }
      
      if(LEFT)//Si oprime Izquierda
      {
         Buzz_on;delay_ms(10);Buzz_off;delay_ms(700);Menu=1;glcd_fillscreen(OFF);
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
            Menu=91;glcd_update();delay_ms(1000);
            glcd_fillScreen(OFF);unidad=1;
         }// Esta parte se puede agregar en el Menu 1 dependiendo de la ubicación del vidrio.
         else
         {
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra,"Clave");
            displayMenu(Menus.Letra,30,0,0,2);
            strcpy(Menus.Letra,"Incorrecta");
            displayMenu(Menus.Letra,0,30,0,2);
            Menu=1;glcd_update();delay_ms(1000);
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
         displayContrasena(Password[0],20,30,1);
         displayContrasena(Password[1],40,30,0);
         displayContrasena(Password[2],60,30,0);
         displayContrasena(Password[3],80,30,0);
      }
      else
      {
         if(unidad==2)
         {
            displayContrasena(Password[0],20,30,0);
            displayContrasena(Password[1],40,30,1);
            displayContrasena(Password[2],60,30,0);
            displayContrasena(Password[3],80,30,0);
         }
         else
         {
            if(unidad==3)
            {
               displayContrasena(Password[0],20,30,0);
               displayContrasena(Password[1],40,30,0);
               displayContrasena(Password[2],60,30,1);
               displayContrasena(Password[3],80,30,0);
            }
            else
            {
               if(unidad==4)
               {
                  displayContrasena(Password[0],20,30,0);
                  displayContrasena(Password[1],40,30,0);
                  displayContrasena(Password[2],60,30,0);
                  displayContrasena(Password[3],80,30,1);
               }
            }
         }
      }
      
      if(UP)//Si oprime hacia arriba
      {
         Buzz_on;delay_ms(10);Buzz_off;
         delay_ms(30);
         for(i=1;i<=4;i++)
         {
            if(unidad==i)
            {
               Password[i-1]++;
               if(Password[i-1]>9)   
               {
                  Password[i-1]=0;
               }
            }
         }
      }
            
      if(DOWN)//Si oprime hacia abajo
      {
         Buzz_on;delay_ms(10);Buzz_off;
         delay_ms(30);
         for(i=1;i<=4;i++)
            {
               if(unidad==i)
               {
                  Password[i-1]--;
                  if(Password[i-1]<0)
                  {
                     Password[i-1]=9;
                  }
               }
            }       
      }
            
      if(RIGHT)//Si oprime RIGHT
      {
         Buzz_on;delay_ms(10);Buzz_off;delay_ms(30);unidad++;
      }
      
      if(LEFT)//Si oprime Izquierda
      {
         Buzz_on;delay_ms(10);Buzz_off;delay_ms(700);Menu=1;glcd_fillscreen(OFF);
      }
      
      if(unidad>4)
      {
         Opcion=1;unidad=1;
         write_eeprom(10,Password[0]);
         delay_ms(50);   
         write_eeprom(11,Password[1]);
         delay_ms(50);   
         write_eeprom(12,Password[2]);
         delay_ms(50);   
         write_eeprom(13,Password[3]);
         delay_ms(1000);glcd_fillScreen(OFF);Menu=1;
      }
    }    
//_--------------------------------------------------------------------------------------------------------------------------------------------------------    

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu>10 && Menu<16){ //Menu Editar Programa 01, Programa 02, Programa 03, Programa 04 y Programa 05
      
         if(Opcion>3)
         {
            Opcion=1;
            if(i<9){
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
         
         if(RIGHT && LEFT){
            Buzz_on;Menu=5;Opcion=1;glcd_fillscreen(OFF);delay_ms(500);Buzz_off;
            
            for(i=0;i<10;i++){
               delay_ms(10);    
               write_eeprom((i*2)+(40*(Programa-1)),make8(Temperatura1[i+((Programa-1)*10)],0));
               delay_ms(10);    
               write_eeprom((i*2)+1+(40*(Programa-1)),make8(Temperatura1[i+((Programa-1)*10)],1));
               delay_ms(10);    
               write_eeprom(i+((40*Programa)-20),Horas1[i+((Programa-1)*10)]);
               delay_ms(10);    
               write_eeprom(i+((40*Programa)-10),Minutos1[i+((Programa-1)*10)]);
            } 
         }
         
         if(RIGHT)//Si oprime SET
         { 
            Buzz_on;delay_ms(50);Opcion++;glcd_fillscreen(OFF);Buzz_off;
         }
         
         if(LEFT){
            Buzz_on;delay_ms(50);Opcion--;glcd_fillscreen(OFF);Buzz_off;
         }
   }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==20){ //Menu de Funcionamiento
      
      // Muestra imagen de fondo en Display GLCD      
      glcd_imagen(5);
      //flag_display=0;

      if(UP && DOWN)//Si oprime hacia abajo
      {
         glcd_init(ON);delay_ms(200);flag_display=!flag_display;         
      }

      if(Temperatura>1200 || Temperatura<0)
      {
         glcd_rect(0, 55, 127, 63, YES, OFF);
         strcpy(Menus.Letra,"Status:Sensor Descon.");
         displayMenu(Menus.Letra,0,55,2,1);
         tiempo_esterilizacion=0;
         Resistencia_off;
         Tpulso=0.0;
         flag_tiempocalentamiento=0;
         Buzz_on;
         delay_ms(500);
         Buzz_off;
         delay_ms(250);
         Control=0;
      }
      else
      {
         
         if(Pausa==0 && !DOOR)
         {
            Control=1;
            
            if(Temperatura>Setpoint+AlTem && AlTem>0){
               Alarma=5;
            }else{  
               Alarma=10;
               if((Temperatura>=Setpoint-10) && (Temperatura<Setpoint+10))
               {
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
               }
               else
               {
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
         
         if(Alarma==5){
            Control=0;            
            flag_tiempocalentamiento=0;
            tiempo_esterilizacion=0;
            Resistencia_off;
            if(flag_clear2==0)
            {
               //Ganancia=read_eeprom(90);
               //Ganancia=Setpoint*(Ganancia/Coeficiente);
               //glcd_fillScreen(OFF);
               glcd_fillScreen(OFF);
               glcd_init(ON);
               glcd_rect(0, 55, 127, 63, YES, OFF);
               Tpulso=0.0;
               flag_clear2=1;
               flag_ganancia=0;
            }
            strcpy(Menus.Letra,"Status:Sobre Temp");
            displayMenu(Menus.Letra,0,55,2,1);
            Buzz_on;
            delay_ms(1000);
            Buzz_off;
            delay_ms(1000);
         }
         
         if(Pausa==1 && !DOOR)
         {
            flag_tiempocalentamiento=0;
            strcpy(Menus.Letra,"Status:Pausado");
            displayMenu(Menus.Letra,0,55,2,1);
            tiempo_esterilizacion=0;
            Control=0;
            Resistencia_off;
            Tpulso=0.0;
         }
         
         if(DOOR)
         {
            Control=0;            
            flag_tiempocalentamiento=0;
            tiempo_esterilizacion=0;
            Resistencia_off;
            if(flag_clear2==0)
            {
               //Ganancia=read_eeprom(90);
               //Ganancia=Setpoint*(Ganancia/Coeficiente);
               //glcd_fillScreen(OFF);
               glcd_fillScreen(OFF);
               glcd_init(ON);
               glcd_rect(0, 55, 127, 63, YES, OFF);
               Tpulso=0.0;
               flag_clear2=1;
               flag_ganancia=0;
            }
            strcpy(Menus.Letra,"Status:Puerta Abierta");
            displayMenu(Menus.Letra,0,55,2,1);
            Buzz_on;
            delay_ms(1000);
            Buzz_off;
            delay_ms(1000);
         }
         else
         {
            Buzz_off;
            if(flag_clear2==1)
            {
               Pausa=Pausa_ant;
               glcd_fillScreen(OFF);
               glcd_rect(0, 55, 127, 63, YES, OFF);
               flag_clear2=0;
            }
         }
      }
      
      if(UP)//Si oprime hacia arriba
      {
         glcd_fillscreen(OFF);
               glcd_line(27,0,27,55,ON);
               glcd_line(27,55,127,55,ON);
               
               strcpy(Menus.Letra,"Grafica Rampas");displayMenu(Menus.Letra,0,0,1,1); 
               
               glcd_line(24,55-10,27,55-10,ON);
               strcpy(Menus.Letra,"300");displayMenu(Menus.Letra,0,55-15,2,1); 
               glcd_line(24,55-20,27,55-20,ON);
               strcpy(Menus.Letra,"600");displayMenu(Menus.Letra,0,55-25,2,1); 
               glcd_line(24,55-30,27,55-30,ON);
               strcpy(Menus.Letra,"900");displayMenu(Menus.Letra,0,55-35,2,1);
               glcd_line(24,55-40,27,55-40,ON);
               strcpy(Menus.Letra,"1200");displayMenu(Menus.Letra,0,55-45,2,1); 
               
               if(Segmento==0){
                  strcpy(Menus.Letra,"s0");displayMenu(Menus.Letra,16,56,1,1);}
                  
               if(Segmento==1){
                  strcpy(Menus.Letra,"s1");displayMenu(Menus.Letra,27,56,1,1);}
                  
               if(Segmento==2){
                  strcpy(Menus.Letra,"s2");displayMenu(Menus.Letra,38,56,1,1);}
                  
               if(Segmento==3){
                  strcpy(Menus.Letra,"s3");displayMenu(Menus.Letra,49,56,1,1);}
                  
               if(Segmento==4){
                  strcpy(Menus.Letra,"s4");displayMenu(Menus.Letra,60,56,1,1);}
                  
               if(Segmento==5){
                  strcpy(Menus.Letra,"s5");displayMenu(Menus.Letra,71,56,1,1);}
                  
               if(Segmento==6){
                  strcpy(Menus.Letra,"s6");displayMenu(Menus.Letra,82,56,1,1);}
                  
               if(Segmento==7){
                  strcpy(Menus.Letra,"s7");displayMenu(Menus.Letra,93,56,1,1);}
                  
               if(Segmento==8){
                  strcpy(Menus.Letra,"s8");displayMenu(Menus.Letra,104,56,1,1);}
               
               if(Segmento==9){
                  strcpy(Menus.Letra,"s9");displayMenu(Menus.Letra,115,56,1,1);}
               
               glcd_line(27+0,55-TempeInicial/30,27+10,55-Temperatura1[ProgramaSeleccionado*10]/30,ON);
               glcd_line(27+10,55-Temperatura1[ProgramaSeleccionado*10]/30,27+20,55-Temperatura1[(ProgramaSeleccionado*10)+1]/30,ON);
               glcd_line(27+20,55-Temperatura1[(ProgramaSeleccionado*10)+1]/30,27+30,55-Temperatura1[(ProgramaSeleccionado*10)+2]/30,ON);
               glcd_line(27+30,55-Temperatura1[(ProgramaSeleccionado*10)+2]/30,27+40,55-Temperatura1[(ProgramaSeleccionado*10)+3]/30,ON);
               glcd_line(27+40,55-Temperatura1[(ProgramaSeleccionado*10)+3]/30,27+50,55-Temperatura1[(ProgramaSeleccionado*10)+4]/30,ON);
               glcd_line(27+50,55-Temperatura1[(ProgramaSeleccionado*10)+4]/30,27+60,55-Temperatura1[(ProgramaSeleccionado*10)+5]/30,ON);
               glcd_line(27+60,55-Temperatura1[(ProgramaSeleccionado*10)+5]/30,27+70,55-Temperatura1[(ProgramaSeleccionado*10)+6]/30,ON);
               glcd_line(27+70,55-Temperatura1[(ProgramaSeleccionado*10)+6]/30,27+80,55-Temperatura1[(ProgramaSeleccionado*10)+7]/30,ON);
               glcd_line(27+80,55-Temperatura1[(ProgramaSeleccionado*10)+7]/30,27+90,55-Temperatura1[(ProgramaSeleccionado*10)+8]/30,ON);
               glcd_line(27+90,55-Temperatura1[(ProgramaSeleccionado*10)+8]/30,27+100,55-Temperatura1[(ProgramaSeleccionado*10)+9]/30,ON);
               glcd_update();
               for(i=0;i<80;i++)
               {
                  Temperatura=Leer_Sensor(MediaMovil);
               }
            delay_ms(3000);
            glcd_fillscreen(OFF);
      }
            
      if(DOWN)//Si oprime hacia abajo
      {  
      }
      
      if(RIGHT)//Si oprime SET
      {  
         if(Pausa!=2)
         {
            if(Pausa==0)
            {
               delay_ms(200);Pausa=1;
            }
            else
            {
               delay_ms(200);Pausa=0;
            }
            glcd_rect(0, 55, 127, 63, YES, OFF);
         }
      }
      
        
      if(Lectura==1)// Utilizado para visualizar la lectura de la temperatura
      {                 
         
         Lectura=0;
         Potencia=(100.0/Tciclo)*Tpulso;
         
 //----------------Muestra Temperatura--------------------------
         if(Temperatura>1200 || Temperatura<0)
         {
            glcd_rect(15, 29, 69, 45, YES, OFF);
            strcpy(Menus.Letra,"-----");displayMenu(Menus.Letra,15,29,2,2);
            Desconectado=1;
            Control=0;
         }
         else
         {
            if(Desconectado==1)
            {
               Desconectado=0;
               delay_ms(2000);
               for(i=0;i<80;i++)
               {
                  Temperatura=Leer_Sensor(80);
               }
            }
            
               displayT(Temperatura,15,29,2);
            
            //displayTemperatura(Temperatura,15,29,0,2);
         }

         //displayT(Temperatura,15,29,2);
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
            
         //displayTiempo(Tpulsomin*10,97,42,3,1);strcpy(Menus.Letra,"%");displayMenu(Menus.Letra,115,42,2,1);
         
         if(flag_display==1){
            displayTiempo(penMufla,97,1,3,1);
            //displayTemperatura(Prom,97,13,0,1);
            //displayFloat(Setpoint-TempeInicial,97,13,1);
            displayFloat(TemperaturaIdeal,97,10,1);}
  //       else{
//            glcd_rect(60, 46, 88, 52, YES, OFF);
//            glcd_rect(97, 13, 88, 52, YES, OFF);
         //}
         
         // Muestra Valor de esterilizacion Programado.
            displayTiempo(Testerilizacion[3],20,2,0,2);displayTiempo(Testerilizacion[2],33,2,0,2);
            strcpy(Menus.Letra,":");displayMenu(Menus.Letra,46,2,2,2); 
            displayTiempo(Testerilizacion[1],55,2,0,2);displayTiempo(Testerilizacion[0],68,2,0,2);
            
            // Muestra Valor de esterilizacion actual.
            strcpy(Menus.Letra,"Time:");displayMenu(Menus.Letra,0,18,2,1); 
            displayTiempo(Testerilizacionp[3],30,18,0,1);displayTiempo(Testerilizacionp[2],36,18,0,1);
            strcpy(Menus.Letra,":");displayMenu(Menus.Letra,42,18,2,1); 
            displayTiempo(Testerilizacionp[1],48,18,0,1);displayTiempo(Testerilizacionp[0],54,18,0,1);
            strcpy(Menus.Letra,"(H:M)");displayMenu(Menus.Letra,60,18,2,1); 
         
            glcd_rect(0, 0, 89, 27, NO, ON);
            glcd_rect(0, 27, 89, 54, NO, ON);
      } 
     
      // Si cumplio el tiempo de esterilización, Muestre ciclo finalizado y detenga control de temperatura.
      if((Testerilizacion[0]>=Testerilizacionp[0])&&(Testerilizacion[1]>=Testerilizacionp[1])&&(Testerilizacion[2]>=Testerilizacionp[2])&&(Testerilizacion[3]>=Testerilizacionp[3]) && Pausa<3)
      {
         flag_calcula=0;
         if(Segmento<Segmentos){
            Segmento++;
            Testerilizacion[0]=0;
            Testerilizacion[1]=0;
            Testerilizacion[2]=0;
            Testerilizacion[3]=0;
         }
         else{
            Pausa=3;
         }
         glcd_fillScreen(OFF);
         Buzz_on;
         delay_ms(1000);
         Buzz_off;
         delay_ms(1000);
         glcd_init(ON);
         strcpy(Menus.Letra,"Status:Cambio Seg.");
         displayMenu(Menus.Letra,0,55,2,1);         
         delay_ms(500);
         
         if(Pausa==3){
            glcd_fillScreen(OFF);
         }
      } 
      
      if(tinicio>=60)
      {
         tinicio=0;
         Testerilizacion[0]++;
         if(Testerilizacion[0]>9)
         {
            Testerilizacion[0]=0;
            Testerilizacion[1]++;
         }
         if(Testerilizacion[1]>5)
         {
            Testerilizacion[1]=0;
            Testerilizacion[2]++;
         }
         if(Testerilizacion[2]>9)
         {
            Testerilizacion[2]=0;
            Testerilizacion[3]++;
         }
         if(Testerilizacion[3]>9)
         {
            Testerilizacion[3]=0;
         }
      }
      
      if(LEFT)//Si oprime boton de Toma.
      {
         if(Pausa<3){
            if(Pausa!=2)
            {
               delay_ms(500);Menu=30;glcd_fillscreen(OFF);Opcion=1;visual=1;Pausa=1;Menu=1;
            }
         }
      }
      
      if(Pausa==3 && !DOOR){         
         strcpy(Menus.Letra,"Status:Finalizado");
         displayMenu(Menus.Letra,0,55,2,1);
         tiempo_esterilizacion=4;
         Control=0;
         Resistencia_off;
         Tpulso=0.0;
      }
        
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu==100){ //Menu de Configuración de Parametros de Control
      
      if(Opcion>11)
      {Opcion=1;}
      if(Opcion<1)
      {Opcion=11;}
      
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
            {strcpy(Menus.Letra,"Resistencia");
            displayMenu(Menus.Letra,0,27,1,1);}
         else
            {strcpy(Menus.Letra,"Resistencia");
            displayMenu(Menus.Letra,0,27,0,1);}     
            
         if(Opcion==11)   
            {strcpy(Menus.Letra,"m Mufla");
            displayMenu(Menus.Letra,0,36,1,1);}
         else
            {strcpy(Menus.Letra,"m Mufla");
            displayMenu(Menus.Letra,0,36,0,1);}     
            
            strcpy(Menus.Letra,"            ");
            displayMenu(Menus.Letra,0,45,0,1);
      }   
      
         if(UP)//Si oprime hacia arriba
         {
            Buzz_on;delay_ms(10);Buzz_off;
            if(Flanco == 0)
               {Flanco = 1;delay_ms(30);Opcion--;visual=1;
               }
         }
            else
               {Flanco = 0;}
                  
         if(DOWN)//Si oprime hacia abajo
         {
            Buzz_on;delay_ms(10);Buzz_off;
            if(Flanco2 == 0)
               {Flanco2 = 1;delay_ms(30);Opcion++;visual=1;
               }     
         }
            else
               {Flanco2 = 0;}  
               
         if(RIGHT)//Si oprime SET
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(Flanco1 == 0)
               {Flanco1 = 1;delay_ms(30);Menu=Opcion+100;Opcion=1;glcd_fillscreen(OFF);visual=1;
               }
         }
            else
               {Flanco1 = 0;} 
               
         if(LEFT)//Si oprime boton de Toma.
         {
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(700);Menu=1;glcd_fillscreen(OFF);Opcion=1;visual=1;            
         }      
        
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==101){ // Menu de Ajuste de Ganancia
      
      if(Ganancia>40)
      {
         Ganancia=2;
      }
      
      if(visual==1)
      {
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"P");
         displayMenu(Menus.Letra,0,0,1,2);
         
         displayTemperatura(Ganancia,30,30,0,3);
         
         visual=0;
      }
      
         if(UP)//Si oprime hacia arriba
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(Ganancia>40)
            {
               Ganancia=40;delay_ms(100);visual=1;
            }
            else
            {
               delay_ms(30);Ganancia+=1.0;visual=1;
            }
         }
               
         if(DOWN)//Si oprime hacia abajo
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(Ganancia>1)
            {
               delay_ms(30);Ganancia-=1.0;visual=1;     
            }
         }
               
         if(RIGHT)//Si oprime SET
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(30);visual=1;
         } 
                  
         if(LEFT)//Si oprime boton de Toma.
         {
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(700);Menu=100;glcd_fillscreen(OFF);Opcion=1;visual=1;
            write_eeprom(250,(int8)Ganancia);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==102){ // Menu de Ajuste de Integral
      
      if(Integral>40)
      {
         Integral=1;
      }
      
      if(visual==1)
      {
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"I");
         displayMenu(Menus.Letra,0,0,1,2);
         
         displayTemperatura(Integral,30,30,0,3);
         
         visual=0;
      }
      
         if(UP)//Si oprime hacia arriba
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(Integral>40)
            {
               Integral=40;delay_ms(100);visual=1;
            }
            else
            {
               delay_ms(30);Integral+=1.0;visual=1;
            }
         }
               
         if(DOWN)//Si oprime hacia abajo
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(Integral>1)
            {
               delay_ms(30);Integral-=1.0;visual=1;     
            }
         }
               
         if(RIGHT)//Si oprime SET
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(30);visual=1;
         } 
                  
         if(LEFT)//Si oprime boton de Toma.
         {
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(700);Menu=100;glcd_fillscreen(OFF);Opcion=1;visual=1;
            write_eeprom(270,(int8)Integral);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==103){ // Menu de Ajuste de Derivativo
      
      if(Derivativo>200)
      {
         Derivativo=2;
      }
      
      if(visual==1)
      {
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"D");
         displayMenu(Menus.Letra,0,0,1,2);
         
         displayTemperatura(Derivativo,30,30,0,3);
         
         visual=0;
      }
      
         if(UP)//Si oprime hacia arriba
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(Derivativo>200)
            {
               Derivativo=200;delay_ms(100);visual=1;
            }
            else
            {
               delay_ms(30);Derivativo+=1.0;visual=1;
            }
         }
               
         if(DOWN)//Si oprime hacia abajo
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(Derivativo>1)
            {
               delay_ms(30);Derivativo-=1.0;visual=1;     
            }
         }
               
         if(RIGHT)//Si oprime SET
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(30);visual=1;
         } 
                  
         if(LEFT)//Si oprime boton de Toma.
         {
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(700);Menu=100;glcd_fillscreen(OFF);Opcion=1;visual=1;
            write_eeprom(271,(int8)Derivativo);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------


//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==104){ // Menu de Ajuste de Ganancia2
      
      if(Tciclo>40)
      {
         Tciclo=40;
      }
      
      if(visual==1)
      {
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Tciclo");
         displayMenu(Menus.Letra,0,0,1,2);
         
         displayTemperatura(Tciclo,30,30,0,3);
         
         visual=0;
      }
      
         if(UP)//Si oprime hacia arriba
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(Tciclo>40)
            {
               Tciclo=40;delay_ms(100);visual=1;
            }
            else
            {
               delay_ms(30);Tciclo+=1.0;visual=1;
            }
         }
               
         if(DOWN)//Si oprime hacia abajo
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(Tciclo>1)
            {
               delay_ms(30);Tciclo-=1.0;visual=1;     
            }
         }
               
         if(RIGHT)//Si oprime SET
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(30);visual=1;
         } 
                  
         if(LEFT)//Si oprime boton de Toma.
         {
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(700);Menu=100;glcd_fillscreen(OFF);Opcion=1;visual=1;
            write_eeprom(251,(int8)Tciclo);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==105){ // Menu de Ajuste de Ajuste
      
      if(Ajuste>100)
      {
         Ajuste=0;
      }
      
      if(visual==1)
      {
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Ajuste");
         displayMenu(Menus.Letra,0,0,1,2);
         
         displayTemperatura(Ajuste,30,30,0,3);
         
         visual=0;
      }
      
         if(UP)//Si oprime hacia arriba
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(Ajuste>100)
            {
               Ajuste=100;delay_ms(100);visual=1;
            }
            else
            {
               delay_ms(30);Ajuste+=1.0;visual=1;
            }
         }
               
         if(DOWN)//Si oprime hacia abajo
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(Ajuste>0)
            {
               delay_ms(30);Ajuste-=1.0;visual=1;     
            }
         }
               
         if(RIGHT)//Si oprime SET
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(30);visual=1;
         } 
                  
         if(LEFT)//Si oprime boton de Toma.
         {
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(700);Menu=100;glcd_fillscreen(OFF);Opcion=1;visual=1;
            write_eeprom(252,Ajuste);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==106){ // Menu de Ajuste de Ajusten
      
      if(Ajusten>100)
      {
         Ajusten=0;
      }
      
      if(visual==1)
      {
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Ajusten");
         displayMenu(Menus.Letra,0,0,1,2);
         
         displayTemperatura(Ajusten,30,30,0,3);
         
         visual=0;
      }
      
         if(UP)//Si oprime hacia arriba
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(Ajusten>100)
            {
               Ajusten=100;delay_ms(100);visual=1;
            }
            else
            {
               delay_ms(30);Ajusten+=1.0;visual=1;
            }
         }
               
         if(DOWN)//Si oprime hacia abajo
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(Ajusten>0)
            {
               delay_ms(30);Ajusten-=1.0;visual=1;     
            }
         }
               
         if(RIGHT)//Si oprime SET
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(30);visual=1;
         } 
                  
         if(LEFT)//Si oprime boton de Toma.
         {
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(700);Menu=100;glcd_fillscreen(OFF);Opcion=1;visual=1;
            write_eeprom(253,Ajusten);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==107){ // Menu de Ajuste de Media Movil
      
      if(MediaMovil>80)
      {
         MediaMovil=10;
      }
      
      if(visual==1)
      {
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Media");
         displayMenu(Menus.Letra,0,0,1,2);
         
         displayTemperatura(MediaMovil,30,30,0,3);
         
         visual=0;
      }
      
         if(UP)//Si oprime hacia arriba
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(MediaMovil>79)
            {
               MediaMovil=79;delay_ms(100);visual=1;
            }
            else
            {
               delay_ms(30);MediaMovil+=1.0;visual=1;
            }
         }
               
         if(DOWN)//Si oprime hacia abajo
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(MediaMovil>1)
            {
               delay_ms(30);MediaMovil-=1.0;visual=1;     
            }
         }
               
         if(RIGHT)//Si oprime SET
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(30);visual=1;
         } 
                  
         if(LEFT)//Si oprime boton de Toma.
         {
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(700);Menu=100;glcd_fillscreen(OFF);Opcion=1;visual=1;
            write_eeprom(254,(int8)MediaMovil);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==108){ // Menu de Ajuste de Coeficiente Regulador
      
      if(Coeficiente>250)
      {
         Coeficiente=100;
      }
      
      if(visual==1)
      {
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Coeficiente");
         displayMenu(Menus.Letra,0,0,1,2);
         
         displayTemperatura(Coeficiente,30,30,0,3);
         
         visual=0;
      }
      
         if(UP)//Si oprime hacia arriba
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(Coeficiente>250)
            {
               Coeficiente=250;delay_ms(100);visual=1;
            }
            else
            {
               delay_ms(30);Coeficiente+=1.0;visual=1;
            }
         }
               
         if(DOWN)//Si oprime hacia abajo
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(Coeficiente>1)
            {
               delay_ms(30);Coeficiente-=1.0;visual=1;     
            }
         }
               
         if(RIGHT)//Si oprime SET
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(30);visual=1;
         } 
                  
         if(LEFT)//Si oprime boton de Toma.
         {
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(700);Menu=100;glcd_fillscreen(OFF);Opcion=1;visual=1;
            write_eeprom(255,(int8)Coeficiente);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==109){ // Menu de Alarma Sobretemperatura
      
      if(AlTem>250)
      {
         AlTem=250;
      }
      
      if(visual==1)
      {
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Alarma");
         displayMenu(Menus.Letra,0,0,1,2);
         
         displayTemperatura(AlTem,30,30,0,3);
         
         visual=0;
      }
      
         if(UP)//Si oprime hacia arriba
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(AlTem>250)
            {
               AlTem=250;delay_ms(100);visual=1;
            }
            else
            {
               delay_ms(30);AlTem+=1.0;visual=1;
            }
         }
               
         if(DOWN)//Si oprime hacia abajo
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            if(AlTem>0)
            {
               delay_ms(30);AlTem-=1.0;visual=1;     
            }
         }
               
         if(RIGHT)//Si oprime SET
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(30);visual=1;
         } 
                  
         if(LEFT)//Si oprime boton de Toma.
         {
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(700);Menu=100;glcd_fillscreen(OFF);Opcion=1;visual=1;
            write_eeprom(256,(int8)AlTem);
            delay_ms(10);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==110){ // Menu de Resistencia conversora I/V
      
      if(resistencia>260)      
         resistencia=240;
      else if(resistencia<230)
         resistencia=240;
      
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"Resis.");
         displayMenu(Menus.Letra,0,0,1,2);
         
         displayTemperatura(resistencia,30,30,0,3);
         
         visual=0;
      }
      
         if(UP)//Si oprime hacia arriba
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            visual=1;
            resistencia+=1.0;
            delay_ms(30);
         }
               
         if(DOWN)//Si oprime hacia abajo
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            visual=1;
            resistencia-=1.0;
            delay_ms(30);
         }
               
         if(RIGHT)//Si oprime SET
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(30);visual=1;
         } 
                  
         if(LEFT)//Si oprime boton de Toma.
         {
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(700);Menu=100;glcd_fillscreen(OFF);Opcion=1;visual=1;
            write_eeprom(257,(int8)resistencia);
            delay_ms(10);
            adcInf=(0.004*resistencia*1023)/5.0;
            adcSup=(0.02*resistencia*1023)/5.0;
            m=(Tsup-Tinf)/(adcSup-adcInf);
            b=Tsup-(m*adcSup);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==111){ // Menu de Pendiente de calentamiento de mufla
      
      if(penMufla>255)      
         penMufla=1;
      else if(penMufla<1)
         penMufla=255;
      
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"m Mufla");
         displayMenu(Menus.Letra,0,0,1,2);
         
         displayTemperatura(penMufla,30,30,0,3);
         
         visual=0;
      }
      
         if(UP)//Si oprime hacia arriba
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            visual=1;
            penMufla+=1.0;
            delay_ms(30);
         }
               
         if(DOWN)//Si oprime hacia abajo
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            visual=1;
            penMufla-=1.0;
            delay_ms(30);
         }
               
         if(RIGHT)//Si oprime SET
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(30);visual=1;
         } 
                  
         if(LEFT)//Si oprime boton de Toma.
         {
            Buzz_on;delay_ms(10);Buzz_off;
            delay_ms(700);Menu=100;glcd_fillscreen(OFF);Opcion=1;visual=1;
            write_eeprom(258,(int8)penMufla);
            delay_ms(10);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------


      if(Lectura2==1)
      {
         Temperatura=Leer_Sensor(MediaMovil);
         Lectura2=0;
      } 

      if(tiemporeset>=60)
      {
         if(Menu!=0){
            glcd_init(ON);
            tiemporeset=0;
         }
      } 


   }
}
