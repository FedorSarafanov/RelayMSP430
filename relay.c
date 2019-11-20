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

unsigned int t=-2; // for start by button p1.1
// unsigned int t=-1;
int t1=10;
int t2=2;
int t3=3;
int t4=4;
int t5=5;
int T=10;
int Tnew=10;
int t1new=2;
int t2new=3;
int t3new=4;
int t4new=5;
int t5new=10;

int get_state (){
	if (P1IN & BIT2){}else{
		// P1OUT |= BIT0;
		t1new=1;
		t2new=2;
		t3new=3;
		t4new=4;
		t5new=5;
		Tnew=20;
	}
	if (P1IN & BIT3){}else{
		t1new=2;
		t2new=0;
		t3new=2;
		t4new=0;
		t5new=2;
		Tnew=20;
	}
	if (P1IN & BIT4){}else{
		t1new=3;
		t2new=0;
		t3new=3;
		t4new=0;
		t5new=3;
		Tnew=20;
	}
}

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
	P1DIR &= ~BIT1;                     // button is an input
	P1OUT |= BIT1;                      // pull-up resistor
	P1REN |= BIT1;                      // resistor enabled
	P1IES |= BIT1;                      // interrupt on low-to-high transition
	P1IE |= BIT1;                       // interrupt enable


	// каждый из выводов будет задавать режим работы
	// // Button setup
	// P1DIR &= ~BIT2+BIT3+BIT4;                     // button is an input
	// P1OUT |= BIT2+BIT3+BIT4;                      // pull-up resistor
	// P1REN |= BIT2+BIT3+BIT4;                      // resistor enabled
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

	// while ((P1IN & BIT1) == BIT1);
	// get_state();
	// T=Tnew; t1=t1new; t2=t2new;t3=t3new;t4=t4new; t5=t5new;
	// for(count=0; count<600000; count++){ /* задержка */ }
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
	// 	T=Tnew; t1=t1new; t2=t2new;t3=t3new;t4=t4new; t5=t5new;
	// }

    if (t == 0) { P6OUT |= BIT0;}
    if (t == t1) { P6OUT &= ~BIT0;}
    if (t == t2) { P6OUT |= BIT1;}
    if (t == t3) { P6OUT &= ~BIT1;}
    if (t == t4) { P6OUT |= BIT2;}
    if (t == t5) { P6OUT &= ~BIT2;}
    if (t == T) { t=-1;}
    // if (t == T) { t=-1; T=Tnew; t1=t1new; t2=t2new;t3=t3new;t4=t4new; t5=t5new;}
	if (t != -2)	{t=t+1;};
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
	P1IFG = 0; // reset interrupt
	// if (t==-2){
		t=0; get_state(); T=Tnew; t1=t1new; t2=t2new;t3=t3new;t4=t4new; t5=t5new;
	// }else{
// 	// if (t!=-2){t=-2; P6OUT &= ~(BIT2+BIT1+BIT0);}
// }
// 	// P1OUT ^= BIT0;
// 	// T=T*2;
}
