/*
 * task_joystick.h
 *
 *  Created on: April 8, 2021
 *      Author: Jarvis Jia
 */

#ifndef TASK_PUSH_BUTTON_H_
#define TASK_PUSH_BUTTON_H_

#include "main.h"


extern TaskHandle_t Task_Push_Button_Handle;
extern TaskHandle_t Task_Push_Button_Timer_Handle;

//check if button S2 has been pressed
volatile extern bool ALERT_S2_PRESSED;

/******************************************************************************
* Configure the IO pins for BOTH the X and Y directions of the analog
* joystick.  The X direction should be configured to place the results in
* MEM[0].  The Y direction should be configured to place the results in MEM[1].
*
* After BOTH analog signals have finished being converted, a SINGLE interrupt
* should be generated.
*
* Parameters
*      None
* Returns
*      None
******************************************************************************/
 void Task_Push_Button_Init(void);

/******************************************************************************
* Used to start an ADC14 Conversion
******************************************************************************/
void Task_Push_Button_Timer(void *pvParameters);

/******************************************************************************
* Used to start an timer32 interrupt 10ms
******************************************************************************/
void T32_1_Interrupt_10ms(void);

/******************************************************************************
* Examines the ADC data from the joystick on the MKII
******************************************************************************/
void Task_Push_Button_Bottom_Half(void *pvParameters);



#endif /* TASK_PUSH_BUTTON_H_ */
