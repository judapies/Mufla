/*******************************************************************************
* File Name: DOWN.h  
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

#if !defined(CY_PINS_DOWN_H) /* Pins DOWN_H */
#define CY_PINS_DOWN_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "DOWN_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 DOWN__PORT == 15 && ((DOWN__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    DOWN_Write(uint8 value);
void    DOWN_SetDriveMode(uint8 mode);
uint8   DOWN_ReadDataReg(void);
uint8   DOWN_Read(void);
void    DOWN_SetInterruptMode(uint16 position, uint16 mode);
uint8   DOWN_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the DOWN_SetDriveMode() function.
     *  @{
     */
        #define DOWN_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define DOWN_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define DOWN_DM_RES_UP          PIN_DM_RES_UP
        #define DOWN_DM_RES_DWN         PIN_DM_RES_DWN
        #define DOWN_DM_OD_LO           PIN_DM_OD_LO
        #define DOWN_DM_OD_HI           PIN_DM_OD_HI
        #define DOWN_DM_STRONG          PIN_DM_STRONG
        #define DOWN_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define DOWN_MASK               DOWN__MASK
#define DOWN_SHIFT              DOWN__SHIFT
#define DOWN_WIDTH              1u

/* Interrupt constants */
#if defined(DOWN__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in DOWN_SetInterruptMode() function.
     *  @{
     */
        #define DOWN_INTR_NONE      (uint16)(0x0000u)
        #define DOWN_INTR_RISING    (uint16)(0x0001u)
        #define DOWN_INTR_FALLING   (uint16)(0x0002u)
        #define DOWN_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define DOWN_INTR_MASK      (0x01u) 
#endif /* (DOWN__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define DOWN_PS                     (* (reg8 *) DOWN__PS)
/* Data Register */
#define DOWN_DR                     (* (reg8 *) DOWN__DR)
/* Port Number */
#define DOWN_PRT_NUM                (* (reg8 *) DOWN__PRT) 
/* Connect to Analog Globals */                                                  
#define DOWN_AG                     (* (reg8 *) DOWN__AG)                       
/* Analog MUX bux enable */
#define DOWN_AMUX                   (* (reg8 *) DOWN__AMUX) 
/* Bidirectional Enable */                                                        
#define DOWN_BIE                    (* (reg8 *) DOWN__BIE)
/* Bit-mask for Aliased Register Access */
#define DOWN_BIT_MASK               (* (reg8 *) DOWN__BIT_MASK)
/* Bypass Enable */
#define DOWN_BYP                    (* (reg8 *) DOWN__BYP)
/* Port wide control signals */                                                   
#define DOWN_CTL                    (* (reg8 *) DOWN__CTL)
/* Drive Modes */
#define DOWN_DM0                    (* (reg8 *) DOWN__DM0) 
#define DOWN_DM1                    (* (reg8 *) DOWN__DM1)
#define DOWN_DM2                    (* (reg8 *) DOWN__DM2) 
/* Input Buffer Disable Override */
#define DOWN_INP_DIS                (* (reg8 *) DOWN__INP_DIS)
/* LCD Common or Segment Drive */
#define DOWN_LCD_COM_SEG            (* (reg8 *) DOWN__LCD_COM_SEG)
/* Enable Segment LCD */
#define DOWN_LCD_EN                 (* (reg8 *) DOWN__LCD_EN)
/* Slew Rate Control */
#define DOWN_SLW                    (* (reg8 *) DOWN__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define DOWN_PRTDSI__CAPS_SEL       (* (reg8 *) DOWN__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define DOWN_PRTDSI__DBL_SYNC_IN    (* (reg8 *) DOWN__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define DOWN_PRTDSI__OE_SEL0        (* (reg8 *) DOWN__PRTDSI__OE_SEL0) 
#define DOWN_PRTDSI__OE_SEL1        (* (reg8 *) DOWN__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define DOWN_PRTDSI__OUT_SEL0       (* (reg8 *) DOWN__PRTDSI__OUT_SEL0) 
#define DOWN_PRTDSI__OUT_SEL1       (* (reg8 *) DOWN__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define DOWN_PRTDSI__SYNC_OUT       (* (reg8 *) DOWN__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(DOWN__SIO_CFG)
    #define DOWN_SIO_HYST_EN        (* (reg8 *) DOWN__SIO_HYST_EN)
    #define DOWN_SIO_REG_HIFREQ     (* (reg8 *) DOWN__SIO_REG_HIFREQ)
    #define DOWN_SIO_CFG            (* (reg8 *) DOWN__SIO_CFG)
    #define DOWN_SIO_DIFF           (* (reg8 *) DOWN__SIO_DIFF)
#endif /* (DOWN__SIO_CFG) */

/* Interrupt Registers */
#if defined(DOWN__INTSTAT)
    #define DOWN_INTSTAT            (* (reg8 *) DOWN__INTSTAT)
    #define DOWN_SNAP               (* (reg8 *) DOWN__SNAP)
    
	#define DOWN_0_INTTYPE_REG 		(* (reg8 *) DOWN__0__INTTYPE)
#endif /* (DOWN__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_DOWN_H */


/* [] END OF FILE */
