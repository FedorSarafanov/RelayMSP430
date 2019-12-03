/*

	Analog module for serial startup of devices using relay
	Sarafanov F.G. (c) 2019

	---------------------
	Алгоритм запуска реле
	---------------------
	Работает по прерыванию  таймера TA0, инкременируя счетчик t.
	По достижению счетчиком определенных значений t1,...,t7, 
	включаются или отключаются ноги P6.1--6.4 (выход на реле).

	По достижению счетчиком значения T сбрасывается счетчик t в 0,
	цикл замыкается.

	Цикл запускается при подтянутом контакте P2.0 к земле, при этом
	фильтруется дребезг контактов: P2.0 проверяется таймером TA2, 
	если два состояния подряд совпадают, происходит переключение.

	--------------------
	Переключение режимов 
	--------------------
	В начале и конце цикла по времени проверяется текущее состояние 
	контактов (см. функции get_timer_state, get_delay_state), подтягивание 
	контакта к земле определяет время задержки и время периода. На задержку
	и период по 10 возможных режимов.
*/


#include <msp430f5529.h> 

volatile unsigned long int t;

volatile unsigned int t1=0; // Спад 1 импульса
volatile unsigned int t2=0; // Фронт 2 импульса
volatile unsigned int t3=0; // Спад 2 импульса
volatile unsigned int t4=0;	// Фронт 3 импульса 
volatile unsigned int t5=0;	// Спад 3 импульса
volatile unsigned int t6=0;	// Фронт 4 импульса
volatile unsigned int t7=0;	// Спад 4 импульса
volatile unsigned long int T=0; // Период цикла

volatile unsigned int j=0;
volatile unsigned int test=0;


int newmode(int timer_mode, int delay_mode){
	// При текущей настройке таймера один отсчет -- 2 мс.
	t1 = 90;
	switch (delay_mode){
		case 1:{ t2 = 275; break; } // 0.55 c
		case 2:{ t2 = 350; break; }	// 0.70 с
		case 3:{ t2 = 425; break; }	// 0.85 с
		case 4:{ t2 = 500; break; }	// 1 с
		case 5:{ t2 = 575; break; }	// 1.15 с
		case 6:{ t2 = 650; break; }	// 1.30 с
		case 7:{ t2 = 725; break; }	// 1.45 с
		case 8:{ t2 = 800; break; }	// 1.6 с
		case 9:{ t2 = 875; break; }	// 1.75 с
		case 10:{t2 = 950; break; }	// 1.9 с
	}
	t3=t2+100;
	switch (timer_mode){
		case 1:{ T = 7500; break; }			// 15 с
		case 2:{ T = 10000; break; }		// 20 c
		case 3:{ T = 12500; break; }		// 25 с
		case 4:{ T = 15000; break; }		// 30 c
		case 5:{ T = 30000; break; }		// 1 мин
		case 6:{ T = 60000; break; }		// 2 мин
		case 7:{ T = 150000; break; }		// 5 мин
		case 8:{ T = 300000; break; }		// 10 мин
		case 9:{ T = 450000; break; }		// 15 мин
		case 10:{ T = 4194967295; break; }  // ~3 месяца
	}
}

int get_timer_state (){
	if (P6IN & BIT5){}else{ return 1;	}
	if (P3IN & BIT4){}else{ return 2;	}
	if (P3IN & BIT3){}else{ return 3;	}
	if (P1IN & BIT6){}else{ return 4;	}
	if (P6IN & BIT6){}else{ return 5;	}
	if (P3IN & BIT2){}else{ return 6;	}
	if (P2IN & BIT7){}else{ return 7;	}
	if (P4IN & BIT2){}else{ return 8;	}
	if (P4IN & BIT1){}else{ return 9;	}
	if (P3IN & BIT5){}else{ return 10;	}
}

int get_delay_state (){
	if (P2IN & BIT5){}else{ return 1;	}
	if (P2IN & BIT4){}else{ return 2;	}
	if (P1IN & BIT5){}else{ return 3;	}
	if (P1IN & BIT4){}else{ return 4;	}
	if (P1IN & BIT3){}else{ return 5;	}
	if (P1IN & BIT2){}else{ return 6;	}
	if (P4IN & BIT3){}else{ return 7;	}
	if (P4IN & BIT0){}else{ return 8;	}
	if (P3IN & BIT7){}else{ return 9;	}
	if (P8IN & BIT2){}else{ return 10;	}
}


int main( void ) {
  // выключаем сторожевой таймер
 	WDTCTL = WDTPW + WDTHOLD;

	// обнуляем регистр
	P6OUT = 0; P7OUT = 0;  P3OUT = 0; P8OUT = 0; P1OUT = 0;
	// настраиваем P6.0,P6.1,P6.2, P7.0 как выходы
	P6DIR |= BIT0+BIT1+BIT2+BIT3+BIT4; 
	P7DIR |= BIT0;
	P3DIR |= BIT6;

	P8DIR |= BIT1; // Для теста таймера 
	P4DIR |= BIT7;

	// Вход P2.0 -- логическое включение/отключение цикла со сбросом t в 0
	P2DIR &= ~BIT0; // P2.0 работает как вход
	P2OUT |= BIT0;  // подтягивающий резистор нужен
	P2REN |= BIT0;  // включаем резистор


	P1DIR |= BIT0;
	// Входы P1.2,P1.3,P1.4,P1.5 будет задавать номер режима работы
	P1DIR &= ~(BIT2 | BIT3 | BIT4 | BIT5); 
	P1OUT |= BIT2+BIT3+BIT4+BIT5;
	P1REN |= BIT2+BIT3+BIT4+BIT5;




	// Как вход всю левую линейку J1 на длину таймера
	P6DIR &= ~(BIT5 | BIT6);
	P3DIR &= ~(BIT4 | BIT3 | BIT2 | BIT5); 
	P1DIR &= ~BIT6;
	P4DIR &= ~(BIT2 | BIT1);
	P2DIR &= ~BIT7;
	/////////////////////////////////////////////////
	P6OUT |= BIT5 + BIT6;
	P3OUT |= BIT4 + BIT3 + BIT2 + BIT5; 
	P1OUT |= BIT6;
	P4OUT |= BIT2 + BIT1;
	P2OUT |= BIT7;
	/////////////////////////////////////////////////
	P6REN |= BIT5 + BIT6;
	P3REN |= BIT4 + BIT3 + BIT2 + BIT5; 
	P1REN |= BIT6;
	P4REN |= BIT2 + BIT1;
	P2REN |= BIT7;
	/////////////////////////////////////////////////



	// Как вход всю правую линейку J2
	P2DIR &= ~(BIT5 |BIT4);
	P1DIR &= ~(BIT5 | BIT4 | BIT3 | BIT2);
	P4DIR &= ~(BIT3 | BIT0);
	P3DIR &= ~BIT7;
	P8DIR &= ~BIT2;
	/////////////////////////////////////////////////
	P2OUT |= BIT5+BIT4;
	P1OUT |= BIT5+BIT4+BIT3+BIT2;
	P4OUT |= BIT3+BIT0;
	P3OUT |= BIT7;
	P8OUT |= BIT2;
	/////////////////////////////////////////////////
	P2REN |= BIT5+BIT4;
	P1REN |= BIT5+BIT4+BIT3+BIT2;
	P4REN |= BIT3+BIT0;
	P3REN |= BIT7;
	P8REN |= BIT2;
	/////////////////////////////////////////////////


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
    if (t == 0) { newmode(get_timer_state(), get_delay_state()); P6OUT |= BIT1;}

    if (test==1)
    {
	    if (j == 0){P8OUT |= BIT1; P1OUT |= BIT0;}
	    // Тест частоты на осциллографе, скважность 1
		j=j+1;
	    if (j == 100){P8OUT &= ~BIT1; P1OUT &= ~BIT0;}
	    if (j == 200){j=0;}
    }


    if (t == t1) { P6OUT &= ~BIT1;}
    if (t == t2) { P6OUT |= BIT2;}
    if (t == t3) { P6OUT &= ~BIT2;}


    // if (t == t4) { P6OUT |= BIT3;}
    // if (t == t5) { P6OUT &= ~BIT3;}
    // if (t == t6) { P6OUT |= BIT4;}
    // if (t == t7) { P6OUT |= BIT4;}


    if (t == T) { newmode(get_timer_state(), get_delay_state()); t=-1;}
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
		// Останов цикла
		if (t==-2){ t=0; }
	}
	if (state==0 && old_state==0){
		// Пуск цикла
		t=-2;	
		P6OUT=0;
	}
}