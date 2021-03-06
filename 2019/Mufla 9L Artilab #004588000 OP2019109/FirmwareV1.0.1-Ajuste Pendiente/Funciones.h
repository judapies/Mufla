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
   //Temperature=(Prom-100.0)/(0.385);  // Ecuaci?n determinada por linealizaci?n en Matlab. Revisar este polinomio.  
   //Temperature=(1.5275*Prom)-299.23; // Recta dada por T 0?C-1200?C y ADC Modo Real   
   Temperature=(m*Prom)-b; // Recta dada por T 0?C-1200?C y ADC Modo Real
   
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
      error=desvio+Setpoint-Temperatura;      // C?lculo del error ******
      
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
   else{// Si Esta realizando una Rampa debe realizar el control de acuerdo a la pendiente de Temperatura. Aprox 10?C * min Maximo Valor de Pendiente.
      PendienteMinutos=PendienteActual*60;
      //PotenciaEntregada=((PendienteMinutos+DifPendiente)*100)/penMufla;
      //HorasActual=(Testerilizacion[3]*10)+Testerilizacion[2];
      //MinutosActual=(Testerilizacion[1]*10)+Testerilizacion[0];
      TiempoRampa=((HorasActual*3600)+(MinutosActual*60)+tinicio);
      if(Segmento==0){
         TemperaturaIdeal=(Pendiente*TiempoRampa)+TempeInicial;
      }else{
         TemperaturaIdeal=(Pendiente*TiempoRampa)+Temperatura1[(ProgramaSeleccionado*10)+Segmento-1];
      }
      PotenciaEntregada=((TemperaturaIdeal-Temperatura)*100)/penMufla;
// Pendiente determinar si el comportamiento de la mufla es lineal 10?C/min o si tiene un comportamiento diferente.
         
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

void displayTiempo2(int digito, int x, int y, int sombreado, int tamano)
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
      sprintf(voltage, "%03u", digito);
      if(sombreado==1)
         {glcd_rect(x, y, x+(tamano*18), y+(tamano*8), YES, ON);glcd_text57(x+1, y+1, voltage, tamano, OFF);}
      if(sombreado==0)
         {glcd_rect(x, y, x+(tamano*18), y+(tamano*8), YES, OFF);glcd_text57(x+1, y+1, voltage, tamano, ON);}
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
   sprintf(voltage, "%03.2f",adc); // Converts adc to text
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
         
         if(Temperatura1[l+((Prog-1)*10)]>1100){
            Horas1[l+((Prog-1)*10)]=0;
         }
         
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

int8 extraeDecimal(float data){
   float entero=0;
   float tmp=0.0;
   
   entero=(int8)data;
   tmp=(data-entero)*100;
   return (int8)tmp;
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
      for(i=0;i<10;i++){
         delay_ms(10); 
         Temperatura1[i+((j-1)*10)]=make16(read_eeprom((i*2)+1+((j-1)*40)),read_eeprom((i*2)+((j-1)*40)));
         if(Temperatura1[i+((j-1)*10)]>1200 || Temperatura1[i+((j-1)*10)]<0){
            Temperatura1[i+((j-1)*10)]=0;
            write_eeprom((i*2)+1+((j-1)*40),0);
            delay_ms(20);
            write_eeprom((i*2)+((j-1)*40),0);
            delay_ms(20);
         }
         delay_ms(10);
         Horas1[i+((j-1)*10)]=read_eeprom(i+((40*j)-20));     
         if(Horas1[i+((j-1)*10)]>99){
            Horas1[i+((j-1)*10)]=0;
            write_eeprom(i+((40*j)-20),0);
            delay_ms(20);
         }
         delay_ms(10);    
         Minutos1[i+((j-1)*10)]=read_eeprom(i+((40*j)-10));
         if(Minutos1[i+((j-1)*10)]>59){
            Minutos1[i+((j-1)*10)]=0;
            write_eeprom(i+((40*j)-10),0);
            delay_ms(20);
         }
         
      }
   }
   
   delay_ms(50);   
   Ganancia=read_eeprom(250);
   delay_ms(50);   
   if(Ganancia>10){
      Ganancia=3;
      write_eeprom(250,3);
      delay_ms(20);
   }
   GananciaIn=Ganancia;
   Tciclo=read_eeprom(251);
   delay_ms(50);   
   if(Tciclo>10){
      Tciclo=10;
      write_eeprom(251,10);
      delay_ms(20);
   }
   Ajuste=read_eeprom(252);
   delay_ms(50);
   if(Ajuste>10){
      Ajuste=0;
      write_eeprom(252,0);
      delay_ms(20);
   }
   Ajusten=read_eeprom(253);
   delay_ms(50);
   if(Ajusten>10){
      Ajusten=0;
      write_eeprom(253,0);
      delay_ms(20);
   }
   MediaMovil=read_eeprom(254);
   delay_ms(50);   
   if(MediaMovil>40){
      MediaMovil=20;
      write_eeprom(254,20);
      delay_ms(20);
   }
   Coeficiente=read_eeprom(255);
   delay_ms(50);   
   AlTem=read_eeprom(256);
   delay_ms(50); 
   resistencia=read_eeprom(257);
   delay_ms(50); 
   if(resistencia>250){
      resistencia=240;
      write_eeprom(257,240);
      delay_ms(20);
   }
   penMufla=read_eeprom(258);
   delay_ms(50); 
   if(penMufla>150){
      penMufla=100;
      write_eeprom(258,100);
      delay_ms(20);
   }
   
   Password[0]=read_eeprom(260);
   delay_ms(50);   
   if(Password[0]>9 || Password[0]<0){
      Password[0]=0;
      write_eeprom(260,0);
      delay_ms(20);
   }
   Password[1]=read_eeprom(261);
   delay_ms(50);   
   if(Password[1]>9 || Password[1]<0){
      Password[1]=0;
      write_eeprom(261,0);
      delay_ms(20);
   }
   Password[2]=read_eeprom(262);
   delay_ms(50);   
   if(Password[2]>9 || Password[2]<0){
      Password[2]=0;
      write_eeprom(262,0);
      delay_ms(20);
   }
   Password[3]=read_eeprom(263);
   delay_ms(50);
   if(Password[3]>9 || Password[3]<0){
      Password[3]=0;
      write_eeprom(263,0);
      delay_ms(20);
   }
   
   Integral=read_eeprom(270);
   delay_ms(50); 
   if(Integral>5){
      Integral=1;
      write_eeprom(270,1);
      delay_ms(20);
   }
   Derivativo=read_eeprom(271);
   delay_ms(50); 
   if(Derivativo>20){
      Derivativo=20;
      write_eeprom(271,20);
      delay_ms(20);
   }
   ActivaImpresion=read_eeprom(280);
   delay_ms(50); 
   if(ActivaImpresion>1){
      ActivaImpresion=1;
      write_eeprom(280,1);
      delay_ms(20);
   }
   timpresion=read_eeprom(281);
   delay_ms(50); 
   if(timpresion>30){
      timpresion=3;
      write_eeprom(281,3);
      delay_ms(20);
   }
   
   Modo=read_eeprom(480);
   delay_ms(50); 
   m_int=read_eeprom(490);
   delay_ms(50); 
   m_dec=read_eeprom(491);
   delay_ms(50); 
   m=m_int+((float)m_dec/100);
   b16=make16(read_eeprom(493),read_eeprom(492));
   b_dec=read_eeprom(494);
   delay_ms(50); 
   b=b16+((float)b_dec/100);
   if(b<300 || b>400){
      b=347.66;
      write_eeprom(492,make8(347,0));
      delay_ms(10);
      write_eeprom(493,make8(347,1));
      delay_ms(10);
      write_eeprom(494,66);
      delay_ms(10);
   }
   if(m<1.0 || m>2.0){
      m=1.79;
      write_eeprom(490,1);
      delay_ms(10);
      write_eeprom(491,79);
      delay_ms(10);
   }
}

short esBisiesto(int8 year) {
     return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}

void LimitaDia(){
   if(MesTx>12)
      MesTx=10;
   if(DiaTx>31)
      DiaTx=16;
   if(YearTx>99)
      YearTx=18;   
   if(HoraTx>24)
      HoraTx=11;      
   if(MinutoTx>59)
      MinutoTx=30;      
   
      if(MesTx==2){
         if(esBisiesto(YearTx)){
            if(DiaTx>29){
               DiaTx=29;
               glcd_fillScreen(OFF);
            }
         }else{
            if(DiaTx>28){
               DiaTx=28;
               glcd_fillScreen(OFF);
            }
         }
      }else{
         if(MesTx<=7){
            if(MesTx % 2 ==0){
               if(DiaTx>30){
                  DiaTx=30; 
                  glcd_fillScreen(OFF);
               }
            }
         }else{
            if(MesTx % 2 ==1){
               if(DiaTx>30){
                  DiaTx=30; 
                  glcd_fillScreen(OFF);
               }
            }
         }
      }
}

void displayFecha(int8 dia, int8 mes, int8 ano,int x, int y, int8 sombreado, int tamano){  
   char voltage[4];
   if(sombreado==1){
      sprintf(voltage, "%02u", dia); 
      glcd_rect(x-1, y-1, x+(tamano*12), y+(tamano*7), YES, ON);
      glcd_text57(x, y, voltage, tamano, OFF);
      strcpy(Menus.Letra,"/");
      glcd_text57(x+(tamano*12), y, Menus.Letra, tamano, ON);
      
      sprintf(voltage, "%02u", mes); 
      glcd_text57(x+(tamano*18), y, voltage, tamano, ON);
      strcpy(Menus.Letra,"/");
      glcd_text57(x+(tamano*30), y, Menus.Letra, tamano, ON);
      
      sprintf(voltage, "%02u", ano); 
      glcd_text57(x+(tamano*36), y, voltage, tamano, ON);
   }else if(sombreado==2){
      sprintf(voltage, "%02u", dia); 
      glcd_text57(x, y, voltage, tamano, ON);
      strcpy(Menus.Letra,"/");
      glcd_text57(x+(tamano*12), y, Menus.Letra, tamano, ON);
      
      glcd_rect(x-1+(tamano*18), y-1, x+(tamano*30), y+(tamano*7), YES, ON);
      sprintf(voltage, "%02u", mes); 
      glcd_text57(x+(tamano*18), y, voltage, tamano, OFF);
      strcpy(Menus.Letra,"/");
      glcd_text57(x+(tamano*30), y, Menus.Letra, tamano, ON);
      
      sprintf(voltage, "%02u", ano); 
      glcd_text57(x+(tamano*36), y, voltage, tamano, ON);
   }else if(sombreado==3){
      sprintf(voltage, "%02u", dia); 
      glcd_text57(x, y, voltage, tamano, ON);
      strcpy(Menus.Letra,"/");
      glcd_text57(x+(tamano*12), y, Menus.Letra, tamano, ON);
            
      sprintf(voltage, "%02u", mes); 
      glcd_text57(x+(tamano*18), y, voltage, tamano, ON);
      strcpy(Menus.Letra,"/");
      glcd_text57(x+(tamano*30), y, Menus.Letra, tamano, ON);
     
      glcd_rect(x-1+(tamano*36), y-1, x+(tamano*48), y+(tamano*7), YES, ON);
      sprintf(voltage, "%02u", ano); 
      glcd_text57(x+(tamano*36), y, voltage, tamano, OFF);
   }else{
      glcd_rect(x, y-1, 127, y+(tamano*7), YES, OFF);
      sprintf(voltage, "%02u", dia); 
      glcd_text57(x, y, voltage, tamano, ON);
      strcpy(Menus.Letra,"/");
      glcd_text57(x+(tamano*12), y, Menus.Letra, tamano, ON);
            
      sprintf(voltage, "%02u", mes); 
      glcd_text57(x+(tamano*18), y, voltage, tamano, ON);
      strcpy(Menus.Letra,"/");
      glcd_text57(x+(tamano*30), y, Menus.Letra, tamano, ON);
          
      sprintf(voltage, "%02u", ano); 
      glcd_text57(x+(tamano*36), y, voltage, tamano, ON);
   }
   glcd_update();
}

void displayHora(int8 hora, int8 minuto, int x, int y, int8 sombreado, int tamano){  
   char voltage[4];
   if(sombreado==4){
      sprintf(voltage, "%02u", hora); 
      glcd_rect(x-1, y-1, x+(tamano*12), y+(tamano*7), YES, ON);
      glcd_text57(x, y, voltage, tamano, OFF);
      strcpy(Menus.Letra,":");
      glcd_text57(x+(tamano*12), y, Menus.Letra, tamano, ON);
      
      sprintf(voltage, "%02u", minuto); 
      glcd_text57(x+(tamano*18), y, voltage, tamano, ON);
   }else if(sombreado==5){
      sprintf(voltage, "%02u", hora); 
      glcd_text57(x, y, voltage, tamano, ON);
      strcpy(Menus.Letra,":");
      glcd_text57(x+(tamano*12), y, Menus.Letra, tamano, ON);
      
      glcd_rect(x-1+(tamano*18), y-1, x+(tamano*30), y+(tamano*7), YES, ON);
      sprintf(voltage, "%02u", minuto); 
      glcd_text57(x+(tamano*18), y, voltage, tamano, OFF);
   }else{
      glcd_rect(x, y-1, 127, y+(tamano*7), YES, OFF);
      sprintf(voltage, "%02u", hora); 
      glcd_text57(x, y, voltage, tamano, ON);
      strcpy(Menus.Letra,":");
      glcd_text57(x+(tamano*12), y, Menus.Letra, tamano, ON);
            
      sprintf(voltage, "%02u", minuto); 
      glcd_text57(x+(tamano*18), y, voltage, tamano, ON);
   }
   glcd_update();
}

void displayFecha2(int8 dia, int8 mes, int8 ano,int x, int y, int8 sombreado, int tamano){  
   char voltage[4];
  
      sprintf(voltage, "%02u", dia); 
      glcd_rect(x-1, y-1, x+(tamano*30), y+(tamano*7), YES, OFF);
      glcd_text57(x, y, voltage, tamano, ON);
      strcpy(Menus.Letra,"-");
      glcd_text57(x+(tamano*12), y, Menus.Letra, tamano, ON);
      
      if(mes==1){
         sprintf(voltage, "Ene"); 
         glcd_text57(x+(tamano*18), y, voltage, tamano, ON); 
      }else if(mes==2){
         sprintf(voltage, "Feb"); 
         glcd_text57(x+(tamano*18), y, voltage, tamano, ON); 
      }else if(mes==3){
         sprintf(voltage, "Mar"); 
         glcd_text57(x+(tamano*18), y, voltage, tamano, ON); 
      }else if(mes==4){
         sprintf(voltage, "Abr"); 
         glcd_text57(x+(tamano*18), y, voltage, tamano, ON); 
      }else if(mes==5){
         sprintf(voltage, "May"); 
         glcd_text57(x+(tamano*18), y, voltage, tamano, ON); 
      }else if(mes==6){
         sprintf(voltage, "Jun"); 
         glcd_text57(x+(tamano*18), y, voltage, tamano, ON); 
      }else if(mes==7){
         sprintf(voltage, "Jul"); 
         glcd_text57(x+(tamano*18), y, voltage, tamano, ON); 
      }else if(mes==8){
         sprintf(voltage, "Ago"); 
         glcd_text57(x+(tamano*18), y, voltage, tamano, ON); 
      }else if(mes==9){
         sprintf(voltage, "Sep"); 
         glcd_text57(x+(tamano*18), y, voltage, tamano, ON); 
      }else if(mes==10){
         sprintf(voltage, "Oct"); 
         glcd_text57(x+(tamano*18), y, voltage, tamano, ON); 
      }else if(mes==12){
         sprintf(voltage, "Nov"); 
         glcd_text57(x+(tamano*18), y, voltage, tamano, ON); 
      }else if(mes==12){
         sprintf(voltage, "Dic"); 
         glcd_text57(x+(tamano*18), y, voltage, tamano, ON); 
      }
      //strcpy(Menus.Letra,"/");
      //glcd_text57(x+(tamano*30), y, Menus.Letra, tamano, ON);
      
      //sprintf(voltage, "%02u", ano); 
      //glcd_text57(x+(tamano*36), y, voltage, tamano, ON);
   glcd_update();
}

void displayHora2(int8 hora, int8 minuto, int x, int y, int8 sombreado, int tamano){  
   char voltage[4];
      sprintf(voltage, "%02u", hora); 
      glcd_rect(x-1, y-1, x+(tamano*30), y+(tamano*7), YES, OFF);
      glcd_text57(x, y, voltage, tamano, ON);
      strcpy(Menus.Letra,":");
      glcd_text57(x+(tamano*12), y, Menus.Letra, tamano, ON);
      sprintf(voltage, "%02u", minuto); 
      glcd_text57(x+(tamano*18), y, voltage, tamano, ON);
  
   glcd_update();
}

void menuAjustes(){
   
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

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==112){ // Menu de Pendiente de termocupla
      
      if(m>2.0)      
         m=1.2;
      else if(m<1.2)
         m=2.0;
      
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"m");
         displayMenu(Menus.Letra,0,0,1,2);
         
         displayFloat(m,10,30,3);
         
         visual=0;
      }
      
         if(UP)//Si oprime hacia arriba
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            visual=1;
            m+=0.01;
            delay_ms(10);
         }
               
         if(DOWN)//Si oprime hacia abajo
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            visual=1;
            m-=0.01;
            delay_ms(10);
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
            write_eeprom(490,(int8)m);
            delay_ms(10);
            write_eeprom(491,extraeDecimal(m));
            delay_ms(10);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------

//_--------------------------------------------------------------------------------------------------------------------------------------------------------      
      if(Menu==113){ // Menu de b de termocupla
      
      if(b>400)      
         b=300;
      else if(b<300)
         b=400;
      
      if(visual==1){
         glcd_fillScreen(OFF);
         strcpy(Menus.Letra,"b");
         displayMenu(Menus.Letra,0,0,1,2);
         
         displayFloat(b,30,30,3);
         
         visual=0;
      }
      
         if(UP)//Si oprime hacia arriba
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            visual=1;
            b+=0.1;
            delay_ms(10);
         }
               
         if(DOWN)//Si oprime hacia abajo
         {  
            Buzz_on;delay_ms(10);Buzz_off;
            visual=1;
            b-=0.1;
            delay_ms(10);
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
            b16=(int16)b;
            write_eeprom(492,make8(b16,0));
            delay_ms(10);
            write_eeprom(493,make8(b16,1));
            delay_ms(10);
            write_eeprom(494,extraeDecimal(b));
            delay_ms(10);
         }
     }
//_--------------------------------------------------------------------------------------------------------------------------------------------------------


}
