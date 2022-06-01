/*
 * task_tank.h
 *
 *  Created on: April 8, 2021
 *      Author: Jarvis Jia
 *              Ruiwen Wang
 */

#ifndef TASK_TANK_H_
#define TASK_TANK_H_

#include <main.h>

#include <stdint.h>
#include <stdbool.h>
#include "msp.h"
#include "buzzer.h"
typedef enum {
    CMD_LEFT,
    CMD_RIGHT,
    CMD_UP,
    CMD_DOWN,
    CMD_CENTER,
    CMD_AMMO,
    CMD_MUSIC
} TANK_CMD_t;



typedef struct {
    TANK_CMD_t cmd;
    uint8_t     value;
}TANK_MSG_t;

typedef struct {
    int    x;
    int    y;
    int    p;
    int    d;
}AMMO_MSG;

typedef struct {
    int    x;
    int    y;
    int    p;
    int    d;
    int    r;
}TANK_MSG;

#define NOTE_C4        64944
#define NOTE_C6        22944 // 1046 Hz
#define NOTE_D6        20425 // 1175 Hz
#define NOTE_E6        18195 // 1319 Hz
#define NOTE_F6        17179 // 1397 Hz
#define NOTE_G6        15305 // 1568 Hz
#define MEASURE_DURATION 12000000    // 500 milliseconds
#define MEASURE_RATIO           2    // 2/4 time
#define DELAY_AMOUNT       240000    // 10  milliseconds
typedef enum measure_time_t {
    ONE_QUARTER,
    ONE_HALF,
    ONE_EIGTH,
    THREE_EIGTH,
    ONE_SIXTEENTH,
    THREE_SIXTEENTH
} measure_time_t;
typedef struct{
    uint32_t period;
    measure_time_t time;
    bool delay;
}Note_t;

extern TaskHandle_t Task_Tank_Handle;
extern QueueHandle_t Queue_Tank;
extern uint8_t speed;
extern uint16_t color;
extern float light;

extern int temp;
extern int prev_temp;
extern int temp2;
extern int prev_temp2;
extern uint8_t lose_cnt;
extern uint8_t win_cnt;
extern int delay1;
extern int delay2;
extern int i;
//  int j = 0;
extern int a;
extern int b;
/******************************************************************************
 * This function will initialize Queue_Tank and LCD
 ******************************************************************************/
void Task_Tank_Init(void);

/******************************************************************************
 * This task manages the movement of the space ship. The joystick task or the
 * console task can send messages to the
 ******************************************************************************/
void Task_Tank(void *pvParameters);

void bad_tank_generate(void);
#endif /* TASK_Tank_H_ */
