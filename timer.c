//my libs
#include "project.h"

//PIC libs
#include <xc.h>
#include "DAC.h"
#include <libpic30.h>
#include <stdbool.h>

//generated libs
#include "i2c2.h"
#include "i2c1.h"
#include "timer.h"

//std libs
#include <stdio.h>

#define NUMBER_OF_TIMERS    5

//local functions
void _timers_update();
void _timer_int_func();

//local vars
timer_t timers[NUMBER_OF_TIMERS];

void timer_init(){
    TMR1_SetInterruptHandler(_timer_int_func);
}

timer_t* timer_create(){
    for(uint8_t i = 0; i < NUMBER_OF_TIMERS; i++){
        if(timers[i].state == TIMER_STATE_NEW){
            return &timers[i];
        }
    }
    return NULL;
}
void timer_start_countdown(timer_t* timer, uint16_t ms){
    timer->remaining_ms = ms;
    timer->state = TIMER_STATE_RUNNING;
}
bool timer_has_finished(timer_t* timer){
    return timer->state == TIMER_STATE_FINISHED;
}
void timer_stop(timer_t* timer){
    timer->remaining_ms = 0;
    timer->state = TIMER_STATE_STOPPED;
}
void timer_delete(timer_t* timer){
    timer->remaining_ms = 0;
    timer->state = TIMER_STATE_NEW;
}

void _timers_update(){
    for(uint8_t i = 0; i<NUMBER_OF_TIMERS; i++){
        if(timers[i].state == TIMER_STATE_RUNNING){
            if(timers[i].remaining_ms > 0){
                timers[i].remaining_ms--;
            }else{
                timers[i].remaining_ms = 0;
                timers[i].state = TIMER_STATE_FINISHED;
            }
        }
    }
}


void _timer_int_func(){
    static uint8_t tick = 0;
    tick++;
    //count to 2 to get millisecs, since timer interrupt is configured for 0.5ms by tmr1.c
    if(tick == 2){
       _timers_update();
       tick = 0;
    }
}