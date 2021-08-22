//generated libs (MCC))
#include "pin_manager.h"
#include "system.h"
#include "mcc_generated_files/uart1.h"
#include "interrupt_manager.h"

//my libs
#include "project.h"
#include "UART.h"

//PIC libs
#include <libpic30.h>
#include <xc.h>

//system libs
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#define RX_BUFFER_SIZE  20

//message states
#define RX_BUFFER_EMPTY             0
#define RX_BUFFER_PART_MSG_RXED     1
#define RX_BUFFER_FULL_MSG_RXED     2
#define RX_BUFFER_ERROR             3

//local types
typedef struct{
    uint8_t rx_input[RX_BUFFER_SIZE];
    uint8_t message_status;
    uint8_t rx_index;
}uart_state_t;

//local vars
uart_state_t uart;

//local functions
void _UART1_Receive_CallBack();



callback_on_msg_t callback_on_msg; 

void UART_init(){
    //init state machine
    memset(uart.rx_input,0,RX_BUFFER_SIZE);
    uart.message_status = RX_BUFFER_EMPTY;
    uart.rx_index = 0;
    UART1_SetRxInterruptHandler(_UART1_Receive_CallBack);
    
}

//The _U1RXInterrupt does something similar, but it seems to be wrong/useless.
//The old code remains in the uart1.c but is of no use. 
void _UART1_Receive_CallBack(){
    uint8_t current_byte = 0;
    while((U1STAbits.URXDA == 1))
    {   
        current_byte = U1RXREG;
        switch(uart.message_status){
            case RX_BUFFER_EMPTY:
                memset(uart.rx_input,0,RX_BUFFER_SIZE);
                uart.rx_index = 0;
                uart.rx_input[uart.rx_index] = current_byte;
                uart.rx_index++;
                uart.message_status = RX_BUFFER_PART_MSG_RXED;
            break;
            case RX_BUFFER_PART_MSG_RXED:
                if(uart.rx_index == RX_BUFFER_SIZE - 1){
                    uart.message_status = RX_BUFFER_ERROR;
                    break;
                }
                uart.rx_input[uart.rx_index] = current_byte;
                uart.rx_index++;
                if(current_byte == '\r'){
                    uart.message_status = RX_BUFFER_FULL_MSG_RXED;
                }
            break;
            case RX_BUFFER_FULL_MSG_RXED:
                //do nothing until the STM is reset externally after the user has
                // read/used it.
            break;
            case RX_BUFFER_ERROR:
                //do nothing until the STM is reset externally. The states
                // FULL and ERROR are needed for the user to decide whether to throw away
                // the message or not.      
            break;
            default:
            break;
        }
//        if(rx_index == 0){
//            memset(rx_input,0,RX_BUFFER_SIZE);
//        }
//        rx_input[rx_index] = U1RXREG;;
//        if(rx_index >= RX_BUFFER_SIZE-1){
//            memset(rx_input,0,RX_BUFFER_SIZE);
//            rx_index = 0;
//        }else if(rx_input[rx_index] == '\r'){
//            message_status = RX_BUFFER_FULL_MSG_RXED;
//            rx_index = 0;
//        }else{
//           rx_index++; 
//        }  
    } 
}


void UART_register_callback_on_msg(callback_on_msg_t cb){
    callback_on_msg = cb;
}

bool UART_update(){
    //In case of error simply reset state (and the STM clears everything).
    //If, however, a full '\r'-ended message is present, call the callback. 
    //Return if the buffer is empty or not. This may be used to pause normal program
    //execution until a full message is received. 
    if(uart.message_status == RX_BUFFER_ERROR){
        uart.message_status = RX_BUFFER_EMPTY;
    }else if(uart.message_status == RX_BUFFER_FULL_MSG_RXED){
        if(callback_on_msg != NULL){
            callback_on_msg(uart.rx_input);
        }
        uart.message_status = RX_BUFFER_EMPTY;
    }
    return uart.message_status == RX_BUFFER_EMPTY;
}
