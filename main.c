/*
 * GccApplication1.c
 *
 * Created: 29.01.2022 12:04:19
 * Author : Adrian
 */ 

#include <util/atomic.h>
#include <avr/io.h> //biblioteka do wejsc i wyjsc
#include <util/delay.h> // do opoznien
#include <avr/interrupt.h> 
#include "lcd_displ.h"

void start_ADC(void);
void wait_ADC(void);
unsigned read_ADC(void);
void init_ADC ();

int main(void)
{
	DDRD |= (1<<6);
	DDRD |= (1<<5);
	DDRD |= (1<<4);
	DDRD |= (1<<3);
	
	int read;
	timer1_init();
	init_ADC();

	OCR1A = 200; // - 90
	
	lcd_init();
	while (1)
	{ 
		_delay_ms(30);
		start_ADC();
		wait_ADC();
		read = read_ADC();
		if (read > 820){ // serwo w pozycji +90
			PORTD |= (1 << 6);
			PORTD |= (1 << 5);
			PORTD |= (1 << 4);
			PORTD |= (1 << 3);
			while(OCR1A <= 675){
				OCR1A++;
				_delay_ms(2);
				};
		} else if (read > 615) { // serwo w pozycji +45
			PORTD |= (1 << 6);
			PORTD |= (1 << 5);
			PORTD |= (1 << 4);
			PORTD &=~ (1 << 3);
			while(OCR1A <= 556){
				OCR1A++;
				_delay_ms(2);
			};
			while(OCR1A >= 556){
				OCR1A--;
				_delay_ms(2);
			}
		}else if (read > 410) { // serwo w pozycji +0
			PORTD |= (1 << 6);
			PORTD |= (1 << 5);
			PORTD &=~ (1 << 4);
			PORTD &=~ (1 << 3);
			while(OCR1A <= 437){
				OCR1A++;
				_delay_ms(2);
			};
			while(OCR1A >= 437){
				OCR1A--;
				_delay_ms(2);
			}
		}else if (read > 205) { // serwo w pozycji -45
			PORTD |= (1 << 6);
			PORTD &=~ (1 << 5);
			PORTD &=~ (1 << 4);
			PORTD &=~ (1 << 3);
			while(OCR1A <= 318){
				OCR1A++;
				_delay_ms(2);
			};
			while(OCR1A >= 318){
				OCR1A--;
				_delay_ms(2);
			}
		}else if (read < 205) { // serwo w pozycji -90
			PORTD &=~ (1 << 6);
			PORTD &=~ (1 << 5);
			PORTD &=~ (1 << 4);
			PORTD &=~ (1 << 3);
			while(OCR1A <= 200){
				OCR1A++;
				_delay_ms(2);
			};
			while(OCR1A >= 200){
				OCR1A--;
				_delay_ms(2);
			}
			
	}	
		lcd_gotoxy(0,0);
		lcd_printf("wartosc %d ", read);
	}
}


void timer1_init(void){
	DDRB |= (1<<PINB1); //wyjscie na pinb1
	TCCR1A |= (1<<COM1A1) | ( 1<<COM1B1) | (1<<WGM11); // CLEAR OC1A OC1B ON COMPARE MATCH, wraz z wzrostem OCR rosnie srednie napiecie
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS11) | (1<<CS10); // WYBOR FAST PWM ORAZ PRESKALERA 64
	ICR1 = (F_CPU/64UL/50UL)-1 ; // 4999 serwo u mnie operuje w zakresie 4% - 13,5% , a katalogowo powinno 5%-10% duty cycle
	// ocr dzia³a w zakresie 200 - 675
}


void start_ADC(){
	ADCSRA|= 1<<ADSC; //uruchomienie pojedynczej konwersji.
}

void wait_ADC(){
	while( ADCSRA & 1<<ADSC ); //czekanie na zakonczenie konwersji
}

unsigned read_ADC() {
	return ADC;
}

void init_ADC () 
{
	ADMUX= 1<<REFS0 ; // napiecie odniesienia pobierane jest z pinu AVCC
	ADCSRA= 1<<ADEN | //uruchomienie przetwornika
	 |  1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0;  // preskaler  128 //ponizej 200k do 1kk
}


