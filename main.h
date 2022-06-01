/*
 * main.h
 *
 *  Created on: April 8, 2021
 *      Author: Jarvis Jia
 */


#ifndef MAIN_H_
#define MAIN_H_

#include "msp.h"
#include "msp432p401r.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>


/* RTOS header files */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

/* ECE353 Includes */
#include <task_accelerometer.h>
#include <task_tank.h>
#include <task_push_button.h>

#include <lcd.h>
#include <graphic.h>

#include "i2c.h"
#include "opt3001.h"


#endif /* MAIN_H_ */
