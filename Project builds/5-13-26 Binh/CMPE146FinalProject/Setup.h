#ifndef SETUP_H
#define SETUP_H

#include <stdint.h>

void setup1();
void send_message(char* msg); //  display message function 
void wait_state(); 
void start_game(); 
void game_state(); 
void timer_start();
uint32_t get_time_ms();

void print_leaderboard();
void update_leaderboard(uint32_t score);
void reset_leaderboard();

#endif