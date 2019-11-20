#include <msp430f5529.h> 

#define BUTTON   BIT1
unsigned int t=0;
int start=0;
int t1=1;
int t2=2;
int t3=3;
int t4=4;
int t5=5;
int T=10;

int main( void ) {
  // выключаем сторожевой таймер
 	WDTCTL = WDTPW + WDTHOLD;

	unsigned long long int count; // будет нужна для задержки
	unsigned int j; // будет нужна для задержки

	// обнуляем регистр
	P6OUT = 0; 
	P1OUT = 0; 
	P4OUT = 0; 
	P6DIR |= BIT0+BIT1+BIT2+BIT3; // настраиваем P6.0,6.1,6.2 как выходы
	P1DIR |= BIT0; 
	P4DIR |= BIT7;



	// Button setup
	P1DIR &= ~BUTTON;                     // button is an input
	P1OUT |= BUTTON;                      // pull-up resistor
	P1REN |= BUTTON;                      // resistor enabled
	P1IES |= BUTTON;                      // interrupt on low-to-high transition
	P1IE |= BUTTON;                       // interrupt enable


	// каждый из выводов будет задавать режим работы
	// // Button setup
	P1DIR &= ~BIT2+BIT3+BIT4;                     // button is an input
	P1OUT |= BIT2+BIT3+BIT4;                      // pull-up resistor
	P1REN |= BIT2+BIT3+BIT4;                      // resistor enabled
	// P1IES |= BIT2;                      // interrupt on low-to-high transition
	// P1IE |= BIT2;                       // interrupt enable


	// настраиваем таймер
	TA0CCR0 = 2192;    // 0.5 Hz
    TA0CCTL0 = CCIE;
    TA0CTL = TASSEL_1 + ID_1 + MC_1 + TACLR;

	// настраиваем таймер
	TA1CCR0 = 16384;    // 0.5 Hz
    TA1CCTL0 = CCIE;
    TA1CTL = TASSEL_1 + ID_1 + MC_1 + TACLR;

	// настраиваем таймер
	TA2CCR0 = 16384;    // 0.5 Hz
    TA2CCTL0 = CCIE;
    TA2CTL = TASSEL_1 + ID_1 + MC_1 + TACLR;


	// while ((P1IN & BUTTON) == BUTTON);
    __enable_interrupt();
    __no_operation();

 //    count=600000;
 //    j=0;
	// while(0) {
	// 	j=j+1;
	// // Импульс первого реле
	// P6OUT |= BIT0; 
	//  __delay_cycles(count); 
	// P6OUT &= ~BIT0;
	// __delay_cycles(60000);

	// P6OUT |= BIT1; 
	//  __delay_cycles(count); 
	// P6OUT &= ~BIT1;
	// __delay_cycles(60000);

	// P6OUT |= BIT2; 
	//  __delay_cycles(count); 
	// P6OUT &= ~BIT2;
	// __delay_cycles(60000);


	// // for(count=0; count<60000; count++){ /* задержка */ }
	// }

}

void __attribute__((interrupt(TIMER0_A0_VECTOR))) mainrun1(void) {
	// P6OUT |= BIT0; 
	// __delay_cycles(6000);
	// P6OUT &= ~BIT0; 
	int Tnew=T;
	int t1new=t1;
	int t2new=t2;
	int t3new=t3;
	int t4new=t4;
	int t5new=t5;
		if (P1IN & BIT2){}else{
			// P1OUT |= BIT0;
			t1new=1;
			t2new=2;
			t3new=3;
			t4new=4;
			t5new=5;
			Tnew=10;
		}
		if (P1IN & BIT3){}else{
			t1new=2;
			t2new=0;
			t3new=2;
			t4new=0;
			t5new=2;
			Tnew=2;
		}
		if (P1IN & BIT4){}else{
			t1new=3;
			t2new=0;
			t3new=3;
			t4new=0;
			t5new=3;
			Tnew=3;
		}

    if (t == 0) { P6OUT |= BIT0;}
    if (t == t1) { P6OUT &= ~BIT0;}
    if (t == t2) { P6OUT |= BIT1;}
    if (t == t3) { P6OUT &= ~BIT1;}
    if (t == t4) { P6OUT |= BIT2;}
    if (t == t5) { P6OUT &= ~BIT2;}
    if (t == T) { t=-1; T=Tnew; t1=t1new; t2=t2new;t3=t3new;t4=t4new; t5=t5new;}
		t=t+1;
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

void __attribute__((interrupt(PORT2_VECTOR))) button1(void) {
}


void __attribute__((interrupt(PORT1_VECTOR))) button2(void) {
	// P1IFG = 0; // reset interrupt
	// P1OUT |= BIT0;
	// T=T*2;
}
