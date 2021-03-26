/**************************************************************************************
* Author: Christina Kidwell
* Course: EGR 226 - 905
* Date: 02/25/2021
* Project: Lab 9 Part 3
* File: main3.c
* Description: Increments and decrements a 7 segment LED with button presses
*
**************************************************************************************/
#include "msp.h"
#include <stdio.h>
#include <string.h>
#include "MySetup.h"
#include "MyTimer.h"

int digitUp = 0;
int digitDn = 0;

void Port2Init(void);
void Port4Init(void);
void displayLED(int digit);

volatile int digitOut=0 ;

/****| Port2Init | ***********************************************
* Brief: a function to initialize port 2 bits 5,6,&7 to be used as
* interrupt driven inputs.
* Param:
* none
* return:
* N/A
*************************************************************/
void Port2Init(void) {
    P2->SEL0 &=~BIT5;
    P2->SEL1 &=~BIT5;
    P2->DIR &=~BIT5; // Set pin as input
    P2->REN |= BIT5; // Enable Internal resistor
    P2->OUT |= BIT5; // Enable pull up resistor
    P2->IES |= BIT5; //Set pin interrupt to trigger when it goes from high to low
    P2->IE |= BIT5; // Enable interrupt for P2 BIT 5

    P2->SEL0 &=~BIT6;
    P2->SEL1 &=~BIT6;
    P2->DIR &=~BIT6; // Set pin as input
    P2->REN |= BIT6; // Enable Internal resistor
    P2->OUT |= BIT6; // Enable pull up resistor
    P2->IES |= BIT6; //Set pin interrupt to trigger when it goes from high to low
    P2->IE |= BIT6; // Enable interrupt for P2 BIT 6

    P2->IFG = 0; // Clear all P2 interrupt flags
}

/****| PORT2_IRQHandler | ***********************************************
* Brief: Interrupt handler for Port 2 bits 5, & 6
* Param:
* none
* return:
* N/A
*************************************************************/
void PORT2_IRQHandler(void) {// Port2 ISR
    if ( P2->IFG & BIT5 ) {
        digitUp = 1;
        P2->IFG &= ~ BIT5;// Reset the interrupt flag
    }
    if ( P2->IFG & BIT6 ) {
        digitDn = 1;
        P2->IFG &= ~ BIT6;// Reset the interrupt flag
    }
}

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

/**
 * main.c
 */
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    setupIO();
    Port2Init();
    Port4Init();
    SysTickInit();

    NVIC_EnableIRQ(PORT2_IRQn);     // Enable Port 2 interrupt on the NVIC
    __enable_interrupt ( );         // Enable global interrupt
    while(1) {
        if(digitUp==1) {                    // if digitUp flag set
            digitOut += 1;                  // increment count
            if(digitOut>9) digitOut=0;      // don't go beyond max count
            msDelay(15);                    // debounce delay
            digitUp=0;                      // clear digitUp flag
        }
        else if(digitDn==1) {               // if digitDn flag set
            digitOut -= 1;                  // increment count
            if(digitOut<0) digitOut=9;      // don't go beyond max count
            msDelay(15);                    // debounce delay
            digitDn=0;                      // clear digitDn flag
        }
        //printf("digit = %d\n", digitOut);
        //msDelay(1000);
        displayLED(digitOut);               // output digit to display
    }
}
