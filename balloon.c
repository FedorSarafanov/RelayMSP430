#include <msp430f5529.h> 

char t=0;                   // резервируем память типа char (один байт)

int main( void ) {
  // выключаем сторожевой таймер
 	WDTCTL = WDTPW + WDTHOLD;

	unsigned long long int count; // будет нужна для задержки
	unsigned int j; // будет нужна для задержки

	P6OUT = 0; // обнуляем регистр
	P1OUT = 0; // обнуляем регистр
	P6DIR |= BIT0+BIT1+BIT2+BIT3; // настраиваем P6.0,6.1,6.2 как выходы
	P1DIR |= BIT0; // настраиваем P1.0 как вход



    // P1REN |=  BIT1;
    // P1OUT |=  BIT1;
    // P1DIR &= ~BIT1; 
    // P1IES |=  BIT1;
    // P1IFG &= ~BIT1;
    // P1IE |=   BIT1;


	// настраиваем таймер
	TA0CCR0 = 16384;    // 0.5 Hz
    TA0CCTL0 = CCIE;
    TA0CTL = TASSEL_1 + ID_1 + MC_1 + TACLR;

	// настраиваем таймер
	TA1CCR0 = 15384;    // 0.5 Hz
    TA1CCTL0 = CCIE;
    TA1CTL = TASSEL_1 + ID_1 + MC_1 + TACLR;

	// настраиваем таймер
	TA2CCR0 = 17384;    // 0.5 Hz
    TA2CCTL0 = CCIE;
    TA2CTL = TASSEL_1 + ID_1 + MC_1 + TACLR;

    __enable_interrupt();
    __no_operation();

    count=600000;
    j=0;
	while(0) {
		j=j+1;
	// Импульс первого реле
	P6OUT |= BIT0; 
	 __delay_cycles(count); 
	P6OUT &= ~BIT0;
	__delay_cycles(60000);

	P6OUT |= BIT1; 
	 __delay_cycles(count); 
	P6OUT &= ~BIT1;
	__delay_cycles(60000);

	P6OUT |= BIT2; 
	 __delay_cycles(count); 
	P6OUT &= ~BIT2;
	__delay_cycles(60000);


	// for(count=0; count<60000; count++){ /* задержка */ }
	}

}

void __attribute__((interrupt(TIMER0_A0_VECTOR))) mainrun1(void) {
	// P6OUT |= BIT0; 
	// __delay_cycles(6000);
	// P6OUT &= ~BIT0; 
	t=t+1;
    if (t == 1) {
        P1OUT ^= BIT0;
        t = 0;
    }
}

void __attribute__((interrupt(TIMER1_A0_VECTOR))) mainrun2(void) {
	// P6OUT |= BIT1; 
	// __delay_cycles(6000);
	// P6OUT &= ~BIT1; 
}

void __attribute__((interrupt(TIMER2_A0_VECTOR))) mainrun3(void) {
	// P6OUT |= BIT2; 
	// __delay_cycles(6000);
	// P6OUT &= ~BIT2; 
}

// void __attribute__((interrupt(PORT2_VECTOR))) button1(void) {
// 		P1OUT ^= BIT0;
// }


// void __attribute__((interrupt(PORT2_VECTOR))) button2(void) {
// 		P1OUT ^= BIT0;
// }
