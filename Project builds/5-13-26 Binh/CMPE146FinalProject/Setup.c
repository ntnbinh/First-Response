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

#define FALSE_START_PENALTY 500
#define BAUD_RATE 115200
#define BIT_DELAY (32000000 / BAUD_RATE)

// simple top 5 leaderboard in RAM for now
uint32_t leaderboard[5] = {99999, 99999, 99999, 99999, 99999};

void setup1()
{
    // power on GPIO
    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    delay_cycles(POWER_STARTUP_DELAY);

    // red LED1 = PA0
    DL_GPIO_initDigitalOutput(IOMUX_PINCM1);
    DL_GPIO_enableOutput(GPIOA, DL_GPIO_PIN_0);

    // green LED2 = PB27
    DL_GPIO_initDigitalOutput(IOMUX_PINCM58);
    DL_GPIO_enableOutput(GPIOB, DL_GPIO_PIN_27);

    // make sure LEDs start off
    DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_0);
    DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_27);

    // S1 = PA18
    DL_GPIO_initDigitalInput(IOMUX_PINCM40);
    DL_GPIO_initDigitalInputFeatures(
        IOMUX_PINCM40,
        DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE,
        DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE
    );

    // S2 = PB21
    DL_GPIO_initDigitalInputFeatures(
        IOMUX_PINCM49,
        DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_PULL_UP,
        DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE
    );


    // UART Setup Code
    static const DL_UART_Main_ClockConfig gUART_0ClockConfig = {
        .clockSel = DL_UART_MAIN_CLOCK_BUSCLK,
        .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
    };

    static const DL_UART_Main_Config gUART_0Config = {
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
    DL_UART_Main_setClockConfig(UART0, (DL_UART_Main_ClockConfig *)&gUART_0ClockConfig);
    DL_UART_Main_init(UART0, (DL_UART_Main_Config *)&gUART_0Config);
    DL_UART_Main_setOversampling(UART0, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART0, 17, 23);
    DL_UART_Main_enable(UART0);

    DL_GPIO_initPeripheralOutputFunction(IOMUX_PINCM21, IOMUX_PINCM21_PF_UART0_TX);
    DL_GPIO_initPeripheralInputFunction(IOMUX_PINCM22, IOMUX_PINCM22_PF_UART0_RX);
    // UART Setup Code End
}

// prints top 5
void print_leaderboard()    {
    char buffer[80];

    send_message("\r\n--- Leaderboard ---\r\n");

    for (int i = 0; i < 5; i++) {
        if (leaderboard[i] == 99999) {
            sprintf(buffer, "%d. ---\r\n", i + 1);
        } else {
            sprintf(buffer, "%d. %lu ms\r\n", i + 1, (unsigned long)leaderboard[i]);
        }

        send_message(buffer);
    }
}

// resets scoreboard
void reset_leaderboard()    {
    for (int i = 0; i < 5; i++) {
        leaderboard[i] = 99999;
    }

    send_message("\r\nLeaderboard reset.\r\n");
    print_leaderboard();
}

// inserts score into sorted leaderboard
void update_leaderboard(uint32_t score) {
    for (int i = 0; i < 5; i++) {
        if (score < leaderboard[i]) {
        for (int j = 4; j > i; j--) {
            leaderboard[j] = leaderboard[j - 1];
        }

        leaderboard[i] = score;
        break;
        }
    }
}

// waits in idle until S1 is pressed
void wait_state()   {
    uint32_t start_hold = 0;
    uint32_t end_hold = 0;
    uint32_t hold_time = 0;

    while(1)    {
        // S1 pressed? active low
        if(DL_GPIO_readPins(GPIOA, DL_GPIO_PIN_18) != 0)    {
            start_hold = get_time_ms();

            while(DL_GPIO_readPins(GPIOA, DL_GPIO_PIN_18) != 0) {
                // wait until released
            }

            end_hold = get_time_ms();
            hold_time = end_hold - start_hold;

            delay_cycles(320000); // small debounce

            // if held long enough, reset leaderboard
            if(hold_time > 2000)    {
                reset_leaderboard();
                send_message("\r\npress S1 to start game\r\n");
            } else {
                return; // short press starts game
            }
        }
    }
}

void start_game()   {
    game_state();
   
}
void game_state()
{
    volatile uint8_t *s2   = ((volatile uint8_t *)&GPIOB->DIN23_20) + 1; // S2 Button pointer 
    int timer = 0;  
    int state = 0;
    bool false_start = false;

    DL_GPIO_clearPins(GPIOB,GPIO_LEDS_USER_LED_2_PIN);
    while(state == 0)
    {
        // random delay from 1500 ms to 4500 ms
        int guess = 1500 + (rand() % 3001);

        char msg1[60];
        sprintf(msg1, "\r\nWaiting %d ms before LED turns on...\r\n", guess);
        send_message(msg1);

        for(int i = 0; i < guess; ++i)
        {
            if(*s2 == 0) // if pressed before lights up 
            {
                char something7[] = "Pressed too early! Penalty of 500ms Added";
                send_message(something7);
                timer = timer + FALSE_START_PENALTY; // check for proper conversion later 
                false_start = true;
                    
                // blink red LED quickly
                for(int j = 0; j < 3; j++)
                {
                    DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_0); // red on
                    delay_cycles(4800000);
                    DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_0);   // red off
                    delay_cycles(4800000);
                }

                // wait until button released so it doesn't instantly trigger again
                while(*s2 == 0)
                {
                }

                break;
            }
            delay_cycles(32000);
        }

        state = 1;
    } 

    // turn green LED on
    DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_27);
    send_message("\r\nGO! Press S2!\r\n");

    // reset and start timer at stimulus moment
    timer_start();
    uint32_t t_start = get_time_ms();

    // wait for S2 press
    while (*s2 != 0) { }

    uint32_t t_end = get_time_ms();

    // turn green LED off
    DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_27);

    // wait for release
    while (*s2 == 0) { }

    // calculate final reaction time including any penalty
    uint32_t reaction_time = (t_end - t_start) + (uint32_t)timer;

    char result[80];
    if (false_start)    {
        sprintf(result, "\r\nReaction time: %lu ms (includes %d ms penalty)\r\n", (unsigned long)reaction_time, timer);
    } else    {
        sprintf(result, "\r\nReaction time: %lu ms\r\n", (unsigned long)reaction_time);
    }

    send_message(result);

    update_leaderboard(reaction_time);
    print_leaderboard();
        
}
