/**************************************************************************************
* Author: Christina Kidwell
* Course: EGR 226 - 905
* Date: 02/25/2021
* Project: Lab 9 Part 1
* File: main1.c
* Description: Use the circuit built in Part II from the previous Lab.
* The PWM will be generated using TIMERAx PWM signal that is connected
* directly to an I/O pin for controlling the speed of a DC motor.
* Motor speed will be controlled by three external push buttons.
**************************************************************************************/
#include "msp.h"
#include <stdio.h>
#include <string.h>
#include "MySetup.h"
#include "MyTimer.h"

float motorSpeed = 0.50;   // set default motor speed at startup to 50%
int speedUp = 0;
int speedDn = 0;
int speedStop = 0;

void Port2Init(void);

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

    P2->SEL0 &=~BIT7;
    P2->SEL1 &=~BIT7;
    P2->DIR &=~BIT7; // Set pin as input
    P2->REN |= BIT7; // Enable Internal resistor
    P2->OUT |= BIT7; // Enable pull up resistor
    P2->IES |= BIT7; //Set pin interrupt to trigger when it goes from high to low
    P2->IE |= BIT7; // Enable interrupt for P2 BIT 7

    P2->IFG = 0; // Clear all P2 interrupt flags
}

/****| PORT2_IRQHandler | ***********************************************
* Brief: Interrupt handler for Port 2 bits 5, 6, & 7
* Param:
* none
* return:
* N/A
*************************************************************/
void PORT2_IRQHandler(void) {   // Port2 ISR
    if ( P2->IFG & BIT5 ) {     // If up button pressed
        speedUp = 1;            //    set up flag
        P2->IFG &= ~ BIT5;      // Reset the interrupt flag
    }
    if ( P2->IFG & BIT6 ) {     // If down button pressed
        speedDn = 1;            //    set down flag
        P2->IFG &= ~ BIT6;      // Reset the interrupt flag
    }
    if ( P2->IFG & BIT7 ) {     // If stop button pressed
        speedStop = 1;          //    set stop flag
        P2->IFG &= ~ BIT7;      // Reset the interrupt flag
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
    SysTickInit();
    TimerAInit();
    TimerASetPeriod(9375);                  // Set PWM period to 40Hz

    NVIC_EnableIRQ(PORT2_IRQn);             // Enable Port 2 interrupt on the NVIC
    __enable_interrupt ( );                 // Enable global interrupt
    while(1) {
        if(speedUp==1) {                    // if speedUp flag set
            motorSpeed += 0.10;             // increment motor speed
            if(motorSpeed>1) motorSpeed=1;  // don't go beyond max speed
            msDelay(15);                    // debounce delay
            speedUp=0;                      // clear speedUp flag
        }
        else if(speedDn==1) {               // if speedUp flag set
            motorSpeed -= 0.10;             // decrement  motor speed
            if(motorSpeed<0) motorSpeed=0;  // don't go beyond min speed
            msDelay(15);                    // debounce delay
            speedDn=0;                      // clear speedDn flag
        }
        else if(speedStop==1) {             // if speedStop flag set
            motorSpeed = 0;                 // set motor speed to zero
            msDelay(15);                    // debounce delay
            speedStop=0;                    // clear speedStop flag
        }
        //printf("speed = %f\n", motorSpeed);
        //msDelay(1000);
        TimerASetMotorSpeed(motorSpeed);    // send motorSpeed to timer
    }
}
