/*******************************************************************************
* File Name: LCD3.h  
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

#if !defined(CY_PINS_LCD3_H) /* Pins LCD3_H */
#define CY_PINS_LCD3_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "LCD3_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 LCD3__PORT == 15 && ((LCD3__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    LCD3_Write(uint8 value);
void    LCD3_SetDriveMode(uint8 mode);
uint8   LCD3_ReadDataReg(void);
uint8   LCD3_Read(void);
void    LCD3_SetInterruptMode(uint16 position, uint16 mode);
uint8   LCD3_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the LCD3_SetDriveMode() function.
     *  @{
     */
        #define LCD3_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define LCD3_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define LCD3_DM_RES_UP          PIN_DM_RES_UP
        #define LCD3_DM_RES_DWN         PIN_DM_RES_DWN
        #define LCD3_DM_OD_LO           PIN_DM_OD_LO
        #define LCD3_DM_OD_HI           PIN_DM_OD_HI
        #define LCD3_DM_STRONG          PIN_DM_STRONG
        #define LCD3_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define LCD3_MASK               LCD3__MASK
#define LCD3_SHIFT              LCD3__SHIFT
#define LCD3_WIDTH              1u

/* Interrupt constants */
#if defined(LCD3__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in LCD3_SetInterruptMode() function.
     *  @{
     */
        #define LCD3_INTR_NONE      (uint16)(0x0000u)
        #define LCD3_INTR_RISING    (uint16)(0x0001u)
        #define LCD3_INTR_FALLING   (uint16)(0x0002u)
        #define LCD3_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define LCD3_INTR_MASK      (0x01u) 
#endif /* (LCD3__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define LCD3_PS                     (* (reg8 *) LCD3__PS)
/* Data Register */
#define LCD3_DR                     (* (reg8 *) LCD3__DR)
/* Port Number */
#define LCD3_PRT_NUM                (* (reg8 *) LCD3__PRT) 
/* Connect to Analog Globals */                                                  
#define LCD3_AG                     (* (reg8 *) LCD3__AG)                       
/* Analog MUX bux enable */
#define LCD3_AMUX                   (* (reg8 *) LCD3__AMUX) 
/* Bidirectional Enable */                                                        
#define LCD3_BIE                    (* (reg8 *) LCD3__BIE)
/* Bit-mask for Aliased Register Access */
#define LCD3_BIT_MASK               (* (reg8 *) LCD3__BIT_MASK)
/* Bypass Enable */
#define LCD3_BYP                    (* (reg8 *) LCD3__BYP)
/* Port wide control signals */                                                   
#define LCD3_CTL                    (* (reg8 *) LCD3__CTL)
/* Drive Modes */
#define LCD3_DM0                    (* (reg8 *) LCD3__DM0) 
#define LCD3_DM1                    (* (reg8 *) LCD3__DM1)
#define LCD3_DM2                    (* (reg8 *) LCD3__DM2) 
/* Input Buffer Disable Override */
#define LCD3_INP_DIS                (* (reg8 *) LCD3__INP_DIS)
/* LCD Common or Segment Drive */
#define LCD3_LCD_COM_SEG            (* (reg8 *) LCD3__LCD_COM_SEG)
/* Enable Segment LCD */
#define LCD3_LCD_EN                 (* (reg8 *) LCD3__LCD_EN)
/* Slew Rate Control */
#define LCD3_SLW                    (* (reg8 *) LCD3__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define LCD3_PRTDSI__CAPS_SEL       (* (reg8 *) LCD3__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define LCD3_PRTDSI__DBL_SYNC_IN    (* (reg8 *) LCD3__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define LCD3_PRTDSI__OE_SEL0        (* (reg8 *) LCD3__PRTDSI__OE_SEL0) 
#define LCD3_PRTDSI__OE_SEL1        (* (reg8 *) LCD3__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define LCD3_PRTDSI__OUT_SEL0       (* (reg8 *) LCD3__PRTDSI__OUT_SEL0) 
#define LCD3_PRTDSI__OUT_SEL1       (* (reg8 *) LCD3__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define LCD3_PRTDSI__SYNC_OUT       (* (reg8 *) LCD3__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(LCD3__SIO_CFG)
    #define LCD3_SIO_HYST_EN        (* (reg8 *) LCD3__SIO_HYST_EN)
    #define LCD3_SIO_REG_HIFREQ     (* (reg8 *) LCD3__SIO_REG_HIFREQ)
    #define LCD3_SIO_CFG            (* (reg8 *) LCD3__SIO_CFG)
    #define LCD3_SIO_DIFF           (* (reg8 *) LCD3__SIO_DIFF)
#endif /* (LCD3__SIO_CFG) */

/* Interrupt Registers */
#if defined(LCD3__INTSTAT)
    #define LCD3_INTSTAT            (* (reg8 *) LCD3__INTSTAT)
    #define LCD3_SNAP               (* (reg8 *) LCD3__SNAP)
    
	#define LCD3_0_INTTYPE_REG 		(* (reg8 *) LCD3__0__INTTYPE)
#endif /* (LCD3__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_LCD3_H */


/* [] END OF FILE */
