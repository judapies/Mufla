/*******************************************************************************
* File Name: LCD128x64_1_Timer_LCD_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "LCD128x64_1_Timer_LCD.h"

static LCD128x64_1_Timer_LCD_backupStruct LCD128x64_1_Timer_LCD_backup;


/*******************************************************************************
* Function Name: LCD128x64_1_Timer_LCD_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  LCD128x64_1_Timer_LCD_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void LCD128x64_1_Timer_LCD_SaveConfig(void) 
{
    #if (!LCD128x64_1_Timer_LCD_UsingFixedFunction)
        LCD128x64_1_Timer_LCD_backup.TimerUdb = LCD128x64_1_Timer_LCD_ReadCounter();
        LCD128x64_1_Timer_LCD_backup.InterruptMaskValue = LCD128x64_1_Timer_LCD_STATUS_MASK;
        #if (LCD128x64_1_Timer_LCD_UsingHWCaptureCounter)
            LCD128x64_1_Timer_LCD_backup.TimerCaptureCounter = LCD128x64_1_Timer_LCD_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!LCD128x64_1_Timer_LCD_UDB_CONTROL_REG_REMOVED)
            LCD128x64_1_Timer_LCD_backup.TimerControlRegister = LCD128x64_1_Timer_LCD_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: LCD128x64_1_Timer_LCD_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  LCD128x64_1_Timer_LCD_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void LCD128x64_1_Timer_LCD_RestoreConfig(void) 
{   
    #if (!LCD128x64_1_Timer_LCD_UsingFixedFunction)

        LCD128x64_1_Timer_LCD_WriteCounter(LCD128x64_1_Timer_LCD_backup.TimerUdb);
        LCD128x64_1_Timer_LCD_STATUS_MASK =LCD128x64_1_Timer_LCD_backup.InterruptMaskValue;
        #if (LCD128x64_1_Timer_LCD_UsingHWCaptureCounter)
            LCD128x64_1_Timer_LCD_SetCaptureCount(LCD128x64_1_Timer_LCD_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!LCD128x64_1_Timer_LCD_UDB_CONTROL_REG_REMOVED)
            LCD128x64_1_Timer_LCD_WriteControlRegister(LCD128x64_1_Timer_LCD_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: LCD128x64_1_Timer_LCD_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  LCD128x64_1_Timer_LCD_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void LCD128x64_1_Timer_LCD_Sleep(void) 
{
    #if(!LCD128x64_1_Timer_LCD_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(LCD128x64_1_Timer_LCD_CTRL_ENABLE == (LCD128x64_1_Timer_LCD_CONTROL & LCD128x64_1_Timer_LCD_CTRL_ENABLE))
        {
            /* Timer is enabled */
            LCD128x64_1_Timer_LCD_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            LCD128x64_1_Timer_LCD_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    LCD128x64_1_Timer_LCD_Stop();
    LCD128x64_1_Timer_LCD_SaveConfig();
}


/*******************************************************************************
* Function Name: LCD128x64_1_Timer_LCD_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  LCD128x64_1_Timer_LCD_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void LCD128x64_1_Timer_LCD_Wakeup(void) 
{
    LCD128x64_1_Timer_LCD_RestoreConfig();
    #if(!LCD128x64_1_Timer_LCD_UDB_CONTROL_REG_REMOVED)
        if(LCD128x64_1_Timer_LCD_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                LCD128x64_1_Timer_LCD_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
