/*
 * task_push_button.c
 *
 *  Created on: April 8, 2021
 *      Author: Jarvis Jia
 *              Ruiwen Wang
 */

#include <main.h>

 TaskHandle_t Task_Push_Button_Handle;
 //TaskHandle_t Task_Push_Button_Timer_Handle;

 //check if button S2 has been pressed
 volatile bool ALERT_S2_PRESSED;


/******************************************************************************
* Init the push button
******************************************************************************/
 void Task_Push_Button_Init(void)
 {
     //init the S2 button
     P3->DIR &= ~BIT5;

     T32_1_Interrupt_10ms();

 }

 /*******************************************************************************
 * Function Name: ece353_MKII_S2
 ********************************************************************************
 * Summary: Check if s2 is pressed or not
 * Returns:
 *  true or false
 *******************************************************************************/
 bool ece353_MKII_S2(void){
     if((P3->IN & BIT5) == 0){
           return true;
       }else{
           return false;
       }
 }


 /******************************************************************************
 * Used to start an timer32 interrupt 10ms
 ******************************************************************************/
 void T32_1_Interrupt_10ms(void)
 {
     TIMER32_1->CONTROL = 0;

        TIMER32_1->LOAD = 239999;//24000000 * 0.01- 1

        //enable the timer 32 interrupt in NVIC
        //__enable_irq();
        NVIC_EnableIRQ(T32_INT1_IRQn);
        NVIC_SetPriority(T32_INT1_IRQn, 2);

        TIMER32_1->CONTROL = TIMER32_CONTROL_ENABLE | //timer on
                             TIMER32_CONTROL_MODE |  //periodic
                             TIMER32_CONTROL_SIZE |  //32 bit timer
                             TIMER32_CONTROL_IE;  //enable interrupt
 }


/******************************************************************************
* Bottom Half Task.  Examines the ADC data from the joy stick on the MKII
******************************************************************************/
void Task_Push_Button_Bottom_Half(void *pvParameters)
{

 // wait for the task notification
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    while(1)
    {
        if(ALERT_S2_PRESSED){
            TANK_MSG_t message = {CMD_AMMO, 1};
            xQueueSendToBack(Queue_Tank, &message, portMAX_DELAY);
            ALERT_S2_PRESSED = false;
        }
//
      vTaskDelay(pdMS_TO_TICKS(5)); //SPEED
    }

}


/*******************************************************************************
* Function Name: T32_INT1_IRQHandler()
********************************************************************************
* Summary: Debounce the button
* Returns:
*  true or false
*******************************************************************************/
void T32_INT1_IRQHandler(){

    BaseType_t xHigherPriorityTaskWoken;

    // initialize the button state
    static uint8_t button_state = 0x00;

    bool button_pressed = ece353_MKII_S2();

    // debounce push button
    button_state = button_state << 1;

    if(button_pressed){
        button_state |= 0x01;
    }

    // S2 has been pressed for a minimum of 70mS.
    if(button_state == 0x7F){
        ALERT_S2_PRESSED = true;

        // send the notification to the push button bottom task
       vTaskNotifyGiveFromISR(
               Task_Push_Button_Handle,
               &xHigherPriorityTaskWoken
       );

      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }

    // Clear the timer interrupt
    TIMER32_1->INTCLR = BIT0;

}



