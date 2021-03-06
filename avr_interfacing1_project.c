#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//c0:c3   decoder   a0:sec1  a1:sec2  a2:min1  a3:min2  a4:hours1  a5:hours2   INT0:pd2  INT1:PD3   INT2:PB2
//timer1:stopwatch(6_bcd)   INT0:RESET(FALLING_EDGE_INTERNAL_PULL_UP
//INT1:PAUSE(RISING EDGE_EXTERNAL_PULL_DOWN   INT2:RESUME(FALLING_EDGE_INTERNAL_PULL_UP




unsigned char sec1 = 0;
unsigned char sec2 = 0;
unsigned char min1 = 0;
unsigned char min2 = 0;
unsigned char hour1 = 0;
unsigned char hour2 = 0;


void timer1_init(void) {
	sei();							 //set I bit
	TIMSK |= (1<<4); 				 //timer1 compare A
	TCCR1A |= (1<<3);   				//NON-PWM MODE
	TCCR1B |= (1<<3);				//CTC WGM12=1 - WGM10,WGM11,WGM13=0
	TCCR1B |= (1<<1) | (1<<0);		//011 N=64 CS12,CS11,CS10 BITS 2,1,0
	TCNT1 = 0;
	//OCR1A=15625;					//combined with N=64 gives 1 sec exactly
	OCR1A=12250;					// OCR1A new value compensating code delays (compared with real StopWatch)
}

ISR (TIMER1_COMPA_vect) {
	TCNT1 = 0;
	sec1++;
	if (sec1 ==10) {
		sec1 = 0;
		sec2++;
		if (sec2==6) {
			sec2=0;
			min1++;
			if (min1==10) {
				min1=0;
				min2++;
				if (min2==6) {
					min2=0;
					hour1++;
					if (hour1==10) {
						hour1=0;
						hour2++;
						if (hour2==10) {
							hour2=0;
						}
					}
				}
			}
		}
	}
}

void IT0_init(void) {
	SREG |= (1<<7);		//sei();   clear i bit -> cli();
	GICR |= (1<<6);		// INT0 ENABLE
	MCUCR |= (1<<1);	//ISC01,ISC00 10 INT0 FALLING EDGE
	DDRD &= ~(1<<2);    //PD2(INT0) INPUT
	PORTD |= (1<<2);    // INTERNAL PULL UP
}

ISR (INT0_vect) {
	sec1 = 0;
	sec2 = 0;
	min1 = 0;
	min2 = 0;
	hour1 = 0;
	hour2 = 0;
	TCNT1 = 0;
}

void int1_init (void) {
	DDRD &= ~(1<<3);	//input
	GICR |= (1<<7);
	sei();
	MCUCR |= (1<<3) | (1<<2);   //ISC11=1 ISC10=0   11->RISING EDGE
}

ISR (INT1_vect) {
	TCCR1B &= ~(1<<1) & ~(1<<0);		//pause
}

void int2_init(void) {
	DDRB &= ~(1<<2);	//B2 INPUT
	PORTB |= (1<<2);	//INTERNAL PULL UP
	sei();
	GICR |= (1<<5);   //INT2 ENABLE
	MCUCSR &=~ (1<<6);   //ISC2=1 FALLING EDGE
 }

ISR (INT2_vect) {
	TCCR1B |= (1<<1) | (1<<0);			//resume
 }

int main (void) {

	DDRA = 0X3F;		//bcd anodes
	DDRC = 0X0F;
	IT0_init();
	int1_init();
	int2_init();
	timer1_init();

	while (1) {


		PORTA &=~ (1<<1);
		PORTA &=~ (1<<2);
		PORTA &=~ (1<<3);
		PORTA &=~ (1<<4);
		PORTA &=~ (1<<5);
		PORTA |= (1<<0);
		PORTC = (PORTC & 0XF0) | (sec1 & 0X0F) ;
		_delay_ms(5);

		PORTA &=~ (1<<0);
		PORTA &=~ (1<<2);
		PORTA &=~ (1<<3);
		PORTA &=~ (1<<4);
		PORTA &=~ (1<<5);
		PORTA |= (1<<1);
		PORTC = (PORTC & 0XF0) | (sec2 & 0X0F) ;
		_delay_ms(5);

		PORTA &=~ (1<<0);
		PORTA &=~ (1<<1);
		PORTA &=~ (1<<3);
		PORTA &=~ (1<<4);
		PORTA &=~ (1<<5);
		PORTA |= (1<<2);
		PORTC = (PORTC & 0XF0) | (min1 & 0X0F) ;
		_delay_ms(5);

		PORTA &=~ (1<<0);
		PORTA &=~ (1<<1);
		PORTA &=~ (1<<2);
		PORTA &=~ (1<<4);
		PORTA &=~ (1<<5);
		PORTA |= (1<<3);
		PORTC = (PORTC & 0XF0) | (min2 & 0X0F) ;
		_delay_ms(5);

		PORTA &=~ (1<<0);
		PORTA &=~ (1<<1);
		PORTA &=~ (1<<2);
		PORTA &=~ (1<<3);
		PORTA &=~ (1<<5);
		PORTA |= (1<<4);
		PORTC = (PORTC & 0XF) | (hour1 & 0X0F) ;
		_delay_ms(5);

		PORTA &=~ (1<<0);
		PORTA &=~ (1<<1);
		PORTA &=~ (1<<2);
		PORTA &=~ (1<<3);
		PORTA &=~ (1<<4);
		PORTA |= (1<<5);
		PORTC = (PORTC & 0XF0) | (hour2 & 0X0F) ;
		_delay_ms(5);
	}
}

