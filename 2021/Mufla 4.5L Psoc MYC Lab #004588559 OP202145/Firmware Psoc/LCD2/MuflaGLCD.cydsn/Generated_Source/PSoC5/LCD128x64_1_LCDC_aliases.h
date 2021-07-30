/*******************************************************************************
* File Name: LCD128x64_1_LCDC.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_LCD128x64_1_LCDC_ALIASES_H) /* Pins LCD128x64_1_LCDC_ALIASES_H */
#define CY_PINS_LCD128x64_1_LCDC_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define LCD128x64_1_LCDC_0			(LCD128x64_1_LCDC__0__PC)
#define LCD128x64_1_LCDC_0_INTR	((uint16)((uint16)0x0001u << LCD128x64_1_LCDC__0__SHIFT))

#define LCD128x64_1_LCDC_1			(LCD128x64_1_LCDC__1__PC)
#define LCD128x64_1_LCDC_1_INTR	((uint16)((uint16)0x0001u << LCD128x64_1_LCDC__1__SHIFT))

#define LCD128x64_1_LCDC_2			(LCD128x64_1_LCDC__2__PC)
#define LCD128x64_1_LCDC_2_INTR	((uint16)((uint16)0x0001u << LCD128x64_1_LCDC__2__SHIFT))

#define LCD128x64_1_LCDC_3			(LCD128x64_1_LCDC__3__PC)
#define LCD128x64_1_LCDC_3_INTR	((uint16)((uint16)0x0001u << LCD128x64_1_LCDC__3__SHIFT))

#define LCD128x64_1_LCDC_4			(LCD128x64_1_LCDC__4__PC)
#define LCD128x64_1_LCDC_4_INTR	((uint16)((uint16)0x0001u << LCD128x64_1_LCDC__4__SHIFT))

#define LCD128x64_1_LCDC_INTR_ALL	 ((uint16)(LCD128x64_1_LCDC_0_INTR| LCD128x64_1_LCDC_1_INTR| LCD128x64_1_LCDC_2_INTR| LCD128x64_1_LCDC_3_INTR| LCD128x64_1_LCDC_4_INTR))
#define LCD128x64_1_LCDC_CS1			(LCD128x64_1_LCDC__CS1__PC)
#define LCD128x64_1_LCDC_CS1_INTR	((uint16)((uint16)0x0001u << LCD128x64_1_LCDC__0__SHIFT))

#define LCD128x64_1_LCDC_CS2			(LCD128x64_1_LCDC__CS2__PC)
#define LCD128x64_1_LCDC_CS2_INTR	((uint16)((uint16)0x0001u << LCD128x64_1_LCDC__1__SHIFT))

#define LCD128x64_1_LCDC_RST			(LCD128x64_1_LCDC__RST__PC)
#define LCD128x64_1_LCDC_RST_INTR	((uint16)((uint16)0x0001u << LCD128x64_1_LCDC__2__SHIFT))

#define LCD128x64_1_LCDC_DI			(LCD128x64_1_LCDC__DI__PC)
#define LCD128x64_1_LCDC_DI_INTR	((uint16)((uint16)0x0001u << LCD128x64_1_LCDC__3__SHIFT))

#define LCD128x64_1_LCDC_E			(LCD128x64_1_LCDC__E__PC)
#define LCD128x64_1_LCDC_E_INTR	((uint16)((uint16)0x0001u << LCD128x64_1_LCDC__4__SHIFT))

#endif /* End Pins LCD128x64_1_LCDC_ALIASES_H */


/* [] END OF FILE */
