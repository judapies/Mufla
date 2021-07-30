// Control para mufla con rampas 
// Se pueden programar 5 Programas de 10 Segmentos cada Uno.
// En un segmento se puede programar una rampa o una meseta.


#include <18F4685.h>
#device adc=10
#fuses HS,NOWDT,NOPROTECT,NOLVP,NODEBUG,NOSTVREN,NOPUT,NOCPD,NOWRT,NOIESO,NOFCMEN,NOPBADEN,NOWRTC,NOWRTB,NOEBTR,NOEBTRB,NOCPB,NOXINST,NOLPT1OSC,NOMCLR
#use delay(clock=20000000)
#use RS232(BAUD=19200, BITS=8, PARITY=N, XMIT=PIN_C6, RCV=PIN_C7, stream=Printer)
#include "HDM64GS12.c"
#include "GRAPHICS.c"
#include "imagen.h"
#include <MATH.h>
#include <DS1302.c>

// Definición de teclado - NO
#define   UP               !input(PIN_A5)
#define   DOWN             !input(PIN_E0)
#define   RIGHT            !input(PIN_E1)
#define   LEFT             !input(PIN_E2)

#define   DOOR             input(PIN_A4)
#define   TERM             input(PIN_A3)

#define   Resistencia_on   output_bit(PIN_C0,1)
#define   Resistencia_off  output_bit(PIN_C0,0)
#define   Buzz_on          output_bit(PIN_C4,1)
#define   Buzz_off         output_bit(PIN_C4,0)

float promedio[90]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float X[2]={0.0,0.0};
short flagup=0,flagdn=0,Desconectado=0,ActivaImpresion=0,flagImprime=0;
int8 h=0,l=0,Ajuste=0,Ajusten=0,MediaMovil=10,tiemporecta=0,flag_ganancia=0,Lectura2=0,tiempoup=0,tiempodn=0,Segmentos=0,Segmento=0,Modo=5;
short Flanco=0,Flanco1=0,Flanco2=0,visual=1,Lectura=0,flag_calcula=0;
short flag_clear2=0,flag_tiempocalentamiento=0,flag_display=0,flag_setpoint=0,Dibuja=0;
int8 y=0,tiemporeset=0,Programa=1,Control=0;
int8 Menu=0, unidad=1,i=0,j=1,Pausa=0,Pausa_ant=0,TiempoControl=0;
int8 Opcion=1,tiempo_esterilizacion=0,ProgramaSeleccionado=0,Alarma=10;
int16 tiempos=0,tiempos3=0,tiempoctrl=0,tinicio=0,tiempocalentamiento=0,tiemposetpoint=0,Tp=0,Tc=0,CalculoImpresion=0,tiempoImpresion=0,timpresion=0;
float PendienteMinutos=0,TempeInicial=20,Tiempo=0,TiempoActual=0,PendienteActual=0,HorasActual=0,MinutosActual=0,TiempoRampa=0;
float Potencia=0.0,PotenciaEntregada=0.0;
signed int  Testerilizacion[4]={0,0,0,0};   // Tiempo transcurrido
signed int  Testerilizacionp[4]={0,0,0,0};   // Tiempo transcurrido
signed int  Contrasena[4]={0,0,0,0};      // Contrasena de 4 digitos
signed int  Password[4]={0,0,0,0};        // Contrasena Almacenada de 4 digitos
float V0=0.0,Tciclo=10.0,Tpulso=0.0,error=0.0,Prom=0.0,Temperature=0.0,tmp=0.0,GananciaIn=0,Ganancia=3.0,desvio=3.0,Coeficiente=150.0,AlTem=20.0,resistencia=240.0;
float Aumento=0.0,Temperatura=0.0,Pendiente=0.0,DifPendiente=0.0,Setpoint=0,Integral=0,Derivativo=0,m=0,b=0,adcSup=0,adcInf=0,Tsup=1250,Tinf=0,penMufla=50;
float TemperaturaIdeal=0,penInicial=0;
signed int16 Temperatura1[50];
signed int16 Horas1[50];
signed int16 Minutos1[50];
int8 Year=18,Mess=9,Dia=13,Hora=0,Minuto=0,Segundo=0,dow=0,Mes=9,Modulo=0;
int8 YearTx=0,MesTx=0,DiaTx=0,HoraTx=0,MinutoTx=0,SegundoTx=0,dowTx=0,SetTime=0,Imprime=0;
/* Tabla de Modulos, Primera Fila Año Regular y el siguiente año Bisiesto
E   F   M   A   M   J   J   A   S   O   N   D
0   3   3   6   1   4   6   2   5   0   3   5
0   3   4   0   2   5   0   3   6   1   4   6
*/
int8 Regular[12]={0,3,3,6,1,4,6,2,5,0,3,5};
int8 Bisiesto[12]={0,3,4,0,2,5,0,3,6,1,4,6};

#include <Funciones.h>

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
      
      if(Menu==20 && tiempoImpresion<CalculoImpresion){
         tiempoImpresion++;
      }else if(tiempoImpresion>=CalculoImpresion){
         flagImprime=1;
         tiempoImpresion=0;
      }
   
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

void main() 
{ 
   delay_ms(1000);
   ConfigInicial();
   
   glcd_init(ON);             //Inicializa la glcd
   glcd_fillScreen(OFF);      //Limpia la pantalla
   strcpy(Menus.Letra,"JP Inglobal");
   displayMenu(Menus.Letra,0,0,0,2);     
   strcpy(Menus.Letra,"   Mufla");
   displayMenu(Menus.Letra,0,20,0,2);       
   strcpy(Menus.Letra,"     9L");
   displayMenu(Menus.Letra,0,45,0,2);
   delay_ms(1000);
   
   Resistencia_off;
   glcd_fillScreen(OFF);      //Limpia la pantalla
   displayContrasena(Contrasena[0],20,30,1);
   displayContrasena(Contrasena[1],40,30,0);
   displayContrasena(Contrasena[2],60,30,0);
   displayContrasena(Contrasena[3],80,30,0); 
   Menu=0;
   LeeEEPROM();
   
   for(i=0;i<80;i++){
      Temperatura=Leer_Sensor(MediaMovil);
   }
   
   adcInf=(0.004*resistencia*1023)/5.02;
   adcSup=(0.02*resistencia*1023)/5.02;
   m=(Tsup-Tinf)/(adcSup-adcInf);
   b=Tsup-(m*adcSup);
   penInicial=penMufla;
   rtc_init();
//Inicio de Impresora
   putc(27);
   putc(64);
   delay_us(10);
   putc(10);
   //32 Caracteres se pueden escribir en una linea de la impresora
 //fprintf(printer,"                                \n");
   CalculoImpresion=timpresion*60;
   rtc_get_date(Dia,Mes,Year,dow);
   rtc_get_time(Hora,Minuto,Segundo);
   if(ActivaImpresion){
      fprintf(printer," MUFLA DIGITAL  JP INGLOBAL  \n");
      delay_us(10);
      fprintf(printer,"   FECHA: %02u-%02u-%02u \n",Dia,Mes,Year);
   }
   while(TRUE) 
   { 
      glcd_update();            
      rtc_get_date(Dia,Mes,Year,dow);
      rtc_get_time(Hora,Minuto,Segundo);
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
      
         if(Opcion>7)
         {Opcion=1;}
         if(Opcion<1)
         {Opcion=7;}
         
         strcpy(Menus.Letra,"Menu");
         displayMenu(Menus.Letra,30,0,1,2);
      
         if(Opcion<=5)
         {
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
         }else{
            if(Opcion==6)   
               {strcpy(Menus.Letra,"Fecha y Hora");
               displayMenu(Menus.Letra,0,18,1,1);}
            else
               {strcpy(Menus.Letra,"Fecha y Hora");
               displayMenu(Menus.Letra,0,18,0,1);}
            
            if(Opcion==7)   
               {strcpy(Menus.Letra,"Ajustes Impresora");
               displayMenu(Menus.Letra,0,27,1,1);}
            else
               {strcpy(Menus.Letra,"Ajustes Impresora");
               displayMenu(Menus.Letra,0,27,0,1);}
               
               strcpy(Menus.Letra,"           ");
               displayMenu(Menus.Letra,0,36,0,1);
               
               strcpy(Menus.Letra,"           ");
               displayMenu(Menus.Letra,0,45,0,1);
               
               strcpy(Menus.Letra,"           ");
               displayMenu(Menus.Letra,0,54,0,1);
         }
            
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
            DiaTx=Dia;
            MesTx=Mes;
            YearTx=Year;
            HoraTx=Hora;
            MinutoTx=Minuto;   
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
            Ganancia=GananciaIn*(Setpoint/Coeficiente);
            
            Tiempo=(Horas1[ProgramaSeleccionado*10]*3600)+(Minutos1[ProgramaSeleccionado*10]*60);         
            Setpoint=Temperatura1[(ProgramaSeleccionado*10)+Segmento];
            Pendiente=((Setpoint-TempeInicial)/Tiempo);
            Testerilizacionp[3]=(int8)Horas1[(ProgramaSeleccionado*10)+Segmento]/10;
            Testerilizacionp[2]=Horas1[(ProgramaSeleccionado*10)+Segmento]-(Testerilizacionp[3]*10);
            Testerilizacionp[1]=Minutos1[(ProgramaSeleccionado*10)+Segmento]/10;
            Testerilizacionp[0]=Minutos1[(ProgramaSeleccionado*10)+Segmento]-(Testerilizacionp[1]*10);
            if(ActivaImpresion){
               fprintf(printer,"OPERADOR:________________\n");
               delay_us(20);
               fprintf(printer,"Programa: %02u\n",ProgramaSeleccionado+1);
               delay_us(20);
               fprintf(printer,"Segmentos: %02u\n",Segmentos);
               delay_us(20);
               fprintf(printer,"  HORA   TEMPER.   SEGMENTO \n");
            }
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
            Buzz_on;delay_ms(50);Menu=Opcion+30;Opcion=1;glcd_fillscreen(OFF);Buzz_off;i=0;
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
      if(Menu==9){ //Menu de Configuración de Fecha y Hora
      
      strcpy(Menus.Letra,"Fecha");
      displayMenu(Menus.Letra,45,0,1,1);
      
      strcpy(Menus.Letra,"Hora");
      displayMenu(Menus.Letra,45,32,1,1);
      
      LimitaDia();
      
      if(Opcion>5)
      {Opcion=1;}
      if(Opcion<1)
      {Opcion=5;}
      
      displayFecha(DiaTx,MesTx,YearTx,15,13,Opcion,2);
      displayHora(HoraTx,MinutoTx,25,44,Opcion,2);
      
         if(UP){
            delay_ms(3);
            if(Opcion==1){
               if(MesTx==2){
                  if(esBisiesto(YearTx)){
                     if(DiaTx<29)
                        DiaTx++;
                     else
                        DiaTx=1;
                  }else{
                     if(DiaTx<28)
                        DiaTx++;
                     else
                        DiaTx=1;
                  }
               }else{
                  if(MesTx<=7){
                     if(MesTx % 2 ==0){
                        if(DiaTx<30)
                           DiaTx++;    
                        else
                           DiaTx=1;
                     }else{
                        if(DiaTx<31)
                           DiaTx++;    
                        else
                           DiaTx=1;
                     }    
                  }else{
                     if(MesTx % 2 ==0){
                        if(DiaTx<31)
                           DiaTx++;    
                        else
                           DiaTx=1;
                     }else{
                        if(DiaTx<30)
                           DiaTx++;    
                        else
                           DiaTx=1;
                     }    
                  }
               }
            }else if(Opcion==2){
               if(MesTx<12)
                  MesTx++;
               else
                  MesTx=1;
            }else if(Opcion==3){
               if(YearTx<99)
                  YearTx++;
               else
                  YearTx=0;
            }else if(Opcion==4){
               if(HoraTx<24)
                  HoraTx++;
               else
                  HoraTx=0;
            }else if(Opcion==5){
               if(MinutoTx<59)
                  MinutoTx++;
               else
                  MinutoTx=0;
            }
         }
                  
         if(DOWN){
            delay_ms(3);
            if(Opcion==1){
               if(DiaTx>0)
                  DiaTx--;
            }else if(Opcion==2){
               if(MesTx>1)
                  MesTx--;
            }else if(Opcion==3){
               if(YearTx>0)
                  YearTx--;
            }else if(Opcion==4){
               if(HoraTx>0)
                  HoraTx--;
            }else if(Opcion==5){
               if(MinutoTx>0)
                  MinutoTx--;
            }
         }
               
         if(RIGHT){  
            delay_ms(10);
            if(Flanco1 == 0){
               Flanco1 = 1;delay_ms(3);Opcion++;glcd_fillscreen(OFF);visual=1;
            }
         }else{
            Flanco1 = 0;
         }
               
         if(LEFT){
            if(esBisiesto(YearTx))
               Modulo=Bisiesto[MesTx];
            else
               Modulo=Regular[MesTx];
         
            dowTx=((YearTx-1)%7+((YearTx-1)/4-3*((YearTx-1)/100+1)/4)%7+Modulo+DiaTx%7)%7;
            rtc_set_datetime(DiaTx,MesTx,YearTx,dowTx,HoraTx,MinutoTx);
            delay_ms(100);
            Menu=1;
            Opcion=1;
            glcd_fillscreen(OFF);visual=1;
            fprintf(printer,"  \n");
            fprintf(printer,"  Ajusto Fecha y hora \n");
            rtc_get_date(Dia,Mes,Year,dow);
            rtc_get_time(Hora,Minuto,Segundo);
            fprintf(printer,"%02u-%02u-%02u  %02u:%02u  %03.1f°C   \n",Dia,Mes,Year,Hora,Minuto,Temperatura); 
         }      
        
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==10){ //Menu de Configuración de Impresora
      
      strcpy(Menus.Letra,"Desea Impresion?");
      displayMenu(Menus.Letra,0,0,1,1);
      
      strcpy(Menus.Letra,"Tiempo de Impresion");
      displayMenu(Menus.Letra,0,32,1,1);
      
      if(timpresion>30)
         timpresion=30;
      
      if(Opcion==1 && ActivaImpresion==0){
         strcpy(Menus.Letra,"No");
         displayMenu(Menus.Letra,40,13,1,2);
      }else if(Opcion==1 && ActivaImpresion==1){
         strcpy(Menus.Letra,"Si");
         displayMenu(Menus.Letra,40,13,1,2);
      }else if(Opcion==2 && ActivaImpresion==1){
         strcpy(Menus.Letra,"Si");
         displayMenu(Menus.Letra,40,13,0,2);
      }else if(Opcion==2 && ActivaImpresion==0){
         strcpy(Menus.Letra,"No");
         displayMenu(Menus.Letra,40,13,0,2);
      }
      if(Opcion==1)
         displayTiempo(timpresion,40,44,0,2);
      if(Opcion==2)
         displayTiempo(timpresion,40,44,1,2);
      
      if(Opcion>2)
      {Opcion=1;}
      if(Opcion<1)
      {Opcion=2;}
      
         if(UP){
            delay_ms(3);
            if(Opcion==1){
               ActivaImpresion=1;
            }else if(Opcion==2){
               timpresion++;       
            }
         }
                  
         if(DOWN){
            delay_ms(3);
            if(Opcion==1){
               ActivaImpresion=0;
            }else if(Opcion==2){
               if(timpresion>1)
                  timpresion--;
            }
         }
               
         if(RIGHT){  
            delay_ms(10);
            if(Flanco1 == 0){
               Flanco1 = 1;delay_ms(3);Opcion++;glcd_fillscreen(OFF);visual=1;
            }
         }else{
            Flanco1 = 0;
         }
               
         if(LEFT){
            Menu=1;
            Opcion=1;
            glcd_fillscreen(OFF);visual=1;
            write_eeprom(280,ActivaImpresion);delay_ms(10);
            write_eeprom(281,timpresion);delay_ms(10);
            CalculoImpresion=timpresion*60;
         }      
        
    }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------       

//_--------------------------------------------------------------------------------------------------------------------------------------------------------
      if(Menu>30 && Menu<36){ //Menu Editar Programa 01, Programa 02, Programa 03, Programa 04 y Programa 05
      
         if(Opcion>3){
            Opcion=1;
            if(i<9){
               i++;  
            }
         }
         
         if(Opcion<1){
            if(i>0)
               i--;
            Opcion=3;
         }
         
         if(Menu==31)
            Programa=1;
         
         if(Menu==32)
            Programa=2;
            
         if(Menu==33)
            Programa=3;
            
         if(Menu==34)
            Programa=4;
            
         if(Menu==35)
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
      
      if(flagImprime && ActivaImpresion==1){
         fprintf(printer,"  %02u:%02u    %03.1f°C  %u\n",Hora,Minuto,Temperatura,Segmento);
         flagImprime=0;
      }
      // Muestra imagen de fondo en Display GLCD      
      glcd_imagen(5);
      //flag_display=0;

      if(UP && DOWN)//Si oprime hacia abajo
      {
         glcd_init(ON);delay_ms(200);flag_display=!flag_display;         
      }

      if(Temperatura>1250 || Temperatura<0)
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
               if((Temperatura>=Setpoint-10) && (Temperatura<Setpoint+50))
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
               Ganancia=GananciaIn*(Setpoint/Coeficiente);
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
            Buzz_on;
            delay_ms(1000);
            Buzz_off;
            delay_ms(1000);
         }
         else
         {
            Buzz_off;
            if(flag_clear2==1){               
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
         if(Temperatura>1250 || Temperatura<0)
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
            
            strcpy(Menus.Letra,"Power:");displayMenu(Menus.Letra,91,34,2,1);
            displayTiempo(Potencia,97,44,3,1);strcpy(Menus.Letra,"%");displayMenu(Menus.Letra,115,44,2,1);
            strcpy(Menus.Letra,"Segm.");displayMenu(Menus.Letra,95,18,2,1);
            displayTiempo(Segmento,96,26,3,1);
            glcd_rect(90, 0, 127, 54, NO, ON);
            
         //displayTiempo(Tpulsomin*10,97,42,3,1);strcpy(Menus.Letra,"%");displayMenu(Menus.Letra,115,42,2,1);
         
         if(flag_display==1){
            displayTiempo(sensores(0),97,1,3,1);
            //displayTemperatura(Prom,97,13,0,1);
            //displayFloat(Setpoint-TempeInicial,97,13,1);
            displayFloat(TemperaturaIdeal,97,10,1);}
         else{
            glcd_rect(0, 0, 89, 27, NO, ON);
            glcd_rect(0, 27, 89, 54, NO, ON);
            displayHora2(Hora,Minuto,92,2,1,1);
            displayFecha2(Dia,Mes,Year,92,10,1,1);
         }
         
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
         if(ActivaImpresion){
            fprintf(printer,"---------------\n");
            delay_us(20);
            fprintf(printer,"Inicia Segmento %02u\n",Segmento);
            delay_us(20);
         }
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
