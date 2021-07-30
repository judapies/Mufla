/******************************************************************************
* File Name: coldJnTemp.c
*  Version 1.0
*
* Description:
*  This file contains the code to measure cold junction temperature
*
* Note:
*
********************************************************************************
* Copyright (2018), Cypress Semiconductor Corporation. All Rights Reserved.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress)
* and is protected by and subject to worldwide patent protection (United
* States and foreign), United States copyright laws and international treaty
* provisions. Cypress hereby grants to licensee a personal, non-exclusive,
* non-transferable license to copy, use, modify, create derivative works of,
* and compile the Cypress Source Code and derivative works for the sole
* purpose of creating custom software in support of licensee product to be
* used only in conjunction with a Cypress integrated circuit as specified in
* the applicable agreement. Any reproduction, modification, translation,
* compilation, or representation of this software except as specified above 
* is prohibited without the express written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH 
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the 
* materials described herein. Cypress does not assume any liability arising out 
* of the application or use of any product or circuit described herein. Cypress 
* does not authorize its products for use as critical components in life-support 
* systems where a malfunction or failure may reasonably be expected to result in 
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of 
* such use and in doing so indemnifies Cypress against all charges. 
*
* Use of this Software may be limited by and subject to the applicable Cypress
* software license agreement. 
*******************************************************************************/
#include <main.h>
#include <ks0108.h>

extern volatile uint8 CJSelectFlag;


uint8 i;
uint8 l2,h2;
uint16 Prom1;
uint16 promedio2[20];

/*******************************************************************************
*  Function Name: MeasureColdJnSensorTemp
********************************************************************************
* Summary:
*  This function is used to select cold junction temperature sensor which is used 
*  as cold junction compensation for thermocouple. The selection is made based on 
*  the value of CJSelectFlag
*
* Parameters:  
* void:
*
* Return: 
* Temperature in °C
*
* Theory:
*
* Side Effects:
* None
*******************************************************************************/
int32 MeasureColdJnSensorTemp(void)
{
    /* Cold Jn temperature is measured using either DS600 IC or Thermistor SW2
        controls the selection*/
    uint16 lm35;
        /* Show ADC2 result on LCD*/
        lm35=0;
        if(dataReady != 0u)
        {
            Prom1=ADC_SAR_CountsTo_mVolts(result);
            if(l2>(10-1))
               l2=0;

            promedio2[l2]=Prom1;l2++;
                 
            for(h2=0;h2<10;h2++){
                lm35+=promedio2[h2];
            } 
            lm35=lm35/10; 
            dataReady = 0u;
        }
    
	return lm35*10;
}

/*******************************************************************************
*  Function Name: MeasureICTemp
********************************************************************************
* Summary:
*  This function is used to measure the DS600 IC temperature which is used as
*  cold junction compensation for thermocouple.
*
* Parameters:  
* void:
*
* Return: 
* int32: Temperature in °C
*
* Theory:
* Cold Junction IC DS600 has temperature coefficient of 6.45mV/°C with 
* 509mv offset at 25°C.
* Analog voltage output from IC is converted to temperature as follows:
* Temp = (Vout - 509mV)/6.45mV
*
* Side Effects:
* None
*******************************************************************************/
int32 MeasureICTemp(void)
{
  	/* Cold junction IC ADC count measurement*/
    int32 ICResult;
   
    /* Cold Junction micro volts converted from ADC reading */
    int32 ICUVolts;
    
    /* Cold Jn temperature measured using DS600 */
    int32 ICTemp;
	
	/* Variable used to calculate VSSA ground reference voltage */
    int32 offset;
    
    
    /* Measure cold junction temperature in Celsius from parameters from the DS600 
	 * Temp Sensor 509 mV at 0 degrees C, divided by 6.45 mV per degree C. The macros
	 * (SCALING, VOLTAGE_PER_DEGREE and ZERO_DEGREE_OFFSET_VOLTAGE have been 
	 * manipulated such that the resultant temperature is in 100th of a degree Celsius*/
    
	ICTemp = ((ICUVolts - ZERO_DEGREE_OFFSET_VOLTAGE) * SCALING) / VOLTAGE_PER_DEGREE;

    return ICTemp;
}

/*******************************************************************************
*  Function Name: MeasureThermistorTemp
********************************************************************************
* Summary:
*  This function is used to measure the thermistor temperature which is used as
*  cold junction compensation for thermocouple.
*
* Parameters:  
* void:
*
* Return: 
* int32: Temperature in 1/100th °C
*
* Theory:
* The thermistor resistance is measured and converted into temperature. See 
* 'AN66477 - Temperature Measurement with thermistor' to understand the resistance 
* measurement technique and the conversion of resistance to temperature using the 
* thermistor component
*
* Side Effects:
* None
*******************************************************************************/

/* [] END OF FILE */

