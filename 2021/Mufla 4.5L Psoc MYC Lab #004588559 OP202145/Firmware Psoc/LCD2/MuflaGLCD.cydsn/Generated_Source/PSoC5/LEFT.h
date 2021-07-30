/*******************************************************************************
* File Name: LEFT.h  
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

#if !defined(CY_PINS_LEFT_H) /* Pins LEFT_H */
#define CY_PINS_LEFT_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "LEFT_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 LEFT__PORT == 15 && ((LEFT__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    LEFT_Write(uint8 value);
void    LEFT_SetDriveMode(uint8 mode);
uint8   LEFT_ReadDataReg(void);
uint8   LEFT_Read(void);
void    LEFT_SetInterruptMode(uint16 position, uint16 mode);
uint8   LEFT_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the LEFT_SetDriveMode() function.
     *  @{
     */
        #define LEFT_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define LEFT_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define LEFT_DM_RES_UP          PIN_DM_RES_UP
        #define LEFT_DM_RES_DWN         PIN_DM_RES_DWN
        #define LEFT_DM_OD_LO           PIN_DM_OD_LO
        #define LEFT_DM_OD_HI           PIN_DM_OD_HI
        #define LEFT_DM_STRONG          PIN_DM_STRONG
        #define LEFT_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define LEFT_MASK               LEFT__MASK
#define LEFT_SHIFT              LEFT__SHIFT
#define LEFT_WIDTH              1u

/* Interrupt constants */
#if defined(LEFT__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in LEFT_SetInterruptMode() function.
     *  @{
     */
        #define LEFT_INTR_NONE      (uint16)(0x0000u)
        #define LEFT_INTR_RISING    (uint16)(0x0001u)
        #define LEFT_INTR_FALLING   (uint16)(0x0002u)
        #define LEFT_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define LEFT_INTR_MASK      (0x01u) 
#endif /* (LEFT__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define LEFT_PS                     (* (reg8 *) LEFT__PS)
/* Data Register */
#define LEFT_DR                     (* (reg8 *) LEFT__DR)
/* Port Number */
#define LEFT_PRT_NUM                (* (reg8 *) LEFT__PRT) 
/* Connect to Analog Globals */                                                  
#define LEFT_AG                     (* (reg8 *) LEFT__AG)                       
/* Analog MUX bux enable */
#define LEFT_AMUX                   (* (reg8 *) LEFT__AMUX) 
/* Bidirectional Enable */                                                        
#define LEFT_BIE                    (* (reg8 *) LEFT__BIE)
/* Bit-mask for Aliased Register Access */
#define LEFT_BIT_MASK               (* (reg8 *) LEFT__BIT_MASK)
/* Bypass Enable */
#define LEFT_BYP                    (* (reg8 *) LEFT__BYP)
/* Port wide control signals */                                                   
#define LEFT_CTL                    (* (reg8 *) LEFT__CTL)
/* Drive Modes */
#define LEFT_DM0                    (* (reg8 *) LEFT__DM0) 
#define LEFT_DM1                    (* (reg8 *) LEFT__DM1)
#define LEFT_DM2                    (* (reg8 *) LEFT__DM2) 
/* Input Buffer Disable Override */
#define LEFT_INP_DIS                (* (reg8 *) LEFT__INP_DIS)
/* LCD Common or Segment Drive */
#define LEFT_LCD_COM_SEG            (* (reg8 *) LEFT__LCD_COM_SEG)
/* Enable Segment LCD */
#define LEFT_LCD_EN                 (* (reg8 *) LEFT__LCD_EN)
/* Slew Rate Control */
#define LEFT_SLW                    (* (reg8 *) LEFT__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define LEFT_PRTDSI__CAPS_SEL       (* (reg8 *) LEFT__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define LEFT_PRTDSI__DBL_SYNC_IN    (* (reg8 *) LEFT__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define LEFT_PRTDSI__OE_SEL0        (* (reg8 *) LEFT__PRTDSI__OE_SEL0) 
#define LEFT_PRTDSI__OE_SEL1        (* (reg8 *) LEFT__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define LEFT_PRTDSI__OUT_SEL0       (* (reg8 *) LEFT__PRTDSI__OUT_SEL0) 
#define LEFT_PRTDSI__OUT_SEL1       (* (reg8 *) LEFT__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define LEFT_PRTDSI__SYNC_OUT       (* (reg8 *) LEFT__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(LEFT__SIO_CFG)
    #define LEFT_SIO_HYST_EN        (* (reg8 *) LEFT__SIO_HYST_EN)
    #define LEFT_SIO_REG_HIFREQ     (* (reg8 *) LEFT__SIO_REG_HIFREQ)
    #define LEFT_SIO_CFG            (* (reg8 *) LEFT__SIO_CFG)
    #define LEFT_SIO_DIFF           (* (reg8 *) LEFT__SIO_DIFF)
#endif /* (LEFT__SIO_CFG) */

/* Interrupt Registers */
#if defined(LEFT__INTSTAT)
    #define LEFT_INTSTAT            (* (reg8 *) LEFT__INTSTAT)
    #define LEFT_SNAP               (* (reg8 *) LEFT__SNAP)
    
	#define LEFT_0_INTTYPE_REG 		(* (reg8 *) LEFT__0__INTTYPE)
#endif /* (LEFT__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_LEFT_H */


/* [] END OF FILE */
