#if !defined(KS0108_H_)
#define KS0108_H_

#include <device.h>
#include <stdio.h>
#include <main.h>
#include "project.h"
    
		
#define SET_PAGE_NO(pageNo)         			  vLcdComand(0xB8 | pageNo)
#define SET_COL_NO(colNo)            			  vLcdComand(0x40 | colNo)
#define SET_Z_ADDR(zPage)          	   			vLcdComand(0xC0 | zPage)    


#define LCD_CHIP_DESELECT_1()					CS_1_Write(LOW);
#define LCD_CHIP_DESELECT_2()					CS_2_Write(LOW);

#define LCD_CHIP_SELECT_1()						CS_1_Write(HIGH);
												

#define LCD_CHIP_SELECT_2()						CS_2_Write(HIGH);
												
                                                    
#define LCD_SELECT_BOTH_CHIPS()       			CS_1_Write(HIGH);\
												CS_2_Write(HIGH);
										
#define WRITE_DATA() 							D_I_Write(HIGH);
											
#define WRITE_INSTRUCTION()						D_I_Write(LOW);

#define READ_WRITE_ON()                         R_W_Write(HIGH);
#define READ_WRITE_OFF()                        R_W_Write(LOW);
    
#define ENABLE_HIGH()							ENABLE_Write(HIGH);
#define ENABLE_LOW()							ENABLE_Write(LOW);

#define DATA_DELAY                              3

#define DISPLAY_ON								0x3F
#define DISPLAY_OFF								0x3E

#define HIGH								    1
#define LOW								        0
    
#define ON								        1
#define OFF								        0

#define YES								        1
#define NO								        0
    
#define NO_OF_LCD_ROWS                			64
#define GLCD_WIDTH               			128

#define GLCD_LEFT    0
#define GLCD_RIGHT   1

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) ((bitvalue) ? bitSet(value, bit) : bitClear(value, bit))
        
/////////////////////////////////////////////////////////////////////////
// Function Prototypes
/////////////////////////////////////////////////////////////////////////
uint8 vLcdDataRead();
void vLcdDataWrite(uint8_t ui8Value);
void glcd_init(short mode);
void glcd_pixel(uint8 x, uint8 y, short color);
void glcd_pixel(uint8 x, uint8 y, short color);
void glcd_line(uint8 x1, uint8 y1, uint8 x2, uint8 y2, short color);
void glcd_rect(uint8 x1, uint8 y1, uint8 x2, uint8 y2, short fill, short color);
void glcd_bar(uint8 x1, uint8 y1, uint8 x2, uint8 y2, uint8 width, short color);
void glcd_text57(uint8 x, uint8 y, char* textptr, uint8 size, short color);
void displayMenu(char palabra[30], uint8 x, uint8 y, short sombreado, short tamano);
void displayTemps(char palabra[30], uint16 digito, uint8 x, uint8 y, short sombreado, short tamano);
void displayTiempo(uint8 digito, uint8 x, uint8 y, short sombreado, short tamano);
void displayTemperatura(int16 digito, uint8 x, uint8 y, short sombreado, short tamano);
void glcd_circle(uint8 x, uint8 y, uint8 radius, short fill, short color);
void glcd_imagen(uint8 x);
void Limpia(uint8 x1,uint8 x2,uint8 y);
void displayT(uint16 adc,uint8 x,uint8 y,uint8 w);
void displayFloat(float adc,uint8 x,uint8 y,uint8 w);
void displayContrasena(uint8 digito, uint8 x, uint8 y, uint8 sombreado);
void displayTiempos(uint8 digito, uint8 x, uint8 y, uint8 sombreado, uint8 tamano);

void glcd_fillScreen(short color);
void glcd_writeByte(short side, uint8 data);
uint8 glcd_readByte(short side);
void glcd_update();
/////////////////////////////////////////////////////////////////////////

struct
{
   uint8 left[512];
   uint8 right[512];
} displayData;

#endif
