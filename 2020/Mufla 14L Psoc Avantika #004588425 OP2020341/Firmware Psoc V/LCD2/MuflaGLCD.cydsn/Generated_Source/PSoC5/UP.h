/*******************************************************************************
* File Name: UP.h  
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

#if !defined(CY_PINS_UP_H) /* Pins UP_H */
#define CY_PINS_UP_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "UP_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 UP__PORT == 15 && ((UP__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    UP_Write(uint8 value);
void    UP_SetDriveMode(uint8 mode);
uint8   UP_ReadDataReg(void);
uint8   UP_Read(void);
void    UP_SetInterruptMode(uint16 position, uint16 mode);
uint8   UP_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the UP_SetDriveMode() function.
     *  @{
     */
        #define UP_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define UP_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define UP_DM_RES_UP          PIN_DM_RES_UP
        #define UP_DM_RES_DWN         PIN_DM_RES_DWN
        #define UP_DM_OD_LO           PIN_DM_OD_LO
        #define UP_DM_OD_HI           PIN_DM_OD_HI
        #define UP_DM_STRONG          PIN_DM_STRONG
        #define UP_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define UP_MASK               UP__MASK
#define UP_SHIFT              UP__SHIFT
#define UP_WIDTH              1u

/* Interrupt constants */
#if defined(UP__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in UP_SetInterruptMode() function.
     *  @{
     */
        #define UP_INTR_NONE      (uint16)(0x0000u)
        #define UP_INTR_RISING    (uint16)(0x0001u)
        #define UP_INTR_FALLING   (uint16)(0x0002u)
        #define UP_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define UP_INTR_MASK      (0x01u) 
#endif /* (UP__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define UP_PS                     (* (reg8 *) UP__PS)
/* Data Register */
#define UP_DR                     (* (reg8 *) UP__DR)
/* Port Number */
#define UP_PRT_NUM                (* (reg8 *) UP__PRT) 
/* Connect to Analog Globals */                                                  
#define UP_AG                     (* (reg8 *) UP__AG)                       
/* Analog MUX bux enable */
#define UP_AMUX                   (* (reg8 *) UP__AMUX) 
/* Bidirectional Enable */                                                        
#define UP_BIE                    (* (reg8 *) UP__BIE)
/* Bit-mask for Aliased Register Access */
#define UP_BIT_MASK               (* (reg8 *) UP__BIT_MASK)
/* Bypass Enable */
#define UP_BYP                    (* (reg8 *) UP__BYP)
/* Port wide control signals */                                                   
#define UP_CTL                    (* (reg8 *) UP__CTL)
/* Drive Modes */
#define UP_DM0                    (* (reg8 *) UP__DM0) 
#define UP_DM1                    (* (reg8 *) UP__DM1)
#define UP_DM2                    (* (reg8 *) UP__DM2) 
/* Input Buffer Disable Override */
#define UP_INP_DIS                (* (reg8 *) UP__INP_DIS)
/* LCD Common or Segment Drive */
#define UP_LCD_COM_SEG            (* (reg8 *) UP__LCD_COM_SEG)
/* Enable Segment LCD */
#define UP_LCD_EN                 (* (reg8 *) UP__LCD_EN)
/* Slew Rate Control */
#define UP_SLW                    (* (reg8 *) UP__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define UP_PRTDSI__CAPS_SEL       (* (reg8 *) UP__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define UP_PRTDSI__DBL_SYNC_IN    (* (reg8 *) UP__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define UP_PRTDSI__OE_SEL0        (* (reg8 *) UP__PRTDSI__OE_SEL0) 
#define UP_PRTDSI__OE_SEL1        (* (reg8 *) UP__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define UP_PRTDSI__OUT_SEL0       (* (reg8 *) UP__PRTDSI__OUT_SEL0) 
#define UP_PRTDSI__OUT_SEL1       (* (reg8 *) UP__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define UP_PRTDSI__SYNC_OUT       (* (reg8 *) UP__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(UP__SIO_CFG)
    #define UP_SIO_HYST_EN        (* (reg8 *) UP__SIO_HYST_EN)
    #define UP_SIO_REG_HIFREQ     (* (reg8 *) UP__SIO_REG_HIFREQ)
    #define UP_SIO_CFG            (* (reg8 *) UP__SIO_CFG)
    #define UP_SIO_DIFF           (* (reg8 *) UP__SIO_DIFF)
#endif /* (UP__SIO_CFG) */

/* Interrupt Registers */
#if defined(UP__INTSTAT)
    #define UP_INTSTAT            (* (reg8 *) UP__INTSTAT)
    #define UP_SNAP               (* (reg8 *) UP__SNAP)
    
	#define UP_0_INTTYPE_REG 		(* (reg8 *) UP__0__INTTYPE)
#endif /* (UP__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_UP_H */


/* [] END OF FILE */
