//
//  Copyright (c), Nikolay V. Ilin, 2018
//


#include <msp430f5529.h> 
#define PHASE_A     BIT5       // Port 2.5
#define PHASE_B     BIT4       // Port 2.4
#define PHASE_C     BIT5       // Port 1.5
#define PHASE_D     BIT3       // Port 1.3
#define HALF_STEPS  2020        // Number of steps to make 180 degree rotation
//#define HALT        51        // Number of pulses to wait, 0.5 sec period for ADC
#define HALT        151        // Number of pulses to wait,  1 sec period for ADC 
     

volatile signed int rotation = 0;
volatile signed int pause = 1;

// unsigned char phase_shifter(unsigned char b)
//     {

//     if (rotation == 0 )
//         {return b;}

//     unsigned char a = 0;

//     if (rotation < HALF_STEPS)
//         {
//         a = ( b << 1) | (b >> 8 - 1);
//         rotation++;
//         if  (b == 0x11 || b == 0x22 || b == 0x44 || b == 0x88  )
//              { b = b | a; }
//          else
//              { b = b & a; }

//         b = a;        // if one need full-step
//         }

//    if (rotation == HALF_STEPS) {
 //       if (++pause == HALT) {
  //          pause = 1;
   //         rotation++;
    //        }}

//    if ((rotation > HALF_STEPS) && (rotation < 2 * HALF_STEPS))
 //       {a =( b >> 1) | (b << 8 - 1);
  //      rotation++;
   //     if  (b == 0x11 || b == 0x22 || b == 0x44 || b == 0x88  )
    //        { b = b | a; }
     //   else
       //     { b = b & a; }

//        b = a;        //  full step
        // }

//    if (rotation == 2 * HALF_STEPS)
//        if (++pause == HALT) {
 //            pause = 1; 
  //           rotation = 1;
   //         }
    //    return b; }
  

volatile unsigned int phi[3];
//volatile unsigned char phase_ctl = 0x00;    // 0x33 for full step
volatile unsigned char phase_ctl = 0x00;    // 0x33 for full step
//volatile unsigned char index = 0;


void main(void)
{
    
    WDTCTL = WDTPW + WDTHOLD;                   // Stop WatchDog timer

    P2DIR |=  (PHASE_A + PHASE_B);              // Set phase control bits
    P1DIR |=  (PHASE_C + PHASE_D);              // set phase control bits

    P1DIR |= BIT0;                              // onboard red LED 
    P4DIR |= BIT7;                              // onboard green LED

    P2OUT &= ~(PHASE_A + PHASE_B);              // init all off
    P1OUT &= ~(PHASE_C + PHASE_D);
    P1OUT &= ~BIT0;
    P4OUT &= ~BIT7;

    P1REN |=  BIT1;
    P1OUT |=  BIT1;
    P1DIR &= ~BIT1; 
    P1IES |=  BIT1;
    P1IFG &= ~BIT1;
    P1IE |=   BIT1;

// -------------------------------------------
//  set Timer_A
//--------------------------------------------
    TA0CCR0 =  64;     // 1 Hz test    
//    TA0CCR0 = 1024 - 1;     // 32 Hz test    
//    TA0CCR0 = 4096 - 1;     // 8 Hz test    
//    TA0CCR0 = 8192 - 1;     // 4 Hz test    
//    TA0CCR0 = 128 - 1;    // 256 Hz test
//    TA0CCR0 = 48;       // 256 Hz test
//    TA0CCR0 = 32;     // 1000 Hz !!!!! Calibrated by Meeeee !!!!! 
//    TA0CCR0 = 16;       // 256 Hz test


    TA0CCTL0 = CCIE;
    TA0CTL = TASSEL_1 + ID_1 + MC_1 + TACLR;
//--------------------------------------------
//   set Timer_B
//--------------------------------------------
    TB0CCR0 = 16384 - 1;    // 0.5 Hz 
    TB0CCTL0 = CCIE;
    TB0CTL = TBSSEL_1 + ID_1 + MC_3 + TBCLR;
//--------------------------------------------
//  set ADC12 
//--------------------------------------------
    P6SEL |= BIT0;          // analog input is P6.0
    ADC12CTL0 = ADC12ON;
    ADC12CTL1 = ADC12SHP;
    ADC12MCTL0 = ADC12INCH_0;
    ADC12CTL0 |= ADC12ENC;
//--------------------------------------------  

    __enable_interrupt();
    __no_operation();
//    while (1){}         
}

void __attribute__((interrupt(TIMER0_A0_VECTOR))) motordriver(void) {

//     P1OUT ^= BIT0;       // test Pulse LED
//      P2OUT = (PHASE_A & (phase_ctl << 5)) + (P2OUT & ~PHASE_A); 
//      P2OUT = (PHASE_B & (phase_ctl << 3)) + (P2OUT & ~PHASE_B); 
//      P1OUT = (PHASE_C & (phase_ctl << 3)) + (P1OUT & ~PHASE_C); 
//      P1OUT = (PHASE_D & (phase_ctl << 0)) + (P1OUT & ~PHASE_D); 

//      phase_ctl = phase_shifter(phase_ctl);

//     if ((rotation  == HALF_STEPS / 2) || (rotation  == 3 * HALF_STEPS / 2))     // ready to adc
//     {   
//         P1OUT ^= BIT0;
// //      P4OUT ^= BIT7;          // working test
// //      READ ADC12 HERE
// //        ADC12CTL0 &= ~ADC12SC;
//         ADC12CTL0 |= ADC12SC;
//         while(!(ADC12IFG & BIT0));
// //        phi[index] = ADC12MEM0 & 0x0FFF;
// //
// //
//         if (ADC12MEM0 & 0x0FFF >= 0x7ff) 
//         {    P4OUT |= BIT7;}          // working test
//         else
//         {    P4OUT &= ~BIT7;}
// //        if ( ++index == 3 ) {index = 0;}

//     ADC12CTL0 &= ~ADC12SC;
    // }

}

void __attribute__((interrupt(TIMER0_B0_VECTOR))) mainrun(void) {
//    P4OUT ^= BIT7;        // working test 

//  TODO    collect all info (ADC GPS ALT) and send to transmitter (UART)
 
    }

void __attribute__((interrupt(PORT1_VECTOR))) button(void) {

    // if (phase_ctl == 0)
    // {       phase_ctl = 0x11;
    //         rotation = 1; }
    // else
    // {       phase_ctl = 0;
    //         rotation = 0;  }

    // P1IFG &= ~BIT1; 
    // P1IE = 0;
    }
