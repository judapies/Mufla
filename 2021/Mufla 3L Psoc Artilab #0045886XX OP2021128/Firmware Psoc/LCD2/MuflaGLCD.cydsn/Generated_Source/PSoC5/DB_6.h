/*******************************************************************************
* File Name: DB_6.h  
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

#if !defined(CY_PINS_DB_6_H) /* Pins DB_6_H */
#define CY_PINS_DB_6_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "DB_6_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 DB_6__PORT == 15 && ((DB_6__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    DB_6_Write(uint8 value);
void    DB_6_SetDriveMode(uint8 mode);
uint8   DB_6_ReadDataReg(void);
uint8   DB_6_Read(void);
void    DB_6_SetInterruptMode(uint16 position, uint16 mode);
uint8   DB_6_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the DB_6_SetDriveMode() function.
     *  @{
     */
        #define DB_6_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define DB_6_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define DB_6_DM_RES_UP          PIN_DM_RES_UP
        #define DB_6_DM_RES_DWN         PIN_DM_RES_DWN
        #define DB_6_DM_OD_LO           PIN_DM_OD_LO
        #define DB_6_DM_OD_HI           PIN_DM_OD_HI
        #define DB_6_DM_STRONG          PIN_DM_STRONG
        #define DB_6_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define DB_6_MASK               DB_6__MASK
#define DB_6_SHIFT              DB_6__SHIFT
#define DB_6_WIDTH              1u

/* Interrupt constants */
#if defined(DB_6__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in DB_6_SetInterruptMode() function.
     *  @{
     */
        #define DB_6_INTR_NONE      (uint16)(0x0000u)
        #define DB_6_INTR_RISING    (uint16)(0x0001u)
        #define DB_6_INTR_FALLING   (uint16)(0x0002u)
        #define DB_6_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define DB_6_INTR_MASK      (0x01u) 
#endif /* (DB_6__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define DB_6_PS                     (* (reg8 *) DB_6__PS)
/* Data Register */
#define DB_6_DR                     (* (reg8 *) DB_6__DR)
/* Port Number */
#define DB_6_PRT_NUM                (* (reg8 *) DB_6__PRT) 
/* Connect to Analog Globals */                                                  
#define DB_6_AG                     (* (reg8 *) DB_6__AG)                       
/* Analog MUX bux enable */
#define DB_6_AMUX                   (* (reg8 *) DB_6__AMUX) 
/* Bidirectional Enable */                                                        
#define DB_6_BIE                    (* (reg8 *) DB_6__BIE)
/* Bit-mask for Aliased Register Access */
#define DB_6_BIT_MASK               (* (reg8 *) DB_6__BIT_MASK)
/* Bypass Enable */
#define DB_6_BYP                    (* (reg8 *) DB_6__BYP)
/* Port wide control signals */                                                   
#define DB_6_CTL                    (* (reg8 *) DB_6__CTL)
/* Drive Modes */
#define DB_6_DM0                    (* (reg8 *) DB_6__DM0) 
#define DB_6_DM1                    (* (reg8 *) DB_6__DM1)
#define DB_6_DM2                    (* (reg8 *) DB_6__DM2) 
/* Input Buffer Disable Override */
#define DB_6_INP_DIS                (* (reg8 *) DB_6__INP_DIS)
/* LCD Common or Segment Drive */
#define DB_6_LCD_COM_SEG            (* (reg8 *) DB_6__LCD_COM_SEG)
/* Enable Segment LCD */
#define DB_6_LCD_EN                 (* (reg8 *) DB_6__LCD_EN)
/* Slew Rate Control */
#define DB_6_SLW                    (* (reg8 *) DB_6__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define DB_6_PRTDSI__CAPS_SEL       (* (reg8 *) DB_6__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define DB_6_PRTDSI__DBL_SYNC_IN    (* (reg8 *) DB_6__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define DB_6_PRTDSI__OE_SEL0        (* (reg8 *) DB_6__PRTDSI__OE_SEL0) 
#define DB_6_PRTDSI__OE_SEL1        (* (reg8 *) DB_6__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define DB_6_PRTDSI__OUT_SEL0       (* (reg8 *) DB_6__PRTDSI__OUT_SEL0) 
#define DB_6_PRTDSI__OUT_SEL1       (* (reg8 *) DB_6__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define DB_6_PRTDSI__SYNC_OUT       (* (reg8 *) DB_6__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(DB_6__SIO_CFG)
    #define DB_6_SIO_HYST_EN        (* (reg8 *) DB_6__SIO_HYST_EN)
    #define DB_6_SIO_REG_HIFREQ     (* (reg8 *) DB_6__SIO_REG_HIFREQ)
    #define DB_6_SIO_CFG            (* (reg8 *) DB_6__SIO_CFG)
    #define DB_6_SIO_DIFF           (* (reg8 *) DB_6__SIO_DIFF)
#endif /* (DB_6__SIO_CFG) */

/* Interrupt Registers */
#if defined(DB_6__INTSTAT)
    #define DB_6_INTSTAT            (* (reg8 *) DB_6__INTSTAT)
    #define DB_6_SNAP               (* (reg8 *) DB_6__SNAP)
    
	#define DB_6_0_INTTYPE_REG 		(* (reg8 *) DB_6__0__INTTYPE)
#endif /* (DB_6__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_DB_6_H */


/* [] END OF FILE */
