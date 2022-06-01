/*
 * buzzer.h
 * Author: Ruiwen Wang
 */

#ifndef BUZZER_H_
#define BUZZER_H_


#include <stdbool.h>
#include <stdint.h>
#include "msp.h"

/**
 * initializing the buzzer
 *  *  PARAMETER
 *      uint16_t ticks_period: period of playing a note
 *     RETURNS
 *      NONE
 */
void MKII_Buzzer_Init(uint16_t ticks_period);


/*****************************************************
 * TURNING THE BUZZER ON
 *  PARAMETER
 *      NONE
 *     RETURNS
 *      NONE
 *
*****************************************************/
void MKII_Buzzer_on(void);

/*****************************************************
 * TURNING THE BUZZER OFF
 *  PARAMETER
 *      NONE
 *  RETURNS
 *      NONE
 *
*****************************************************/
void MKII_Buzzer_off(void);


#endif /* BUZZER_H_ */
