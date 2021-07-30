/*******************************************************************************
* File Name: LCDC.h  
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

#if !defined(CY_PINS_LCDC_ALIASES_H) /* Pins LCDC_ALIASES_H */
#define CY_PINS_LCDC_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define LCDC_0			(LCDC__0__PC)
#define LCDC_0_INTR	((uint16)((uint16)0x0001u << LCDC__0__SHIFT))

#define LCDC_1			(LCDC__1__PC)
#define LCDC_1_INTR	((uint16)((uint16)0x0001u << LCDC__1__SHIFT))

#define LCDC_2			(LCDC__2__PC)
#define LCDC_2_INTR	((uint16)((uint16)0x0001u << LCDC__2__SHIFT))

#define LCDC_3			(LCDC__3__PC)
#define LCDC_3_INTR	((uint16)((uint16)0x0001u << LCDC__3__SHIFT))

#define LCDC_4			(LCDC__4__PC)
#define LCDC_4_INTR	((uint16)((uint16)0x0001u << LCDC__4__SHIFT))

#define LCDC_INTR_ALL	 ((uint16)(LCDC_0_INTR| LCDC_1_INTR| LCDC_2_INTR| LCDC_3_INTR| LCDC_4_INTR))
#define LCDC_CS1			(LCDC__CS1__PC)
#define LCDC_CS1_INTR	((uint16)((uint16)0x0001u << LCDC__0__SHIFT))

#define LCDC_CS2			(LCDC__CS2__PC)
#define LCDC_CS2_INTR	((uint16)((uint16)0x0001u << LCDC__1__SHIFT))

#define LCDC_RST			(LCDC__RST__PC)
#define LCDC_RST_INTR	((uint16)((uint16)0x0001u << LCDC__2__SHIFT))

#define LCDC_DI			(LCDC__DI__PC)
#define LCDC_DI_INTR	((uint16)((uint16)0x0001u << LCDC__3__SHIFT))

#define LCDC_E			(LCDC__E__PC)
#define LCDC_E_INTR	((uint16)((uint16)0x0001u << LCDC__4__SHIFT))

#endif /* End Pins LCDC_ALIASES_H */


/* [] END OF FILE */
