/*
 * task_tank.c
 *
 *  Created on: April 8, 2021
 *      Author: Jarvis Jia
 *              Ruiwen Wang
 */
#include "msp.h"
#include <main.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "buzzer.h"


#define SHIP_QUEUE_LEN  3

TaskHandle_t Task_Tank_Handle;
QueueHandle_t Queue_Tank;

AMMO_MSG amo_array[100];
TANK_MSG tank_array[100];

int index = 0;

//color of tank
uint16_t color;
//position of tank
uint8_t x = 64;
uint8_t y = 120;

//the ambient light strength
float light = 0.0;
//number of collisions between tank and bad tank
uint8_t lose_cnt = 0;
//number of bad tank that has been shot
uint8_t win_cnt = 0;
//number of bad tank that has been initialized
int bad_cnt = 0;

//used to generate temporary seed for random generation
int temp;
int temp2;
int prev_temp;
int prev_temp2;

//delay for the second and third bad tank generation queue
int delay1;

//indexes for looping to generate bad tank and checking collision
int i = 0;
int a = 0;
int b = 0;

//check if anything in screen changed
bool obj_change = true;
bool new_game = false;

//count y
int y_count = 0;

//win song note
Note_t WinSong[] =
{
    {NOTE_C6,ONE_EIGTH,true},
    {NOTE_D6,ONE_EIGTH,true},
    {NOTE_E6,ONE_EIGTH,true},
    {NOTE_F6,ONE_EIGTH,true},
    {NOTE_G6,ONE_EIGTH,true}
};

//lose song note
Note_t LoseSong[] =
{
    {NOTE_C4,ONE_EIGTH,true},
    {NOTE_G6,ONE_EIGTH,true},
    {NOTE_F6,ONE_EIGTH,true},
    {NOTE_E6,ONE_EIGTH,true},
    {NOTE_D6,ONE_EIGTH,true},
    {NOTE_C6,ONE_EIGTH,true}
};

/******************************************************************************
 * This function will initialize Queue_Tank and initialize the LCD
 ******************************************************************************/
void Task_Tank_Init(void)
{
    i2c_init();
    //initializing ambient light
    opt3001_init();
    Queue_Tank = xQueueCreate(SHIP_QUEUE_LEN,sizeof(TANK_MSG_t));

    // initialize the LCD
    Crystalfontz128x128_Init();

    //initialize ammo and tank array
    int i = 0;
    for(i = 0; i < 100; i ++){
        amo_array[i].x = 0;
        amo_array[i].y = 0;
        amo_array[i].p = 0;
        amo_array[i].d = 0;
        tank_array[i].x =0;
        tank_array[i].y = -10;
        tank_array[i].p = 1;
        tank_array[i].d = 0;
    }

    //initializing counting
    bad_cnt = 0;
    lose_cnt = 0;
    win_cnt = 0;
    x = 64;
    y = 120;

    //initializing indexing
    i = 0;
    a = 0;
    b = 0;

    //initializing delay
    delay1 = 0;

    //draw the default start image
    lcd_draw_rectangle(64, 64, LCD_HORIZONTAL_MAX, LCD_HORIZONTAL_MAX, LCD_COLOR_BLACK);
    lcd_draw_image(64,64,startWidthPixels,startHeightPixels,startBitmaps,LCD_COLOR_WHITE,LCD_COLOR_BLACK);
}

// ammo collide tank to let tank vanish
void tank_ammo(void){
    TANK_MSG_t message;

    int i = 0;
    int j = 0;
    for(i = 0; i < 100; i ++){
      for(j = 0; j < 100; j ++){
        if((tank_array[i].p == 1)&&(amo_array[j].p == 1)){
            if((amo_array[j].x > ( tank_array[i].x - 15)) &&
              (amo_array[j].x < ( tank_array[i].x + 15)) &&
              ((amo_array[j].y - 3) < tank_array[i].y )){
                amo_array[j].d = 1;
                tank_array[i].d = 1;
                win_cnt ++;

                //when shoot ten bad tanks, win the game
                if(win_cnt == 15){
                       //draw the winning image
                       lcd_draw_rectangle(64, 64, LCD_HORIZONTAL_MAX, LCD_HORIZONTAL_MAX, LCD_COLOR_MAGENTA);
                       lcd_draw_image(64,64,wINWidthPixels,wINHeightPixels,wINBitmaps,
                                      LCD_COLOR_YELLOW,LCD_COLOR_MAGENTA);
                       new_game = true;

                       //play the winning song
                       int m;
                       for(m = 0; m < 5; m++){//loop through the song
                           //initialize the Buzzer
                           MKII_Buzzer_Init(WinSong[m].period);
                           //turn the buzzer on and start playing the note
                           MKII_Buzzer_on();
                           vTaskDelay(pdMS_TO_TICKS(250));
                           MKII_Buzzer_off();
                       }

                       //wait user to press S2 to begin new game
                       while(!(message.cmd == CMD_AMMO)){xQueueReceive(Queue_Tank, &message, portMAX_DELAY);}
                       Task_Tank_Init();
           }
         }
       }
     }
  }
}

// check tank and bad tank collide
void tank_bad_tank(void){
    TANK_MSG_t message;
    int m;
    int q = 0;
    for(q = 0; q < 100; q ++){
      if(tank_array[q].p == 1){
        if((x + 15 ) > ( tank_array[q].x - 15) && ( tank_array[q].x + 15) > (x - 15)){
           if((y + 7 ) > ( tank_array[q].y - 7) && ( tank_array[q].y + 7) > (y - 7)){
              tank_array[q].d=1;
              lose_cnt ++;

              //tank and bad tank collide once, and lose one heart
              if(lose_cnt ==1){
                 lcd_draw_image(10,40,heartWidthPixels,heartHeightPixels,heartBitmaps,LCD_COLOR_BLACK,LCD_COLOR_BLACK);

               }

              //tank and bad tank collide the twice time, and lose the second heart
              else if(lose_cnt ==2){
                   lcd_draw_image(10,25,heartWidthPixels,heartHeightPixels,heartBitmaps,LCD_COLOR_BLACK,LCD_COLOR_BLACK);

                  }

              //tank and bad tank collide the third time, and lose the game
              else if(lose_cnt == 3){
                      lcd_draw_rectangle(64, 64, LCD_HORIZONTAL_MAX, LCD_HORIZONTAL_MAX, LCD_COLOR_RED);
                      lcd_draw_image(64,64,loseWidthPixels,loseHeightPixels,loseBitmaps,LCD_COLOR_RED,LCD_COLOR_BLACK);
                      for(m = 1; m < 6; m++){//loop through the song
                          //initialize the Buzzer
                          //set duty cycle to 50% and load note period to timerA
                          MKII_Buzzer_Init(LoseSong[m].period);
                          //turn the buzzer on and start playing the note
                          MKII_Buzzer_on();
                          vTaskDelay(pdMS_TO_TICKS(250));
                          //when finish this note, turn off the buzzer
                          MKII_Buzzer_off();
                      }

              //press S2 to start a new game
              while(!(message.cmd == CMD_AMMO)){xQueueReceive(Queue_Tank, &message, portMAX_DELAY);}
              Task_Tank_Init();
              new_game = true;
              }
            }
         }
      }
  }
}

/******************************************************************************
 * This task manages the movement of the space ship. The joystick task or the
 * console task can send messages to SHIP_QUEUE_LEN
 ******************************************************************************/
void Task_Tank(void *pvParameters)
{
    TANK_MSG_t message;  // message to receive

    i = 0;
    a = 0;
    b = 0;

    while(!(message.cmd == CMD_AMMO)){xQueueReceive(Queue_Tank, &message, portMAX_DELAY);}

    //////Draw initial images////////
    //tank.
    lcd_draw_rectangle(64, 64, LCD_HORIZONTAL_MAX, LCD_HORIZONTAL_MAX, LCD_COLOR_BLACK);
    lcd_draw_image(x,y,tankWidthPixels,tankHeightPixels,tankBitmaps,LCD_COLOR_GREEN,LCD_COLOR_BLACK);

    //three hearts
    lcd_draw_image(10,10,heartWidthPixels,heartHeightPixels,heartBitmaps,LCD_COLOR_RED,LCD_COLOR_BLACK);
    lcd_draw_image(10,25,heartWidthPixels,heartHeightPixels,heartBitmaps,LCD_COLOR_RED,LCD_COLOR_BLACK);
    lcd_draw_image(10,40,heartWidthPixels,heartHeightPixels,heartBitmaps,LCD_COLOR_RED,LCD_COLOR_BLACK);

    while(1)
        {
                ////Color Detection//////
                //read the ambient light and determine the color of tank
                light = opt3001_read_lux();
                if(light < 200.0){
                    color = LCD_COLOR_BLUE2; //change tank color to blue
                }else if (light < 500.0){
                    color =LCD_COLOR_GREEN; //change tank color to GREEN
                }else{
                    color = LCD_COLOR_YELLOW;//change tank color to YELLOW
                }

            /////wait for message to receive/////
            xQueueReceive(Queue_Tank, &message, portMAX_DELAY);

               ////Tank Movement Direction Detection//////
                // receive the shift right command
                if(message.cmd == CMD_RIGHT){
                        // limit the space ship inside the screen
                        if(x <= 104){x ++;}
                        lcd_draw_image(x,y,tankWidthPixels,tankHeightPixels,tankBitmaps,color,LCD_COLOR_BLACK);

                // receive the shift left command
                } else if(message.cmd == CMD_LEFT){
                        // limit the space ship inside the screen
                       if(x >= 28){ x --;}
                       lcd_draw_image(x,y,tankWidthPixels,tankHeightPixels,tankBitmaps,color,LCD_COLOR_BLACK);
                // receive the shift up command
                }  else if(message.cmd == CMD_UP){
                        // limit the space ship inside the screen
                        if(y >= 22){y--;}
                        lcd_draw_image(x,y,tankWidthPixels,tankHeightPixels,tankBitmaps,color,LCD_COLOR_BLACK);

                // receive the shift down command
                }  else if(message.cmd == CMD_DOWN){
                     // limit the space ship inside the screen
                    if(y <= 120){y++;}
                    lcd_draw_image(x,y,tankWidthPixels,tankHeightPixels,tankBitmaps,color,LCD_COLOR_BLACK);
                } else if(message.cmd == CMD_CENTER){
                    lcd_draw_image(x,y,tankWidthPixels,tankHeightPixels,tankBitmaps,color,LCD_COLOR_BLACK);
                }

                //send out ammo
                if(message.cmd == CMD_AMMO){
                    index = index % 100;
                    amo_array[index].x = x;
                    amo_array[index].y = y - 10;
                    amo_array[index].p = 1;
                    index++;
                }

               // describe the ammo
               for(i = 0; i < 100; i ++){
                     if(amo_array[i].p != 0){
                         lcd_draw_image(amo_array[i].x,amo_array[i].y,ammoWidthPixels, ammoHeightPixels,
                             ammoBitmaps,color,LCD_COLOR_BLACK);
                         amo_array[i].y--;
                     }

                     if(amo_array[i].y < 10 &&  amo_array[i].p==1){
                         lcd_draw_image(amo_array[i].x,amo_array[i].y,ammoWidthPixels,ammoHeightPixels,
                           ammoBitmaps,LCD_COLOR_BLACK,LCD_COLOR_BLACK);
                        amo_array[i].x=0;
                        amo_array[i].y=0;
                        amo_array[i].p=0;
                    }

                     if(amo_array[i].d == 1 &&  amo_array[i].p==1){
                            lcd_draw_image(amo_array[i].x,amo_array[i].y,ammoWidthPixels,ammoHeightPixels,
                              ammoBitmaps,LCD_COLOR_BLACK,LCD_COLOR_BLACK);
                           amo_array[i].x=0;
                           amo_array[i].y=0;
                           amo_array[i].p=0;
                           amo_array[i].d=0;
                     }
               }

               tank_ammo();  //check ammo and enermy tank

               //check if new game starts
               if(new_game){
                   xQueueReceive(Queue_Tank, &message, portMAX_DELAY);
                   while(!(message.cmd == CMD_AMMO)){
                       xQueueReceive(Queue_Tank, &message, portMAX_DELAY);
                   }

                   // Draw the initial starting image of the tank.
                   lcd_draw_rectangle(64, 64, LCD_HORIZONTAL_MAX, LCD_HORIZONTAL_MAX, LCD_COLOR_BLACK);
                   lcd_draw_image(x,y,tankWidthPixels,tankHeightPixels,tankBitmaps,
                        LCD_COLOR_GREEN, LCD_COLOR_BLACK);
                    lcd_draw_image(10,10,heartWidthPixels,heartHeightPixels,
                        heartBitmaps,LCD_COLOR_RED,LCD_COLOR_BLACK);

                    lcd_draw_image(10,25,heartWidthPixels,heartHeightPixels,
                        heartBitmaps,LCD_COLOR_RED,LCD_COLOR_BLACK);

                    lcd_draw_image(10,40,heartWidthPixels,heartHeightPixels,
                        heartBitmaps,LCD_COLOR_RED,LCD_COLOR_BLACK);
                    new_game = false;
               }

               tank_bad_tank();  //check lose condition

               //check if new game starts
               if(new_game){
                   while(!(message.cmd == CMD_AMMO)){xQueueReceive(Queue_Tank, &message, portMAX_DELAY);}
                   lcd_draw_rectangle(64, 64, LCD_HORIZONTAL_MAX, LCD_HORIZONTAL_MAX, LCD_COLOR_BLACK);
                   lcd_draw_image(x,y,tankWidthPixels,tankHeightPixels,tankBitmaps,
                        LCD_COLOR_GREEN,LCD_COLOR_BLACK);
                    lcd_draw_image(10,10,heartWidthPixels,heartHeightPixels,heartBitmaps,
                         LCD_COLOR_RED,LCD_COLOR_BLACK);
                    lcd_draw_image(10,25,heartWidthPixels,heartHeightPixels,heartBitmaps,
                         LCD_COLOR_RED,LCD_COLOR_BLACK);
                    lcd_draw_image(10,40,heartWidthPixels,heartHeightPixels,heartBitmaps,
                                             LCD_COLOR_RED,LCD_COLOR_BLACK);
                    new_game = false;
               }

               //get random seed from ambient light
               temp =  opt3001_read_lux();
               if(prev_temp == temp){
                   temp = temp + 33;
               }
               srand(temp);
               prev_temp = temp;

               //first level only one tank appears
               if(delay1 < 300){
                   for(b = 0;b < 100; b ++){
                       // init the tank position
                       if(bad_cnt >= 98){

                       }else{
                           tank_array[b].x = rand() % 72 + 38;
                       }

                       if(tank_array[b].p != 0){
                          lcd_draw_image(tank_array[b].x,tank_array[b].y,badTankWidthPixels,badTankHeightPixels,
                                     badTankBitmaps,LCD_COLOR_RED,LCD_COLOR_BLACK);
                          bad_cnt ++; //add bad tank count
                          tank_array[b].y++; //increase bad tank y
                          if(bad_cnt >= 98){
                                b = 99;
                          }
                          delay1++; //add level


                         }
                   }
               }

               //second level two tanks appear the same time
               if(delay1 >= 300){
                   // init the tank position
                   for(b = 0;b < 50; b ++){
                       if(bad_cnt >= 98){
                          // tank_array[b].x = tank_array[b].x;
                       }else{
                           tank_array[b].x = (rand()+36) % 15 + 90;

                       }
                       if(tank_array[b].p != 0){
                           lcd_draw_image(tank_array[b].x,tank_array[b].y,badTankWidthPixels,badTankHeightPixels,
                                                            badTankBitmaps,LCD_COLOR_RED,LCD_COLOR_BLACK);
                           bad_cnt ++;//add bad tank count
                           tank_array[b].y++;
                           if(bad_cnt >= 98){b = 49;}
                           delay1++;
                         }
                       y_count ++;
                    }

                   if(y_count > 10){
                       // init the tank position
                       for(b = 51;b < 100; b ++){
                            if(bad_cnt >= 98){
                               // tank_array[b].x = tank_array[b].x;
                            }else{
                                tank_array[b].x = (rand()+36) % 15 + 38;

                            }
                            if(tank_array[b].p != 0){
                                lcd_draw_image(tank_array[b].x,tank_array[b].y,badTankWidthPixels,badTankHeightPixels,
                                                                 badTankBitmaps,LCD_COLOR_RED,LCD_COLOR_BLACK);
                                bad_cnt ++;//add bad tank count
                                tank_array[b].y++;
                                if(bad_cnt >= 98){b = 99;}
                              }
                         }

                   }
               }

               //third level two tank speed up first time
               if(delay1 >= 500){
                   // init the tank position
                   for(b = 0;b < 50; b ++){
                         if(bad_cnt >= 98){

                         }else{
                             tank_array[b].x = (rand()+36) % 15 + 90;
                         }
                         if(tank_array[b].p != 0){
                             lcd_draw_image(tank_array[b].x,tank_array[b].y,badTankWidthPixels,badTankHeightPixels,
                                                              badTankBitmaps,LCD_COLOR_RED,LCD_COLOR_BLACK);
                             bad_cnt ++;//add bad tank count
                             tank_array[b].y++;
                             if(bad_cnt >= 98){b = 49;}
                             delay1++;
                           }
                      }

                     for(b = 51;b < 100; b ++){
                         // init the tank position
                          if(bad_cnt >= 98){
                             // tank_array[b].x = tank_array[b].x;
                          }else{
                              tank_array[b].x = (rand()+36) % 15 + 38;
                          }
                          if(tank_array[b].p != 0){
                              lcd_draw_image(tank_array[b].x,tank_array[b].y,badTankWidthPixels,badTankHeightPixels,
                                                               badTankBitmaps,LCD_COLOR_RED,LCD_COLOR_BLACK);
                              bad_cnt ++;//add bad tank count
                              tank_array[b].y++;
                              if(bad_cnt >= 98){b = 99;}
                            }
                       }
               }

               // fourth level two tanks speed up second time
               if(delay1 >= 800){
                   // init the tank position
                 for(b = 0;b < 50; b ++){
                       if(bad_cnt >= 98){
                          // tank_array[b].x = tank_array[b].x;
                       }else{
                           tank_array[b].x = (rand()+36) % 15 + 90;
                       }
                       if(tank_array[b].p != 0){
                           lcd_draw_image(tank_array[b].x,tank_array[b].y,badTankWidthPixels,badTankHeightPixels,
                                                            badTankBitmaps,LCD_COLOR_RED,LCD_COLOR_BLACK);
                           bad_cnt ++;//add bad tank count
                           tank_array[b].y++;
                           if(bad_cnt >= 98){b = 49;}
                           delay1++;
                         }
                    }

                     // init the tank position
                       for(b = 51;b < 100; b ++){
                            if(bad_cnt >= 98){
                               // tank_array[b].x = tank_array[b].x;
                            }else{
                                tank_array[b].x = (rand()+36) % 15 + 38;
                            }
                            if(tank_array[b].p != 0){
                                lcd_draw_image(tank_array[b].x,tank_array[b].y,badTankWidthPixels,badTankHeightPixels,
                                                                 badTankBitmaps,LCD_COLOR_RED,LCD_COLOR_BLACK);
                                bad_cnt ++;//add bad tank count
                                tank_array[b].y++;
                                if(bad_cnt >= 98){b = 99;}
                              }
                         }
                }

               // method to delete all the dead tank image
               for(a = 0; a < 100; a ++){
                   //redraw the graph of died enemy tank
                     if(tank_array[a].y >124 &&  tank_array[a].p==1){
                         lcd_draw_image(tank_array[a].x,tank_array[a].y,badTankWidthPixels,badTankHeightPixels,
                          badTankBitmaps,LCD_COLOR_BLACK,LCD_COLOR_BLACK);
                         tank_array[a].p=0;
                         bad_cnt --;
                      }
                     if(tank_array[a].d == 1 &&  tank_array[a].p==1){
                          lcd_draw_image(tank_array[a].x,tank_array[a].y,badTankWidthPixels,badTankHeightPixels,
                           badTankBitmaps,LCD_COLOR_BLACK,LCD_COLOR_BLACK);
                         tank_array[a].p=0;
                         tank_array[a].d=0;
                         bad_cnt --;
                     }
                }
                vTaskDelay(pdMS_TO_TICKS(1));  // delay the speed time
            }

}


