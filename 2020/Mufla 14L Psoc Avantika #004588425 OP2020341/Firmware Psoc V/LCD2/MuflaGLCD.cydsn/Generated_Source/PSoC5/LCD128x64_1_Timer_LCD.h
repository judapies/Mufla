/*******************************************************************************
* File Name: LCD128x64_1_Timer_LCD.h
* Version 2.80
*
*  Description:
*     Contains the function prototypes and constants available to the timer
*     user module.
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_TIMER_LCD128x64_1_Timer_LCD_H)
#define CY_TIMER_LCD128x64_1_Timer_LCD_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 LCD128x64_1_Timer_LCD_initVar;

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component Timer_v2_80 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */


/**************************************
*           Parameter Defaults
**************************************/

#define LCD128x64_1_Timer_LCD_Resolution                 16u
#define LCD128x64_1_Timer_LCD_UsingFixedFunction         1u
#define LCD128x64_1_Timer_LCD_UsingHWCaptureCounter      0u
#define LCD128x64_1_Timer_LCD_SoftwareCaptureMode        0u
#define LCD128x64_1_Timer_LCD_SoftwareTriggerMode        0u
#define LCD128x64_1_Timer_LCD_UsingHWEnable              0u
#define LCD128x64_1_Timer_LCD_EnableTriggerMode          0u
#define LCD128x64_1_Timer_LCD_InterruptOnCaptureCount    0u
#define LCD128x64_1_Timer_LCD_RunModeUsed                1u
#define LCD128x64_1_Timer_LCD_ControlRegRemoved          0u

#if defined(LCD128x64_1_Timer_LCD_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG)
    #define LCD128x64_1_Timer_LCD_UDB_CONTROL_REG_REMOVED            (0u)
#elif  (LCD128x64_1_Timer_LCD_UsingFixedFunction)
    #define LCD128x64_1_Timer_LCD_UDB_CONTROL_REG_REMOVED            (0u)
#else 
    #define LCD128x64_1_Timer_LCD_UDB_CONTROL_REG_REMOVED            (1u)
#endif /* End LCD128x64_1_Timer_LCD_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG */


/***************************************
*       Type defines
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for Timer Component
 *************************************************************************/
typedef struct
{
    uint8 TimerEnableState;
    #if(!LCD128x64_1_Timer_LCD_UsingFixedFunction)

        uint16 TimerUdb;
        uint8 InterruptMaskValue;
        #if (LCD128x64_1_Timer_LCD_UsingHWCaptureCounter)
            uint8 TimerCaptureCounter;
        #endif /* variable declarations for backing up non retention registers in CY_UDB_V1 */

        #if (!LCD128x64_1_Timer_LCD_UDB_CONTROL_REG_REMOVED)
            uint8 TimerControlRegister;
        #endif /* variable declaration for backing up enable state of the Timer */
    #endif /* define backup variables only for UDB implementation. Fixed function registers are all retention */

}LCD128x64_1_Timer_LCD_backupStruct;


/***************************************
*       Function Prototypes
***************************************/

void    LCD128x64_1_Timer_LCD_Start(void) ;
void    LCD128x64_1_Timer_LCD_Stop(void) ;

void    LCD128x64_1_Timer_LCD_SetInterruptMode(uint8 interruptMode) ;
uint8   LCD128x64_1_Timer_LCD_ReadStatusRegister(void) ;
/* Deprecated function. Do not use this in future. Retained for backward compatibility */
#define LCD128x64_1_Timer_LCD_GetInterruptSource() LCD128x64_1_Timer_LCD_ReadStatusRegister()

#if(!LCD128x64_1_Timer_LCD_UDB_CONTROL_REG_REMOVED)
    uint8   LCD128x64_1_Timer_LCD_ReadControlRegister(void) ;
    void    LCD128x64_1_Timer_LCD_WriteControlRegister(uint8 control) ;
#endif /* (!LCD128x64_1_Timer_LCD_UDB_CONTROL_REG_REMOVED) */

uint16  LCD128x64_1_Timer_LCD_ReadPeriod(void) ;
void    LCD128x64_1_Timer_LCD_WritePeriod(uint16 period) ;
uint16  LCD128x64_1_Timer_LCD_ReadCounter(void) ;
void    LCD128x64_1_Timer_LCD_WriteCounter(uint16 counter) ;
uint16  LCD128x64_1_Timer_LCD_ReadCapture(void) ;
void    LCD128x64_1_Timer_LCD_SoftwareCapture(void) ;

#if(!LCD128x64_1_Timer_LCD_UsingFixedFunction) /* UDB Prototypes */
    #if (LCD128x64_1_Timer_LCD_SoftwareCaptureMode)
        void    LCD128x64_1_Timer_LCD_SetCaptureMode(uint8 captureMode) ;
    #endif /* (!LCD128x64_1_Timer_LCD_UsingFixedFunction) */

    #if (LCD128x64_1_Timer_LCD_SoftwareTriggerMode)
        void    LCD128x64_1_Timer_LCD_SetTriggerMode(uint8 triggerMode) ;
    #endif /* (LCD128x64_1_Timer_LCD_SoftwareTriggerMode) */

    #if (LCD128x64_1_Timer_LCD_EnableTriggerMode)
        void    LCD128x64_1_Timer_LCD_EnableTrigger(void) ;
        void    LCD128x64_1_Timer_LCD_DisableTrigger(void) ;
    #endif /* (LCD128x64_1_Timer_LCD_EnableTriggerMode) */


    #if(LCD128x64_1_Timer_LCD_InterruptOnCaptureCount)
        void    LCD128x64_1_Timer_LCD_SetInterruptCount(uint8 interruptCount) ;
    #endif /* (LCD128x64_1_Timer_LCD_InterruptOnCaptureCount) */

    #if (LCD128x64_1_Timer_LCD_UsingHWCaptureCounter)
        void    LCD128x64_1_Timer_LCD_SetCaptureCount(uint8 captureCount) ;
        uint8   LCD128x64_1_Timer_LCD_ReadCaptureCount(void) ;
    #endif /* (LCD128x64_1_Timer_LCD_UsingHWCaptureCounter) */

    void LCD128x64_1_Timer_LCD_ClearFIFO(void) ;
#endif /* UDB Prototypes */

/* Sleep Retention APIs */
void LCD128x64_1_Timer_LCD_Init(void)          ;
void LCD128x64_1_Timer_LCD_Enable(void)        ;
void LCD128x64_1_Timer_LCD_SaveConfig(void)    ;
void LCD128x64_1_Timer_LCD_RestoreConfig(void) ;
void LCD128x64_1_Timer_LCD_Sleep(void)         ;
void LCD128x64_1_Timer_LCD_Wakeup(void)        ;


/***************************************
*   Enumerated Types and Parameters
***************************************/

/* Enumerated Type B_Timer__CaptureModes, Used in Capture Mode */
#define LCD128x64_1_Timer_LCD__B_TIMER__CM_NONE 0
#define LCD128x64_1_Timer_LCD__B_TIMER__CM_RISINGEDGE 1
#define LCD128x64_1_Timer_LCD__B_TIMER__CM_FALLINGEDGE 2
#define LCD128x64_1_Timer_LCD__B_TIMER__CM_EITHEREDGE 3
#define LCD128x64_1_Timer_LCD__B_TIMER__CM_SOFTWARE 4



/* Enumerated Type B_Timer__TriggerModes, Used in Trigger Mode */
#define LCD128x64_1_Timer_LCD__B_TIMER__TM_NONE 0x00u
#define LCD128x64_1_Timer_LCD__B_TIMER__TM_RISINGEDGE 0x04u
#define LCD128x64_1_Timer_LCD__B_TIMER__TM_FALLINGEDGE 0x08u
#define LCD128x64_1_Timer_LCD__B_TIMER__TM_EITHEREDGE 0x0Cu
#define LCD128x64_1_Timer_LCD__B_TIMER__TM_SOFTWARE 0x10u


/***************************************
*    Initialial Parameter Constants
***************************************/

#define LCD128x64_1_Timer_LCD_INIT_PERIOD             65535u
#define LCD128x64_1_Timer_LCD_INIT_CAPTURE_MODE       ((uint8)((uint8)1u << LCD128x64_1_Timer_LCD_CTRL_CAP_MODE_SHIFT))
#define LCD128x64_1_Timer_LCD_INIT_TRIGGER_MODE       ((uint8)((uint8)0u << LCD128x64_1_Timer_LCD_CTRL_TRIG_MODE_SHIFT))
#if (LCD128x64_1_Timer_LCD_UsingFixedFunction)
    #define LCD128x64_1_Timer_LCD_INIT_INTERRUPT_MODE (((uint8)((uint8)0u << LCD128x64_1_Timer_LCD_STATUS_TC_INT_MASK_SHIFT)) | \
                                                  ((uint8)((uint8)0 << LCD128x64_1_Timer_LCD_STATUS_CAPTURE_INT_MASK_SHIFT)))
#else
    #define LCD128x64_1_Timer_LCD_INIT_INTERRUPT_MODE (((uint8)((uint8)0u << LCD128x64_1_Timer_LCD_STATUS_TC_INT_MASK_SHIFT)) | \
                                                 ((uint8)((uint8)0 << LCD128x64_1_Timer_LCD_STATUS_CAPTURE_INT_MASK_SHIFT)) | \
                                                 ((uint8)((uint8)0 << LCD128x64_1_Timer_LCD_STATUS_FIFOFULL_INT_MASK_SHIFT)))
#endif /* (LCD128x64_1_Timer_LCD_UsingFixedFunction) */
#define LCD128x64_1_Timer_LCD_INIT_CAPTURE_COUNT      (2u)
#define LCD128x64_1_Timer_LCD_INIT_INT_CAPTURE_COUNT  ((uint8)((uint8)(1u - 1u) << LCD128x64_1_Timer_LCD_CTRL_INTCNT_SHIFT))


/***************************************
*           Registers
***************************************/

#if (LCD128x64_1_Timer_LCD_UsingFixedFunction) /* Implementation Specific Registers and Register Constants */


    /***************************************
    *    Fixed Function Registers
    ***************************************/

    #define LCD128x64_1_Timer_LCD_STATUS         (*(reg8 *) LCD128x64_1_Timer_LCD_TimerHW__SR0 )
    /* In Fixed Function Block Status and Mask are the same register */
    #define LCD128x64_1_Timer_LCD_STATUS_MASK    (*(reg8 *) LCD128x64_1_Timer_LCD_TimerHW__SR0 )
    #define LCD128x64_1_Timer_LCD_CONTROL        (*(reg8 *) LCD128x64_1_Timer_LCD_TimerHW__CFG0)
    #define LCD128x64_1_Timer_LCD_CONTROL2       (*(reg8 *) LCD128x64_1_Timer_LCD_TimerHW__CFG1)
    #define LCD128x64_1_Timer_LCD_CONTROL2_PTR   ( (reg8 *) LCD128x64_1_Timer_LCD_TimerHW__CFG1)
    #define LCD128x64_1_Timer_LCD_RT1            (*(reg8 *) LCD128x64_1_Timer_LCD_TimerHW__RT1)
    #define LCD128x64_1_Timer_LCD_RT1_PTR        ( (reg8 *) LCD128x64_1_Timer_LCD_TimerHW__RT1)

    #if (CY_PSOC3 || CY_PSOC5LP)
        #define LCD128x64_1_Timer_LCD_CONTROL3       (*(reg8 *) LCD128x64_1_Timer_LCD_TimerHW__CFG2)
        #define LCD128x64_1_Timer_LCD_CONTROL3_PTR   ( (reg8 *) LCD128x64_1_Timer_LCD_TimerHW__CFG2)
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */
    #define LCD128x64_1_Timer_LCD_GLOBAL_ENABLE  (*(reg8 *) LCD128x64_1_Timer_LCD_TimerHW__PM_ACT_CFG)
    #define LCD128x64_1_Timer_LCD_GLOBAL_STBY_ENABLE  (*(reg8 *) LCD128x64_1_Timer_LCD_TimerHW__PM_STBY_CFG)

    #define LCD128x64_1_Timer_LCD_CAPTURE_LSB         (* (reg16 *) LCD128x64_1_Timer_LCD_TimerHW__CAP0 )
    #define LCD128x64_1_Timer_LCD_CAPTURE_LSB_PTR       ((reg16 *) LCD128x64_1_Timer_LCD_TimerHW__CAP0 )
    #define LCD128x64_1_Timer_LCD_PERIOD_LSB          (* (reg16 *) LCD128x64_1_Timer_LCD_TimerHW__PER0 )
    #define LCD128x64_1_Timer_LCD_PERIOD_LSB_PTR        ((reg16 *) LCD128x64_1_Timer_LCD_TimerHW__PER0 )
    #define LCD128x64_1_Timer_LCD_COUNTER_LSB         (* (reg16 *) LCD128x64_1_Timer_LCD_TimerHW__CNT_CMP0 )
    #define LCD128x64_1_Timer_LCD_COUNTER_LSB_PTR       ((reg16 *) LCD128x64_1_Timer_LCD_TimerHW__CNT_CMP0 )


    /***************************************
    *    Register Constants
    ***************************************/

    /* Fixed Function Block Chosen */
    #define LCD128x64_1_Timer_LCD_BLOCK_EN_MASK                     LCD128x64_1_Timer_LCD_TimerHW__PM_ACT_MSK
    #define LCD128x64_1_Timer_LCD_BLOCK_STBY_EN_MASK                LCD128x64_1_Timer_LCD_TimerHW__PM_STBY_MSK

    /* Control Register Bit Locations */
    /* Interrupt Count - Not valid for Fixed Function Block */
    #define LCD128x64_1_Timer_LCD_CTRL_INTCNT_SHIFT                  0x00u
    /* Trigger Polarity - Not valid for Fixed Function Block */
    #define LCD128x64_1_Timer_LCD_CTRL_TRIG_MODE_SHIFT               0x00u
    /* Trigger Enable - Not valid for Fixed Function Block */
    #define LCD128x64_1_Timer_LCD_CTRL_TRIG_EN_SHIFT                 0x00u
    /* Capture Polarity - Not valid for Fixed Function Block */
    #define LCD128x64_1_Timer_LCD_CTRL_CAP_MODE_SHIFT                0x00u
    /* Timer Enable - As defined in Register Map, part of TMRX_CFG0 register */
    #define LCD128x64_1_Timer_LCD_CTRL_ENABLE_SHIFT                  0x00u

    /* Control Register Bit Masks */
    #define LCD128x64_1_Timer_LCD_CTRL_ENABLE                        ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_CTRL_ENABLE_SHIFT))

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define LCD128x64_1_Timer_LCD_CTRL2_IRQ_SEL_SHIFT                 0x00u
    #define LCD128x64_1_Timer_LCD_CTRL2_IRQ_SEL                      ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_CTRL2_IRQ_SEL_SHIFT))

    #if (CY_PSOC5A)
        /* Use CFG1 Mode bits to set run mode */
        /* As defined by Verilog Implementation */
        #define LCD128x64_1_Timer_LCD_CTRL_MODE_SHIFT                 0x01u
        #define LCD128x64_1_Timer_LCD_CTRL_MODE_MASK                 ((uint8)((uint8)0x07u << LCD128x64_1_Timer_LCD_CTRL_MODE_SHIFT))
    #endif /* (CY_PSOC5A) */
    #if (CY_PSOC3 || CY_PSOC5LP)
        /* Control3 Register Bit Locations */
        #define LCD128x64_1_Timer_LCD_CTRL_RCOD_SHIFT        0x02u
        #define LCD128x64_1_Timer_LCD_CTRL_ENBL_SHIFT        0x00u
        #define LCD128x64_1_Timer_LCD_CTRL_MODE_SHIFT        0x00u

        /* Control3 Register Bit Masks */
        #define LCD128x64_1_Timer_LCD_CTRL_RCOD_MASK  ((uint8)((uint8)0x03u << LCD128x64_1_Timer_LCD_CTRL_RCOD_SHIFT)) /* ROD and COD bit masks */
        #define LCD128x64_1_Timer_LCD_CTRL_ENBL_MASK  ((uint8)((uint8)0x80u << LCD128x64_1_Timer_LCD_CTRL_ENBL_SHIFT)) /* HW_EN bit mask */
        #define LCD128x64_1_Timer_LCD_CTRL_MODE_MASK  ((uint8)((uint8)0x03u << LCD128x64_1_Timer_LCD_CTRL_MODE_SHIFT)) /* Run mode bit mask */

        #define LCD128x64_1_Timer_LCD_CTRL_RCOD       ((uint8)((uint8)0x03u << LCD128x64_1_Timer_LCD_CTRL_RCOD_SHIFT))
        #define LCD128x64_1_Timer_LCD_CTRL_ENBL       ((uint8)((uint8)0x80u << LCD128x64_1_Timer_LCD_CTRL_ENBL_SHIFT))
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */

    /*RT1 Synch Constants: Applicable for PSoC3 and PSoC5LP */
    #define LCD128x64_1_Timer_LCD_RT1_SHIFT                       0x04u
    /* Sync TC and CMP bit masks */
    #define LCD128x64_1_Timer_LCD_RT1_MASK                        ((uint8)((uint8)0x03u << LCD128x64_1_Timer_LCD_RT1_SHIFT))
    #define LCD128x64_1_Timer_LCD_SYNC                            ((uint8)((uint8)0x03u << LCD128x64_1_Timer_LCD_RT1_SHIFT))
    #define LCD128x64_1_Timer_LCD_SYNCDSI_SHIFT                   0x00u
    /* Sync all DSI inputs with Mask  */
    #define LCD128x64_1_Timer_LCD_SYNCDSI_MASK                    ((uint8)((uint8)0x0Fu << LCD128x64_1_Timer_LCD_SYNCDSI_SHIFT))
    /* Sync all DSI inputs */
    #define LCD128x64_1_Timer_LCD_SYNCDSI_EN                      ((uint8)((uint8)0x0Fu << LCD128x64_1_Timer_LCD_SYNCDSI_SHIFT))

    #define LCD128x64_1_Timer_LCD_CTRL_MODE_PULSEWIDTH            ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_CTRL_MODE_SHIFT))
    #define LCD128x64_1_Timer_LCD_CTRL_MODE_PERIOD                ((uint8)((uint8)0x02u << LCD128x64_1_Timer_LCD_CTRL_MODE_SHIFT))
    #define LCD128x64_1_Timer_LCD_CTRL_MODE_CONTINUOUS            ((uint8)((uint8)0x00u << LCD128x64_1_Timer_LCD_CTRL_MODE_SHIFT))

    /* Status Register Bit Locations */
    /* As defined in Register Map, part of TMRX_SR0 register */
    #define LCD128x64_1_Timer_LCD_STATUS_TC_SHIFT                 0x07u
    /* As defined in Register Map, part of TMRX_SR0 register, Shared with Compare Status */
    #define LCD128x64_1_Timer_LCD_STATUS_CAPTURE_SHIFT            0x06u
    /* As defined in Register Map, part of TMRX_SR0 register */
    #define LCD128x64_1_Timer_LCD_STATUS_TC_INT_MASK_SHIFT        (LCD128x64_1_Timer_LCD_STATUS_TC_SHIFT - 0x04u)
    /* As defined in Register Map, part of TMRX_SR0 register, Shared with Compare Status */
    #define LCD128x64_1_Timer_LCD_STATUS_CAPTURE_INT_MASK_SHIFT   (LCD128x64_1_Timer_LCD_STATUS_CAPTURE_SHIFT - 0x04u)

    /* Status Register Bit Masks */
    #define LCD128x64_1_Timer_LCD_STATUS_TC                       ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_STATUS_TC_SHIFT))
    #define LCD128x64_1_Timer_LCD_STATUS_CAPTURE                  ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_STATUS_CAPTURE_SHIFT))
    /* Interrupt Enable Bit-Mask for interrupt on TC */
    #define LCD128x64_1_Timer_LCD_STATUS_TC_INT_MASK              ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_STATUS_TC_INT_MASK_SHIFT))
    /* Interrupt Enable Bit-Mask for interrupt on Capture */
    #define LCD128x64_1_Timer_LCD_STATUS_CAPTURE_INT_MASK         ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_STATUS_CAPTURE_INT_MASK_SHIFT))

#else   /* UDB Registers and Register Constants */


    /***************************************
    *           UDB Registers
    ***************************************/

    #define LCD128x64_1_Timer_LCD_STATUS              (* (reg8 *) LCD128x64_1_Timer_LCD_TimerUDB_rstSts_stsreg__STATUS_REG )
    #define LCD128x64_1_Timer_LCD_STATUS_MASK         (* (reg8 *) LCD128x64_1_Timer_LCD_TimerUDB_rstSts_stsreg__MASK_REG)
    #define LCD128x64_1_Timer_LCD_STATUS_AUX_CTRL     (* (reg8 *) LCD128x64_1_Timer_LCD_TimerUDB_rstSts_stsreg__STATUS_AUX_CTL_REG)
    #define LCD128x64_1_Timer_LCD_CONTROL             (* (reg8 *) LCD128x64_1_Timer_LCD_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG )
    
    #if(LCD128x64_1_Timer_LCD_Resolution <= 8u) /* 8-bit Timer */
        #define LCD128x64_1_Timer_LCD_CAPTURE_LSB         (* (reg8 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define LCD128x64_1_Timer_LCD_CAPTURE_LSB_PTR       ((reg8 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define LCD128x64_1_Timer_LCD_PERIOD_LSB          (* (reg8 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define LCD128x64_1_Timer_LCD_PERIOD_LSB_PTR        ((reg8 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define LCD128x64_1_Timer_LCD_COUNTER_LSB         (* (reg8 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__A0_REG )
        #define LCD128x64_1_Timer_LCD_COUNTER_LSB_PTR       ((reg8 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__A0_REG )
    #elif(LCD128x64_1_Timer_LCD_Resolution <= 16u) /* 8-bit Timer */
        #if(CY_PSOC3) /* 8-bit addres space */
            #define LCD128x64_1_Timer_LCD_CAPTURE_LSB         (* (reg16 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define LCD128x64_1_Timer_LCD_CAPTURE_LSB_PTR       ((reg16 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define LCD128x64_1_Timer_LCD_PERIOD_LSB          (* (reg16 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define LCD128x64_1_Timer_LCD_PERIOD_LSB_PTR        ((reg16 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define LCD128x64_1_Timer_LCD_COUNTER_LSB         (* (reg16 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__A0_REG )
            #define LCD128x64_1_Timer_LCD_COUNTER_LSB_PTR       ((reg16 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__A0_REG )
        #else /* 16-bit address space */
            #define LCD128x64_1_Timer_LCD_CAPTURE_LSB         (* (reg16 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__16BIT_F0_REG )
            #define LCD128x64_1_Timer_LCD_CAPTURE_LSB_PTR       ((reg16 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__16BIT_F0_REG )
            #define LCD128x64_1_Timer_LCD_PERIOD_LSB          (* (reg16 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__16BIT_D0_REG )
            #define LCD128x64_1_Timer_LCD_PERIOD_LSB_PTR        ((reg16 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__16BIT_D0_REG )
            #define LCD128x64_1_Timer_LCD_COUNTER_LSB         (* (reg16 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__16BIT_A0_REG )
            #define LCD128x64_1_Timer_LCD_COUNTER_LSB_PTR       ((reg16 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__16BIT_A0_REG )
        #endif /* CY_PSOC3 */
    #elif(LCD128x64_1_Timer_LCD_Resolution <= 24u)/* 24-bit Timer */
        #define LCD128x64_1_Timer_LCD_CAPTURE_LSB         (* (reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define LCD128x64_1_Timer_LCD_CAPTURE_LSB_PTR       ((reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define LCD128x64_1_Timer_LCD_PERIOD_LSB          (* (reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define LCD128x64_1_Timer_LCD_PERIOD_LSB_PTR        ((reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define LCD128x64_1_Timer_LCD_COUNTER_LSB         (* (reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__A0_REG )
        #define LCD128x64_1_Timer_LCD_COUNTER_LSB_PTR       ((reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__A0_REG )
    #else /* 32-bit Timer */
        #if(CY_PSOC3 || CY_PSOC5) /* 8-bit address space */
            #define LCD128x64_1_Timer_LCD_CAPTURE_LSB         (* (reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define LCD128x64_1_Timer_LCD_CAPTURE_LSB_PTR       ((reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define LCD128x64_1_Timer_LCD_PERIOD_LSB          (* (reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define LCD128x64_1_Timer_LCD_PERIOD_LSB_PTR        ((reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define LCD128x64_1_Timer_LCD_COUNTER_LSB         (* (reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__A0_REG )
            #define LCD128x64_1_Timer_LCD_COUNTER_LSB_PTR       ((reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__A0_REG )
        #else /* 32-bit address space */
            #define LCD128x64_1_Timer_LCD_CAPTURE_LSB         (* (reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__32BIT_F0_REG )
            #define LCD128x64_1_Timer_LCD_CAPTURE_LSB_PTR       ((reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__32BIT_F0_REG )
            #define LCD128x64_1_Timer_LCD_PERIOD_LSB          (* (reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__32BIT_D0_REG )
            #define LCD128x64_1_Timer_LCD_PERIOD_LSB_PTR        ((reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__32BIT_D0_REG )
            #define LCD128x64_1_Timer_LCD_COUNTER_LSB         (* (reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__32BIT_A0_REG )
            #define LCD128x64_1_Timer_LCD_COUNTER_LSB_PTR       ((reg32 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__32BIT_A0_REG )
        #endif /* CY_PSOC3 || CY_PSOC5 */ 
    #endif

    #define LCD128x64_1_Timer_LCD_COUNTER_LSB_PTR_8BIT       ((reg8 *) LCD128x64_1_Timer_LCD_TimerUDB_sT16_timerdp_u0__A0_REG )
    
    #if (LCD128x64_1_Timer_LCD_UsingHWCaptureCounter)
        #define LCD128x64_1_Timer_LCD_CAP_COUNT              (*(reg8 *) LCD128x64_1_Timer_LCD_TimerUDB_sCapCount_counter__PERIOD_REG )
        #define LCD128x64_1_Timer_LCD_CAP_COUNT_PTR          ( (reg8 *) LCD128x64_1_Timer_LCD_TimerUDB_sCapCount_counter__PERIOD_REG )
        #define LCD128x64_1_Timer_LCD_CAPTURE_COUNT_CTRL     (*(reg8 *) LCD128x64_1_Timer_LCD_TimerUDB_sCapCount_counter__CONTROL_AUX_CTL_REG )
        #define LCD128x64_1_Timer_LCD_CAPTURE_COUNT_CTRL_PTR ( (reg8 *) LCD128x64_1_Timer_LCD_TimerUDB_sCapCount_counter__CONTROL_AUX_CTL_REG )
    #endif /* (LCD128x64_1_Timer_LCD_UsingHWCaptureCounter) */


    /***************************************
    *       Register Constants
    ***************************************/

    /* Control Register Bit Locations */
    #define LCD128x64_1_Timer_LCD_CTRL_INTCNT_SHIFT              0x00u       /* As defined by Verilog Implementation */
    #define LCD128x64_1_Timer_LCD_CTRL_TRIG_MODE_SHIFT           0x02u       /* As defined by Verilog Implementation */
    #define LCD128x64_1_Timer_LCD_CTRL_TRIG_EN_SHIFT             0x04u       /* As defined by Verilog Implementation */
    #define LCD128x64_1_Timer_LCD_CTRL_CAP_MODE_SHIFT            0x05u       /* As defined by Verilog Implementation */
    #define LCD128x64_1_Timer_LCD_CTRL_ENABLE_SHIFT              0x07u       /* As defined by Verilog Implementation */

    /* Control Register Bit Masks */
    #define LCD128x64_1_Timer_LCD_CTRL_INTCNT_MASK               ((uint8)((uint8)0x03u << LCD128x64_1_Timer_LCD_CTRL_INTCNT_SHIFT))
    #define LCD128x64_1_Timer_LCD_CTRL_TRIG_MODE_MASK            ((uint8)((uint8)0x03u << LCD128x64_1_Timer_LCD_CTRL_TRIG_MODE_SHIFT))
    #define LCD128x64_1_Timer_LCD_CTRL_TRIG_EN                   ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_CTRL_TRIG_EN_SHIFT))
    #define LCD128x64_1_Timer_LCD_CTRL_CAP_MODE_MASK             ((uint8)((uint8)0x03u << LCD128x64_1_Timer_LCD_CTRL_CAP_MODE_SHIFT))
    #define LCD128x64_1_Timer_LCD_CTRL_ENABLE                    ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_CTRL_ENABLE_SHIFT))

    /* Bit Counter (7-bit) Control Register Bit Definitions */
    /* As defined by the Register map for the AUX Control Register */
    #define LCD128x64_1_Timer_LCD_CNTR_ENABLE                    0x20u

    /* Status Register Bit Locations */
    #define LCD128x64_1_Timer_LCD_STATUS_TC_SHIFT                0x00u  /* As defined by Verilog Implementation */
    #define LCD128x64_1_Timer_LCD_STATUS_CAPTURE_SHIFT           0x01u  /* As defined by Verilog Implementation */
    #define LCD128x64_1_Timer_LCD_STATUS_TC_INT_MASK_SHIFT       LCD128x64_1_Timer_LCD_STATUS_TC_SHIFT
    #define LCD128x64_1_Timer_LCD_STATUS_CAPTURE_INT_MASK_SHIFT  LCD128x64_1_Timer_LCD_STATUS_CAPTURE_SHIFT
    #define LCD128x64_1_Timer_LCD_STATUS_FIFOFULL_SHIFT          0x02u  /* As defined by Verilog Implementation */
    #define LCD128x64_1_Timer_LCD_STATUS_FIFONEMP_SHIFT          0x03u  /* As defined by Verilog Implementation */
    #define LCD128x64_1_Timer_LCD_STATUS_FIFOFULL_INT_MASK_SHIFT LCD128x64_1_Timer_LCD_STATUS_FIFOFULL_SHIFT

    /* Status Register Bit Masks */
    /* Sticky TC Event Bit-Mask */
    #define LCD128x64_1_Timer_LCD_STATUS_TC                      ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_STATUS_TC_SHIFT))
    /* Sticky Capture Event Bit-Mask */
    #define LCD128x64_1_Timer_LCD_STATUS_CAPTURE                 ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_STATUS_CAPTURE_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define LCD128x64_1_Timer_LCD_STATUS_TC_INT_MASK             ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_STATUS_TC_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define LCD128x64_1_Timer_LCD_STATUS_CAPTURE_INT_MASK        ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_STATUS_CAPTURE_SHIFT))
    /* NOT-Sticky FIFO Full Bit-Mask */
    #define LCD128x64_1_Timer_LCD_STATUS_FIFOFULL                ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_STATUS_FIFOFULL_SHIFT))
    /* NOT-Sticky FIFO Not Empty Bit-Mask */
    #define LCD128x64_1_Timer_LCD_STATUS_FIFONEMP                ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_STATUS_FIFONEMP_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define LCD128x64_1_Timer_LCD_STATUS_FIFOFULL_INT_MASK       ((uint8)((uint8)0x01u << LCD128x64_1_Timer_LCD_STATUS_FIFOFULL_SHIFT))

    #define LCD128x64_1_Timer_LCD_STATUS_ACTL_INT_EN             0x10u   /* As defined for the ACTL Register */

    /* Datapath Auxillary Control Register definitions */
    #define LCD128x64_1_Timer_LCD_AUX_CTRL_FIFO0_CLR             0x01u   /* As defined by Register map */
    #define LCD128x64_1_Timer_LCD_AUX_CTRL_FIFO1_CLR             0x02u   /* As defined by Register map */
    #define LCD128x64_1_Timer_LCD_AUX_CTRL_FIFO0_LVL             0x04u   /* As defined by Register map */
    #define LCD128x64_1_Timer_LCD_AUX_CTRL_FIFO1_LVL             0x08u   /* As defined by Register map */
    #define LCD128x64_1_Timer_LCD_STATUS_ACTL_INT_EN_MASK        0x10u   /* As defined for the ACTL Register */

#endif /* Implementation Specific Registers and Register Constants */

#endif  /* CY_TIMER_LCD128x64_1_Timer_LCD_H */


/* [] END OF FILE */
