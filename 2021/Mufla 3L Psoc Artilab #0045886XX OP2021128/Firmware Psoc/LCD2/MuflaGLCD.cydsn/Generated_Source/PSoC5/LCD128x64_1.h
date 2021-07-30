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
// 	FILE NAME: 	glcd.h
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
//#ifndef __H__
//#define __H__

#if !defined(LCD128x64_1_H) 
#define LCD128x64_1_H
#include "stdlib.h"     // содержит  функциюю   abs для PSoC5
#include "cytypes.h"        //Обеспечивает macros и еще что-то
#include "cylib.h"
#include "cyfitter.h"     //Файл определяет адреса при компиляции.


//#include "fonts.h"


//#include <device.h>

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------
#define GLCD_MODE_NORMAL	0
#define GLCD_MODE_INVERTED  1

#define GLCD_MODE_CLEAR     0
#define GLCD_MODE_SET       1
#define GLCD_MODE_XOR       2

#define GLCD_CLEAR_BUFFER   0
#define GLCD_CLEAR_ALL      1

/* display definition */
#define GLCD_NUMCONTROLLER   2
#define GLCD_NUMCONTROLLERX  2
#define GLCD_NUMCONTROLLERY  1

#define GLCD_MAXPAGE 		 8
#define GLCD_MAXADDRESS		 64

#define GLCD_LEFT			0
#define GLCD_TOP			0
#define GLCD_RIGHT			(GLCD_NUMCONTROLLERX*GLCD_MAXADDRESS-1)
#define GLCD_BOTTOM			(GLCD_NUMCONTROLLERY*GLCD_MAXPAGE*8-1)


//-----------------------------------------------------------------------------
//  Prototypes
//-----------------------------------------------------------------------------
void LCD128x64_1_Init(void);

void LCD128x64_1_SetInvert(uint8 mode);

void LCD128x64_1_Update(uint16 top, uint16 bottom);
#define LCD128x64_1_UpdateAll() LCD128x64_1_Update(GLCD_TOP, GLCD_BOTTOM);

void LCD128x64_1_ClrScr(uint8 bufferAndDisplay);

void LCD128x64_1_Dot(uint8 x, uint8 y, uint8 mode);
#define LCD128x64_1_SetDot(x,y) GLCD_Dot(x, y, GLCD_MODE_SET)
#define LCD128x64_1_ClearDot(x,y) GLCD_Dot(x, y, GLCD_MODE_CLEAR);

void LCD128x64_1_Line(uint8 x1, uint8 y1,
	uint8 x2, uint8 y2, uint8 mode);
void LCD128x64_1_Circle(uint8 xCenter, uint8 yCenter,
		uint8 radius, uint8 mode);
void LCD128x64_1_Rect(uint8 x, uint8 y, uint8 width, uint8 height,
		uint8 mode);
void LCD128x64_1_Box(uint8 x, uint8 y, uint8 width, uint8 height,
		uint8 mode);

void LCD128x64_1_Text(uint8 left, uint8 top, uint8 font, const char *str);

void LCD128x64_1_ScreenBitmap(const uint8 * pbitmap);

//-----------------------------------------------------------------------------
#endif
