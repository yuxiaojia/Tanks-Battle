/*
 * task_accelerometer.c
 *
 *  Created on: April 8, 2021
 *      Author: Jarvis Jia
 *              Ruiwen Wang
 */

#include <main.h>

 TaskHandle_t Task_Accelerometer_Handle;
 TaskHandle_t Task_Accelerometer_Timer_Handle;

 volatile uint32_t ACCELEROMETER_X_DIR = 0;
 volatile uint32_t ACCELEROMETER_Y_DIR = 0;


/******************************************************************************
* This function will run the same configuration code that you would have
* written for HW02.
******************************************************************************/
 void Task_Accelerometer_Init(void)
 {
     // Configure the X direction as an analog input pin.
     P6->SEL0 |= BIT1;
     P6->SEL1 |= BIT1;

     // Configure the Y direction as an analog input pin.
     P4->SEL0 |= BIT0;
     P4->SEL1 |= BIT0;

     // Configure CTL0 to sample 16-times in pulsed sample mode.
     // NEW -- Indicate that this is a sequence-of-channels.
      ADC14->CTL0 = ADC14_CTL0_SHP |
                    ADC14_CTL0_SHT02 |
                    ADC14_CTL0_CONSEQ_1;

     // Configure ADC to return 14-bit values
      //CHANGE THIS AND SOLVE THE PROBLEM
      ADC14->CTL1 = ADC14_CTL1_RES_3;

     // Associate the X direction analog signal with MEM[0]
      ADC14->MCTL[0] |= ADC14_MCTLN_INCH_14;

     // Associate the Y direction analog signal with MEM[1]
      ADC14->MCTL[1] |= ADC14_MCTLN_INCH_13| ADC14_MCTLN_EOS;;

     // Enable interrupts in the ADC AFTER a value is written into MEM[1].
      ADC14->IER0 = ADC14_IER0_IE0;
      ADC14->IER0 = ADC14_IER0_IE1;

     // Enable ADC Interrupt in the NVIC
      NVIC_EnableIRQ(ADC14_IRQn);

      // Interrupt priorities must NOT be set to 0.  It is preferred if they are set to a value that is > than 1 too.
       NVIC_SetPriority(ADC14_IRQn,2);

      // Turn ADC ON
      ADC14->CTL0 |= ADC14_CTL0_ON;




 }




 /******************************************************************************
 * Used to start an ADC14 Conversion
 ******************************************************************************/
 void Task_Accelerometer_Timer(void *pvParameters)
 {
     //start an ADC Conversion every 5 ms

     while(1){
     ADC14->CTL0 |= ADC14_CTL0_SC | ADC14_CTL0_ENC;
     vTaskDelay(pdMS_TO_TICKS(5));
     }
 }


/******************************************************************************
* Bottom Half Task.  Examines the ADC data from the joy stick on the MKII
******************************************************************************/
void Task_Accelerometer_Bottom_Half(void *pvParameters)
{

    // wait for the task notification
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    while(1){
        if(ACCELEROMETER_X_DIR > 8700 )
               { // send right shift massage

            TANK_MSG_t message = {CMD_RIGHT, 1};
                   xQueueSendToBack(Queue_Tank, &message, portMAX_DELAY);

               }
               else if(ACCELEROMETER_X_DIR < 7700)
               {  // send left shift massage
                   TANK_MSG_t message = {CMD_LEFT, 1};
                   xQueueSendToBack(Queue_Tank, &message, portMAX_DELAY);
               }
               else if(ACCELEROMETER_Y_DIR > 8700 )
               {  // send up shift massage
                   TANK_MSG_t message = {CMD_UP, 1};
                   xQueueSendToBack(Queue_Tank, &message, portMAX_DELAY);
               }
               else if(ACCELEROMETER_Y_DIR < 7700 )
               {  // send down shift massage
                   TANK_MSG_t message = {CMD_DOWN, 1};
                   xQueueSendToBack(Queue_Tank, &message, portMAX_DELAY);
               }
               else
               {
                   TANK_MSG_t message = {CMD_CENTER, 1};
                  xQueueSendToBack(Queue_Tank, &message, portMAX_DELAY);
                   // keep in the center
               }
        vTaskDelay(pdMS_TO_TICKS(5)); //SPEED
    }
}


/******************************************************************************
* Top Half of ADC14 Handler.
******************************************************************************/
void ADC14_IRQHandler(void)
{
    // Read the X value
    ACCELEROMETER_X_DIR = ADC14->MEM[0];
    // Read the Y value
    ACCELEROMETER_Y_DIR = ADC14->MEM[1];


    BaseType_t xHigherPriorityTaskWoken;
    // send the notification to the accelerometer bottom task
    vTaskNotifyGiveFromISR(
            Task_Accelerometer_Handle,
            &xHigherPriorityTaskWoken
    );

   portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}



