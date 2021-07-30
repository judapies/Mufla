/*******************************************************************************
* File Name: Luz.h  
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

#if !defined(CY_PINS_Luz_H) /* Pins Luz_H */
#define CY_PINS_Luz_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Luz_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Luz__PORT == 15 && ((Luz__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Luz_Write(uint8 value);
void    Luz_SetDriveMode(uint8 mode);
uint8   Luz_ReadDataReg(void);
uint8   Luz_Read(void);
void    Luz_SetInterruptMode(uint16 position, uint16 mode);
uint8   Luz_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Luz_SetDriveMode() function.
     *  @{
     */
        #define Luz_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Luz_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Luz_DM_RES_UP          PIN_DM_RES_UP
        #define Luz_DM_RES_DWN         PIN_DM_RES_DWN
        #define Luz_DM_OD_LO           PIN_DM_OD_LO
        #define Luz_DM_OD_HI           PIN_DM_OD_HI
        #define Luz_DM_STRONG          PIN_DM_STRONG
        #define Luz_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Luz_MASK               Luz__MASK
#define Luz_SHIFT              Luz__SHIFT
#define Luz_WIDTH              1u

/* Interrupt constants */
#if defined(Luz__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Luz_SetInterruptMode() function.
     *  @{
     */
        #define Luz_INTR_NONE      (uint16)(0x0000u)
        #define Luz_INTR_RISING    (uint16)(0x0001u)
        #define Luz_INTR_FALLING   (uint16)(0x0002u)
        #define Luz_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Luz_INTR_MASK      (0x01u) 
#endif /* (Luz__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Luz_PS                     (* (reg8 *) Luz__PS)
/* Data Register */
#define Luz_DR                     (* (reg8 *) Luz__DR)
/* Port Number */
#define Luz_PRT_NUM                (* (reg8 *) Luz__PRT) 
/* Connect to Analog Globals */                                                  
#define Luz_AG                     (* (reg8 *) Luz__AG)                       
/* Analog MUX bux enable */
#define Luz_AMUX                   (* (reg8 *) Luz__AMUX) 
/* Bidirectional Enable */                                                        
#define Luz_BIE                    (* (reg8 *) Luz__BIE)
/* Bit-mask for Aliased Register Access */
#define Luz_BIT_MASK               (* (reg8 *) Luz__BIT_MASK)
/* Bypass Enable */
#define Luz_BYP                    (* (reg8 *) Luz__BYP)
/* Port wide control signals */                                                   
#define Luz_CTL                    (* (reg8 *) Luz__CTL)
/* Drive Modes */
#define Luz_DM0                    (* (reg8 *) Luz__DM0) 
#define Luz_DM1                    (* (reg8 *) Luz__DM1)
#define Luz_DM2                    (* (reg8 *) Luz__DM2) 
/* Input Buffer Disable Override */
#define Luz_INP_DIS                (* (reg8 *) Luz__INP_DIS)
/* LCD Common or Segment Drive */
#define Luz_LCD_COM_SEG            (* (reg8 *) Luz__LCD_COM_SEG)
/* Enable Segment LCD */
#define Luz_LCD_EN                 (* (reg8 *) Luz__LCD_EN)
/* Slew Rate Control */
#define Luz_SLW                    (* (reg8 *) Luz__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Luz_PRTDSI__CAPS_SEL       (* (reg8 *) Luz__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Luz_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Luz__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Luz_PRTDSI__OE_SEL0        (* (reg8 *) Luz__PRTDSI__OE_SEL0) 
#define Luz_PRTDSI__OE_SEL1        (* (reg8 *) Luz__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Luz_PRTDSI__OUT_SEL0       (* (reg8 *) Luz__PRTDSI__OUT_SEL0) 
#define Luz_PRTDSI__OUT_SEL1       (* (reg8 *) Luz__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Luz_PRTDSI__SYNC_OUT       (* (reg8 *) Luz__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Luz__SIO_CFG)
    #define Luz_SIO_HYST_EN        (* (reg8 *) Luz__SIO_HYST_EN)
    #define Luz_SIO_REG_HIFREQ     (* (reg8 *) Luz__SIO_REG_HIFREQ)
    #define Luz_SIO_CFG            (* (reg8 *) Luz__SIO_CFG)
    #define Luz_SIO_DIFF           (* (reg8 *) Luz__SIO_DIFF)
#endif /* (Luz__SIO_CFG) */

/* Interrupt Registers */
#if defined(Luz__INTSTAT)
    #define Luz_INTSTAT            (* (reg8 *) Luz__INTSTAT)
    #define Luz_SNAP               (* (reg8 *) Luz__SNAP)
    
	#define Luz_0_INTTYPE_REG 		(* (reg8 *) Luz__0__INTTYPE)
#endif /* (Luz__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Luz_H */


/* [] END OF FILE */
