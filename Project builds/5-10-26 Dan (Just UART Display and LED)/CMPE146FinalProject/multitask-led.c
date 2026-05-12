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
#define THREADSTACKSIZE 1024
#define BAUD_RATE 115200
#define BIT_DELAY (32000000 / BAUD_RATE)


void send_message(char* msg) //  display message function 
{
    int index = 0;


    while(msg[index] != '\0')
    {
        DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_10);
        delay_cycles(BIT_DELAY);


        for(int bit = 0; bit < 8; bit++)    {
            if(msg[index] & (1 << bit)) {
                DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_10);
            }
            else {
                DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_10);
            }
            delay_cycles(BIT_DELAY);
        }



            UART0->TXDATA = msg[index];
        DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_10);
        delay_cycles(BIT_DELAY);


        index++;
    }
}



int main()
{
   SYSCFG_DL_init();
//UART Setup Code 
static const DL_UART_Main_ClockConfig gUART_0ClockConfig = 
{
    .clockSel = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};
static const DL_UART_Main_Config gUART_0Config = 
{
    .mode = DL_UART_MAIN_MODE_NORMAL,
    .direction = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity = DL_UART_MAIN_PARITY_NONE,
    .wordLength = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits = DL_UART_MAIN_STOP_BITS_ONE
};

DL_UART_Main_reset(UART0);
DL_UART_Main_enablePower(UART0);
delay_cycles(POWER_STARTUP_DELAY);
DL_UART_Main_setClockConfig(UART0, (DL_UART_Main_ClockConfig *) &gUART_0ClockConfig);
DL_UART_Main_init(UART0, (DL_UART_Main_Config *) &gUART_0Config);
DL_UART_Main_setOversampling(UART0, DL_UART_OVERSAMPLING_RATE_16X);
DL_UART_Main_setBaudRateDivisor(UART0, 17, 23);
DL_UART_Main_enable(UART0);
DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM21, IOMUX_PINCM21_PF_UART0_TX);
DL_GPIO_initPeripheralInputFunction(IOMUX_PINCM22, IOMUX_PINCM22_PF_UART0_RX);
//UART Setup Code End 

    //LED Setupcode 
    DL_GPIO_initDigitalOutput(IOMUX_PINCM21);
    DL_GPIO_initDigitalInput(IOMUX_PINCM22);
    DL_GPIO_enableOutput(GPIOA, DL_GPIO_PIN_10);
    DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_10);
    DL_GPIO_enablePower(GPIOB);
    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM50);
    DL_GPIO_enableOutput(GPIOB, DL_GPIO_PIN_22);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM1);
    DL_GPIO_enableOutput(GPIOA, DL_GPIO_PIN_0);

    DL_GPIO_initPeripheralInputFunctionFeatures(
        IOMUX_PINCM49,
        IOMUX_PINCM49_PF_GPIOB_DIO21,
        DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE
    );
    //LED Setup Code End 

    delay_cycles(32000000 * 0.1);
 
  static char something[] = {"Initializing....Welcome to First Response!"};

  send_message(something); //Uart Print initialization 
 //Uart Print hello 
 //
 /**/
    //LED blink for startup, then display message to prompt user to press s1 to start 
    DL_GPIO_setPins(GPIOB,GPIO_LEDS_USER_LED_2_PIN);
    delay_cycles(32000000);
    DL_GPIO_clearPins(GPIOB,GPIO_LEDS_USER_LED_2_PIN);
    delay_cycles(32000000);
    DL_GPIO_setPins(GPIOB,GPIO_LEDS_USER_LED_2_PIN);
    DL_GPIO_setPins(GPIOB,GPIO_LEDS_USER_LED_3_PIN);
    char something2[] = {"please press S1 to start game"};
    send_message(something2);

    //if statement to press s1 

 
   



  
    return 0; 
}