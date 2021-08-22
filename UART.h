/* 
 * File:   UART.h
 * Author: use-less-vars
 *
 * Created on 19. August 2021, 18:01
 */

#ifndef UART_H
#define	UART_H

typedef void (*callback_on_msg_t)(uint8_t* msg);
    
void UART_init();
void UART_register_callback_on_msg(callback_on_msg_t cb);
bool UART_update();



#endif	/* UART_H */

