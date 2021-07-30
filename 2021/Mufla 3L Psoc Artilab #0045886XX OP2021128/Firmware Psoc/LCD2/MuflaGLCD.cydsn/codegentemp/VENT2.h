/*******************************************************************************
* File Name: VENT2.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_VENT2_H) /* Pins VENT2_H */
#define CY_PINS_VENT2_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "VENT2_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 VENT2__PORT == 15 && ((VENT2__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    VENT2_Write(uint8 value);
void    VENT2_SetDriveMode(uint8 mode);
uint8   VENT2_ReadDataReg(void);
uint8   VENT2_Read(void);
void    VENT2_SetInterruptMode(uint16 position, uint16 mode);
uint8   VENT2_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the VENT2_SetDriveMode() function.
     *  @{
     */
        #define VENT2_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define VENT2_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define VENT2_DM_RES_UP          PIN_DM_RES_UP
        #define VENT2_DM_RES_DWN         PIN_DM_RES_DWN
        #define VENT2_DM_OD_LO           PIN_DM_OD_LO
        #define VENT2_DM_OD_HI           PIN_DM_OD_HI
        #define VENT2_DM_STRONG          PIN_DM_STRONG
        #define VENT2_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define VENT2_MASK               VENT2__MASK
#define VENT2_SHIFT              VENT2__SHIFT
#define VENT2_WIDTH              1u

/* Interrupt constants */
#if defined(VENT2__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in VENT2_SetInterruptMode() function.
     *  @{
     */
        #define VENT2_INTR_NONE      (uint16)(0x0000u)
        #define VENT2_INTR_RISING    (uint16)(0x0001u)
        #define VENT2_INTR_FALLING   (uint16)(0x0002u)
        #define VENT2_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define VENT2_INTR_MASK      (0x01u) 
#endif /* (VENT2__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define VENT2_PS                     (* (reg8 *) VENT2__PS)
/* Data Register */
#define VENT2_DR                     (* (reg8 *) VENT2__DR)
/* Port Number */
#define VENT2_PRT_NUM                (* (reg8 *) VENT2__PRT) 
/* Connect to Analog Globals */                                                  
#define VENT2_AG                     (* (reg8 *) VENT2__AG)                       
/* Analog MUX bux enable */
#define VENT2_AMUX                   (* (reg8 *) VENT2__AMUX) 
/* Bidirectional Enable */                                                        
#define VENT2_BIE                    (* (reg8 *) VENT2__BIE)
/* Bit-mask for Aliased Register Access */
#define VENT2_BIT_MASK               (* (reg8 *) VENT2__BIT_MASK)
/* Bypass Enable */
#define VENT2_BYP                    (* (reg8 *) VENT2__BYP)
/* Port wide control signals */                                                   
#define VENT2_CTL                    (* (reg8 *) VENT2__CTL)
/* Drive Modes */
#define VENT2_DM0                    (* (reg8 *) VENT2__DM0) 
#define VENT2_DM1                    (* (reg8 *) VENT2__DM1)
#define VENT2_DM2                    (* (reg8 *) VENT2__DM2) 
/* Input Buffer Disable Override */
#define VENT2_INP_DIS                (* (reg8 *) VENT2__INP_DIS)
/* LCD Common or Segment Drive */
#define VENT2_LCD_COM_SEG            (* (reg8 *) VENT2__LCD_COM_SEG)
/* Enable Segment LCD */
#define VENT2_LCD_EN                 (* (reg8 *) VENT2__LCD_EN)
/* Slew Rate Control */
#define VENT2_SLW                    (* (reg8 *) VENT2__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define VENT2_PRTDSI__CAPS_SEL       (* (reg8 *) VENT2__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define VENT2_PRTDSI__DBL_SYNC_IN    (* (reg8 *) VENT2__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define VENT2_PRTDSI__OE_SEL0        (* (reg8 *) VENT2__PRTDSI__OE_SEL0) 
#define VENT2_PRTDSI__OE_SEL1        (* (reg8 *) VENT2__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define VENT2_PRTDSI__OUT_SEL0       (* (reg8 *) VENT2__PRTDSI__OUT_SEL0) 
#define VENT2_PRTDSI__OUT_SEL1       (* (reg8 *) VENT2__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define VENT2_PRTDSI__SYNC_OUT       (* (reg8 *) VENT2__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(VENT2__SIO_CFG)
    #define VENT2_SIO_HYST_EN        (* (reg8 *) VENT2__SIO_HYST_EN)
    #define VENT2_SIO_REG_HIFREQ     (* (reg8 *) VENT2__SIO_REG_HIFREQ)
    #define VENT2_SIO_CFG            (* (reg8 *) VENT2__SIO_CFG)
    #define VENT2_SIO_DIFF           (* (reg8 *) VENT2__SIO_DIFF)
#endif /* (VENT2__SIO_CFG) */

/* Interrupt Registers */
#if defined(VENT2__INTSTAT)
    #define VENT2_INTSTAT            (* (reg8 *) VENT2__INTSTAT)
    #define VENT2_SNAP               (* (reg8 *) VENT2__SNAP)
    
	#define VENT2_0_INTTYPE_REG 		(* (reg8 *) VENT2__0__INTTYPE)
#endif /* (VENT2__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_VENT2_H */


/* [] END OF FILE */
