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

#include <Funciones.h>
#include <ks0108.h>
#include <main.h>


uint8 extraeDecimal(float data){
   float entero=0;
   float tmp=0.0;
   
   entero=(uint8)data;
   tmp=(data-entero)*100;
   return (uint8)tmp;
}

float inttofloat(uint8 decimal){
   float tmp;
   tmp=decimal;
   return tmp/10;
}

uint8 convert8(uint16 data,uint8 nible){
    uint8 dato=0;
    if(nible==0){
        dato=(uint8)data;   
    }else if(nible==1){
        dato=data>>8;
    }
    return dato;
}

uint16 convert16(uint8 nibleh,uint8 niblel){
    uint16 temp,var;
    temp=(uint16)nibleh;
    var=temp<<8 | (uint16)niblel;
    return var;
}

void leeEEPROM(){ 
    uint8 j;
   for(j=1;j<6;j++){
      for(i=0;i<Pasos;i++){
         CyDelay(10); 
         Temperatura1[i+((j-1)*Pasos)]=convert16(EEPROM_ReadByte((i*2)+1+((j-1)*(Pasos*4))),EEPROM_ReadByte((i*2)+((j-1)*(Pasos*4))));
         if(Temperatura1[i+((j-1)*Pasos)]>1200 || Temperatura1[i+((j-1)*Pasos)]<0){
            Temperatura1[i+((j-1)*Pasos)]=0;
            EEPROM_WriteByte(0,(i*2)+1+((j-1)*(Pasos*4)));
            CyDelay(20);
            EEPROM_WriteByte(0,(i*2)+((j-1)*(Pasos*4)));
            CyDelay(20);
         }
         CyDelay(10);
         Horas1[i+((j-1)*Pasos)]=EEPROM_ReadByte(i+(((Pasos*4)*j)-(Pasos*2)));     
         if(Horas1[i+((j-1)*Pasos)]>99){
            Horas1[i+((j-1)*Pasos)]=0;
            EEPROM_WriteByte(0,i+(((Pasos*4)*j)-(Pasos*2)));
            CyDelay(20);
         }
         CyDelay(10);    
         Minutos1[i+((j-1)*Pasos)]=EEPROM_ReadByte(i+(((Pasos*4)*j)-Pasos));
         if(Minutos1[i+((j-1)*Pasos)]>59){
            Minutos1[i+((j-1)*Pasos)]=0;
            EEPROM_WriteByte(0,i+(((Pasos*4)*j)-Pasos));
            CyDelay(20);
         }
      }
   }
   
   CyDelay(50);   
   Ganancia=EEPROM_ReadByte(450);
   CyDelay(50);   
   if(Ganancia>40 || Ganancia==0){
      Ganancia=3;
      EEPROM_WriteByte(3,450);
      CyDelay(20);
   }
   GananciaIn=Ganancia;
   Tciclo=EEPROM_ReadByte(451);
   CyDelay(50);   
   if(Tciclo>20 || Tciclo<5){
      Tciclo=10;
      EEPROM_WriteByte(10,451);
      CyDelay(20);
   }
   Ajuste=EEPROM_ReadByte(452);
   CyDelay(50);
   if(Ajuste>30){
      Ajuste=0;
      EEPROM_WriteByte(0,452);
      CyDelay(20);
   }
   Ajusten=EEPROM_ReadByte(453);
   CyDelay(50);
   if(Ajusten>30){
      Ajusten=0;
      EEPROM_WriteByte(0,453);
      CyDelay(20);
   }
   MediaMovil=EEPROM_ReadByte(454);
   CyDelay(50);   
   if(MediaMovil>80 || MediaMovil<1){
      MediaMovil=1;
      EEPROM_WriteByte(1,454);
      CyDelay(20);
   }
   Coeficiente=EEPROM_ReadByte(455);
   CyDelay(50);   
   if(Coeficiente<100){
      Coeficiente=100;
      EEPROM_WriteByte(100,455);
      CyDelay(20);
   }

   penMufla=EEPROM_ReadByte(458);
   CyDelay(50); 
   if(penMufla>150 || penMufla<10){
      penMufla=50;
      EEPROM_WriteByte(50,458);
      CyDelay(20);
   }
   
   Password[0]=EEPROM_ReadByte(460);
   CyDelay(50);   
   if(Password[0]>9 || Password[0]<0){
      Password[0]=0;
      EEPROM_WriteByte(0,460);
      CyDelay(20);
   }
   Password[1]=EEPROM_ReadByte(461);
   CyDelay(50);   
   if(Password[1]>9 || Password[1]<0){
      Password[1]=0;
      EEPROM_WriteByte(0,461);
      CyDelay(20);
   }
   Password[2]=EEPROM_ReadByte(462);
   CyDelay(50);   
   if(Password[2]>9 || Password[2]<0){
      Password[2]=0;
      EEPROM_WriteByte(0,462);
      CyDelay(20);
   }
   Password[3]=EEPROM_ReadByte(463);
   CyDelay(50);
   if(Password[3]>9 || Password[3]<0){
      Password[3]=0;
      EEPROM_WriteByte(0,463);
      CyDelay(20);
   }

   Tempocp[0]=EEPROM_ReadByte(465);
   CyDelay(50);
   if(Tempocp[0]>9 || Tempocp[0]<0){
      Tempocp[0]=0;
      EEPROM_WriteByte(0,465);
      CyDelay(20);
   } 
   
   Tempocp[1]=EEPROM_ReadByte(466);
   CyDelay(50);
   if(Tempocp[1]>9 || Tempocp[1]<0){
      Tempocp[1]=1;
      EEPROM_WriteByte(1,466);
      CyDelay(20);
   } 

   Tempocp[2]=EEPROM_ReadByte(467);
   CyDelay(50);
   if(Tempocp[2]>9 || Tempocp[2]<0){
      Tempocp[2]=0;
      EEPROM_WriteByte(0,467);
      CyDelay(20);
   } 

   Tempocp[3]=EEPROM_ReadByte(468);
   CyDelay(50);
   if(Tempocp[3]>9 || Tempocp[3]<0){
      Tempocp[3]=0;
      EEPROM_WriteByte(0,468);
      CyDelay(20);
   } 

   Integral=EEPROM_ReadByte(470);
   CyDelay(50); 
   if(Integral>5 || Integral==0){
      Integral=1;
      EEPROM_WriteByte(1,470);
      CyDelay(20);
   }
   Derivativo=EEPROM_ReadByte(471);
   CyDelay(50); 
   if(Derivativo>60 || Derivativo<5){
      Derivativo=30;
      EEPROM_WriteByte(30,471);
      CyDelay(20);
   }
   AlTem=convert16(EEPROM_ReadByte(473),EEPROM_ReadByte(472));
   CyDelay(50); 
   if(AlTem>500){
      AlTem=200;
      EEPROM_WriteByte(200,472);
      CyDelay(20);
      EEPROM_WriteByte(0,473);
      CyDelay(20);
   }
   //Modo=EEPROM_ReadByte(480);
   //CyDelay(50); 
   m_int=EEPROM_ReadByte(490);
   CyDelay(50); 
   m_dec=EEPROM_ReadByte(491);
   CyDelay(50); 
   m=m_int+((float)m_dec/100);
   b16=convert16(EEPROM_ReadByte(493),EEPROM_ReadByte(492));
   b_dec=EEPROM_ReadByte(494);
   CyDelay(50); 
   b=b16+((float)b_dec/100);
   if(b>200){
      b=0;
      EEPROM_WriteByte(0,492);
      CyDelay(10);
      EEPROM_WriteByte(0,493);
      CyDelay(10);
      EEPROM_WriteByte(66,494);
      CyDelay(10);
   }
   if(m<0.1 || m>2.0){
      m=1.0;
      EEPROM_WriteByte(1,490);
      CyDelay(10);
      EEPROM_WriteByte(0,491);
      CyDelay(10);
   }
   
   controlT=EEPROM_ReadByte(495);
   CyDelay(50); 
   if(controlT>1){
      controlT=0;
      EEPROM_WriteByte(0,495);
      CyDelay(20);
   }

   ProgramaSeleccionado=EEPROM_ReadByte(496);
   CyDelay(50); 
   if(ProgramaSeleccionado>4){
      controlT=0;
      EEPROM_WriteByte(0,495);
      ProgramaSeleccionado=0;
      CyDelay(20);
      EEPROM_WriteByte(0,496);
      CyDelay(20);
   }

   Segmento=EEPROM_ReadByte(497);
   CyDelay(50); 
   if(Segmento>Pasos){
      controlT=0;
      EEPROM_WriteByte(0,495);
      CyDelay(20);
      ProgramaSeleccionado=0;
      EEPROM_WriteByte(0,496);
      CyDelay(20);
      Segmento=0;
      EEPROM_WriteByte(0,497);
      CyDelay(20);
   }

   control1=EEPROM_ReadByte(498);
   CyDelay(50);    
   control2=convert16(EEPROM_ReadByte(500),EEPROM_ReadByte(499));
   CyDelay(50); 
   control3=convert16(EEPROM_ReadByte(502),EEPROM_ReadByte(501));
   CyDelay(50); 
   patron1=EEPROM_ReadByte(503);
   CyDelay(50); 
   patron2=convert16(EEPROM_ReadByte(505),EEPROM_ReadByte(504));
   CyDelay(50); 
   patron3=convert16(EEPROM_ReadByte(507),EEPROM_ReadByte(506));
   CyDelay(50); 
   
   if(control1>100 || control1<20){
      control1=20;
      EEPROM_WriteByte(20,498);
      CyDelay(20);
   }
   if(patron1>100 || patron1<20){
      patron1=20;
      EEPROM_WriteByte(20,503);
      CyDelay(20);
   }
   if(control2>600 || control2<100){
      control2=600;
      EEPROM_WriteByte(0x58,499);
      CyDelay(20);
      EEPROM_WriteByte(0x02,500);
      CyDelay(20);
   }
   if(control3>1200 || control3<600){
      control3=1200;
      EEPROM_WriteByte(0xB0,501);
      CyDelay(20);
      EEPROM_WriteByte(0x04,502);
      CyDelay(20);
   }
   if(patron2>600 || patron2<100){
      patron2=600;
      EEPROM_WriteByte(0x58,504);
      CyDelay(20);
      EEPROM_WriteByte(0x02,505);
      CyDelay(20);
   }
   if(patron3>1200 || patron3<600){
      patron3=1200;
      EEPROM_WriteByte(0xB0,506);
      CyDelay(20);
      EEPROM_WriteByte(0x04,507);
      CyDelay(20);
   }
}

short PidePassword(uint8 MenuAnt, uint8 MenuActual, uint8 MenuPost, uint8 clave0,uint8 clave1, uint8 clave2,uint8 clave3){
      MenuActual=0;
      if(n==0){
         displayContrasena(Contrasena[0],20,30,1);displayContrasena(Contrasena[1],40,30,0);
         displayContrasena(Contrasena[2],60,30,0);displayContrasena(Contrasena[0],80,30,0);
         strcpy(Menus.Letra,"Clave");
         displayMenu(Menus.Letra,20,0,1,3);
         n=1;
      }
      
      if(UP_Read()==0){
         if(Flanco == 0){
            Flanco = 1;CyDelay(200);
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
            
      if(DOWN_Read()==0){
         if(Flanco2 == 0){
            Flanco2 = 1;CyDelay(200);
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
            
      if(RIGHT_Read()==0){
         if(Flanco1 == 0){
            Flanco1 = 1;CyDelay(200);unidad++;
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
      
      if(LEFT_Read()==0){
         CyDelay(100);
         if(LEFT_Read()==0){
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
            EEPROM_WriteByte(0,0);CyDelay(20);EEPROM_WriteByte(1,0);CyDelay(20);// Reestablece a contraseña de Fabrica y reinicia Programa.
            EEPROM_WriteByte(2,0);CyDelay(20);EEPROM_WriteByte(3,0);CyDelay(20);
            reset_cpu();
         }
      */
         if((Contrasena[0]==clave0)&&(Contrasena[1]==clave1)&&(Contrasena[2]==clave2)&&(Contrasena[3]==clave3)){
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra,"Clave");
            displayMenu(Menus.Letra,30,0,0,2);
            strcpy(Menus.Letra,"Correcta");
            displayMenu(Menus.Letra,15,30,0,2);
            Menu=MenuPost;glcd_update();CyDelay(1000);glcd_fillScreen(OFF);
            Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;
            n=0;
            return 1;
         }else{
            glcd_fillScreen(OFF);
            strcpy(Menus.Letra,"Clave");
            displayMenu(Menus.Letra,30,0,0,2);
            strcpy(Menus.Letra,"Incorrecta");
            displayMenu(Menus.Letra,0,30,0,2);
            unidad=1;glcd_update();CyDelay(1000);Contrasena[0]=0;Contrasena[1]=0;Contrasena[2]=0;Contrasena[3]=0;glcd_fillScreen(OFF);
            glcd_rect(0, 0, 127, 25, YES, ON);strcpy(Menus.Letra,"Clave");glcd_text57(25, 1, Menus.Letra, 3, OFF);displayContrasena(Contrasena[0],20,30,1);
            displayContrasena(Contrasena[1],40,30,0);displayContrasena(Contrasena[2],60,30,0);displayContrasena(Contrasena[0],80,30,0);
            Menu=MenuAnt;glcd_fillScreen(OFF);n=0;
            return 0;
         }
      }else{
        return 0;
      }
}

void MuestraSegmento(uint8 Opcio,uint8 l,uint8 Prog ){
         if(Temperatura1[l+((Prog-1)*Pasos)]>1200)
            Temperatura1[l+((Prog-1)*Pasos)]=0;
         
         if(Temperatura1[l+((Prog-1)*Pasos)]<0)
            Temperatura1[l+((Prog-1)*Pasos)]=1200;
         
         if(Temperatura1[l+((Prog-1)*Pasos)]>=1100)
            Horas1[l+((Prog-1)*Pasos)]=0;
         
         //if(!refresh){   
            strcpy(Menus.Letra,"P 0");
            displayMenu(Menus.Letra,30,0,1,2);
            displayTiempo(Prog,68,0,1,2);
             
            strcpy(Menus.Letra,"Segmento ");
            displayMenu(Menus.Letra,0,20,1,1);
            displayTiempo(l,63,20,1,1);
             
            strcpy(Menus.Letra,"TA:");
            displayMenu(Menus.Letra,80,20,3,1);
          //  refresh=ON;
        //}
         
         if(l>0)
            displayTemperatura(Temperatura1[l-1+((Prog-1)*Pasos)],100,20,1,1);
         
         if(Opcio==1){            
            //if(!refresh2){
                strcpy(Menus.Letra,"Temperatura:");
                displayTemps(Menus.Letra,Temperatura1[l+((Prog-1)*Pasos)],0,32,1,1);
                strcpy(Menus.Letra,"Horas:");
                displayTemps(Menus.Letra,Horas1[l+((Prog-1)*Pasos)],0,44,10,1);
                strcpy(Menus.Letra,"Minutos:");
                displayTemps(Menus.Letra,Minutos1[l+((Prog-1)*Pasos)],0,55,10,1);
                refresh2=ON;
            //}
            
               if(!x10){
                    if(tup>30){
                        BUZZER_Write(1);CyDelay(5);BUZZER_Write(0);
                        Temperatura1[l+((Prog-1)*Pasos)]++;
                        if(Temperatura1[l+((Prog-1)*Pasos)]%10==0){
                            x10=ON;
                        }
                        
                        if(l<Pasos-1)
                            Temperatura1[l+1+((Prog-1)*Pasos)]=Temperatura1[l+((Prog-1)*Pasos)]; 
                    }else if(tdown>30){
                        BUZZER_Write(1);CyDelay(5);BUZZER_Write(0);
                        Temperatura1[l+((Prog-1)*Pasos)]--;
                        if(Temperatura1[l+((Prog-1)*Pasos)]%10==0){
                            x10=ON;
                        }
                        if(l<Pasos-1)
                            Temperatura1[l+1+((Prog-1)*Pasos)]=Temperatura1[l+((Prog-1)*Pasos)]; 
                    }else{
                        x10=OFF;
                    }
                }else{
                    if(tup>30){
                        if(conteoTecla<=100)
                            conteoTecla++;
                    }else if(tdown>30){
                        if(conteoTecla<=100)
                            conteoTecla++;
                    }else{
                        conteoTecla=0;
                        x10=OFF;
                    }
             
                    if(conteoTecla>4){
                        if(tup>30){                            
                            BUZZER_Write(1);CyDelay(5);BUZZER_Write(0);
                            Temperatura1[l+((Prog-1)*Pasos)]+=10;
                            if(l<Pasos-1)
                                Temperatura1[l+1+((Prog-1)*Pasos)]=Temperatura1[l+((Prog-1)*Pasos)]; 
                        }else if(tdown>30){
                            BUZZER_Write(1);CyDelay(5);BUZZER_Write(0);
                            Temperatura1[l+((Prog-1)*Pasos)]-=10;
                            if(l<Pasos-1)
                                Temperatura1[l+1+((Prog-1)*Pasos)]=Temperatura1[l+((Prog-1)*Pasos)]; 
                        }else{
                            x10=OFF;
                        }
                    }
                }
         }
         
         if(Opcio==2){
            //if(!refresh2){
                strcpy(Menus.Letra,"Temperatura:");
                displayTemps(Menus.Letra,Temperatura1[l+((Prog-1)*Pasos)],0,32,0,1);
                strcpy(Menus.Letra,"Horas:");
                displayTemps(Menus.Letra,Horas1[l+((Prog-1)*Pasos)],0,44,11,1);
                strcpy(Menus.Letra,"Minutos:");
                displayTemps(Menus.Letra,Minutos1[l+((Prog-1)*Pasos)],0,55,10,1);
                refresh2=ON;
            //}
            
                if(!x10){
                    if(tup>30){
                        BUZZER_Write(1);CyDelay(5);BUZZER_Write(0);
                        Horas1[l+((Prog-1)*Pasos)]++;
                        if(Horas1[l+((Prog-1)*Pasos)]%10==0){
                            x10=ON;
                        }
                    }else if(tdown>30){
                        BUZZER_Write(1);CyDelay(5);BUZZER_Write(0);
                        Horas1[l+((Prog-1)*Pasos)]--;
                        if(Horas1[l+((Prog-1)*Pasos)]%10==0){
                            x10=ON;
                        }
                    }else{
                        x10=OFF;
                    }
                }else{
                    if(tup>30){
                        if(conteoTecla<=100)
                            conteoTecla++;
                    }else if(tdown>30){
                        if(conteoTecla<=100)
                            conteoTecla++;
                    }else{
                        conteoTecla=0;
                        x10=OFF;
                    }
             
                    if(conteoTecla>4){
                        if(tup>30){                            
                            BUZZER_Write(1);CyDelay(5);BUZZER_Write(0);
                            Horas1[l+((Prog-1)*Pasos)]+=10;
                        }else if(tdown>30){
                            BUZZER_Write(1);CyDelay(5);BUZZER_Write(0);
                            Horas1[l+((Prog-1)*Pasos)]-=10;
                        }else{
                            x10=OFF;
                        }
                    }
                }
            
            if(Horas1[l+((Prog-1)*Pasos)]>99)
                  Horas1[l+((Prog-1)*Pasos)]=0;
                
            if(Horas1[l+((Prog-1)*Pasos)]<0)
                  Horas1[l+((Prog-1)*Pasos)]=99;    
         }
         
         if(Opcio==3){
            //if(!refresh2){
                strcpy(Menus.Letra,"Temperatura:");
                displayTemps(Menus.Letra,Temperatura1[l+((Prog-1)*Pasos)],0,32,0,1);
                strcpy(Menus.Letra,"Horas:");
                displayTemps(Menus.Letra,Horas1[l+((Prog-1)*Pasos)],0,44,10,1);
                strcpy(Menus.Letra,"Minutos:");
                displayTemps(Menus.Letra,Minutos1[l+((Prog-1)*Pasos)],0,55,11,1);
                refresh2=ON;
            //}
            
                if(!x10){
                    if(tup>30){
                        BUZZER_Write(1);CyDelay(5);BUZZER_Write(0);
                        Minutos1[l+((Prog-1)*Pasos)]++;
                        if(Minutos1[l+((Prog-1)*Pasos)]%10==0){
                            x10=ON;
                        }
                    }else if(tdown>30){
                        BUZZER_Write(1);CyDelay(5);BUZZER_Write(0);
                        Minutos1[l+((Prog-1)*Pasos)]--;
                        if(Minutos1[l+((Prog-1)*Pasos)]%10==0){
                            x10=ON;
                        }
                    }else{
                        x10=OFF;
                    }
                }else{
                    if(tup>30){
                        if(conteoTecla<=100)
                            conteoTecla++;
                    }else if(tdown>30){
                        if(conteoTecla<=100)
                            conteoTecla++;
                    }else{
                        conteoTecla=0;
                        x10=OFF;
                    }
             
                    if(conteoTecla>4){
                        if(tup>30){                            
                            BUZZER_Write(1);CyDelay(5);BUZZER_Write(0);
                            Minutos1[l+((Prog-1)*Pasos)]+=10;
                        }else if(tdown>30){
                            BUZZER_Write(1);CyDelay(5);BUZZER_Write(0);
                            Minutos1[l+((Prog-1)*Pasos)]-=10;
                        }else{
                            x10=OFF;
                        }
                    }
                }
                
            if(Minutos1[l+((Prog-1)*Pasos)]>59)
                  Minutos1[l+((Prog-1)*Pasos)]=0;
            if(Minutos1[l+((Prog-1)*Pasos)]<0)
                  Minutos1[l+((Prog-1)*Pasos)]=59;    
         }
}


/* [] END OF FILE */
