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
  // srand(2343251); //supposed to randomize seed, but it keeps generating 0. Putting static value seed for now
  srand(DL_Timer_getTimerCount(TIMG12));
  int guess = rand();
  SYSCFG_DL_init();
  setup1();
  timer_start();
  srand(DL_Timer_getTimerCount(TIMG12));

  delay_cycles(32000000);

  static char something[] = {"Initializing....Welcome to First Response!\n"};
  send_message(something); // Uart Print initialization

  static char something1[] = {"S1 = start game\r\n"};
  send_message(something1);

  static char something2[] = {"S2 = reaction button\r\n"};
  send_message(something2);

  static char something3[] = {"Hold S1 in idle to reset leaderboard later if needed\r\n"};
  send_message(something3);

  print_leaderboard();

  while (1) {
    char prompt[] = {"\r\nPlease press S1 to start game\r\n"};
    send_message(prompt);

    wait_state();
    start_game();
  }

  return 0; 
}