#include <stdio.h>
#include <pthread.h>
#include <FreeRTOS.h>
#include <task.h>
#include "ti_msp_dl_config.h"
#include <unistd.h> 
#include <semaphore.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Setup.h"

#define CPUCLK_FREQ 32000000

// function for sending UART message one character at a time
void send_message(char *msg) {
  int i = 0;

  while (msg[i] != '\0') {
    while (DL_UART_isBusy(UART0)) {
      // wait until UART is ready
    }

    DL_UART_Main_transmitData(UART0, msg[i]);
    i++;
  }
}

// timer setup for TIMG12
void timer_start() {
  DL_Timer_enablePower(TIMG12);

  DL_Timer_ClockConfig config;
  config.clockSel = DL_TIMER_CLOCK_BUSCLK;
  config.divideRatio = DL_TIMER_CLOCK_DIVIDE_1;
  config.prescale = 0;
  DL_Timer_setClockConfig(TIMG12, &config);

  DL_Timer_TimerConfig timerConfig;
  timerConfig.timerMode = DL_TIMER_TIMER_MODE_PERIODIC_UP;
  timerConfig.period = 0xFFFFFFFF;
  timerConfig.startTimer = DL_TIMER_START;
  timerConfig.genIntermInt = DL_TIMER_INTERM_INT_DISABLED;
  timerConfig.counterVal = 0;

  DL_Timer_initTimerMode(TIMG12, &timerConfig);
}

// returns current timer value converted to ms
uint32_t get_time_ms() {
  uint32_t count = DL_Timer_getTimerCount(TIMG12);
  return count / 32000;
}