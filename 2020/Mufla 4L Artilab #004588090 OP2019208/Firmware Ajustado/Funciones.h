//Funciones utilizadas en la Mufla

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
         Tiempo=(Horas1[ProgramaSeleccionado*Pasos]*3600)+(Minutos1[ProgramaSeleccionado*Pasos]*60);         
         Setpoint=Temperatura1[(ProgramaSeleccionado*Pasos)+Segmento];
         Pendiente=((Setpoint-TempeInicial)/Tiempo);
         Testerilizacionp[3]=(int8)Horas1[(ProgramaSeleccionado*Pasos)+Segmento]/10;
         Testerilizacionp[2]=Horas1[(ProgramaSeleccionado*Pasos)+Segmento]-(Testerilizacionp[3]*10);
         Testerilizacionp[1]=Minutos1[(ProgramaSeleccionado*Pasos)+Segmento]/10;
         Testerilizacionp[0]=Minutos1[(ProgramaSeleccionado*Pasos)+Segmento]-(Testerilizacionp[1]*10);
         flag_calcula=1;
         Ganancia=GananciaIn*(Setpoint/Coeficiente);
      }
   }
   else{
      if(flag_calcula==0){
         Tiempo=(Horas1[(ProgramaSeleccionado*Pasos)+Segmento]*3600)+(Minutos1[(ProgramaSeleccionado*Pasos)+Segmento]*60);
         Pendiente=((Temperatura1[(ProgramaSeleccionado*Pasos)+Segmento]-Temperatura1[(ProgramaSeleccionado*Pasos)+Segmento-1])/Tiempo);
         Setpoint=Temperatura1[(ProgramaSeleccionado*Pasos)+Segmento];
         Testerilizacionp[3]=(int8)Horas1[(ProgramaSeleccionado*Pasos)+Segmento]/10;
         Testerilizacionp[2]=Horas1[(ProgramaSeleccionado*Pasos)+Segmento]-(Testerilizacionp[3]*10);
         Testerilizacionp[1]=Minutos1[(ProgramaSeleccionado*Pasos)+Segmento]/10;
         Testerilizacionp[0]=Minutos1[(ProgramaSeleccionado*Pasos)+Segmento]-(Testerilizacionp[1]*10);
         flag_calcula=1;
         Ganancia=GananciaIn*(Setpoint/Coeficiente);
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

void displayTiempos(int digito, int x, int y, int sombreado, int tamano)
{
   char voltage[9];
   
   if(sombreado==2)
   {
      sprintf(voltage, "%02d", digito);
      glcd_rect(x, y, x+(tamano*12), y+(tamano*8), YES, OFF);
      glcd_text57(x+1, y+1, voltage, tamano, ON);
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

void MuestraSegmento(int8 Opcio,int8 l,int8 Prog ){
         if(Temperatura1[l+((Prog-1)*Pasos)]>1200)
            Temperatura1[l+((Prog-1)*Pasos)]=0;
         
         if(Temperatura1[l+((Prog-1)*Pasos)]<0)
            Temperatura1[l+((Prog-1)*Pasos)]=1200;
         
         if(Temperatura1[l+((Prog-1)*Pasos)]>1100)
            Horas1[l+((Prog-1)*Pasos)]=0;
         
         strcpy(Menus.Letra,"P 0");
         displayMenu(Menus.Letra,30,0,1,2);
         displayTiempo(Prog,68,0,1,2);
         
         strcpy(Menus.Letra,"Segmento ");
         displayMenu(Menus.Letra,0,20,1,1);
         displayTiempo(l,63,20,1,1);
         
         strcpy(Menus.Letra,"TA:");
         displayMenu(Menus.Letra,80,20,3,1);
         
         if(l>0)
            displayTemperatura(Temperatura1[l-1+((Prog-1)*Pasos)],100,20,1,1);
         
         if(Opcio==1){
            strcpy(Menus.Letra,"Temperatura:");
            displayTemps(Menus.Letra,Temperatura1[l+((Prog-1)*Pasos)],0,32,1,1);
            strcpy(Menus.Letra,"Horas:");
            displayTemps(Menus.Letra,Horas1[l+((Prog-1)*Pasos)],0,44,10,1);
            strcpy(Menus.Letra,"Minutos:");
            displayTemps(Menus.Letra,Minutos1[l+((Prog-1)*Pasos)],0,55,10,1);
            if(UP)//Si oprime hacia arriba
            {
               Buzz_on;delay_ms(10);Buzz_off;
               if(tiempoup<4){
                  flagup=1;
                  Temperatura1[l+((Prog-1)*Pasos)]++;
               }
               else{
                  flagup=0;
                  Temperatura1[l+((Prog-1)*Pasos)]+=10;
               } 
               if(l<Pasos-1)
                  Temperatura1[l+1+((Prog-1)*Pasos)]=Temperatura1[l+((Prog-1)*Pasos)]; 
                  
               if(Temperatura1[l+((Prog-1)*Pasos)]>1200)
                  Temperatura1[l+((Prog-1)*Pasos)]=0;
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
                  Temperatura1[l+((Prog-1)*Pasos)]--;
               }
               else{
                  flagdn=0;
                  Temperatura1[l+((Prog-1)*Pasos)]-=10;
               }
               Temperatura1[l+1+((Prog-1)*Pasos)]=Temperatura1[l+((Prog-1)*Pasos)]; 
               
               if(Temperatura1[l+((Prog-1)*Pasos)]<0)
                  Temperatura1[l+((Prog-1)*Pasos)]=1200;
            }
            else{
               flagdn=0;tiempodn=0;
            }
         }
         
         if(Opcio==2){
            strcpy(Menus.Letra,"Temperatura:");
            displayTemps(Menus.Letra,Temperatura1[l+((Prog-1)*Pasos)],0,32,0,1);
            strcpy(Menus.Letra,"Horas:");
            displayTemps(Menus.Letra,Horas1[l+((Prog-1)*Pasos)],0,44,11,1);
            strcpy(Menus.Letra,"Minutos:");
            displayTemps(Menus.Letra,Minutos1[l+((Prog-1)*Pasos)],0,55,10,1);
            if(UP)//Si oprime hacia arriba
            {
               Buzz_on;delay_ms(10);Buzz_off;
               if(tiempoup<4){
                  flagup=1;
                  Horas1[l+((Prog-1)*Pasos)]++;
               }
               else{
                  flagup=0;
                  Horas1[l+((Prog-1)*Pasos)]+=10;
               }
               if(Horas1[l+((Prog-1)*Pasos)]>99)
                  Horas1[l+((Prog-1)*Pasos)]=0;
            }
            else{
               flagup=0;tiempoup=0;
            }
                     
            if(DOWN)//Si oprime hacia abajo
            {
               Buzz_on;delay_ms(10);Buzz_off;
               if(tiempodn<4){
                  flagdn=1;
                  Horas1[l+((Prog-1)*Pasos)]--;
               }
               else{
                  flagdn=0;
                  Horas1[l+((Prog-1)*Pasos)]-=10;
               }
               if(Horas1[l+((Prog-1)*Pasos)]<0)
                  Horas1[l+((Prog-1)*Pasos)]=99;
            }
            else{
               flagdn=0;tiempodn=0;
            }
         }
         
         if(Opcio==3){
            strcpy(Menus.Letra,"Temperatura:");
            displayTemps(Menus.Letra,Temperatura1[l+((Prog-1)*Pasos)],0,32,0,1);
            strcpy(Menus.Letra,"Horas:");
            displayTemps(Menus.Letra,Horas1[l+((Prog-1)*Pasos)],0,44,10,1);
            strcpy(Menus.Letra,"Minutos:");
            displayTemps(Menus.Letra,Minutos1[l+((Prog-1)*Pasos)],0,55,11,1);
            if(UP)//Si oprime hacia arriba
            {
               Buzz_on;delay_ms(10);Buzz_off;
               if(tiempoup<4){
                  flagup=1;
                  Minutos1[l+((Prog-1)*Pasos)]++;
               }  
               else{
                  flagup=0;
                  Minutos1[l+((Prog-1)*Pasos)]+=10;
               }
               if(Minutos1[l+((Prog-1)*Pasos)]>59)
                  Minutos1[l+((Prog-1)*Pasos)]=0;
            }
            else{
               flagup=0;tiempoup=0;
            }
                     
            if(DOWN)//Si oprime hacia abajo
            {
               Buzz_on;delay_ms(10);Buzz_off;
               if(tiempodn<4){
                  flagdn=1;
                  Minutos1[l+((Prog-1)*Pasos)]--;
               }
               else{
                  flagdn=0;
                  Minutos1[l+((Prog-1)*Pasos)]-=10;
               }
               if(Minutos1[l+((Prog-1)*Pasos)]<0)
                  Minutos1[l+((Prog-1)*Pasos)]=59;
            }
            else{
               flagdn=0;tiempodn=0;
            }
         }
}

void ConfigInicial(){
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);
   enable_interrupts(INT_TIMER1);
   setup_adc_ports(AN0_TO_AN2);
   setup_adc(ADC_CLOCK_DIV_64);
   enable_interrupts(global);
}

void LeeEEPROM(){ 
   for(j=1;j<6;j++){
      for(i=0;i<Pasos;i++){
         delay_ms(10); 
         Temperatura1[i+((j-1)*Pasos)]=make16(read_eeprom((i*2)+1+((j-1)*(Pasos*4))),read_eeprom((i*2)+((j-1)*(Pasos*4))));
         if(Temperatura1[i+((j-1)*Pasos)]>1200 || Temperatura1[i+((j-1)*Pasos)]<0){
            Temperatura1[i+((j-1)*Pasos)]=0;
            write_eeprom((i*2)+1+((j-1)*(Pasos*4)),0);
            delay_ms(20);
            write_eeprom((i*2)+((j-1)*(Pasos*4)),0);
            delay_ms(20);
         }
         delay_ms(10);
         Horas1[i+((j-1)*Pasos)]=read_eeprom(i+(((Pasos*4)*j)-(Pasos*2)));     
         if(Horas1[i+((j-1)*Pasos)]>99){
            Horas1[i+((j-1)*Pasos)]=0;
            write_eeprom(i+(((Pasos*4)*j)-(Pasos*2)),0);
            delay_ms(20);
         }
         delay_ms(10);    
         Minutos1[i+((j-1)*Pasos)]=read_eeprom(i+(((Pasos*4)*j)-Pasos));
         if(Minutos1[i+((j-1)*Pasos)]>59){
            Minutos1[i+((j-1)*Pasos)]=0;
            write_eeprom(i+(((Pasos*4)*j)-Pasos),0);
            delay_ms(20);
         }
         
      }
   }
   
   delay_ms(50);   
   Ganancia=read_eeprom(450);
   delay_ms(50);   
   if(Ganancia>10){
      Ganancia=3;
      write_eeprom(450,3);
      delay_ms(20);
   }
   GananciaIn=Ganancia;
   Tciclo=read_eeprom(451);
   delay_ms(50);   
   if(Tciclo>10){
      Tciclo=10;
      write_eeprom(451,10);
      delay_ms(20);
   }
   Ajuste=read_eeprom(452);
   delay_ms(50);
   if(Ajuste>10){
      Ajuste=0;
      write_eeprom(452,0);
      delay_ms(20);
   }
   Ajusten=read_eeprom(453);
   delay_ms(50);
   if(Ajusten>10){
      Ajusten=0;
      write_eeprom(453,0);
      delay_ms(20);
   }
   MediaMovil=read_eeprom(454);
   delay_ms(50);   
   if(MediaMovil>40){
      MediaMovil=20;
      write_eeprom(454,20);
      delay_ms(20);
   }
   Coeficiente=read_eeprom(455);
   delay_ms(50);   
   resistencia=read_eeprom(457);
   delay_ms(50); 
   if(resistencia>250){
      resistencia=238;
      write_eeprom(457,240);
      delay_ms(20);
   }
   penMufla=read_eeprom(458);
   delay_ms(50); 
   if(penMufla>70){
      penMufla=50;
      write_eeprom(458,100);
      delay_ms(20);
   }
   
   Password[0]=read_eeprom(460);
   delay_ms(50);   
   if(Password[0]>9 || Password[0]<0){
      Password[0]=0;
      write_eeprom(460,0);
      delay_ms(20);
   }
   Password[1]=read_eeprom(461);
   delay_ms(50);   
   if(Password[1]>9 || Password[1]<0){
      Password[1]=0;
      write_eeprom(461,0);
      delay_ms(20);
   }
   Password[2]=read_eeprom(462);
   delay_ms(50);   
   if(Password[2]>9 || Password[2]<0){
      Password[2]=0;
      write_eeprom(462,0);
      delay_ms(20);
   }
   Password[3]=read_eeprom(463);
   delay_ms(50);
   if(Password[3]>9 || Password[3]<0){
      Password[3]=0;
      write_eeprom(463,0);
      delay_ms(20);
   }
   
   Integral=read_eeprom(470);
   delay_ms(50); 
   if(Integral>5){
      Integral=1;
      write_eeprom(470,1);
      delay_ms(20);
   }
   Derivativo=read_eeprom(471);
   delay_ms(50); 
   if(Derivativo>40){
      Derivativo=30;
      write_eeprom(471,30);
      delay_ms(20);
   }
   AlTem=make16(read_eeprom(473),read_eeprom(472));
   delay_ms(50); 
   if(AlTem>500){
      AlTem=200;
      write_eeprom(472,200);
      delay_ms(20);
      write_eeprom(473,0);
      delay_ms(20);
   }
   Modo=read_eeprom(480);
   delay_ms(50); 
}

short PidePassword(int8 MenuAnt, int8 MenuActual, int8 MenuPost, int8 clave0,int8 clave1, int8 clave2,int8 clave3){
      if(n==0)
      {
         displayContrasena(Contrasena[0],20,30,1);displayContrasena(Contrasena[1],40,30,0);
         displayContrasena(Contrasena[2],60,30,0);displayContrasena(Contrasena[0],80,30,0);
         strcpy(Menus.Letra,"Clave");
         displayMenu(Menus.Letra,20,0,1,3);
         n=1;
      }
      
      if(UP){
         if(Flanco == 0){
            Flanco = 1;delay_ms(5);
            for(i=1;i<=4;i++){
               if(unidad==i){
                  Contrasena[i-1]++;
                  if(Contrasena[i-1]>9){
                     Contrasena[i-1]=0;
                  }
                  displayContrasena(Contrasena[i-1],i*20,30,1);
               }else{
                  displayContrasena(Contrasena[i-1],i*20,30,0);
               }
            }
         }
      }else{
         Flanco = 0;
      }
            
      if(DOWN){
         if(Flanco2 == 0){
            Flanco2 = 1;delay_ms(5);
            for(i=1;i<=4;i++){
               if(unidad==i){
                  Contrasena[i-1]--;
                  if(Contrasena[i-1]<0){
                     Contrasena[i-1]=9;
                  }
                  displayContrasena(Contrasena[i-1],i*20,30,1);
               }else{
                  displayContrasena(Contrasena[i-1],i*20,30,0);
               }
            }      
         }     
      }else{
         Flanco2 = 0;
      }
            
      if(RIGHT){
         if(Flanco1 == 0){
            Flanco1 = 1;delay_ms(5);unidad++;
            for(i=1;i<=4;i++){
               if(unidad==i){
                  displayContrasena(Contrasena[i-1],i*20,30,1);
               }else{
                  displayContrasena(Contrasena[i-1],i*20,30,0);
               }
            }
         }
      }else{
         Flanco1 = 0;
      }
      
      if(LEFT){
         delay_ms(100);
         if(LEFT){
            Menu=MenuAnt;glcd_fillScreen(OFF);n=0;Opcion=1;unidad=1;
            Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
            glcd_fillScreen(OFF);glcd_update();
            return 1;
         }
      }
    
      if(unidad>4){
         glcd_fillScreen(OFF);unidad=1;
         /*
         if(Contrasena[0]==3&&Contrasena[1]==8&&Contrasena[2]==9&&Contrasena[3]==2 && MenuActual==0){ // Si Ingresa clave para reset general del sistema.
            write_eeprom(0,0);delay_ms(20);write_eeprom(1,0);delay_ms(20);// Reestablece a contraseña de Fabrica y reinicia Programa.
            write_eeprom(2,0);delay_ms(20);write_eeprom(3,0);delay_ms(20);
            reset_cpu();
         }
      */
         if((Contrasena[0]==clave0)&&(Contrasena[1]==clave1)&&(Contrasena[2]==clave2)&&(Contrasena[3]==clave3)){
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra,"Clave");
            displayMenu(Menus.Letra,30,0,0,2);
            strcpy(Menus.Letra,"Correcta");
            displayMenu(Menus.Letra,15,30,0,2);
            Menu=MenuPost;glcd_update();delay_ms(1000);glcd_fillScreen(OFF);
            Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
            n=0;
            return 1;
         }else{
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra,"Clave");
            displayMenu(Menus.Letra,30,0,0,2);
            strcpy(Menus.Letra,"Incorrecta");
            displayMenu(Menus.Letra,0,30,0,2);
            unidad=1;glcd_update();delay_ms(1000);Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;glcd_fillScreen(OFF);
            glcd_rect(0, 0, 127, 25, YES, ON);strcpy(Menus.Letra,"Clave");glcd_text57(25, 1, Menus.Letra, 3, OFF);displayContrasena(Contrasena[0],20,30,1);
            displayContrasena(Contrasena[1],40,30,0);displayContrasena(Contrasena[2],60,30,0);displayContrasena(Contrasena[0],80,30,0);
            Menu=MenuAnt;glcd_fillScreen(OFF);n=0;
            return 0;
         }
      }
}
