/*
	Analog module for serial startup of devices using relay
	Sarafanov F.G. (c) 2019

	---------------------
	Алгоритм запуска реле
	---------------------
	Работает по прерыванию  таймера TA0, инкременируя счетчик t.
	По достижению счетчиком определенных значений t1,...,t5, 
	включаются или отключаются ноги P6.0, P6.1, P6.2 (выход на реле).

	По достижению счетчиком значения T сбрасывается счетчик t в 0,
	цикл замыкается.

	Цикл запускается при получении прерывания от кнопки P1.1

	--------------------
	Переключение режимов 
	--------------------
	Пока работает проверкой в конце цикла, 
	
	----
	TODO
	----
	* добавить к переменным volatile, чтобы компилятор не делал "все пропало"

	* сделать переключение режимов
	 путь 1: переключать режимы по прерываниям
		* найти способ обрабатывать прерывания от пинов P1.2, P1.3, P1.4
		* исключить дребезг контактов при обработке прерывания

	 путь 2: проверять режим в конце цикла

	---------------------------------
	Известные баги текущей реализации
	---------------------------------

	При подключении питания почему-то иногда запускается цикл сам, без 
	переключения переменной t=-2 -> =0 с помощью прерывания по кнопке

	Запускается кусок не того режима


*/
#include <msp430f5529.h> 

volatile unsigned int t;

volatile int t1=1;
volatile int t2=2;
volatile int t3=3;
volatile int t4=4;
volatile int t5=5;
volatile int t6=6;
volatile int t7=7;
volatile int T=8;

int get_state (){
	if (P1IN & BIT2){}else{
		// P1OUT |= BIT0;
		t1=1;
		t2=2;
		t3=3;
		t4=4;
		t5=5;
		T=20;
	}
	if (P1IN & BIT3){}else{
		t1=2;
		t2=0;
		t3=2;
		t4=0;
		t5=2;
		T=20;
	}
	if (P1IN & BIT4){}else{
		t1=3;
		t2=0;
		t3=3;
		t4=0;
		t5=3;
		T=20;
	}
}

int main( void ) {
  // выключаем сторожевой таймер
 	WDTCTL = WDTPW + WDTHOLD;

	volatile unsigned long long int count; // будет нужна для задержки
	volatile unsigned int j; // будет нужна для задержки

	// обнуляем регистр
	P6OUT = 0; 
	P7OUT = 0; 
	P1OUT = 0; 
	P4OUT = 0; 
	P6DIR |= BIT0+BIT1+BIT2; // настраиваем P6.0,P6.1,P6.2 как выходы
	P1DIR |= BIT0; 
	P4DIR |= BIT7;
	P7DIR |= BIT0;


	P2DIR &= ~BIT0; // P2.0 работает как вход
	P2OUT |= BIT0;  // подтягивающий резистор нужен
	P2REN |= BIT0;  // включаем резистор

	// Button setup
	P1DIR &= ~BIT1;                     // button is an input
	P1OUT |= BIT1;                      // pull-up resistor
	// P1IES |= BIT1;                      // interrupt on low-to-high transition
	// P1IE |= BIT1;                       // interrupt enable


	// каждый из выводов будет задавать режим работы
	P1DIR &= ~BIT2+BIT3+BIT4;                     // button is an input
	P1OUT |= BIT2+BIT3+BIT4;                      // pull-up resistor
	P1REN |= BIT2+BIT3+BIT4;                      // resistor enabled
	// P1IES |= BIT2+BIT3+BIT4;                      // interrupt on low-to-high transition
	// P1IE |= BIT2+BIT3+BIT4;                       // interrupt enable


	// настраиваем таймер
	TA0CCR0 = 2192;    // 0.5 Hz
    TA0CCTL0 = CCIE;
    TA0CTL = TASSEL_1 + ID_1 + MC_1 + TACLR;

	// настраиваем таймер
	// TA1CCR0 = 16384;    // 0.5 Hz
 //    TA1CCTL0 = CCIE;
 //    TA1CTL = TASSEL_1 + ID_1 + MC_1 + TACLR;

	// таймер, фильтрующий дребезг контактов кнопки пуска
	TA2CCR0 = 2000;  
	// TA2CCR0 = 16384-1;    // 1 Hz
    TA2CCTL0 = CCIE;
    TA2CTL = TASSEL_1 + ID_1 + MC_1 + TACLR;

	// while ((P1IN & BIT1) == BIT1);
	// get_state();
	// T=T; t1=t1; t2=t2;t3=t3;t4=t4; t5=t5;
	// for(count=0; count<600000; count++){ /* задержка */ }
	t=0;
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

	// get_state();

	// if (t == -1){
	// 	t=t+1;
	// 	T=T; t1=t1; t2=t2;t3=t3;t4=t4; t5=t5;
	// }

    if (t == 0) { get_state(); P6OUT |= BIT0;}
    if (t == t1) { P6OUT &= ~BIT0;}
    if (t == t2) { P6OUT |= BIT1;}
    if (t == t3) { P6OUT &= ~BIT1;}
    if (t == t4) { P6OUT |= BIT2;}
    if (t == t5) { P6OUT &= ~BIT2;}
    if (t == t6) { P7OUT |= BIT0;}
    if (t == t7) { P7OUT &= ~BIT0;}
    // if (t == T) { t=-1;}
    if (t == T) { get_state(); t=-1;}
	if (t != -2)	{t=t+1;};
}

// void __attribute__((interrupt(TIMER1_A0_VECTOR))) mainrun2(void) {
// 	get_state();
// }


// volatile int old_state=10;
// Здесь будем проверять установку режима
volatile int state=0;
volatile int old_state=0;

void __attribute__((interrupt(TIMER2_A0_VECTOR))) mainrun3(void) {
	if (P2IN & BIT0){
		old_state=state;
		state=0;
	} else {
		old_state=state;
		state=1;

	}
	if (state==1 && old_state==1){
		if (t==-2){
			t=0;
		}
	}
	if (state==0 && old_state==0){
		t=-2;
		P6OUT=0;
		P7OUT=0;
	}
	// get_state();
	// P7OUT |= BIT0; 
	// __delay_cycles(60000);
	// P7OUT &= ~BIT0; 
}

void __attribute__((interrupt(PORT2_VECTOR))) button1(void) {
}


void __attribute__((interrupt(PORT1_VECTOR))) button2(void) {
	// volatile char pin=0; // Определим, какой пин был замкнут
	// pin=P1IFG;
	// switch (pin){
	// 	case 2: { // P1.1 (кнопка)
	// 		// t=0; get_state(); T=T; t1=t1; t2=t2;t3=t3;t4=t4; t5=t5;
	// 			P1OUT |= BIT0;
	// 		}
	// 	case 4: { // P1.2
	// 			P1OUT |= BIT0;
	// 			break;
	// 		}
	// 	case 8: { // P1.3
	// 			P1OUT |= BIT0;
	// 			// P1OUT &= ~BIT0;
	// 			break;
	// 		}
	// 	case 16: {// P1.4
	// 			P1OUT |= BIT0;
	// 			// __delay_cycles(60000);
	// 			// P1OUT &= ~BIT0;
	// 			break;
	// 		}
	// }
	// P1IFG = 0; // reset interrupt
	// // P1IE=0;P1IFG=0;P1IES=0; // disable interrupt
	// pin = 0;
}
