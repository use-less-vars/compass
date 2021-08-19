/* 
 * File:   timer.h
 * Author: joek
 *
 * Created on 19. August 2021, 01:02
 */

#ifndef TIMER_H
#define	TIMER_H

#include <stdbool.h>
#include <stdint.h>

#define TIMER_STATE_NEW       0
#define TIMER_STATE_RUNNING   1
#define TIMER_STATE_STOPPED   2
#define TIMER_STATE_FINISHED  3

typedef struct{
    int32_t remaining_ms;
    uint8_t state;
}timer_t;

#ifdef	__cplusplus
extern "C" {
#endif
 
//interface
void timer_init();
timer_t* timer_create();
void timer_start_countdown(timer_t* timer, uint16_t ms);
bool timer_has_finished(timer_t* timer);
void timer_stop(timer_t* timer);
void timer_delete(timer_t* timer);


#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_H */

