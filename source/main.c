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
void TimerISR(){ TimerFlag = 1; }
enum states { start, init, on, off } state;
double freq_array[11] = { 261.63, 329.63, 293.66, 293.66, 392, 329.63, 329.63, 329.63, 293.66, 392, 261.33 };
double time_held[55] = { 587, 587, 554, 554, 587, 587, 587, 587, 698.5, 698.5, 698.5, 698.5,554, 554, 554, 554, 554, 554, 554, 494, 494, 494, 494, 440, 440, 494, 494, 494, 494, 587, 587, 587, 587, 698.5, 698.5, 698.5, 698.5, 830.6, 830.6, 698.5, 698.5, 830.6, 830.6, 987.8, 987.8, 987.8, 987.8, 698.5, 698.5, 698.5, 698.5, 659.3, 659.3, 587, 587 };
int count = 0;
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
	static double current_frequency;
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
	unsigned char button = (~PINA & 0x07);
	switch(state){
		case start:
			state = init;
		break;
		case init:
			if(button == 0x01){
				count = 0;
				state = on;
			} else {	
				state = init;
			}
		break;
		case on:
			if(count < 55){
				state = on;
			} else {
				if(button == 0x01) {
					state = off;
				} else {
					state = init;
				}	
			}
		break;
		case off:
			if(button == 0x01){
				state = off;
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
			set_PWM(0);
		break;
		case on:
			if(count < 55) {
				//int beat = 0;
				//while(beat != 200){
					set_PWM(freq_array[count]);
				//	beat ++;
				//}
				count ++;
			}
		break;
		case off:
			set_PWM(0);
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
