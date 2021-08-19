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

//local functions
void _UART1_Receive_CallBack();

//local vars
char rx_input[RX_BUFFER_SIZE];
uint8_t message_status = RX_BUFFER_EMPTY;

void UART_init(){
    memset(rx_input,0,RX_BUFFER_SIZE);
    UART1_SetRxInterruptHandler(_UART1_Receive_CallBack);
}

//The _U1RXInterrupt does something similar, but it seems to be wrong/useless.
//The old code remains in the uart1.c but is of no use. 
void _UART1_Receive_CallBack(){
    static uint8_t rx_index = 0;
    while((U1STAbits.URXDA == 1))
    {
        if(rx_index == 0){
            memset(rx_input,0,RX_BUFFER_SIZE);
        }
        rx_input[rx_index] = U1RXREG;;
        if(rx_index >= RX_BUFFER_SIZE-1){
            memset(rx_input,0,RX_BUFFER_SIZE);
            rx_index = 0;
        }else if(rx_input[rx_index] == '\r'){
            message_status = RX_BUFFER_FULL_MSG_RXED;
            rx_index = 0;
        }else{
           rx_index++; 
        }  
    } 
}
