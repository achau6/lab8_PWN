/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section: 
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
enum states { start, init, change } state;
void TimerISR(){ TimerFlag = 1; }
enum states { start, init, Con, Don, Eon } state;
void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}
void TimerOff(){
	TCCR1B = 0x00;
}
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
void set_PWM(double frequency){
	statis double current_frequency;
	if(frequency != current_frequency){
		if(!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }
		if(frequency < 0.954) { OCR3A = 0xFFFF; }
		else if(frequency > 32150) { OCR3A = 0x0000; }
		else { OCR3A = (short)(8000000 / (128*frequency)) - 1; }
		TCNT3 = 0;
		current_frequency = frequency;
	}
}
void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}
void PWM_off(){
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}
void tick(){
	unsigned char button = ~PINA & 0x03;
	switch(state){
		case start:
			state = init;
		break;
		case init:
			if(button == 0x01){
				state = Con;
			} else if(button == 0x02){
				state = Don;
			} else if(button == 0x03){
				state = Eon;
			} else {	
				state = init;
			}
		break;
		case Con:
			if(button == 0x01){
				state = Con;
			} else {
				state = init;
			}
		break;
		case Don:
			if(button == 0x02){
				state = Don;
			} else {
				state = init;
			}
		break;
		case Eon:
			if(button == 0x03){
				state = Eon;
			} else {
				state = init;
			}
		break;
		default:
		break;

	}

	switch(state){
		case start:
		break;
		case init:
			set_PWN(0);
		break;
		case Con:
			set_PWM(261.63);
		break;
		case Don:
			set_PWM(293.66);
		break;
		case Eon:
			set_PWM(329.63);
		break;
		default:
		break;

	}
}
int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(100);
	TimerOn();
	PWM_on();
	state = start;
    /* Insert your solution below */
    while (1) {
	    tick();
	    while(!TimerFlag){};
	    TimerFlag = 0;

    }
    return 1;
}
