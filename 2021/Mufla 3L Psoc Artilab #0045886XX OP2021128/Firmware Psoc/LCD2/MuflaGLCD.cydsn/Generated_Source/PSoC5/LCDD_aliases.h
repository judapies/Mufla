/*******************************************************************************
* File Name: LCDD.h  
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

#if !defined(CY_PINS_LCDD_ALIASES_H) /* Pins LCDD_ALIASES_H */
#define CY_PINS_LCDD_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define LCDD_0			(LCDD__0__PC)
#define LCDD_0_INTR	((uint16)((uint16)0x0001u << LCDD__0__SHIFT))

#define LCDD_1			(LCDD__1__PC)
#define LCDD_1_INTR	((uint16)((uint16)0x0001u << LCDD__1__SHIFT))

#define LCDD_2			(LCDD__2__PC)
#define LCDD_2_INTR	((uint16)((uint16)0x0001u << LCDD__2__SHIFT))

#define LCDD_3			(LCDD__3__PC)
#define LCDD_3_INTR	((uint16)((uint16)0x0001u << LCDD__3__SHIFT))

#define LCDD_4			(LCDD__4__PC)
#define LCDD_4_INTR	((uint16)((uint16)0x0001u << LCDD__4__SHIFT))

#define LCDD_5			(LCDD__5__PC)
#define LCDD_5_INTR	((uint16)((uint16)0x0001u << LCDD__5__SHIFT))

#define LCDD_6			(LCDD__6__PC)
#define LCDD_6_INTR	((uint16)((uint16)0x0001u << LCDD__6__SHIFT))

#define LCDD_7			(LCDD__7__PC)
#define LCDD_7_INTR	((uint16)((uint16)0x0001u << LCDD__7__SHIFT))

#define LCDD_INTR_ALL	 ((uint16)(LCDD_0_INTR| LCDD_1_INTR| LCDD_2_INTR| LCDD_3_INTR| LCDD_4_INTR| LCDD_5_INTR| LCDD_6_INTR| LCDD_7_INTR))

#endif /* End Pins LCDD_ALIASES_H */


/* [] END OF FILE */
