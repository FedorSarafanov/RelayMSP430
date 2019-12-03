/*

	Analog module for serial startup of devices using relay
	Sarafanov F.G. (c) 2019

	---------------------
	Алгоритм запуска реле
	---------------------
	Работает по прерыванию  таймера TA0, инкременируя счетчик t.
	По достижению счетчиком определенных значений t1,...,t7, 
	включаются или отключаются ноги P6.0, P6.1, P6.2 (выход на реле).

	По достижению счетчиком значения T сбрасывается счетчик t в 0,
	цикл замыкается.

	Цикл запускается при подтянутом контакте P2.0 к земле, при этом
	фильтруется дребезг контактов: P2.0 проверяется таймером TA2, 
	если два состояния подряд совпадают, происходит переключение.

	--------------------
	Переключение режимов 
	--------------------
	В начале и конце цикла по времени проверяется текущее состояние 
	контактов P1.2,P1.3,P1.4,P1.5. Замкнутый контакт определяет номер
	текущего режима  t1,...,t7,T
	
*/


#include <msp430f5529.h> 

volatile unsigned long int t;

volatile int t1=0;
volatile int t2=0;
volatile int t3=0;
volatile int t4=0;
volatile int t5=0;
volatile int t6=0;
volatile int t7=0;
volatile long int T=0;

int mode(int num_mode){
	switch (num_mode){
		case 1:{
			t1=90; 
			t2=400; t3=t2+100;
			

			t4=1300; t5=1400; 
			t6=1500; t7=1600; 

			T=300000;
			break;
		}
		case 2:{
			t1=1; t2=0; t3=1;
			t4=0; t5=1; t6=0;
			t7=1; T=2;
			break;
		}
		case 3:{
			t1=4; t2=5; t3=6;
			t4=1; t5=2; t6=3;
			t7=7; T=8;
			break;
		}
		case 4:{
			t1=1; t2=0; t3=1;
			t4=0; t5=1; t6=0;
			t7=1; T=2;
			break;
		}
	}

}

int get_state (){
	if (P1IN & BIT2){}else{
		return 1;
	}
	if (P1IN & BIT3){}else{
		return 2;
	}
	if (P1IN & BIT4){}else{
		return 3;
	}
	if (P1IN & BIT5){}else{
		return 4;
	}
}

int main( void ) {
  // выключаем сторожевой таймер
 	WDTCTL = WDTPW + WDTHOLD;

	// обнуляем регистр
	P6OUT = 0; P7OUT = 0;  P3OUT = 0;
	// настраиваем P6.0,P6.1,P6.2, P7.0 как выходы
	P6DIR |= BIT0+BIT1+BIT2; 
	P7DIR |= BIT0;
	P3DIR |= BIT6;


	// Вход P2.0 -- логическое включение/отключение цикла со сбросом t в 0
	P2DIR &= ~BIT0; // P2.0 работает как вход
	P2OUT |= BIT0;  // подтягивающий резистор нужен
	P2REN |= BIT0;  // включаем резистор


	// Входы P1.2,P1.3,P1.4,P1.5 будет задавать номер режима работы
	P1DIR &= ~BIT2+BIT3+BIT4+BIT5; 
	P1OUT |= BIT2+BIT3+BIT4+BIT5;
	P1REN |= BIT2+BIT3+BIT4+BIT5;
	// P1IES |= BIT2+BIT3+BIT4+BIT5; // interrupt on low-to-high transition
	// P1IE |= BIT2+BIT3+BIT4+BIT5; // interrupt enable


	// настраиваем основной таймер, в тактах которого измеряется время t,t1..t7,T
	TA0CCR0 = 32;
    TA0CCTL0 = CCIE;
    TA0CTL = TASSEL_1 + ID_1 + MC_1 + TACLR;


	// таймер, фильтрующий дребезг контактов кнопки пуска
	TA2CCR0 = 2000;  
    TA2CCTL0 = CCIE;
    TA2CTL = TASSEL_1 + ID_1 + MC_1 + TACLR;

	t=-2;
    __enable_interrupt();
    __no_operation();
}


void __attribute__((interrupt(TIMER0_A0_VECTOR))) cycle(void) {

    if (t == 0) { mode(get_state()); P6OUT |= BIT0;}
    if (t == t1) { P6OUT &= ~BIT0;}
    if (t == t2) { P6OUT |= BIT1;}
    if (t == t3) { P6OUT &= ~BIT1;}
    // if (t == t4) { P6OUT |= BIT2;}
    // if (t == t5) { P6OUT &= ~BIT2;}
    // if (t == t6) { P7OUT |= BIT0; P3OUT |= BIT6;}
    // if (t == t7) { P7OUT &= ~BIT0; P3OUT &= ~BIT6;}
    if (t == T) { mode(get_state()); t=-1;}
	if (t != -2)	{t=t+1;};

}


// Здесь будем проверять установку режима, устраняя дребезг контактов
volatile int state=0;
volatile int old_state=0;

void __attribute__((interrupt(TIMER2_A0_VECTOR))) turn(void) {
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
}