/*
 * Graphics-LCD KS0108 with PSoC5
 * by EA4FRB - Melchor Varela, Madrid, Spain. <melchor.varela@gmail.com>
 *
 * Code ported from Graphics-LCD KS0108 with LPC2106 
 * by Martin THOMAS <eversmith@heizung-thomas.de>
 *
 * - text-output based on code from a 8051-sample-application by Michael J. Karas
 * - graphics-functions (line/circle) from the "ape" AVR-ks0108-library
 */

/*
The following license is for all parts of this code done by 
Martin Thomas. Code from others used here may have other license terms.

Copyright (C) 2004 Martin THOMAS

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

! The above copyright notice and this permission notice shall be included in all
! copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
//*****************************************************************************/
//
//	PROJECT:	GLCD Library
// 	FILE NAME: 	glcd-ks0108-psoc5.c
// 	AUTHOR:		EA4FRB - Melchor Varela
//
// 	DESCRIPTION
//	
//	Graphics-LCD KS0108 with PSoC5
//
// 	HISTORY
//
//	NAME   	DATE		REMARKS
//
//	MVM	   	July 2010	Creation
//
//*****************************************************************************/
/*
PSoC5 Port Description:

This port has been prepared for the PSoC5 FirstTouch StarterKit. The display is 
connected to the kit using the expansion board. Since the display is powered at 5V 
and the PSoC5 is operating at 3.3V and the I/O ports are not 5V compatible, it has been
decided using the display in write mode only and perform the busy check with a
timer. Therefore the display R/W is cabled to '0' level.

Hardware resources:

- Timer_LCD: Timer 16bit operating @ 24 Mhz in one-shot mode
- LCDC: Digital output pins block for display control 
- LCDD: Digital output pins block for display data 
- LCD_DATA: Control register connected to LCDD for display data

Pin Mapping:
	- CS1	P2_7
	- CS2	P2_6
	- RST	P6_7
	- E		P6_5
	- DI	P6_6
	- D0	P6_3
	- D1	P6_2
	- D2	P6_1
	- D3	P6_0
	- D4	P4_7
	- D5	P4_6
	- D6	P4_5
	- D7	P4_4
	- R/W	"0"
*/

//#include <device.h>

//#include "glcd.h"
//#include "fonts.h"

#include "LCD128x64_1.h"
#include <device.h>



//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
#define GLCD_MODECMD  0
#define GLCD_MODEDATA 1

#define GLCD_ON				0x3F
#define GLCD_DISP_START0	0xC0
#define GLCD_SET_PAGE       0xB8
#define GLCD_SET_ADDRESS	0x40

#define GLCD_X_BYTES 	(GLCD_NUMCONTROLLERX*GLCD_MAXADDRESS)
#define GLCD_Y_BYTES 	(GLCD_NUMCONTROLLERY*GLCD_MAXPAGE)

#define GLCD_SELECTNONE		99

//#define TIMER_LCD_PERIOD_NS	(1000000000/24000000)
#define TIMER_LCD_PERIOD_NS	(1000000000/24000000)
#define NS_TO_COUNT(ns)		((ns)/TIMER_LCD_PERIOD_NS)			

//#define	BUSY_TIME_NS		1000	//1uS
#define	BUSY_TIME_NS		1000	//1uS
//#define E_WIDTH_NS			450
//#define ADDR_SETUP_NS		140
#define E_WIDTH_NS			450
#define ADDR_SETUP_NS		140
//-----------------------------------------------------------------------------
//  Locals
//-----------------------------------------------------------------------------
static uint8 gGLCD_inverted;

static uint8 gGLCD_buffer[GLCD_Y_BYTES][GLCD_X_BYTES];

/* from MJK: pixel level bit masks for display */
/* this array is setup to map the order */
/* of bits in a byte to the vertical order */
/* of bits at the LCD controller */
/* TODO: use somekind of (1<<n) macro */
static const uint8 GLCD_MaskArray[8] =
         {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

//-----------------------------------------------------------------------------
//  Prototypes
//-----------------------------------------------------------------------------
static void LCD128x64_1_SetupIO(void);
static void LCD128x64_1_TimerWait(void);
static void LCD128x64_1_TimerSet(uint16 count);
static void LCD128x64_1_SetPage(uint8 page);
static void LCD128x64_1_SetAddress(uint8 address);
static void LCD128x64_1_SelectChip(uint8 chip);
static void LCD128x64_1_WriteDataCmd(uint8 byte, uint8 mode);
static void LCD128x64_1_WriteCommand(uint8 byte);
static void LCD128x64_1_WriteData(uint8 byte);

static void LCD128x64_1_FillBuffer(uint8 pattern);

static void LCD128x64_1_Glyph(uint8 left, uint8 top,
			   uint8 width, uint8 height,
			   const uint8 *glyph, uint8 store_width);

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_SetupIO
//
//  DESCRIPTION:
//
//	Initializes I/O ports
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void LCD128x64_1_SetupIO(void)
{
	LCD128x64_1_LCD_DATA_Write(0);
	
	CyPins_ClearPin(LCD128x64_1_LCDC_E);
	CyPins_ClearPin(LCD128x64_1_LCDC_CS1);
	CyPins_ClearPin(LCD128x64_1_LCDC_CS2);
	CyPins_ClearPin(LCD128x64_1_LCDC_DI);

	CyPins_ClearPin(LCD128x64_1_LCDC_RST);
	CyPins_SetPin(LCD128x64_1_LCDC_RST);
	
	LCD128x64_1_Timer_LCD_Start();
	LCD128x64_1_TimerSet(NS_TO_COUNT(BUSY_TIME_NS));
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_SelectChip
//
//  DESCRIPTION:
//
//	Selects controller
//
//  ARGUMENTS:
//     chip		{0,1,GLCD_SELECTNONE} 
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void LCD128x64_1_SelectChip(uint8 chip)
{
	switch (chip) {
		case 0 : 
			CyPins_ClearPin(LCD128x64_1_LCDC_CS1);
			CyPins_SetPin(LCD128x64_1_LCDC_CS2);
			break;
		case 1:
			CyPins_SetPin(LCD128x64_1_LCDC_CS1);
			CyPins_ClearPin(LCD128x64_1_LCDC_CS2);
			break;
		case GLCD_SELECTNONE:
			CyPins_ClearPin(LCD128x64_1_LCDC_CS1);
			CyPins_ClearPin(LCD128x64_1_LCDC_CS2);
			break;
		default: 
			break;
	}
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_TimerWait
//
//  DESCRIPTION:
//
//	Waits for timer end count
//
//  ARGUMENTS:
//     none.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void LCD128x64_1_TimerWait(void)
{
	while (LCD128x64_1_Timer_LCD_ReadCounter()!=0)
	{;}
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_TimerSet
//
//  DESCRIPTION:
//
//	Sets timer count
//
//  ARGUMENTS:
//     count	number of counts (clock @ 24Mhz)
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void LCD128x64_1_TimerSet(uint16 count)
{
	LCD128x64_1_Timer_LCD_WriteCounter(count+1);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_WriteDataCmd
//
//  DESCRIPTION:
//
//	Writes to display controller
//
//  ARGUMENTS:
//     	datacmd		data or command
//		mode		{GLCD_MODECMD, GLCD_MODEDATA}
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void LCD128x64_1_WriteDataCmd(uint8 datacmd, uint8 mode)
{				
	LCD128x64_1_TimerWait();
	
	if (mode == GLCD_MODECMD)
		CyPins_ClearPin(LCD128x64_1_LCDC_DI);	// DI to 0 (command)																							
	else
		CyPins_SetPin(LCD128x64_1_LCDC_DI);		// DI to 1 (data)																							

									// Address setup time
	LCD128x64_1_TimerSet(NS_TO_COUNT(ADDR_SETUP_NS));
	LCD128x64_1_TimerWait();

	LCD128x64_1_LCD_DATA_Write(datacmd);
	
	CyPins_SetPin(LCD128x64_1_LCDC_E);
									// Need E high for at least 450ns
	LCD128x64_1_TimerSet(NS_TO_COUNT(E_WIDTH_NS));
	LCD128x64_1_TimerWait();
	CyPins_ClearPin(LCD128x64_1_LCDC_E);
									// Starts busy wait time
	LCD128x64_1_TimerSet(NS_TO_COUNT(BUSY_TIME_NS));
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_WriteCommand
//
//  DESCRIPTION:
//
//	Writes command to display controller
//
//  ARGUMENTS:
//     	byte		command
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void LCD128x64_1_WriteCommand(uint8 byte)
{
	LCD128x64_1_WriteDataCmd(byte, GLCD_MODECMD);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_WriteData
//
//  DESCRIPTION:
//
//	Writes data to display controller
//
//  ARGUMENTS:
//     	byte		data
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void LCD128x64_1_WriteData(uint8 byte)
{
	LCD128x64_1_WriteDataCmd(byte, GLCD_MODEDATA);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_SetPage
//
//  DESCRIPTION:
//
//	Sets display page in display controller
//
//  ARGUMENTS:
//     	page		
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void LCD128x64_1_SetPage(uint8 page)
{
	LCD128x64_1_WriteCommand(GLCD_SET_PAGE+page);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_SetAddress
//
//  DESCRIPTION:
//
//	Sets display address in display controller
//
//  ARGUMENTS:
//     	address		
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void LCD128x64_1_SetAddress(uint8 address)
{
	LCD128x64_1_WriteCommand(GLCD_SET_ADDRESS+address);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_Init
//
//  DESCRIPTION:
//
//	Initializes display. 
//
//  ARGUMENTS:
//     none.		
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void LCD128x64_1_Init(void)
{	
	uint8 i;
	
	LCD128x64_1_SetupIO();
	
	for (i=0;i<GLCD_NUMCONTROLLER;i++) {
		LCD128x64_1_SelectChip(i);
		LCD128x64_1_WriteCommand(GLCD_ON); 
		LCD128x64_1_WriteCommand(GLCD_DISP_START0);
		// GLCD_SetPage(0);
		// GLCD_SetAddress(0);
	}
}
//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_SetInvert
//
//  DESCRIPTION:
//
//	Sets normal or inverse mode
//
//  ARGUMENTS:
//     mode		{GLCD_MODE_INVERT,GLCD_MODE_NORMAL}
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void LCD128x64_1_SetInvert(uint8 mode)
{
	gGLCD_inverted = mode;
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_FillBuffer
//
//  DESCRIPTION:
//
//	Fills memory buffer with specified pattern
//
//  ARGUMENTS:
//     pattern		
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
static void LCD128x64_1_FillBuffer(uint8 pattern)
{
	uint16 x,y;
	
	for (y=0;y<GLCD_Y_BYTES;y++)
		for (x=0;x<GLCD_X_BYTES;x++) 
			gGLCD_buffer[y][x]=pattern;
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_ClrScr
//
//  DESCRIPTION:
//
//	Clear memory buffer and optionally flushes to display
//
//  ARGUMENTS:
//     bufferAndDisplay		true to flush display
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void LCD128x64_1_ClrScr(uint8 bufferAndDisplay)
{
	LCD128x64_1_FillBuffer(0x00);
	if ( bufferAndDisplay ) 
		LCD128x64_1_UpdateAll();
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_Update
//
//  DESCRIPTION:
//
//	Copy buffer-RAM to display-RAM
//
//  ARGUMENTS:
//      top     top line of area to update.         
//		bottom  bottom line of area to update.
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void LCD128x64_1_Update(uint16 top, uint16 bottom)
{
	uint8 x, y, i;
	uint8 yt, yb;
	uint16 xoffset;
	uint8 data;
	
	yt = top >> 3;				/* setup bytes of range */
	yb = bottom >> 3;

	for (i=0;i<GLCD_NUMCONTROLLER;i++) {
		LCD128x64_1_SelectChip(i);
		for (y=yt;y<=yb;y++) {
			LCD128x64_1_SetPage(y);
			LCD128x64_1_SetAddress(0);
			xoffset = i * GLCD_MAXADDRESS;
			for (x=0;x<GLCD_MAXADDRESS;x++) {
				data = gGLCD_buffer[y][xoffset+x];
				if (gGLCD_inverted) data=~data;
				LCD128x64_1_WriteData(data);
			}
		}
	}
	LCD128x64_1_SelectChip(GLCD_SELECTNONE);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_Dot
//
//  DESCRIPTION:
//
//	Change pixel state in memory buffer
//
//  ARGUMENTS:
//      x     	
//		y		
//		mode	{GLCD_MODE_CLEAR,GLCD_MODE_SET,GLCD_MODE_XOR}
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void LCD128x64_1_Dot(uint8 x, uint8 y, uint8 mode) {
// here 2*1 setup - TODO: for 2*2 setting
	uint8 bitnum, bitmask, yByte;
	
	if ( (x>=GLCD_RIGHT) || (y>=GLCD_BOTTOM) ) return;
	
	yByte   = y >> 3; // / 8
	bitnum  = y & 0x07;
	bitmask = GLCD_MaskArray[bitnum];
	switch (mode) {
		case GLCD_MODE_SET:
			gGLCD_buffer[yByte][x] |= bitmask;
			break;
		case GLCD_MODE_CLEAR:
			gGLCD_buffer[yByte][x] &= ~bitmask;
			break;
		case GLCD_MODE_XOR:
			gGLCD_buffer[yByte][x] ^= bitmask;
			break;
		default: break;
	}
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_Line
//
//  DESCRIPTION:
//
//	Draws line in memory buffer
//
//  ARGUMENTS:
//      x1     	
//		y1		
//      x2     	
//		y2		
//		mode	{GLCD_MODE_CLEAR,GLCD_MODE_SET,GLCD_MODE_XOR}
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void LCD128x64_1_Line( uint8 x1, uint8 y1, 
		uint8 x2, uint8 y2,
		uint8 mode ) 
{
	uint8 length, xTmp, yTmp, i, y, yAlt;
	int16 m;
	
	if(x1 == x2) {									// vertical line
		// x1|y1 must be the upper point
		if(y1 > y2) {
			xTmp = x1;
			yTmp = y1;
			x1 = x2;
			y1 = y2;
			x2 = xTmp;
			y2 = yTmp;
		}
		
		length = y2-y1;
		for(i=0; i<=length; i++) {
			LCD128x64_1_Dot(x1, y1+i, mode);
		}
	} else if(y1 == y2) {							// horizontal line
		// x1|y1 must be the left point
		if(x1 > x2) {
			xTmp = x1;
			yTmp = y1;
			x1 = x2;
			y1 = y2;
			x2 = xTmp;
			y2 = yTmp;
		}
		
		length = x2-x1;
		for(i=0; i<=length; i++) {
			LCD128x64_1_Dot(x1+i, y1, mode);
		}
	} else {
		// x1 must be smaller than x2
		if(x1 > x2) {
			xTmp = x1;
			yTmp = y1;
			x1 = x2;
			y1 = y2;
			x2 = xTmp;
			y2 = yTmp;
		}
		
		if((y2-y1) >= (x2-x1) || (y1-y2) >= (x2-x1)) {	// angle larger or equal 45?
			length = x2-x1;								// not really the length :)
			m = ((y2-y1)*200)/length;
			yAlt = y1;
			for(i=0; i<=length; i++) {
				y = ((m*i)/200)+y1;
				
				if((m*i)%200 >= 100)
					y++;
				else if((m*i)%200 <= -100)
					y--;
				
				LCD128x64_1_Line(x1+i, yAlt, x1+i, y, mode ); /* wuff wuff recurs. */
				if(length<=(y2-y1) && y1 < y2)
					yAlt = y+1;
				else if(length<=(y1-y2) && y1 > y2)
					yAlt = y-1;
				else
					yAlt = y;
			}
		} else {										// angle smaller 45?
			// y1 must be smaller than y2
			if(y1 > y2) {
				xTmp = x1;
				yTmp = y1;
				x1 = x2;
				y1 = y2;
				x2 = xTmp;
				y2 = yTmp;
			}
			length = y2-y1;
			m = ((x2-x1)*200)/length;
			yAlt = x1;
			for(i=0; i<=length; i++) {
				y = ((m*i)/200)+x1;
				
				if((m*i)%200 >= 100)
					y++;
				else if((m*i)%200 <= -100)
					y--;
				
				LCD128x64_1_Line(yAlt, y1+i, y, y1+i, mode); /* wuff */
				if(length<=(x2-x1) && x1 < x2)
					yAlt = y+1;
				else if(length<=(x1-x2) && x1 > x2)
					yAlt = y-1;
				else
					yAlt = y;
			}
		}
	}
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_Rect
//
//  DESCRIPTION:
//
//	Draws rectangle in memory buffer
//
//  ARGUMENTS:
//      x     	
//		y		
//      width  	
//		height
//		mode	{GLCD_MODE_CLEAR,GLCD_MODE_SET,GLCD_MODE_XOR}
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void LCD128x64_1_Rect(uint8 x, uint8 y, uint8 width, uint8 height,
		uint8 mode) 
{
	width--;
	height--;
	LCD128x64_1_Line(x, y, x+width, y, mode);		// top
	LCD128x64_1_Line(x, y, x, y+height, mode);	// left
	LCD128x64_1_Line(x, y+height, x+width, y+height, mode);	// bottom
	LCD128x64_1_Line(x+width, y, x+width, y+height, mode);	// right
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_Box
//
//  DESCRIPTION:
//
//	Draws box in memory buffer
//
//  ARGUMENTS:
//      x     
//		y	
//      width  	
//		height	
//		mode	{GLCD_MODE_CLEAR,GLCD_MODE_SET,GLCD_MODE_XOR}
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void LCD128x64_1_Box(uint8 x, uint8 y, uint8 width, uint8 height,
		uint8 mode) 
{
	uint8 i;
	
	if (!width) return; 
	
	width--;
	
	for (i=y;i<y+height;i++) 
		LCD128x64_1_Line(x, i, x+width, i, mode);
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_Circle
//
//  DESCRIPTION:
//
//	
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void LCD128x64_1_Circle(uint8 xCenter, uint8 yCenter, 
		uint8 radius, uint8 mode) 
{
  	int16 tSwitch, y, x = 0;
  	uint8 d;

  	d = yCenter - xCenter;
  	y = radius;
  	tSwitch = 3 - 2 * radius;
  	
  	while (x <= y) {
    	LCD128x64_1_Dot(xCenter + x, yCenter + y, mode);
    	LCD128x64_1_Dot(xCenter + x, yCenter - y, mode);
    	
    	LCD128x64_1_Dot(xCenter - x, yCenter + y, mode);
    	LCD128x64_1_Dot(xCenter - x, yCenter - y, mode);
    	
    	LCD128x64_1_Dot(yCenter + y - d, yCenter + x, mode);
    	LCD128x64_1_Dot(yCenter + y - d, yCenter - x, mode);
    	
    	LCD128x64_1_Dot(yCenter - y - d, yCenter + x, mode);
    	LCD128x64_1_Dot(yCenter - y - d, yCenter - x, mode);

    	if (tSwitch < 0) {
    		tSwitch += (4 * x + 6);
    	} else {
      		tSwitch += (4 * (x - y) + 10);
      		y--;
    	}
    	x++;
  	}
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_Glyph
//
//  DESCRIPTION:
//
//	Writes a glyph("letter") to the display at location x,y
//	(adapted function from the MJK-code)
//
//  ARGUMENTS:
//      column    - x coordinate of the left part of glyph          
//		row       - y coordinate of the top part of glyph       
//      width  	  - size in pixels of the width of the glyph    
//		height 	  - size in pixels of the height of the glyph   
//		glyph     - an unsigned char pointer to the glyph pixels 
//        			to write assumed to be of length "width"
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void LCD128x64_1_Glyph(uint8 left, uint8 top,
			   uint8 width, uint8 height,
			   const uint8 *glyph, uint8 store_width)
{
	uint8 bit_pos;
	uint8 byte_offset;
	uint8 y_bits;
	uint8 remaining_bits;
	uint8 mask;
	uint8 char_mask;
	uint8 x;
	const uint8 *glyph_scan;
	uint8 glyph_offset;

  	bit_pos = top & 0x07;		/* get the bit offset into a byte */

	glyph_offset = 0;			/* start at left side of the glyph rasters */
    char_mask = 0x80;			/* initial character glyph mask */

  	for (x = left; x < (left + width); x++)
  	{
    	byte_offset = top >> 3;        	/* get the byte offset into y direction */
		y_bits = height;				/* get length in y direction to write */
		remaining_bits = 8 - bit_pos;	/* number of bits left in byte */
		mask = GLCD_MaskArray[bit_pos];	/* get mask for this bit */
		glyph_scan = glyph + glyph_offset;	 /* point to base of the glyph */

    	/* boundary checking here to account for the possibility of  */
    	/* write past the bottom of the screen.                        */
    	while((y_bits) && (byte_offset < GLCD_Y_BYTES)) /* while there are bits still to write */
    	{
			/* check if the character pixel is set or not */
			if(*glyph_scan & char_mask)
			{
				gGLCD_buffer[byte_offset][x] |= mask;	/* set image pixel */
			}
			else
			{
      			gGLCD_buffer[byte_offset][x] &= ~mask;	/* clear the image pixel */
			}

			if(GLCD_MaskArray[0] & 0x80)
			{
				mask >>= 1;
			}
			else
			{
				mask <<= 1;
			}
			
			y_bits--;
			remaining_bits--;
      		if(remaining_bits == 0)
      		{
				/* just crossed over a byte boundry, reset byte counts */
				remaining_bits = 8;
				byte_offset++;
				mask = GLCD_MaskArray[0];
      		}

			/* bump the glyph scan to next raster */
			glyph_scan += store_width;
		}

		/* shift over to next glyph bit */
		char_mask >>= 1;
		if(char_mask == 0)				/* reset for next byte in raster */
		{
			char_mask = 0x80;
			glyph_offset++;
	    }
	}
}
//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_Text
//
//  DESCRIPTION:
//
//	 Prints the given string at location x,y in the specified font.
//	 Prints each character given via calls to lcd_glyph. The entry string
//   is null terminated and non 0x20->0x7e characters are ignored.
//   (adapted function from the MJK-code)
//
//  ARGUMENTS:
//		left       coordinate of left start of string.                
//		top        coordinate of top of string.
//		font       font number to use for display (see fonts.h)
//		str   	   text string to display (null-terminted)
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void LCD128x64_1_Text(uint8 left, uint8 top, uint8 font, const char *str)
{
  	uint8 x = left;
  	uint8 glyph;
  	uint8 width;
	uint8 height;
	uint8 store_width;
	const uint8 *glyph_ptr;

  	while(*str != 0x00)
  	{
    	glyph = (uint8)*str;

		//Если буква за пределами допуска то заменяем ее на "нейзвестный символ"
		if((glyph < fonts[font].glyph_beg) || (glyph > fonts[font].glyph_end))
		{ 
			glyph = fonts[font].glyph_def;  //заменамна на символ по умолчанию
		}


    	/* Пересчет кода символа в табличный (от 0) ромер символа. */
    	glyph -= fonts[font].glyph_beg; // номер символа в таблице  хххх_dot_glyph_table[]
    	width = fonts[font].fixed_width;	/* check if it is a fixed width */
		if(width == 0)
		{               //Если 0 то использовать таблицу значений ширины для каждого символа
			width=fonts[font].width_table[glyph];	/* get the variable width instead */
		}


		height = fonts[font].glyph_height;      //Высота символа из структуры шрифта
		store_width = fonts[font].store_width;  //Ширина символа в таблице(байт)

		glyph_ptr = fonts[font].glyph_table + ((unsigned int)glyph * (unsigned int)store_width * (unsigned int)height);
        //          five_dot_glyph_table ---> номер символа в табл х   кол-во байт на символ   х  высота символа в пикселях  

		/* range check / limit things here ( не рисуем вне экрана) */
		if(x > GLCD_RIGHT)
		{
			x = GLCD_RIGHT; 
		}
		if((x + width) > GLCD_RIGHT+1)
		{
			width = GLCD_RIGHT - x + 1;
		}
		if(top > GLCD_BOTTOM)
		{
			top = GLCD_BOTTOM;
		}
		if((top + height) > GLCD_BOTTOM+1)
		{
			height = GLCD_BOTTOM - top + 1;
		}

		LCD128x64_1_Glyph(x,top,width,height,glyph_ptr,store_width);  /* plug symbol into buffer */

		x += width;							/* move right for next character */
		str++;								/* point to next character in string */
	}
}

//-----------------------------------------------------------------------------
//  FUNCTION NAME:	GLCD_ScreenBitmap
//
//  DESCRIPTION:
//
//	Display full screen bitmap.
//	Image has to be rotated and bits flipped
//
//	Bitmaps can be created by "The Dot Factory" program
//		Order: LSB first
//		Flip/rotate: 270?
//
//  ARGUMENTS:
//		pbitmap
//
//  RETURNS:
//     none.
//
//-----------------------------------------------------------------------------
void LCD128x64_1_ScreenBitmap(const uint8 * pbitmap)
{
	// pin in demopic
	int i,x,y;
	uint8 data;
	uint16 idx;
	
	for (i=0;i<GLCD_NUMCONTROLLER;i++) {
		LCD128x64_1_SelectChip(i);
		for (y=0;y<GLCD_MAXPAGE;y++) {
			LCD128x64_1_SetPage(y);
			LCD128x64_1_SetAddress(0);
			for (x=0;x<GLCD_MAXADDRESS;x++) {
				idx=((GLCD_NUMCONTROLLER-1-i)*GLCD_MAXADDRESS*GLCD_MAXPAGE);
				idx+=GLCD_MAXPAGE*(GLCD_MAXADDRESS-1-x);
				idx+=y;
				data=pbitmap[idx];
				if (gGLCD_inverted) data=~data;
				LCD128x64_1_WriteData(data);
			}
		}
	}
	LCD128x64_1_SelectChip(GLCD_SELECTNONE);
}

