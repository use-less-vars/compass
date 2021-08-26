//my libs
#include "project.h"
#include "timer.h"

//PIC libs
#include <xc.h>
#include "DAC.h"
#include <libpic30.h>
#include <stdbool.h>

//generated libs
#include "i2c2.h"
#include "i2c1.h"
#include "tmr1.h"


//std libs
#include <stdio.h>
#include <string.h>

#define NUMBER_OF_TIMERS    5

//local functions
void _timers_update();

//local vars
timer_t timers[NUMBER_OF_TIMERS];

void timer_init(){
    memset(timers,0,sizeof(timers));
    TMR1_SetInterruptHandler(_timers_update);
}

timer_t* timer_create(){
    for(uint8_t i = 0; i < NUMBER_OF_TIMERS; i++){
        if(timers[i].state == TIMER_STATE_NEW){
            //change state before returning the timer, such that with the next create call
            //it won't be given out twice.
            timers[i].state = TIMER_STATE_STOPPED;
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

