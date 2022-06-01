/*
 * buzzer.c
 *
 * Author: Ruiwen Wang
 */

#include "buzzer.h"

/**
 * initializing the buzzer
 * set PWM duty cycle to 50% and period according to the input
 *
 *  *  PARAMETER
 *      uint16_t ticks_period: period of playing a note
 *     RETURNS
 *      NONE
 */
void MKII_Buzzer_Init(uint16_t ticks_period){
    //set P2.7 GPIO OUTPUT Pin
    P2->DIR |= BIT7;

    //Set TimerA to control the Buzzer
    P2->SEL0 |= BIT7;
    P2->SEL1 &= ~BIT7;

    //turn off timerA
    TIMER_A0->CTL = 0;

    //set the period of timerA to note.period
    TIMER_A0->CCR[0] = ticks_period -1;

    //configure the duty cycle of timerA to 50%
    TIMER_A0->CCR[4] =  (ticks_period /2) -1;

    //set TA0.4 to set/reset mode
    TIMER_A0->CCTL[4] = TIMER_A_CCTLN_OUTMOD_7;
    //timer source: master clock
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK;
}



/*****************************************************
 * TURNING THE BUZZER ON
 *  PARAMETER
 *      NONE
 *     RETURNS
 *      NONE
 *
*****************************************************/
void MKII_Buzzer_on(void)
{
    //clear the current mode control bits
    TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;
    //set mode control to UP and Clear the current count
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;
}

/*****************************************************
 * TURNING THE BUZZER OFF
 *  PARAMETER
 *      NONE
 *  RETURNS
 *      NONE
 *
*****************************************************/
void MKII_Buzzer_off(void)
{
    //clear the current mode control bits
    TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;
}




