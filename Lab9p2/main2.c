/**************************************************************************************
* Author: Christina Kidwell
* Course: EGR 226 - 905
* Date: 02/25/2021
* Project: Lab 9 Part 2
* File: main2.c
* Description: Counts up and down on a 7 segment display
*
**************************************************************************************/
#include "msp.h"
#include <stdio.h>
#include <string.h>

void Port4Init(void);
void displayLED(int digit);
void SysTickINTInit (void);

volatile int digitOut=0 ;

/****| Port4Init | ***********************************************
* Brief: a function to initialize port 4 as all outputs to drive
* a seven segment LED dispaly
* interrupt driven inputs.
* Param:
* none
* return:
* N/A
*************************************************************/
void Port4Init(void) {      // Configure all port 4 bits as inputs with pullups
    P4->SEL0 &= 0x00;       // clear bits 0-7 in SEL0
    P4->SEL1 &= 0x00;       // clear bits 0-7 in SEL1
    P4->DIR |= 0xFF;        // set bits 0-7 in DIR to set all pins to outputs
    P4->REN &= 0x00;        // clear bits 0-7 in REN
    P4->OUT &= 0x00;        // clear bits 0-7 in OUT to turn off all segments
}

/****| displayLED | ***********************************************
* Brief: a function to output the segments to illuminate on a
* seven segment LED display
* Param:
* int digit = the digit to be displayed
* return:
* N/A
*************************************************************/
void displayLED(int digit) {
    switch(digit) {
        case 0  :
            P4->OUT = 0b00111111;
            break;
        case 1  :
            P4->OUT = 0b00000110;
            break;
        case 2  :
            P4->OUT = 0b01011011;
            break;
        case 3  :
            P4->OUT = 0b01001111;
            break;
        case 4  :
            P4->OUT = 0b01100110;
            break;
        case 5  :
            P4->OUT = 0b01101101;
            break;
        case 6  :
            P4->OUT = 0b01111101;
            break;
        case 7  :
            P4->OUT = 0b00000111;
            break;
        case 8  :
            P4->OUT = 0b01111111;
            break;
        case 9  :
            P4->OUT = 0b01100111;
            break;
       default :
            P4->OUT = 0b00000000;
    }
}

/****| SysTickINTInit | ***********************************************
* Brief: a function to initialize the SysTick timer to generate an
* interrupt every second
* interrupt driven inputs.
* Param:
* none
* return:
* N/A
*************************************************************/
void SysTickINTInit (void) {       // initialization of systic timer
    SysTick-> CTRL = 0;            // disable SysTick During step
    SysTick-> LOAD = 3000000;      // reload value for 1s interrupts
    SysTick-> VAL = 0;             // any write to current clears it
    SysTick-> CTRL = 0x00000007 ;  // enable systic , 3MHz, Interrupts
}

/****| SysTick_Handler | ***********************************************
* Brief: an interrupt handler to increment the digit being displayed
* when an interrupt is generated
* Param:
* none
* return:
* N/A
*************************************************************/
void SysTick_Handler(void) {
    digitOut += 1;
    if(digitOut>9) digitOut=0;
}

/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    Port4Init();
    SysTickINTInit ();

    __enable_irq();         // Enable global interrupt
    while(1) {
        displayLED(digitOut);
    }
}
