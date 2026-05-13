#include <stdio.h>
#include <pthread.h>
#include <FreeRTOS.h>
#include <task.h>
#include "ti_msp_dl_config.h"
#include <unistd.h> //able to use sleep() and usleep() now
#include <semaphore.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Setup.h"
#include <time.h> 
#define THREADSTACKSIZE 1024
#define BAUD_RATE 115200
#define BIT_DELAY (32000000 / BAUD_RATE)

int main()

{
  srand(2343251); //supposed to randomize seed, but it keeps generating 0. Putting static value seed for now 
   SYSCFG_DL_init();
   setup1();
  delay_cycles(32000000);
 
  static char something[] = {"Initializing....Welcome to First Response!\n"};
  send_message(something); //Uart Print initialization 
 //Uart Print hello 
 //
 /**/
    //LED blink for startup, then display message to prompt user to press s1 to start 
    DL_GPIO_setPins(GPIOB,GPIO_LEDS_USER_LED_2_PIN);
  
    DL_GPIO_clearPins(GPIOB,GPIO_LEDS_USER_LED_2_PIN);
    delay_cycles(32000000);
    DL_GPIO_setPins(GPIOB,GPIO_LEDS_USER_LED_2_PIN);
    DL_GPIO_setPins(GPIOB,GPIO_LEDS_USER_LED_3_PIN);
    char something2[] = {"please press S2 to start game"};
    send_message(something2);
    wait_state();
      

    //if statement to press s1 

    return 0; 
}